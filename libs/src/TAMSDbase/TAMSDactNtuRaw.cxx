/*!
  \file
  \version $Id: TAMSDactNtuRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TAMSDactNtuRaw.
*/


#include "TAMSDactNtuRaw.hxx"
#include "TAGrecoManager.hxx"

/*!
  \class TAMSDactNtuRaw TAMSDactNtuRaw.hxx "TAMSDactNtuRaw.hxx"
  \brief Get MSD raw data from DAQ. **
*/

#define MIP_ADC 18

ClassImp(TAMSDactNtuRaw);

UInt_t TAMSDactNtuRaw::fkgThreshold = 0;
Bool_t TAMSDactNtuRaw::fgPedestalSub = true;
Bool_t TAMSDactNtuRaw::fgCommonModeSub = true;

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMSDactNtuRaw::TAMSDactNtuRaw(const char* name,
                             TAGdataDsc* dscdatraw,
                             TAGdataDsc* dscdatdaq,
                             TAGparaDsc* dscparmap,
                             TAGparaDsc* dscparcal,
                             TAGparaDsc* dscpargeo)
  : TAGaction(name, "TAMSDactNtuRaw - Unpack MSD raw data"),
    fpDatRaw(dscdatraw),
    fpDatDaq(dscdatdaq),
    fpParMap(dscparmap),
    fpParCal(dscparcal),
    fpParGeo(dscpargeo)
{
  if (FootDebugLevel(1))
    cout<<"TAMSDactNtuRaw::default constructor::Creating the MSD hit Ntuplizer"<<endl;
   
  AddDataOut(dscdatraw, "TAMSDntuRaw");
  AddPara(dscparmap, "TAMSDparMap");
  AddPara(dscparcal, "TAMSDparCal");
  AddPara(dscpargeo, "TAMSDparGeo");
  AddDataIn(dscdatdaq, "TAGdaqEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMSDactNtuRaw::~TAMSDactNtuRaw()
{
}

//------------------------------------------+-----------------------------------

void TAMSDactNtuRaw::CreateHistogram()
{
  DeleteHistogram();
  
   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpParGeo->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {

      fpHisStripMap[i] = new TH1F(Form("msStripMap%d", i+1), Form("MSD - strip map for sensor %d", i+1),
                                  pGeoMap->GetStripsN(), 0, pGeoMap->GetStripsN());
      AddHistogram(fpHisStripMap[i]);

      fpHisCommonMode[i] = new TH1F(Form("msCommonMode%d", i+1), Form("MSD - common mode noise for sensor %d", i+1),
                                    200, -50, 50);
      AddHistogram(fpHisCommonMode[i]);
   }
   
  SetValidHistogram(kTRUE);
}

Double_t TAMSDactNtuRaw::ComputeCN(Int_t strip, Double_t *VaContent, Int_t type)
{
   Float_t cn = 0;
   Int_t cnt = 0;
   Float_t mean = TMath::Mean(64, VaContent);
   Float_t rms = TMath::RMS(64, VaContent);

   if (type == 0) //simple common noise wrt VA mean ADC value
   {
      for (size_t i = 0; i < 64; i++)
      {
         if (VaContent[i] > mean - 2 * rms && VaContent[i] < mean + 2 * rms)
         {
            cn += VaContent[i];
            cnt++;
         }
      }
      if (cnt != 0)
      {
         return cn / cnt;
      }
      else
      {
         return -999;
      }
   }
   else if (type == 1) //Common noise with fixed threshold to exclude potential real signal strips
   {
      for (size_t i = 0; i < 64; i++)
      {
         if (VaContent[i] < MIP_ADC / 2) //very conservative cut: half the value expected for a Minimum Ionizing Particle
         {
            cn += VaContent[i];
            cnt++;
         }
      }
      if (cnt != 0)
      {
         return cn / cnt;
      }
      else
      {
         return -999;
      }
   }
   else //Common Noise with 'self tuning' threshold: we use the some of the channels to calculate a baseline level, then we use all the strips in a band around that value to compute the CN
   {
      Float_t hard_cm = 0;
      Int_t cnt2 = 0;
      for (size_t i = 8; i < 23; i++)
      {
         if (VaContent[i] < 1.5 * MIP_ADC) //looser constraint than algo 2
         {
            hard_cm += VaContent[i];
            cnt2++;
         }
      }
      if (cnt2 != 0)
      {
         hard_cm = hard_cm / cnt2;
      }
      else
      {
         return -999;
      }

      for (size_t i = 23; i < 55; i++)
      {
         if (VaContent[i] > hard_cm - 2 * rms && VaContent[i] < hard_cm + 2 * rms) //we use only channels with a value around the baseline calculated at the previous step
         {
            cn += VaContent[i];
            cnt++;
         }
      }
      if (cnt != 0)
      {
         return cn / cnt;
      }
      else
      {
         return -999;
      }
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

   for (Int_t i = 0; i < nFragments; ++i) {
       TString type = p_datdaq->GetClassType(i);
       if (type.Contains("DEMSDEvent")) {
         const DEMSDEvent* evt = static_cast<const DEMSDEvent*> (p_datdaq->GetFragment(i));
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
//! Action.
Bool_t TAMSDactNtuRaw::DecodeHits(const DEMSDEvent* evt)
{
   TAMSDntuRaw*    p_datraw = (TAMSDntuRaw*)    fpDatRaw->Object();
   TAMSDparGeo*    p_pargeo = (TAMSDparGeo*)    fpParGeo->Object();
   TAMSDparCal*    p_parcal = (TAMSDparCal*)    fpParCal->Object();
   TAMSDparMap*    p_parmap = (TAMSDparMap*)    fpParMap->Object();

   // decode here
   Int_t boardId = (evt->boardHeader & 0xF)-1;
   Double_t cnX = 0;
   Double_t cnY = 0;

   for (Int_t i = 0; i < p_pargeo->GetStripsN(); ++i) {
      
      UInt_t adcX = evt->Xplane[i];
      UInt_t adcY = evt->Yplane[i];
      Int_t view  = -1;
      Int_t sensorId = -1;
      Bool_t status  = true;
      
      Double_t valueX = 99;
      Double_t valueY = 99;

      Double_t meanX  = 0;
      Double_t meanY  = 0;

      Double_t VaContent[64] = {0};
       
      view = 1;
      sensorId = p_parmap->GetSensorId(boardId, view);
      auto pedestal = p_parcal->GetPedestal( sensorId, i );

      if( pedestal.status ) {
         if (fgPedestalSub) {
             valueX = p_parcal->GetPedestalValue(sensorId, pedestal);
             meanX = pedestal.mean;
             valueX = adcX - valueX;

            if (fgCommonModeSub)
            {
               if (i % 64 == 0)
               {
                  for (int VaChan = 0; VaChan < 64; VaChan++)
                  {
                     VaContent[VaChan] = evt->Xplane[i + VaChan] - p_parcal->GetPedestal(sensorId, i + VaChan).mean;
                  }
                  cnX = ComputeCN(i, VaContent, 0);
                  if (ValidHistogram())
                  fpHisCommonMode[sensorId]->Fill(cnX);
               }
            }
         }
         if (valueX > 0) {
            p_datraw->AddStrip(sensorId, i, 0, adcX-meanX-cnX);
            if (ValidHistogram())
               fpHisStripMap[sensorId]->Fill(i, adcX-meanX-cnX);
         }
      }
      view = 0;
      sensorId = p_parmap->GetSensorId(boardId, view);
      pedestal = p_parcal->GetPedestal( sensorId, i );

      if( pedestal.status ) {
         if (fgPedestalSub) {
            valueY = p_parcal->GetPedestalValue(sensorId, pedestal);
            meanY = pedestal.mean;
            valueY = adcY - valueY;

            if (fgCommonModeSub)
            {
               if (i % 64 == 0)
               {
                  for (int VaChan = 0; VaChan < 64; VaChan++)
                  {
                     VaContent[VaChan] = evt->Yplane[i + VaChan] - p_parcal->GetPedestal(sensorId, i + VaChan).mean;
                  }
                  cnY = ComputeCN(i, VaContent, 0);
               }
            }
         }
         if (valueY > 0)
         {
            p_datraw->AddStrip(sensorId, i, 1, adcY-meanY-cnY);
            if (ValidHistogram())
            {
               fpHisStripMap[sensorId]->Fill(i, adcY-meanY-cnY);
               fpHisCommonMode[sensorId]->Fill(cnY);
            }
         }
      }
   }
   
   return kTRUE;
}
