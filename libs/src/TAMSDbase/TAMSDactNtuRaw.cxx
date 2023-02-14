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
//! Compute common noise
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
   UInt_t adcFirst = 0;
   UInt_t adcSecond = 0;
   Int_t view = -1;
   Int_t sensorId = -1;

   Double_t valueFirst = -99;
   Double_t valueSecond = -99;

   Double_t meanFirst  = 0;
   Double_t meanSecond  = 0;

   Double_t vaContent[CN_CH] = {0};

   TAMSDrawHit* hit;

   Bool_t seedFirst = false;
   Bool_t seedSecond = false;

   
   for (Int_t i = 0; i < p_pargeo->GetStripsN(); ++i) {

      adcFirst = evt->FirstPlane[i];
      adcSecond = evt->SecondPlane[i];

      if(fPedestal) { // for decoding pedestal data acquired with mixed trigger mode in HIT2022 (all channels are flagged as hit)
         
         view=0;
         sensorId = p_parmap->GetSensorId(boardId, view);
         hit = p_datraw->AddStrip(sensorId, view, i, adcFirst);
         
         view=1;
         sensorId = p_parmap->GetSensorId(boardId, view);
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
               const UInt_t* adcPtr = &evt->FirstPlane[i];
               
               for (int VaChan = 0; VaChan < CN_CH; VaChan++) {
                  if (pedestal.status) continue;
                  vaContent[VaChan] = *(adcPtr+VaChan) - p_parcal->GetPedestal(sensorId, i + VaChan).mean;
               }
               cnFirst = ComputeCN(vaContent, 10);
               if (ValidHistogram())
                  fpHisCommonMode[sensorId]->Fill(cnFirst);
            }
         }         

         if (p_parconf->GetSensorPar(sensorId).DeadStripMap[i] == 1) continue;
   

         if (fgPedestalSub) { // pedestal subtraction is enabled
            seedFirst = false;
            valueFirst = p_parcal->GetPedestalThreshold(sensorId, pedestal, true);
            meanFirst = pedestal.mean;
            
            valueFirst = adcFirst - valueFirst -cnFirst;

            if (valueFirst > 0) {
               seedFirst = true;
               if (ValidHistogram())
                  fpHisSeedMap[sensorId]->Fill(i, adcFirst - meanFirst - cnFirst);
            }

            valueFirst = p_parcal->GetPedestalThreshold(sensorId, pedestal, false);
            valueFirst = adcFirst - valueFirst - cnFirst;

            if (valueFirst > 0) {               
               hit = p_datraw->AddStrip(sensorId, view, i, adcFirst - meanFirst - cnFirst);
               hit->SetSeed(seedFirst);

               if (ValidHistogram())
                  fpHisStripMap[sensorId]->Fill(i, adcFirst - meanFirst - cnFirst);
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
               const UInt_t* adcPtr = &evt->SecondPlane[i];
                  
               for (int VaChan = 0; VaChan < CN_CH; VaChan++) {
                  if (pedestal.status) continue;
                  vaContent[VaChan] = *(adcPtr+VaChan) - p_parcal->GetPedestal(sensorId, i + VaChan).mean;
               }
               cnSecond = ComputeCN(vaContent, 10);
               if (ValidHistogram())
                  fpHisCommonMode[sensorId]->Fill(cnSecond);
            }
         }         
         
         if (p_parconf->GetSensorPar(sensorId).DeadStripMap[i] == 1) continue;

         
         if (fgPedestalSub) { // pedestal subtraction is enabled
            seedSecond = false;
            valueSecond = p_parcal->GetPedestalThreshold(sensorId, pedestal, true);
            meanSecond = pedestal.mean;
            
            valueSecond = adcSecond - valueSecond - cnSecond;

            if (valueSecond > 0) {
               seedSecond = true;
               if (ValidHistogram())
                  fpHisSeedMap[sensorId]->Fill(i, adcSecond-meanSecond-cnSecond);
            }
            
            valueSecond = p_parcal->GetPedestalThreshold(sensorId, pedestal, false);
            valueSecond = adcSecond - valueSecond - cnSecond;

            if (valueSecond > 0) {
               hit = p_datraw->AddStrip(sensorId, view, i, adcSecond-meanSecond-cnSecond);
               hit->SetSeed(seedSecond);
               
               if (ValidHistogram())
                  fpHisStripMap[sensorId]->Fill(i, adcSecond-meanSecond-cnSecond);
            }
            
            if(FootDebugLevel(2)) {
               if(valueFirst>0 || valueSecond>0)
                  cout<<" Sens:: "<<sensorId<<" View:: "<<view<<" Strip:: "<<i<<endl;
            }
         }
         
      }
   }
   
   return kTRUE;

}
