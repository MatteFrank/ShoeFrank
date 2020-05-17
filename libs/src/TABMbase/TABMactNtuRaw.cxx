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
			   TAGparaDsc* p_geomap, 
			   TAGparaDsc* p_parcon)
  : TAGaction(name, "TABMactNtuRaw - NTuplize BM raw data"),
    fpNtuRaw(p_nturaw),
    fpDatRaw(p_datraw),
    fpGeoMap(p_geomap),
    fpParCon(p_parcon)
{
  if (FootDebugLevel(1))
   cout<<"TABMactNtuRaw::default constructor::Creating the Beam Monitor data Ntuplizer"<<endl;
  AddDataOut(p_nturaw, "TABMntuRaw");
  AddDataIn(p_datraw, "TABMdatRaw");
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
  fpHisDiscAccept=new TH1I( "BM_Raw_Accepted and discharged hits", "Number of hits accepted/discharged in the time cut; -1=discharged 1=accepted; Events", 3, -1, 2);
  AddHistogram(fpHisDiscAccept);   
  fpHisDiscTime = new TH1F( "BM_Raw_DischargedTime", "Discharged time measurements; Time_meas-T0-trigger-Timecut or negative Time_meas [ns]; numevent", 500, -500., 500.);
  AddHistogram(fpHisDiscTime);   

  SetValidHistogram(kTRUE);
}

Bool_t TABMactNtuRaw::Action()
{
  TABMdatRaw* p_datraw = (TABMdatRaw*) fpDatRaw->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuRaw->Object();
  TABMparGeo* p_pargeo = (TABMparGeo*) fpGeoMap->Object();
  TABMparCon* p_parcon = (TABMparCon*) fpParCon->Object();


  p_nturaw->SetupClones();
  p_nturaw->ResetEffPaoloni();
  
  Double_t i_time;
  p_nturaw->ClearCellOccupy();
  
  //provv
  //~ p_parmap->tdc2cell(channel)>=0 && ((((Double_t) measurement)/10.) - p_parcon->GetT0(p_parmap->tdc2cell(channel))-used_trigger)<p_parcon->GetHitTimecut()
  
  for (Int_t i = 0; i < p_datraw->GetHitsN(); i++) {//loop on p_datrawhit
    const TABMrawHit* hit = p_datraw->GetHit(i);
    
    //retrive hit parameters
    i_time = hit->GetTime()- p_parcon->GetT0(hit->GetView(),hit->GetPlane(),hit->GetCell()) - p_datraw->GetTrigtime();
    if(i_time<p_parcon->GetHitTimecut() && i_time>-40){//apply cut 
      //Temporary time cut set at -40; it should be few ns, but at the first GSI data taking there is a jitter of ~ tens of ns not measured 
      if(i_time<0){ 
        if(p_parcon->GetT0switch()!=3 || p_parcon->GetT0sigma()==0)
          i_time=0.;
        else if(p_parcon->GetT0sigma()>0)
          while(i_time<0)
            i_time=gRandom->Gaus(hit->GetTime()- p_parcon->GetT0(hit->GetView(),hit->GetPlane(),hit->GetCell()) - p_datraw->GetTrigtime(), p_parcon->GetT0sigma());
      }
      
      Double_t i_drift = p_parcon->FirstSTrel(i_time);
      if(FootDebugLevel(3))
        cout<<"TABMactNtuRaw::Action:: charging hit i_time="<<i_time<<"  i_drift="<<i_drift<<"  cell="<<hit->GetCell()<<"  view="<<hit->GetView()<<"  Plane="<<hit->GetPlane()<<"   hit->time="<<hit->GetTime()<<"  T0="<<p_parcon->GetT0(hit->GetView(),hit->GetPlane(),hit->GetCell())<<"  trigtime="<<p_datraw->GetTrigtime()<<endl;
      
      
      //create the hit (no selection of hit)
      TABMntuHit *mytmp = p_nturaw->NewHit(hit->GetView(), hit->GetPlane(), hit->GetCell(),hit->GetIdcell(), i_drift, i_time, p_parcon->ResoEval(i_drift));
      p_nturaw->AddCellOccupyHit(hit->GetIdcell());
      if (ValidHistogram()){
        fpHisCell->Fill(hit->GetCell());
        fpHisView->Fill(hit->GetView());
        fpHisPlane->Fill(hit->GetPlane());
        fpHisRdrift->Fill(i_drift);
        fpHisTdrift->Fill(i_time);
        fpHisDiscAccept->Fill(1);
      }
    }else{//hit discharged, it will not be saved in the TABMntuHit collection
      if (ValidHistogram()){
        fpHisDiscAccept->Fill(-1);  
        fpHisDiscTime->Fill( (i_time<-40) ? i_time-p_parcon->GetHitTimecut() : i_time);
      }
    }    
  }
  
  //print cell_occupy
  if(FootDebugLevel(3))
    p_nturaw->PrintCellOccupy();

  //Evaluate the raw hit efficiency
  Int_t pivot[8]={0};
  Int_t probe[8]={0};  
  Double_t efftot, effxview, effyview;
  p_nturaw->Efficiency_paoloni(pivot, probe,efftot, effxview, effyview);
  p_nturaw->SetEfficiency(efftot, effxview, effyview);
  if (ValidHistogram()){
    for(Int_t i=0;i<8;i++){
      fpHisProbe_paoloni->Fill(i,probe[i]);
      fpHisPivot_paoloni->Fill(i,pivot[i]);
    }
    if(p_nturaw->GetEffPaoloni()>=0)
      fpHisEval_paoloni->Fill(efftot);
    if(effxview>=0)
      fpHisEval_paoloni_Xview->Fill(effxview);
    if(effyview>=0)
      fpHisEval_paoloni_Yview->Fill(effyview);
  } 
  
  if(FootDebugLevel(1))
    cout<<"TABMacnNtuRaw::Action():: done  Number of hit added="<<p_nturaw->GetHitsN()<<";  overall efficiency="<<efftot<<";  xz plane efficiency="<<effxview<<";  yzplane"<<effyview<<endl;

  fpNtuRaw->SetBit(kValid);
  
  return kTRUE;
}





