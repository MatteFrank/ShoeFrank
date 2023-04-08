/*!
 \file TAMSDactNtuHit.cxx
 \brief   Implementation of TAMSDactNtuHit.
 */

#include "TAMSDparMap.hxx"
#include "TAMSDparCal.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDactNtuHit.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAMSDactNtuHit
 \brief Get MSD raw data from WD. **
 */

//! Class imp
ClassImp(TAMSDactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] p_datraw raw data input container descriptor
//! \param[out] p_nturaw hit output container descriptor
//! \param[in] p_pargeo geometry parameter descriptor
//! \param[in] p_parconf configuration parameter descriptor
//! \param[in] p_parcal calibration parameter descriptor
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
      
      for(int ih = 0; ih < nstrip; ++ih) {
         TAMSDrawHit *strip = p_datraw->GetStrip(i, ih);
         
         Int_t sensorId   = strip->GetSensorId();
         Int_t stripId    = strip->GetStrip();
         Int_t view       = strip->GetView();
         Double_t charge  = strip->GetCharge();
         Bool_t isSeed    = strip->IsSeed();
         Bool_t isNoisy   = strip->IsNoisy();
         Float_t posStrip = p_geoMap->GetPosition(stripId);
         if(FootDebugLevel(2))
            cout<<" Sens "<<sensorId<<" strip "<<stripId<<" View "<<view<<" position (cm) "<<posStrip<<" "<<endl;
	 
         if (FootDebugLevel(1))
            printf("sensor: %d strip: %d view: %d charge: %f\n", sensorId, stripId, view, charge);
         
         TAMSDhit* hit = p_nturaw->NewStrip(sensorId, charge, view, stripId);
         hit->SetPosition(posStrip);
         hit->SetSeed(isSeed);
         hit->SetNoisy(isNoisy);
      }
   }
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}


//------------------------------------------+-----------------------------------
//! Histograms
void TAMSDactNtuHit::CreateHistogram()
{
   DeleteHistogram();
   
   SetValidHistogram(kTRUE);
   
}
