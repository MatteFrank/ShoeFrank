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

  //fill fDrawMap
  TABMparGeo* p_pargeo = (TABMparGeo*) fpGeoMap->Object();
	fDrawMap.clear();
	for(Int_t i=0;i<36;++i){
    Int_t cell, view, plane;
		p_pargeo->GetBMNlvc(i, plane, view, cell);
	  fDrawMap[i]=cell*26+plane*2+27+(((plane%2==0 && view==1) || (plane%2==1 && view==0)) ? -13:0);
	}

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuRaw::~TABMactNtuRaw()
{}


//------------------------------------------+-----------------------------------
//! Action.


void TABMactNtuRaw::CreateHistogram(){

  DeleteHistogram();

  fpHisPivot_paoloni = new TH1I( "bmRawEffPivot", "pivot counter for the plane efficiency method; index; Counter", 8, -0.5, 7.5);
  AddHistogram(fpHisPivot_paoloni);
  fpHisProbe_paoloni = new TH1I( "bmRawEffProbe", "probe counter for the plane efficiency method; index; Counter", 8, -0.5, 7.5);
  AddHistogram(fpHisProbe_paoloni);
  fpHisEval_paoloni = new TH1F( "bmRawEffEvalTot", "Overall hit detection efficiency ; Efficiency; Events", 110, 0., 1.1);
  AddHistogram(fpHisEval_paoloni);
  fpHisEval_paoloni_Xview = new TH1F( "bmRaweffEvalXview", "Hit detection efficiency on xz view; Efficiency; Events", 110, 0., 1.1);
  AddHistogram(fpHisEval_paoloni_Xview);
  fpHisEval_paoloni_Yview = new TH1F( "bmRaweffEvalYview", "Hit detection efficiency on yz view; Efficiency; Events", 110, 0., 1.1);
  AddHistogram(fpHisEval_paoloni_Yview);
  fpHisCell = new TH1I( "bmRawHitCell", "cell index; index; Counter", 3, -0.5, 2.5);
  AddHistogram(fpHisCell);
  fpHisView = new TH1I( "bmRawHitView", "view index; index; Counter", 2, -0.5, 1.5);
  AddHistogram(fpHisView);
  fpHisPlane = new TH1I( "bmRawHitPlane", "plane index; index; Counter", 6, -0.5, 5.5);
  AddHistogram(fpHisPlane);
	fpHisHitTotDist=new TH1I( "bmRawHiTotDist", "Total Number of accepted hits x event; Number of hits; Events", 31, -0.5, 30.5);
  AddHistogram(fpHisHitTotDist);
	fpHisHitXzviewDist=new TH1I( "bmRawHiXzviewDist", "Number of accepted hits on XZ view x event; Number of hits; Events", 21, -0.5, 20.5);
  AddHistogram(fpHisHitXzviewDist);
	fpHisHitYzviewDist=new TH1I( "bmRawHiYzviewDist", "Number of accepted hits on YZ view x event; Number of hits; Events", 21, -0.5, 20.5);
  AddHistogram(fpHisHitYzviewDist);
  fpHisRdrift = new TH1F( "bmRawHitRdrift", "Rdrift; Rdrift [cm]; numevent", 100, 0., 1.);
  AddHistogram(fpHisRdrift);
  fpHisTdrift = new TH1F( "bmRawHitTdrift", "Tdrift; Tdrift [s]; numevent", 400, 0., 400.);
  AddHistogram(fpHisTdrift);
  fpHisDiscAccept=new TH1I( "bmRawHitAccDisc", "Number of hits accepted/discharged in the time cut; -1=discharged 1=accepted; Events", 3, -1.5, 1.5);
  AddHistogram(fpHisDiscAccept);
  fpHisDiscTime = new TH1F( "bmRawHitDiscTime", "Discharged time measurements; Time_meas-T0-trigger-Timecut or negative Time_meas [ns]; numevent", 500, -1000., 1000.);
  AddHistogram(fpHisDiscTime);

	fpHisMapX=new TH2I( "bmRawCellRawOccupancyX", "Cell occupancy for raw hits; z; x", 11, -5.5, 5.5,7, -2.8,2.8);
	AddHistogram(fpHisMapX);
	fpHisMapY=new TH2I( "bmRawCellRawOccupancyY", "Cell occupancy for raw hits; z; y", 11, -5.5, 5.5,7, -2.8,2.8);
	AddHistogram(fpHisMapY);

  //Need setbincontent to 1 in order to use addbincontent later
	for(Int_t i=0;i<36;++i){
		if((i/3)%2==0){
			fpHisMapY->SetBinContent(fDrawMap[i],i);
			fpHisMapY->SetBinContent(fDrawMap[i]+13,i);
		}else{
			fpHisMapX->SetBinContent(fDrawMap[i],i);
			fpHisMapX->SetBinContent(fDrawMap[i]+13,i);
		}
  }

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

  for (Int_t i = 0; i < p_datraw->GetHitsN(); i++) {//loop on p_datrawhit
    const TABMrawHit* hit = p_datraw->GetHit(i);

		if (ValidHistogram()){
			if(hit->GetView()==0){
				fpHisMapY->AddBinContent(fDrawMap[hit->GetIdCell()],1);
				fpHisMapY->AddBinContent(fDrawMap[hit->GetIdCell()]+13,1);
			}else{
				fpHisMapX->AddBinContent(fDrawMap[hit->GetIdCell()],1);
				fpHisMapX->AddBinContent(fDrawMap[hit->GetIdCell()]+13,1);
			}
	  }

    //retrive hit parameters
    i_time = hit->GetTime()- p_parcon->GetT0(hit->GetView(),hit->GetPlane(),hit->GetCell()) - p_datraw->GetTrigtime();
    if(i_time<p_parcon->GetHitTimeCut() && i_time>-40){//apply cut
      //Temporary time cut set at -40; it should be few ns, but at the first GSI data taking there is a jitter of ~ tens of ns not measured
      if(i_time<0)
          i_time=0.;

      Double_t i_drift = p_parcon->STrelEval(i_time);
      if(FootDebugLevel(3))
        cout<<"TABMactNtuRaw::Action:: charging hit i_time="<<i_time<<"  i_drift="<<i_drift<<"  cell="<<hit->GetCell()<<"  view="<<hit->GetView()<<"  Plane="<<hit->GetPlane()<<"   hit->time="<<hit->GetTime()<<"  T0="<<p_parcon->GetT0(hit->GetView(),hit->GetPlane(),hit->GetCell())<<"  trigtime="<<p_datraw->GetTrigtime()<<endl;


      //create the hit (no selection of hit)
      TABMntuHit *mytmp = p_nturaw->NewHit(hit->GetIdCell(), hit->GetPlane(), hit->GetView(), hit->GetCell(), i_drift, i_time, p_parcon->ResoEval(i_drift));
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
        fpHisDiscTime->Fill( (i_time<-40) ? i_time-p_parcon->GetHitTimeCut() : i_time);
      }
    }
  }

  //print cell_occupy
  if(FootDebugLevel(3))
    p_nturaw->PrintCellOccupy();

  //Evaluate the raw hit efficiency and fill histograms
  Int_t pivot[8]={0};
  Int_t probe[8]={0};
  p_nturaw->Efficiency_paoloni(pivot, probe);
  if (ValidHistogram()){
    for(Int_t i=0;i<8;i++){
      fpHisProbe_paoloni->Fill(i,probe[i]);
      fpHisPivot_paoloni->Fill(i,pivot[i]);
    }
    if(p_nturaw->GetEffPaoloni()>=0)
      fpHisEval_paoloni->Fill(p_nturaw->GetEffPaoloni());
    if(p_nturaw->GetEffPaolonixview()>=0)
      fpHisEval_paoloni_Xview->Fill(p_nturaw->GetEffPaolonixview());
    if(p_nturaw->GetEffPaoloniyview()>=0)
      fpHisEval_paoloni_Yview->Fill(p_nturaw->GetEffPaoloniyview());
		fpHisHitTotDist->Fill(p_nturaw->GetHitsN());
		fpHisHitXzviewDist->Fill(p_nturaw->GetNtothitx());
		fpHisHitYzviewDist->Fill(p_nturaw->GetNtothity());
  }

  if(FootDebugLevel(1))
    cout<<"TABMacnNtuRaw::Action():: done  Number of hit added="<<p_nturaw->GetHitsN()<<";  overall efficiency="<<p_nturaw->GetEffPaoloni()<<";  xz plane efficiency="<<p_nturaw->GetEffPaolonixview()<<";  yzplane"<<p_nturaw->GetEffPaoloniyview()<<endl;

  fpNtuRaw->SetBit(kValid);

  return kTRUE;
}
