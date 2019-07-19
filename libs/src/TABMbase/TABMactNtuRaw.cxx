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
			   TAGparaDsc* p_geomap, 
			   TAGparaDsc* p_parcon)
  : TAGaction(name, "TABMactNtuRaw - NTuplize BM raw data"),
    fpNtuRaw(p_nturaw),
    fpDatRaw(p_datraw),
    fpTimRaw(p_timraw),
    fpGeoMap(p_geomap),
    fpParCon(p_parcon)
{
  AddDataOut(p_nturaw, "TABMntuRaw");
  AddDataIn(p_datraw, "TABMdatRaw");
  AddDataIn(p_timraw, "TASTntuRaw");
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
   
   fpHisPivot_paoloni = new TH1I( "BM_Raw_eff_paoloni_pivot", "pivot counter for the plane efficiency method; index; Counter", 8, 0., 8.);
   AddHistogram(fpHisPivot_paoloni);   
   fpHisProbe_paoloni = new TH1I( "BM_Raw_eff_paoloni_probe", "probe counter for the plane efficiency method; index; Counter", 8, 0., 8.);
   AddHistogram(fpHisProbe_paoloni);   
   fpHisEval_paoloni = new TH1F( "BM_Raw_eff_paoloni_eval", "efficiency for the paoloni plane method; Efficiency; Events", 110, 0., 1.1);
   AddHistogram(fpHisEval_paoloni);   
   fpHisEval_paoloni_Xview = new TH1F( "BM_Raw_eff_paoloni_eval_xview", "efficiency for the paoloni plane method for x view; Efficiency; Events", 110, 0., 1.1);
   AddHistogram(fpHisEval_paoloni_Xview);   
   fpHisEval_paoloni_Yview = new TH1F( "BM_Raw_eff_paoloni_eval_yview", "efficiency for the paoloni plane method for y view; Efficiency; Events", 110, 0., 1.1);
   AddHistogram(fpHisEval_paoloni_Yview);   
   fpHisCell = new TH1I( "BM_Raw_hit_cell", "cell index; index; Counter", 3, 0., 3.);
   AddHistogram(fpHisCell);   
   fpHisView = new TH1I( "BM_Raw_hit_view", "view index; index; Counter", 2, 0., 2.);
   AddHistogram(fpHisView);   
   fpHisPlane = new TH1I( "BM_Raw_hit_plane", "plane index; index; Counter", 6, 0., 6.);
   AddHistogram(fpHisPlane);   
   fpHisRdrift = new TH1F( "BM_Raw_hit_rdrift", "Rdrift; Rdrift [cm]; numevent", 100, 0., 1.);
   AddHistogram(fpHisRdrift);   
   fpHisTdrift = new TH1F( "BM_Raw_hit_tdrift", "Tdrift; Tdrift [s]; numevent", 400, 0., 400.);
   AddHistogram(fpHisTdrift);   


   
   SetValidHistogram(kTRUE);
}

Bool_t TABMactNtuRaw::Action()
{
  TABMdatRaw* p_datraw = (TABMdatRaw*) fpDatRaw->Object();
  TASTntuRaw* p_timraw = (TASTntuRaw*) fpTimRaw->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuRaw->Object();
  TABMparGeo* p_pargeo = (TABMparGeo*) fpGeoMap->Object();
  TABMparCon* p_parcon = (TABMparCon*) fpParCon->Object();


  p_nturaw->SetupClones();
  p_nturaw->ResetEffPaoloni();
  
  if(p_timraw->GetTriggerTime() == -10000 || p_timraw->GetTriggerTime()<-9999999999) {
     Info("Action()","Trigger IR Time is Missing!!!");
     fpNtuRaw->SetBit(kValid);
     return kTRUE;
  }

  Double_t i_time;
  p_nturaw->ClearCellOccupy();
  
  for (Int_t i = 0; i < p_datraw->NHit(); i++) {//loop on p_datrawhit
    const TABMrawHit& hit = p_datraw->Hit(i);
    
    //retrive hit parameters
    i_time = hit.Time()- p_parcon->GetT0(hit.View(),hit.Plane(),hit.Cell()) - p_datraw->GetTrigtime();
    
    if(i_time<0){ 
      if(p_parcon->GetT0switch()!=2 && p_parcon->GetT0sigma()==0)
        i_time=0.;
      else if(p_parcon->GetT0sigma()>0)
        while(i_time<0)
          i_time=p_parcon->GetRand()->Gaus(hit.Time()- p_parcon->GetT0(hit.View(),hit.Plane(),hit.Cell()) - p_datraw->GetTrigtime(), p_parcon->GetT0sigma());  
    }
    
    Double_t i_drift = p_parcon->FirstSTrel(i_time);
    if(FootDebugLevel(3))
      cout<<"TABMactNtuRaw::Action:: charging hit i_time="<<i_time<<"  i_drift="<<i_drift<<"  cell="<<hit.Cell()<<"  view="<<hit.View()<<"  Plane="<<hit.Plane()<<"   hit.time="<<hit.Time()<<"  T0="<<p_parcon->GetT0(hit.View(),hit.Plane(),hit.Cell())<<"  trigtime="<<p_datraw->GetTrigtime()<<endl;
    
    
    //create the hit (no selection of hit)
    TABMntuHit *mytmp = p_nturaw->NewHit(hit.View(), hit.Plane(), hit.Cell(), i_drift, i_time, p_parcon->ResoEval(i_drift));
    p_nturaw->AddCellOccupyHit(hit.Idcell());
    if (ValidHistogram()){
      fpHisCell->Fill(hit.Cell()); 
      fpHisView->Fill(hit.View()); 
      fpHisPlane->Fill(hit.Plane()); 
      fpHisRdrift->Fill(i_drift);
      fpHisTdrift->Fill(i_time);
    }    
  }
  
  //print cell_occupy
  if(FootDebugLevel(3))
    p_nturaw->PrintCellOccupy();

  vector<Int_t>pivot (8,0);
  vector<Int_t>probe (8,0);  
  p_nturaw->Efficiency_paoloni(pivot, probe); 
  if (ValidHistogram()){
    for(Int_t i=0;i<8;i++){
      fpHisProbe_paoloni->Fill(i,probe.at(i));
      fpHisPivot_paoloni->Fill(i,pivot.at(i));
    }
    if(p_nturaw->GetEffPaoloni()>=0)
      fpHisEval_paoloni->Fill(p_nturaw->GetEffPaoloni());
    if(p_nturaw->GetEffPaolonixview()>=0)
      fpHisEval_paoloni_Xview->Fill(p_nturaw->GetEffPaolonixview());
    if(p_nturaw->GetEffPaoloniyview()>=0)
      fpHisEval_paoloni_Yview->Fill(p_nturaw->GetEffPaoloniyview());
  } 
  
  fpNtuRaw->SetBit(kValid);
  
  if(FootDebugLevel(2))
    cout<<"TABMacnNtuRaw::Action():: done"<<endl;
  return kTRUE;
}





