/*!
  \file
  \version $Id: TABMactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TABMactNtuHitMC.
*/

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TABMactNtuHitMC.hxx"

/*!
  \class TABMactNtuHitMC TABMactNtuHitMC.hxx "TABMactNtuHitMC.hxx"
  \brief NTuplizer for BM raw hits. **
*/


ClassImp(TABMactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuHitMC::TABMactNtuHitMC(const char* name,
                                 TAGdataDsc* dscntuMC,
                                 TAGdataDsc* dscntuEve,
                                 TAGdataDsc* dscnturaw,
                                 TAGparaDsc* dscbmcon,
                                 TAGparaDsc* dscbmgeo)
  : TAGaction(name, "TABMactNtuHitMC - NTuplize ToF raw data"),
    fpNtuMC(dscntuMC),
    fpNtuEve(dscntuEve),
    fpNtuRaw(dscnturaw),
    fpParCon(dscbmcon),
    fpParGeo(dscbmgeo)
{
   if (FootDebugLevel(1))
      cout<<"TABMactNtuHitMC::default constructor::Creating the Beam Monitor MC tuplizer action"<<endl;

   AddDataIn(fpNtuMC, "TAMCntuHit");
   AddDataIn(fpNtuEve, "TAMCntuEve");
   AddDataOut(fpNtuRaw, "TABMntuRaw");

   AddPara(fpParCon, "TABMparCon");
   AddPara(fpParGeo, "TABMparGeo");

   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuHitMC::~TABMactNtuHitMC()
{
  delete fDigitizer;
}

//------------------------------------------+-----------------------------------
void TABMactNtuHitMC::CreateDigitizer()
{
   TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuRaw->Object();
	 TABMparCon* p_bmcon  = (TABMparCon*) fpParCon->Object();

   fDigitizer = new TABMdigitizer(p_nturaw, p_bmcon);
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TABMactNtuHitMC::Action()
{
  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  TABMntuRaw* p_nturaw  = (TABMntuRaw*) fpNtuRaw->Object();
  TABMparCon* p_bmcon   = (TABMparCon*) fpParCon->Object();
  TABMparGeo* p_bmgeo   = (TABMparGeo*) fpParGeo->Object();
  TAMCntuHit* pNtuMC    = (TAMCntuHit*) fpNtuMC->Object();
  TAMCntuEve* pNtuEve   = (TAMCntuEve*) fpNtuEve->Object();

  Int_t cell, view, lay, ipoint, cellid;
  Double_t rdrift;

  TVector3 loc, gmom, mom,  glo;
  p_nturaw->SetupClones();
  p_nturaw->ResetEffPaoloni();
  p_nturaw->ClearCellOccupy();
  fDigitizer->ClearMap();

  if (FootDebugLevel(1))
    cout<<"TABMactNtuHitMC::Processing event number"<<gTAGroot->CurrentEventNumber()<<"    number of hits="<<pNtuMC->GetHitsN()<<endl;

  //loop for double hits and hits with energy less than enxcell_cut:
  for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
    TAMChit* hitMC = pNtuMC->GetHit(i);
    Int_t trackId  = hitMC->GetTrackIdx()-1;

    TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();
    TAMCeveTrack*  track = pNtuEve->GetTrack(trackId);

    if(track->GetCharge() != 0 && track->GetTrkLength() > 0.1){//selection criteria: no neutral particles, at least 0,1 mm
      cell = hitMC->GetCell();
      lay = hitMC->GetLayer();
      view = hitMC->GetView() == -1 ? 1:0;
      cellid=p_bmgeo->GetBMNcell(lay, view, cell);

      glo.SetXYZ(hitMC->GetInPosition()[0], hitMC->GetInPosition()[1], hitMC->GetInPosition()[2]);
      loc = geoTrafo->FromGlobalToBMLocal(glo);
      gmom.SetXYZ(hitMC->GetInMomentum()[0], hitMC->GetInMomentum()[1], hitMC->GetInMomentum()[2]);

      if(gmom.Mag()!=0){
        rdrift=p_bmgeo->FindRdrift(loc, gmom, p_bmgeo->GetWirePos(view, lay,p_bmgeo->GetSenseId(cell)),p_bmgeo->GetWireDir(view),false);
        Bool_t added=fDigitizer->Process(rdrift,cellid,lay,view,cell, i, trackId);
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


  fpNtuRaw->SetBit(kValid);
  if(FootDebugLevel(2))
    cout<<"TABMactNtuHitMC::Action():: done without problems!"<<endl;

  return kTRUE;
}

//------------------------------------------+-----------------------------------
void TABMactNtuHitMC::CreateHistogram(){
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
void TABMactNtuHitMC::CreateFakeHits()
{
  TABMparGeo* p_bmgeo   = (TABMparGeo*) fpParGeo->Object();
  Int_t nfake=(Int_t)fabs(gRandom->Gaus(0,4));
  for(Int_t i=0;i<nfake;i++){
    Int_t cellid=(Int_t)gRandom->Uniform(0,35.9);
    Int_t cell, lay, view;
    p_bmgeo->GetBMNlvc(cellid,lay, view, cell);
    fDigitizer->Process(gRandom->Uniform(0.,0.8),cellid,lay,view,cell ,-1,-1);
  }

  return ;
}
