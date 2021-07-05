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

ClassImp(TAMSDactNtuRaw);

 UInt_t TAMSDactNtuRaw::fkgThreshold = 0;

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
   }
   
  SetValidHistogram(kTRUE);
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

   // decode here
   Int_t boardId = (evt->boardHeader & 0xF)-1;
   
   for (Int_t i = 0; i < p_pargeo->GetStripsN(); ++i) {
      
      UInt_t adcX = evt->Xplane[i];
      UInt_t adcY = evt->Yplane[i];
      Int_t view  = -1;
      Int_t sensorId = -1;
      Bool_t status  = true;
      
      view = 1;
      sensorId = TAMSDparGeo::GetSensorId(boardId, view);
      status   = p_parcal->GetPedestalStatus(sensorId, i);
      if (status == 0) {
         Double_t valueX = p_parcal->GetPedestalValue(sensorId, i);
         Double_t meanX  = p_parcal->GetPedestalMean(sensorId, i);
         valueX  = adcX - valueX;
         
         if (valueX > 0) {
            p_datraw->AddStrip(sensorId, i, view, adcX-meanX);
            if (ValidHistogram())
               fpHisStripMap[sensorId]->Fill(i, adcX-meanX);
         }
      }
      
      view = 0;
      sensorId = TAMSDparGeo::GetSensorId(boardId, view);
      status   = p_parcal->GetPedestalStatus(sensorId, i);
      if (status == 0) {
         Double_t valueY = p_parcal->GetPedestalValue(sensorId, i);
         Double_t meanY  = p_parcal->GetPedestalMean(sensorId, i);
         valueY  = adcY - valueY;
         
         if (valueY > 0) {
            p_datraw->AddStrip(sensorId, i, view, adcY-meanY);
            if (ValidHistogram())
               fpHisStripMap[sensorId]->Fill(i, adcY-meanY);
         }
      }
   }
   
   return kTRUE;
}
