/*!
  \file
  \version $Id: TABMactNtuRaw.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TABMactNtuRaw.
*/

#include "math.h"
#include "TABMactNtuRaw.hxx"

/*!
  \class TABMactNtuRaw TABMactNtuRaw.hxx "TABMactNtuRaw.hxx"
  \brief NTuplizer for BM raw hits. **
*/

ClassImp(TABMactNtuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuRaw::TABMactNtuRaw(const char* name,
			   TAGdataDsc* p_nturaw, 
			   TAGdataDsc* p_datraw, 
			   TAGdataDsc* p_timraw, 
			   //~ TAGdataDsc* p_triraw, 
			   TAGparaDsc* p_geomap, 
			   TAGparaDsc* p_parcon)
  : TAGaction(name, "TABMactNtuRaw - NTuplize BM raw data"),
    fpNtuRaw(p_nturaw),
    fpDatRaw(p_datraw),
    fpTimRaw(p_timraw),
    //~ fpTriRaw(p_triraw),
    fpGeoMap(p_geomap),
    fpParCon(p_parcon)
{
  AddDataOut(p_nturaw, "TABMntuRaw");
  AddDataIn(p_datraw, "TABMdatRaw");
  AddDataIn(p_timraw, "TASTdatRaw");
  //~ AddDataIn(p_triraw, "TASTdatRaw");
  AddPara(p_geomap, "TABMparGeo");
  AddPara(p_parcon, "TABMparCon");

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuRaw::~TABMactNtuRaw()
{}


//------------------------------------------+-----------------------------------
//! Action.


void TABMactNtuRaw::CreateHistogram(){
   
   DeleteHistogram();
   
   fpNhitXEvent = new TH1F("bm_nturaw_Nhits_xevent", "BM Number of hits x event", 36, 0, 36);
   AddHistogram(fpNhitXEvent);

   SetValidHistogram(kTRUE);
}

Bool_t TABMactNtuRaw::Action()
{
  TABMdatRaw* p_datraw = (TABMdatRaw*) fpDatRaw->Object();
  TASTdatRaw* p_timraw = (TASTdatRaw*) fpTimRaw->Object();
  //~ TASTdatRaw* p_triraw = (TASTdatRaw*) fpTriRaw->Object();//è l'equivalente del BM...
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuRaw->Object();
  TABMparGeo* p_pargeo = (TABMparGeo*) fpGeoMap->Object();
  TABMparCon* p_parcon = (TABMparCon*) fpParCon->Object();


  p_nturaw->SetupClones();
  
  if(p_timraw->TrigTime() == -10000) {
     Info("Action()","Trigger IR Time is Missing!!!");
     fpNtuRaw->SetBit(kValid);
     return kTRUE;
  }

  Double_t i_time; 
  for (Int_t i = 0; i < p_datraw->NHit(); i++) {//loop on p_datrawhit
    const TABMrawHit& hit = p_datraw->Hit(i);
    
    //retrive hit parameters
    //~ Double_t t0_corr = (p_parcon->GetT0(hit.View(),hit.Plane(),hit.Cell()) > -10000) ? p_parcon->GetT0(hit.View(),hit.Plane(),hit.Cell()) : 0.; //to avoid not settled T0
    i_time = hit.Time()- p_parcon->GetT0(hit.View(),hit.Plane(),hit.Cell()) -p_timraw->TrigTime();
    
    if(p_parcon->GetT0switch()<2 && i_time<0)
      i_time=0.;
    
    Double_t i_drift = p_parcon->FirstSTrel(i_time);

    //create the hit (no selection of hit)
    TABMntuHit *mytmp = p_nturaw->NewHit(0, hit.View(), hit.Plane(), hit.Cell(), i_drift, i_time, p_parcon->ResoEval(i_drift));
  }
  fpNhitXEvent->Fill(p_datraw->NHit());

  fpNtuRaw->SetBit(kValid);
  return kTRUE;


  
}

