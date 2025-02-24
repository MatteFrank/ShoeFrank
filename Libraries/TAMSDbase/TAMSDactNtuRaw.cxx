/*!
  \file TAMSDactNtuRaw.cxx
  \brief   Implementation of TAMSDactNtuRaw.
*/

#include "TAMSDactNtuRaw.hxx"
#include "TAGrecoManager.hxx"

/*!
  \class TAMSDactNtuRaw
  \brief Get MSD raw data from DAQ. **
*/

//! Class imp
ClassImp(TAMSDactNtuRaw);

UInt_t TAMSDactNtuRaw::fkgThreshold    = 0;
Bool_t TAMSDactNtuRaw::fgPedestalSub   = true;
Bool_t TAMSDactNtuRaw::fgCommonModeSub = true;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] dscdatraw data output container descriptor
//! \param[in] dscdatdaq raw data input container descriptor
//! \param[in] dscparmap mapping parameter descriptor
//! \param[in] dscparcal calibration parameter descriptor
//! \param[in] dscpargeo geometry parameter descriptor
//! \param[in] dscparconf configuration parameter descriptor
TAMSDactNtuRaw::TAMSDactNtuRaw(const char* name,
                             TAGdataDsc* dscdatraw,
                             TAGdataDsc* dscdatdaq,
                             TAGparaDsc* dscparmap,
                             TAGparaDsc* dscparcal,
                             TAGparaDsc* dscpargeo,
                             TAGparaDsc* dscparconf)
  : TAGaction(name, "TAMSDactNtuRaw - Unpack MSD raw data"),
    fpDatRaw(dscdatraw),
    fpDatDaq(dscdatdaq),
    fpParMap(dscparmap),
    fpParCal(dscparcal),
    fpParGeo(dscpargeo),
    fpParConf(dscparconf)
{
  if (FootDebugLevel(1))
    cout<<"TAMSDactNtuRaw::default constructor::Creating the MSD hit Ntuplizer"<<endl;
   
  AddDataOut(dscdatraw, "TAMSDntuRaw");
  AddPara(dscparmap, "TAMSDparMap");
  AddPara(dscparcal, "TAMSDparCal");
  AddPara(dscpargeo, "TAMSDparGeo");
  AddPara(dscparconf, "TAMSDparConf");
  AddDataIn(dscdatdaq, "TAGdaqEvent");
   
   TAMSDparConf* p_parconf= (TAMSDparConf*)   fpParConf->Object();
   fPedestal = p_parconf->GetAnalysisPar().PedestalFlag;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuRaw::~TAMSDactNtuRaw()
{
}

//------------------------------------------+-----------------------------------
//! Create histograms
void TAMSDactNtuRaw::CreateHistogram()
{
  DeleteHistogram();
  
   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpParGeo->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {

      fpHisSeedMap[i] = new TH1F(Form("msSeedMap%d", i+1), Form("MSD - seed map for sensor %d", i+1),
                                  pGeoMap->GetStripsN(), 0, pGeoMap->GetStripsN());
      AddHistogram(fpHisSeedMap[i]);

      fpHisStripMap[i] = new TH1F(Form("msStripMap%d", i+1), Form("MSD - strip map for sensor %d", i+1),
                                  pGeoMap->GetStripsN(), 0, pGeoMap->GetStripsN());
      AddHistogram(fpHisStripMap[i]);

      fpHisCommonMode[i] = new TH1F(Form("msCommonMode%d", i+1), Form("MSD - common mode noise for sensor %d", i+1),
                                    200, -50, 50);
      AddHistogram(fpHisCommonMode[i]);
   }
   
  SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Compute common noise (with median)
//!
//! \param[in] vaContent input ASIC vector
//! \param[in] threshold threshold value
Double_t TAMSDactNtuRaw::ComputeCN(Double_t* vaContent, Double_t threshold)
{
   Float_t cn = 0;

   std::vector<Double_t> goodStrips;

   // Get the median of the strips below the threshold
   for (size_t i = 0; i < CN_CH; i++) {
      if (vaContent[i] < threshold)
         goodStrips.push_back(vaContent[i]);
   }

   if(goodStrips.size() > 32)
   {
      cn = TMath::Median(goodStrips.size(), &goodStrips[0]);
   }
   else
   {
      ComputeCN(vaContent, threshold+1);
   }

   return cn;
}

//------------------------------------------+-----------------------------------
//! Compute common noise
//!
//! \param[in] strip strip number
//! \param[in] vaContent input vector
//! \param[in] type type
Double_t TAMSDactNtuRaw::ComputeCN(Int_t strip, Double_t* vaContent, Int_t type)
{
   Float_t cn   = 0;
   Int_t cnt    = 0;

   Float_t mean = TMath::Mean(CN_CH, vaContent);
   Float_t rms  = TMath::RMS(CN_CH, vaContent);

   if (type == 0) { //simple common noise wrt VA mean ADC value
   
      for (size_t i = 0; i < CN_CH; i++) {
         if (vaContent[i] > mean - 2 * rms && vaContent[i] < mean + 2 * rms) {
            cn += vaContent[i];
            cnt++;
         }
      }
      
      if (cnt != 0)
         return cn / cnt;
      else
         return 0;
      
   } else if (type == 1) { //Common noise with fixed threshold to exclude potential real signal strips
   
      for (size_t i = 0; i < CN_CH; i++) {
         if (vaContent[i] < MIP_ADC / 2) { //very conservative cut: half the value expected for a Minimum Ionizing Particle
            cn += vaContent[i];
            cnt++;
         }
      }
      if (cnt != 0)
         return cn / cnt;
      else
         return 0;
      
   } else {//Common Noise with 'self tuning' threshold: we use the some of the channels to calculate a baseline level, then we use all the strips in a band around that value to compute the CN

      Float_t hard_cm = 0;
      Int_t cnt2 = 0;
      for (size_t i = 8; i < 23; i++) {
         if (vaContent[i] < 1.5 * MIP_ADC) {//looser constraint than algo 2
            hard_cm += vaContent[i];
            cnt2++;
         }
      }
      
      if (cnt2 != 0)
         hard_cm = hard_cm / cnt2;
      else
         return 0;

      for (size_t i = 23; i < 55; i++) {
         if (vaContent[i] > hard_cm - 2 * rms && vaContent[i] < hard_cm + 2 * rms) { //we use only channels with a value around the baseline calculated at the previous step
            cn += vaContent[i];
            cnt++;
         }
      }
      
      if (cnt != 0)
         return cn / cnt;
      else
         return 0;
   }
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAMSDactNtuRaw::Action()
{
   TAGdaqEvent*   p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();
   
   Int_t nFragments = p_datdaq->GetFragmentsN();
   
   if(FootDebugLevel(1))
      cout<<"TAMSDactNtuRaw::Action():: I'm going to charge "<<nFragments<<" number of fragments"<<endl;
   
   Int_t size = (int)p_datdaq->GetFragmentSize("DEMSDEvent");
   for (Int_t i = 0; i < size; ++i) {
      const DEMSDEvent* evt = static_cast<const DEMSDEvent*> (p_datdaq->GetFragment("DEMSDEvent", i));
      
      if (evt) {
         if (FootDebugLevel(1))
            evt->printData();
         
         DecodeHits(evt);
      }
   }
   
   fpDatRaw->SetBit(kValid);
   
   if(FootDebugLevel(2))
      cout<<"TAMSDactNtuRaw::Action():: done"<<endl;
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Decode hits
//!
//! \param[in] evt DAQ event
Bool_t TAMSDactNtuRaw::DecodeHits(const DEMSDEvent* evt)
{
   TAMSDntuRaw*    p_datraw = (TAMSDntuRaw*)    fpDatRaw->Object();
   TAMSDparGeo*    p_pargeo = (TAMSDparGeo*)    fpParGeo->Object();
   TAMSDparConf*   p_parconf= (TAMSDparConf*)   fpParConf->Object();
   TAMSDparCal*    p_parcal = (TAMSDparCal*)    fpParCal->Object();
   TAMSDparMap*    p_parmap = (TAMSDparMap*)    fpParMap->Object();

   if(FootDebugLevel(2)) {
     cout<<"****************************"<<endl;
     cout<<"  NtuRaw hits "<<endl;
     cout<<"****************************"<<endl;
   }
   
   // decode here
   if (evt->detectorHeader == 0x00000bad) {
      if (FootDebugLevel(1))
         cout << "Data is empty, skipping DecodeHits" << endl;
         sleep(1);
      return kFALSE;
   }

   Int_t boardId = (evt->boardHeader & 0xF)-1;
   Double_t cnFirst = 0;
   Double_t cnSecond = 0;
   
   for (Int_t i = 0; i < p_pargeo->GetStripsN(); ++i) {
      
      UInt_t adcSecond = evt->SecondPlane[i];
      UInt_t adcFirst = evt->FirstPlane[i];
      UInt_t adcDummy;
      Int_t view  = -1;
      Int_t sensorId = -1;
      Bool_t status  = true;
      
      Double_t valueFirst = -99;
      Double_t valueSecond = -99;
      
      Double_t meanFirst  = 0;
      Double_t meanSecond  = 0;
      
      Double_t vaContent[CN_CH] = {0};
      
      TAMSDrawHit* hit;

      if(fPedestal) {
         
         view=0;
         sensorId = p_parmap->GetSensorId(boardId, view);
         if(sensorId % 2 == 0)
            hit = p_datraw->AddStrip(sensorId, view, i, adcFirst);
         else
            hit = p_datraw->AddStrip(sensorId, view, i, adcSecond);
         
         view=1;
         sensorId = p_parmap->GetSensorId(boardId, view);
         if (sensorId % 2 == 0)
            hit = p_datraw->AddStrip(sensorId, view, i, adcFirst);
         else
            hit = p_datraw->AddStrip(sensorId, view, i, adcSecond);

      } else {
         
         view = 0;
         sensorId = p_parmap->GetSensorId(boardId, view);
         if (sensorId < 0) {
            Warning("DecodeHits()", "Bad sensorId %d BoardId %d View %d", sensorId, boardId, view);
            continue;
         }
         auto pedestal = p_parcal->GetPedestal( sensorId, i );

         if (fgCommonModeSub) { // common mode subtraction is enabled
            if (i % CN_CH == 0) {
               const UInt_t* adcPtr = sensorId % 2 == 0 ? &evt->FirstPlane[i] : &evt->SecondPlane[i];

               for (int VaChan = 0; VaChan < CN_CH; VaChan++)
                  vaContent[VaChan] = *(adcPtr+VaChan) - p_parcal->GetPedestal(sensorId, i + VaChan).mean;
   
               cnFirst = ComputeCN(vaContent, 10);
               if (ValidHistogram())
                  fpHisCommonMode[sensorId]->Fill(cnFirst);
            }
         }
         
         if( !pedestal.status ) {
            if (fgPedestalSub) {
               Bool_t seedFirst = false;
               valueFirst = p_parcal->GetPedestalThreshold(sensorId, pedestal, true);
               meanFirst = pedestal.mean;
               if(sensorId%2 == 0)
                  adcDummy = adcFirst;
               else
                  adcDummy = adcSecond;
               
               valueFirst = adcDummy - valueFirst -cnFirst;

               if (valueFirst > 0)
               {
                  seedFirst = true;
                  if (ValidHistogram())
                     fpHisSeedMap[sensorId]->Fill(i, adcDummy - meanFirst - cnFirst);
               }

               valueFirst = p_parcal->GetPedestalThreshold(sensorId, pedestal, false);
               valueFirst = adcDummy - valueFirst - cnFirst;
               if (valueFirst > 0)
               {
                  TAMSDrawHit *hit = p_datraw->AddStrip(sensorId, view, i, adcDummy - meanFirst - cnFirst);
                  hit->SetSeed(seedFirst);

                  if (ValidHistogram())
                     fpHisStripMap[sensorId]->Fill(i, adcDummy - meanFirst - cnFirst);

                  if (pedestal.status)
                     hit->SetNoisy();
               }
            }
         }
         
         view = 1;
         sensorId = p_parmap->GetSensorId(boardId, view);
         if (sensorId < 0) {
            Warning("DecodeHits()", "Bad sensorId %d BoardId %d View %d", sensorId, boardId, view);
            continue;
         }
         pedestal = p_parcal->GetPedestal( sensorId, i );

         if (fgCommonModeSub) { // common mode subtraction is enabled
            if (i % CN_CH == 0) {
               const UInt_t* adcPtr = sensorId % 2 == 0 ? &evt->FirstPlane[i] : &evt->SecondPlane[i];

               for (int VaChan = 0; VaChan < CN_CH; VaChan++)
                  vaContent[VaChan] = *(adcPtr+VaChan) - p_parcal->GetPedestal(sensorId, i + VaChan).mean;
   
               cnSecond = ComputeCN(vaContent, 10);
               if (ValidHistogram())
                  fpHisCommonMode[sensorId]->Fill(cnSecond);
            }
         }
         
         if( !pedestal.status ) {
            if (fgPedestalSub) {
               Bool_t seedSecond = false;
               valueSecond = p_parcal->GetPedestalThreshold(sensorId, pedestal, true);
               meanSecond = pedestal.mean;
               UInt_t *adcPtr;
               if (sensorId % 2 == 0)
                     adcDummy = adcFirst;
               else
                     adcDummy = adcSecond;
               
               valueSecond = adcDummy - valueSecond - cnSecond;
               
               if (valueSecond > 0) {
                  seedSecond = true;
                  if (ValidHistogram())
                     fpHisSeedMap[sensorId]->Fill(i, adcDummy-meanSecond-cnSecond);
               }
               
               valueSecond = p_parcal->GetPedestalThreshold(sensorId, pedestal, false);
               valueSecond = adcDummy - valueSecond - cnSecond;
               if (valueSecond > 0) {
                  TAMSDrawHit* hit = p_datraw->AddStrip(sensorId, view, i, adcDummy-meanSecond-cnSecond);
                  hit->SetSeed(seedSecond);
                  
                  if (ValidHistogram())
                     fpHisStripMap[sensorId]->Fill(i, adcDummy-meanSecond-cnSecond);

                  if (pedestal.status)
                     hit->SetNoisy();
               }

               if(FootDebugLevel(2)) {
                  if(valueFirst>0 || valueSecond>0)
                     cout<<" Sens:: "<<sensorId<<" View:: "<<view<<" Strip:: "<<i<<endl;
               }
            }
         }
      }
   }
   
   return kTRUE;
}