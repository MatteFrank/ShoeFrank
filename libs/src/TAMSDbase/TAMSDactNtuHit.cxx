/*!
 \file
 \version $Id: TAMSDactNtuHit.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
 \brief   Implementation of TAMSDactNtuHit.
 */

#include "TAMSDparMap.hxx"
#include "TAMSDparCal.hxx"
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
                               TAGparaDsc* p_parcal)
 : TAGaction(name, "TAMSDactNtuHit - Unpack MSD calibrated data"),
   fpDatRaw(p_datraw),
   fpNtuRaw(p_nturaw),
   fpGeoMap(p_pargeo),
   fpParCal(p_parcal)
{
   AddDataIn(p_datraw, "TAMSDntuRaw");
   AddDataOut(p_nturaw, "TAMSDntuHit");
   
   AddPara(p_pargeo, "TAMSDparGeo");
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
   TAMSDntuRaw*   p_datraw = (TAMSDntuRaw*) fpDatRaw->Object();
   TAMSDntuHit*   p_nturaw = (TAMSDntuHit*) fpNtuRaw->Object();
   TAMSDparGeo*   p_geoMap = (TAMSDparGeo*) fpGeoMap->Object();
   
   
   // loop over boards
   for (Int_t i = 0; i < p_geoMap->GetSensorsN(); ++i) {
      
      int nstrip = p_datraw->GetStripsN(i);

      for(int ih = 0; ih < nstrip; ++ih) {
         TAMSDrawHit *strip = p_datraw->GetStrip(i, ih);
         
         Int_t sensorId   = strip->GetSensorId();
         Int_t stripId    = strip->GetStrip();
         Int_t view       = strip->GetView();
         UInt_t charge    = strip->GetCharge();
         Float_t posStrip = p_geoMap->GetPosition(stripId);
         
        if (FootDebugLevel(1))
            printf("sensor: %d strip: %d view: %d charge: %d\n", sensorId, stripId, view, charge);
         
         // here we need the calibration file
         Double_t energy = GetEnergy(charge, sensorId, stripId);
         TAMSDhit* hit = p_nturaw->NewStrip(sensorId, energy, view, stripId);
         hit->SetPosition(posStrip);
      }
   }
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
Double_t TAMSDactNtuHit::GetEnergy(Double_t rawenergy, Int_t sensorId, Int_t stripId)
{
   TAMSDparCal* p_parcal = (TAMSDparCal*) fpParCal->Object();
   
   Double_t p0 = p_parcal->GetElossParam(sensorId, stripId, 0);
   Double_t p1 = p_parcal->GetElossParam(sensorId, stripId, 1);
   
   return p0 + p1 * rawenergy;
   
}


//------------------------------------------+-----------------------------------
//! Histograms

void TAMSDactNtuHit::CreateHistogram(){
   
   DeleteHistogram();
   
   SetValidHistogram(kTRUE);
   
}
