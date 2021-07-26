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
Bool_t TAMSDactNtuRaw::fgPedestalSub = true;

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
   TAMSDparMap*    p_parmap = (TAMSDparMap*)    fpParMap->Object();

   // decode here
   Int_t boardId = (evt->boardHeader & 0xF)-1;
   
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

      view = 1;
      sensorId = p_parmap->GetSensorId(boardId, view);
      auto pedestal = p_parcal->GetPedestal( sensorId, i );

      if( pedestal.status ) {
         if (fgPedestalSub) {
             valueX = p_parcal->GetPedestalValue(sensorId, pedestal);
             meanX = pedestal.mean;
             valueX = adcX - valueX;
         }
         if (valueX > 0) {
            p_datraw->AddStrip(sensorId, i, 0, adcX-meanX);
            if (ValidHistogram())
               fpHisStripMap[sensorId]->Fill(i, adcX-meanX);
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
         }
         if (valueY > 0) {
            p_datraw->AddStrip(sensorId, i, 1, adcY-meanY);
            if (ValidHistogram())
               fpHisStripMap[sensorId]->Fill(i, adcY-meanY);
         }
      }
   }
   
   return kTRUE;
}
