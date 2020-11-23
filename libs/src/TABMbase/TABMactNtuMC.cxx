/*!
  \file
  \version $Id: TABMactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TABMactNtuMC.
*/

#include "TABMactNtuMC.hxx"

/*!
  \class TABMactNtuMC TABMactNtuMC.hxx "TABMactNtuMC.hxx"
  \brief NTuplizer for BM raw hits. **
*/


ClassImp(TABMactNtuMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuMC::TABMactNtuMC(const char* name,
			   TAGdataDsc* dscnturaw,
			   TAGparaDsc* dscbmcon,
			   TAGparaDsc* dscbmgeo,
			   EVENT_STRUCT* evStr)
  : TAGaction(name, "TABMactNtuMC - NTuplize BM raw data"),
    fpNtuMC(dscnturaw),
    fpParCon(dscbmcon),
    fpParGeo(dscbmgeo),
    fpEvtStr(evStr)
{
 if (FootDebugLevel(1))
   cout<<"TABMactNtuMC::default constructor::Creating the Beam Monitor MC tuplizer action"<<endl;
 AddPara(fpParCon, "TABMparConf");
 AddPara(fpParGeo, "TABMparGeo");
 AddDataOut(fpNtuMC, "TABMntuRaw");

 CreateDigitizer();

}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TABMactNtuMC::CreateDigitizer()
{
   TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuMC->Object();
   TABMparGeo* p_bmgeo  = (TABMparGeo*) fpParGeo->Object();
   TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();

   fDigitizer = new TABMdigitizer(p_nturaw, p_bmgeo, p_bmcon);
}


//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuMC::~TABMactNtuMC()
{
 delete fDigitizer;
}

//------------------------------------------+-----------------------------------
void TABMactNtuMC::CreateHistogram(){
  DeleteHistogram();

  fpHisCell = new TH1I( "bmMcHitCell", "cell index; index; Counter", 3, -0.5, 2.5);
  AddHistogram(fpHisCell);
  fpHisView = new TH1I( "bmMcHitView", "view index; index; Counter", 2, -0.5, 1.5);
  AddHistogram(fpHisView);
  fpHisPlane = new TH1I( "bmMcHitPlane", "plane index; index; Counter", 6, -0.5, 5.5);
  AddHistogram(fpHisPlane);
  fpDisRdrift = new TH1F( "bmMcHitDischargedRdrift", "Discharged hits according to BM efficiency; Rdrift [cm]; numevent", 100, 0., 1.);
  AddHistogram(fpDisRdrift);
  fpHisRdrift = new TH1F( "bmMcHitRdrift", "Rdrift; Rdrift [cm]; numevent", 100, 0., 1.);
  AddHistogram(fpHisRdrift);
  fpHisHitNum=new TH1I( "bmMcHiDistribution", "Number of ACCEPTED hits x event; Number of hits; Events", 31, -0.5, 30.5);
  AddHistogram(fpHisHitNum);
  fpHisFakeIndex=new TH1I( "bmMCHitFake", "Charged hits fake index; 0=Hit from primaries, 1=Other fluka hits, 2=Random hit not from fluka; Events", 3, -0.5, 2.5);
  AddHistogram(fpHisFakeIndex);

  SetValidHistogram(kTRUE);
}


//------------------------------------------+-----------------------------------
//! Action.

Bool_t TABMactNtuMC::Action()
{
  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  TABMntuRaw* p_nturaw  = (TABMntuRaw*) fpNtuMC->Object();
  TABMparConf* p_bmcon  = (TABMparConf*) fpParCon->Object();
  TABMparGeo* p_bmgeo   = (TABMparGeo*) fpParGeo->Object();

  Int_t cell, view, lay, ipoint, wireid;
  Double_t rdrift;

  TVector3 loc, gmom, mom,  glo;
  p_nturaw->SetupClones();
  p_nturaw->ResetEffPaoloni();
  p_nturaw->ClearCellOccupy();
  fDigitizer->ClearMap();

  if (FootDebugLevel(1))
    cout<<"TABMactNtuMC::Processing event number="<<gTAGroot->CurrentEventNumber()<<"    number of hits="<<fpEvtStr->BMNn<<endl;

  //loop for double hits and hits with energy less than enxcell_cut:
  for (Int_t i = 0; i < fpEvtStr->BMNn; ++i) {
    if(fpEvtStr->TRcha[fpEvtStr->BMNid[i]-1]!=0 && fpEvtStr->TRtrlen[fpEvtStr->BMNid[i]-1]>0.1){//selection criteria: no neutral particles, at least 0,1 mm of track lenght
      cell = fpEvtStr->BMNicell[i];
      lay = fpEvtStr->BMNilay[i];
      view = fpEvtStr->BMNiview[i]==-1 ? 1:0;
      ipoint = fpEvtStr->BMNid[i]-1;
      wireid = p_bmgeo->GetAbsWireId(view, lay, cell);

      glo.SetXYZ(fpEvtStr->BMNxin[i],fpEvtStr->BMNyin[i],fpEvtStr->BMNzin[i]);
      loc = geoTrafo->FromGlobalToBMLocal(glo);
      gmom.SetXYZ(fpEvtStr->BMNpxin[i],fpEvtStr->BMNpyin[i],fpEvtStr->BMNpzin[i]);
      Double_t edep = 0;
      Double_t zout = 0;
      Double_t time = 0;
      Int_t    Z    = 0;

      if(gmom.Mag()!=0){
        rdrift=p_bmgeo->FindRdrift(loc, gmom, p_bmgeo->GetWirePos(view, lay,p_bmgeo->GetSenseId(cell)),p_bmgeo->GetWireDir(view),false);
        Bool_t added=fDigitizer->Process(edep, loc[0], loc[1], loc[2], zout, time, wireid, Z,
                                         gmom[0], gmom[1], gmom[2]);
        TABMntuHit* hit = fDigitizer->GetCurrentHit();
        hit->SetIsFake((ipoint==0) ? 0 : (ipoint>0 ? 1 : 2));
        hit->AddMcTrackIdx(ipoint, i);
        
	      if(ValidHistogram() && !added)
						fpDisRdrift->Fill(rdrift);
      }
    }
	}

  if(p_bmcon->GetSmearHits())
	  CreateFakeHits();

  //histos
	if(ValidHistogram()){
	  fpHisHitNum->Fill(p_nturaw->GetHitsN());
		for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
			TABMntuHit* p_hit=p_nturaw->GetHit(i);
		  fpHisCell->Fill(p_hit->GetCell());
      fpHisView->Fill(p_hit->GetView());
      fpHisPlane->Fill(p_hit->GetPlane());
      fpHisRdrift->Fill(p_hit->GetRdrift());
      fpHisFakeIndex->Fill(p_hit->GetIsFake());
		}
	}

  fpNtuMC->SetBit(kValid);
  if(FootDebugLevel(2))
    cout<<"TABMactNtuMC::Action():: done without problems!"<<endl;

  return kTRUE;
}


//___________________________________________________________________________________________
void TABMactNtuMC::CreateFakeHits()
{
	TABMparGeo* p_bmgeo   = (TABMparGeo*) fpParGeo->Object();
  Int_t nfake=(Int_t)fabs(gRandom->Gaus(0,4));
  
  for(Int_t i=0;i<nfake;i++){
    Int_t cellid=(Int_t)gRandom->Uniform(0,35.9);
		Int_t cell, lay, view;
		p_bmgeo->GetBMNlvc(cellid,lay, view, cell);
    Int_t wireid = p_bmgeo->GetAbsWireId(view, lay, cell);

    Double_t edep = 0;
    Double_t zout = 0;
    Double_t time = 0;
    Int_t    Z    = 0;
    TVector3 loc(0,0,0);
    TVector3 gmom(0,0,0);
    
    Bool_t added=fDigitizer->Process(edep, loc[0], loc[1], loc[2], zout, time, wireid, Z,
                                       gmom[0], gmom[1], gmom[2]);
    
    TABMntuHit* hit = fDigitizer->GetCurrentHit();
    hit->AddMcTrackIdx(-99, -99);
	}

  return ;
}
