/*!
 \file
 \version $Id: TAMSDactNtuHit.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
 \brief   Implementation of TAMSDactNtuHit.
 */

#include "TAMSDparMap.hxx"
#include "TAMSDparCal.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDactNtuHit.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAMSDactNtuHit TAMSDactNtuHit.hxx "TAMSDactNtuHit.hxx"
 \brief Get Beam Monitor raw data from WD. **
 */

ClassImp(TAMSDactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMSDactNtuHit::TAMSDactNtuHit(const char* name,
                               TAGdataDsc* p_datraw,
                               TAGdataDsc* p_nturaw,
                               TAGparaDsc* p_pargeo,
                               TAGparaDsc* p_parconf,
                               TAGparaDsc* p_parcal)
 : TAGaction(name, "TAMSDactNtuHit - Unpack MSD calibrated data"),
   fpDatRaw(p_datraw),
   fpNtuRaw(p_nturaw),
   fpGeoMap(p_pargeo),
   fpParConf(p_parconf),
   fpParCal(p_parcal)
{
   AddDataIn(p_datraw, "TAMSDntuRaw");
   AddDataOut(p_nturaw, "TAMSDntuHit");
   
   AddPara(p_pargeo, "TAMSDparGeo");
   AddPara(p_parconf, "TAMSDparConf");
   AddPara(p_parcal, "TAMSDparCal");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuHit::~TAMSDactNtuHit()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMSDactNtuHit::Action()
{
   TAMSDntuRaw*   p_datraw = (TAMSDntuRaw*)  fpDatRaw->Object();
   TAMSDntuHit*   p_nturaw = (TAMSDntuHit*)  fpNtuRaw->Object();
   TAMSDparGeo*   p_geoMap = (TAMSDparGeo*)  fpGeoMap->Object();
   TAMSDparConf*  p_config = (TAMSDparConf*) fpParConf->Object();

   if(FootDebugLevel(2)) {
     cout<<"****************************"<<endl;
     cout<<"  NtuHit hits "<<endl;
     cout<<"****************************"<<endl;
   }
   
   // loop over boards
   for (Int_t i = 0; i < p_geoMap->GetSensorsN(); ++i) {
      
      int nstrip = p_datraw->GetStripsN(i);
      int nseed = p_datraw->GetSeedsN(i);
      
      for(int ih = 0; ih < nstrip; ++ih) {
         TAMSDrawHit *strip = p_datraw->GetStrip(i, ih);
         
         Int_t sensorId   = strip->GetSensorId();
         Int_t stripId    = strip->GetStrip();
         Int_t view       = strip->GetView();
         Double_t charge  = strip->GetCharge();
         Float_t posStrip = p_geoMap->GetPosition(stripId);
         if(FootDebugLevel(2))
            cout<<" Sens "<<sensorId<<" strip "<<stripId<<" View "<<view<<" position (cm) "<<posStrip<<" "<<endl;
	 
         if (FootDebugLevel(1))
            printf("sensor: %d strip: %d view: %d charge: %f\n", sensorId, stripId, view, charge);
         
         if (p_config->GetSensorPar(sensorId).DeadStripMap[stripId] == 1) continue;

         TAMSDhit* hit = p_nturaw->NewStrip(sensorId, charge, view, stripId);
         hit->SetPosition(posStrip);
      }

      for(int ih = 0; ih < nseed; ++ih) {
         TAMSDrawHit *seed = p_datraw->GetSeed(i, ih);
         
         Int_t sensorId   = seed->GetSensorId();
         Int_t seedId    = seed->GetStrip();
         Int_t view       = seed->GetView();
         Double_t charge    = seed->GetCharge();
         Float_t posSeed = p_geoMap->GetPosition(seedId);
         if(FootDebugLevel(2))
            cout<<"\tSens "<<sensorId<<" seed "<<seedId<<" View "<<view<<" position (cm) "<<posSeed<<" "<<endl;
	 
         if (FootDebugLevel(1))
            printf("sensor: %d seed: %d view: %d charge: %f\n", sensorId, seedId, view, charge);
         
         if (p_config->GetSensorPar(sensorId).DeadStripMap[seedId] == 1) continue;

         TAMSDhit* hitSeed = p_nturaw->NewSeed(sensorId, charge, view, seedId);
         hitSeed->SetPosition(posSeed);	 
      }
   }
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}


//------------------------------------------+-----------------------------------
//! Histograms

void TAMSDactNtuHit::CreateHistogram(){
   
   DeleteHistogram();
   
   SetValidHistogram(kTRUE);
   
}
