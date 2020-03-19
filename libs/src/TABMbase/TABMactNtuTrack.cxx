/*!
  \file
  \version $Id: TABMactNtuTrack.cxx,v 1.12 2003/07/08 18:58:35 mueller Exp $
  \brief   Implementation of TABMactNtuTrack.
*/

#include "TABMactNtuTrack.hxx"

using namespace std;
/*!
  \class TABMactNtuTrack TABMactNtuTrack.hxx "TABMactNtuTrack.hxx"
  \brief Track builder for Beam Monitor. **
*/

ClassImp(TABMactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuTrack::TABMactNtuTrack(const char* name,
                                 TAGdataDsc* dscntutrk,
                                 TAGdataDsc* dscnturaw,
                                 TAGparaDsc* dscbmgeo,
                                 TAGparaDsc* dscbmcon,
                                 TAGparaDsc* dsctggeo)
  : TAGaction(name, "TABMactNtuTrack - Track finder for BM"),
    fpNtuTrk(dscntutrk),
    fpNtuHit(dscnturaw),
    fpBMGeo(dscbmgeo),
    fpBMCon(dscbmcon),
    fpTGgeo(dsctggeo)
{
  if (FootDebugLevel(1))
     cout<<"TABMactNtuTrack::default constructor::Creating the Beam Monitor Track ntuplizer"<<endl;
   
  AddDataOut(fpNtuTrk, "TABMntuTrack");
  AddDataIn(fpNtuHit,  "TABMntuRaw");
  AddPara(fpBMGeo,  "TABMparGeo");
  AddPara(fpBMCon,  "TABMparCon");

  TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpBMGeo->Object();
  TAGparGeo*  p_tggeo = (TAGparGeo*)  fpTGgeo->Object();

  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  fTmpTrackTr=new TABMntuTrackTr();

  //geometry parameters
  TVector3 tgCenter = geoTrafo->GetTGCenter();
  TVector3 bmCenter = geoTrafo->GetBMCenter();
  Float_t tgThick = p_tggeo->GetTargetPar().Size[2];
  TVector3 targetPos(tgCenter[0], tgCenter[1], -bmCenter[2]+tgCenter[2]+tgThick/2.);
  p_bmgeo->SetTarget(targetPos);

  //new chi2 ROOT based
  fFunctorx= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2Xview,2);
  fMinx = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");
  // fMinx->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  // fMinx->SetMaxIterations(p_bmcon->GetNumIte());
  // fMinx->SetTolerance(p_bmcon->GetParMove());
  // fMinx->SetPrintLevel(0);
  // fMinx->SetFunction(*fFunctorx);
  // fMinx->SetVariable(0,"mx",0., p_bmcon->GetParMove());
  // fMinx->SetVariable(1,"qx",0., p_bmcon->GetParMove());

  fFunctory= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2Yview,2);
  fMiny = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");
  // fMiny->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  // fMiny->SetMaxIterations(p_bmcon->GetNumIte());
  // fMiny->SetTolerance(p_bmcon->GetParMove());
  // fMiny->SetPrintLevel(0);
  // fMiny->SetFunction(*fFunctory);
  // fMiny->SetVariable(0,"my",0., p_bmcon->GetParMove());
  // fMiny->SetVariable(1,"qy",0., p_bmcon->GetParMove());

  fFunctorGSIx= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2GSIx,3);
  fFunctorGSIy= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2GSIy,3);
  //legendre
  fLegpolxsum=new TH2D("fLegpolxsum","Legendre polynomial space on xz plane; mx; q[cm]",p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(), p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());
  fLegpolysum=new TH2D("fLegpolysum","Legendre polynomial space on yz plane; my; q[cm]",p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(),  p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuTrack::~TABMactNtuTrack(){
delete fTmpTrackTr;
delete fMinx;
delete fMiny;
delete fFunctorx;
delete fFunctory;
delete fFunctorGSIx;
delete fFunctorGSIy;
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TABMactNtuTrack::CreateHistogram()
{
   DeleteHistogram();

   fpResTot = new TH2F("BM_Track_bm_residual_tot","Residual vs Rdrift; Residual [cm]; Measured rdrift [cm]", 6000, -0.3, 0.3,250 , 0., 1.);
   AddHistogram(fpResTot);
   fpHisMap = new TH2F("BM_Track_bmMap","BM - Position of the track at target center", 250, -3., 3.,250 , -3, 3);
   AddHistogram(fpHisMap);
   fpHisMylar12d = new TH2F("BM_Track_bmmylar1_2d","BM - Position of the track on the BM center plane", 500, -3., 3.,500 , -3., 3.);
   AddHistogram(fpHisMylar12d);
   fpHisMylar22d = new TH2F("BM_Track_bmmylar2_2d","BM - Position of the track on the BM center plane", 500, -3., 3.,500 , -3., 3.);
   AddHistogram(fpHisMylar22d);
   fpHisTrackStatus = new TH1I("BM_Track_track_status","Track status; -2=maxhitcut -1=minhitcut 0=ok 1/2=firedplane 3=hitrejected 4=noconv 5=chi2cut 6=planehitcut 7+=Error; Events", 20, -5, 15);
   AddHistogram(fpHisTrackStatus);
   fpHisNhitTrack = new TH1I("BM_Trak_nhitsxtrack","Number of hits x track; N hits; Events", 30, 0, 30);
   AddHistogram(fpHisNhitTrack);
   fpHisNrejhitTrack = new TH1I("BM_Trak_nhitsrejectedxtrack","Number of hits rejected x track; N hits; Events", 30, 0, 30);
   AddHistogram(fpHisNrejhitTrack);
   fpHisTrackFakeHit = new TH1I("BM_Trak_fakehits","Selected hits vs fake hits; -1=Lost primary hit 0=Selected primary hit 1=Wrong hit selected; Events", 8, -2, 2);
   AddHistogram(fpHisTrackFakeHit);
   fpHisChi2Red = new TH1F("BM_Track_chi2red","chi2red", 1000, 0., 100.);
   AddHistogram(fpHisChi2Red);
   fpHisTrackEff = new TH1F("BM_Track_FitEff","Efficiency with pivot-probe method on FITTED tracks", 110, 0., 1.1);
   AddHistogram(fpHisTrackEff);
   fpHisTrigTime = new TH1I("BM_Track_FitTrigTime","Fitted trigger time shift; [ns]", 40, -20, 20);
   AddHistogram(fpHisTrigTime);

   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuTrack::Action()
{
  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  TABMparCon*   p_bmcon  = (TABMparCon*)   fpBMCon->Object();
  TABMparGeo*   p_bmgeo  = (TABMparGeo*)   fpBMGeo->Object();
  TABMntuRaw*   p_nturaw = (TABMntuRaw*)   fpNtuHit->Object();

  p_ntutrk->Clear();
  Int_t i_nhit = p_nturaw->GetHitsN();
  Int_t firedx=0, firedy=0, tmp_int;
  //NB.: If the preselection reject the event no track will be saved

  if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack::Action::start"<<endl;

  //check the number of hits for each view
  for(Int_t i_h = 0; i_h < i_nhit; i_h++) {
    TABMntuHit* fpHit = p_nturaw->GetHit(i_h);
    if(FootDebugLevel(3))
      cout<<"hit="<<i_h<<" plane="<<fpHit->Plane()<<"  view="<<fpHit->View()<<"  cell="<<fpHit->Cell()<<"  wireplane="<<p_bmgeo->GetWirePlane(fpHit->Plane(),fpHit->View())<<"  rdrift="<<fpHit->Dist()<<"  isfake="<<fpHit->GetIsFake()<<endl;
    if(fpHit->View()==1)
      ++firedx;
    else
      ++firedy;
  }

  //Apply cuts on the number of hits
  if(firedy<p_bmcon->GetPlanehitcut() || firedx<p_bmcon->GetPlanehitcut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!: firedy="<<firedy<<"  firedx="<<firedx<<"   planehitcut="<<p_bmcon->GetPlanehitcut()<<endl;
    if(firedy<p_bmcon->GetPlanehitcut())
      p_ntutrk->GetTrackStatus()=1;
    else
      p_ntutrk->GetTrackStatus()=2;
    if(ValidHistogram())
      fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());
    fpNtuTrk->SetBit(kValid);
    return kTRUE;
  }else if(i_nhit>=p_bmcon->GetMaxnhit_cut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!:the number of hits is too high:  number of hit="<<i_nhit<<"  Maxhitcut="<<p_bmcon->GetMaxnhit_cut()<<endl;
    p_ntutrk->GetTrackStatus()=-2;
    if(ValidHistogram())
      fpHisTrackStatus->Fill(-2);
    fpNtuTrk->SetBit(kValid);
    return kTRUE;
  }else if(i_nhit<=p_bmcon->GetMinnhit_cut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!:the number of hits is too low:  number of hit="<<i_nhit<<"  Minhitcut="<<p_bmcon->GetMinnhit_cut()<<endl;
    p_ntutrk->GetTrackStatus()=-1;
    if(ValidHistogram())
      fpHisTrackStatus->Fill(-1);
    fpNtuTrk->SetBit(kValid);
    return kTRUE;
  }else
    p_ntutrk->GetTrackStatus()=-1000;

  //tracking
  if(p_bmcon->GetFitterIndex()!=0){
    ChargeLegendrePoly();
    p_nturaw->ClearCellOccupy();//in order to evaluate the fitted efficiency

    //find the highest peak in legendre space for x/y view and check the number of associated hits
    tmp_int=FindLegendreBestValues();
    CheckAssHits((firedx<fLegpolxsum->GetMaximum() && tmp_int%100==0) ? firedx : fLegpolxsum->GetMaximum(), (firedy<fLegpolysum->GetMaximum() && tmp_int/100==0) ? firedy : fLegpolysum->GetMaximum()); //check the associated hits

    //check for cuts
    if((i_nhit-p_nturaw->GetNselhitx()-p_nturaw->GetNselhity())>p_bmcon->GetRejmaxcut()){
      if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack::Number of hits rejected exceeds the max rejected hits selection criteria:  max_rejhit="<<p_bmcon->GetRejmaxcut()<<"  i_nhit="<<i_nhit<<"  number of hits on xz plane="<<p_nturaw->GetNselhitx()<<"  hit on yz plane="<<p_nturaw->GetNselhity()<<endl<<"No tracks will be reconstructed from the BM"<<endl;
      p_ntutrk->GetTrackStatus()=3;
      if(ValidHistogram())
        fpHisTrackStatus->Fill(3);
      fpNtuTrk->SetBit(kValid);
      return kTRUE;
    }else if(p_nturaw->GetNselhity()<p_bmcon->GetPlanehitcut() || p_nturaw->GetNselhitx()<p_bmcon->GetPlanehitcut()){
      if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack::The number of hits on one view is less than the minimum required:  min hit x view="<<p_bmcon->GetPlanehitcut()<<"  number of hits on xz plane="<<p_nturaw->GetNselhitx()<<"  number of hits on yz plane="<<p_nturaw->GetNselhity()<<endl<<"No tracks will be reconstructed from the BM"<<endl;
      p_ntutrk->GetTrackStatus()=6;
      if(ValidHistogram())
        fpHisTrackStatus->Fill(6);
      fpNtuTrk->SetBit(kValid);
      return kTRUE;
    }else{
       //ok we can reconstruct the particle track using the minimization of Chi2
      fTmpTrackTr->Clean();
      fTmpTrackTr->SetNhit(p_nturaw->GetNselhits());
      TVector3 tmp_tvector3(fLegpolxsum->GetXaxis()->GetBinCenter(fBestMxbin),fLegpolysum->GetXaxis()->GetBinCenter(fBestMybin),1.);
      fTmpTrackTr->SetPvers(tmp_tvector3);
      fTmpTrackTr->SetR0(fLegpolxsum->GetXaxis()->GetBinCenter(fBestRxbin),fLegpolysum->GetXaxis()->GetBinCenter(fBestRybin),0.);
      // tmp_int=NumericalMinimizationGSI();
      tmp_int=NumericalMinimizationDouble();
      if(tmp_int!=0){
        p_ntutrk->GetTrackStatus()=7+tmp_int;
        if(ValidHistogram())
          fpHisTrackStatus->Fill(7+tmp_int);
        fpNtuTrk->SetBit(kValid);
        return kTRUE;
      }else if(ComputeDataAll()){
        p_ntutrk->GetTrackStatus()=7;//something is wrong, this should not happen!
        if(ValidHistogram())
          fpHisTrackStatus->Fill(7);
        fpNtuTrk->SetBit(kValid);
        return kTRUE;
      }
    }
  }//end of tracking

  //evaluate the efficiency with the same Paoloni's method adopted in TABMactNtuRaw on the Fitted track hits instead of raw hits
  if(fTmpTrackTr->GetChi2Red()<=p_bmcon->GetChi2Redcut()){//use only the track with chi2<chi2cut
    Int_t pivot[8]={0};
    Int_t probe[8]={0};
    Double_t efftot, effxview, effyview;
    p_nturaw->Efficiency_paoloni(pivot, probe,efftot, effxview, effyview);
    fTmpTrackTr->SetEffFittedPlane(efftot);
  }

  //add the reconstructed track
  TABMntuTrackTr* trk = new((*(p_ntutrk->GetListOfTracks()))[p_ntutrk->GetTracksN()]) TABMntuTrackTr(*fTmpTrackTr);
  p_ntutrk->GetTrackStatus()= (fTmpTrackTr->GetChi2Red()>=p_bmcon->GetChi2Redcut())? 5:0;

  //fill histos
  if (ValidHistogram()){
    TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    Float_t posZ = geoTrafo->FromGlobalToBMLocal(TVector3(0,0,0)).Z();
    TVector3 pos = trk->PointAtLocalZ(posZ);
    fpHisMap->Fill(pos[0], pos[1]);
    pos=trk->PointAtLocalZ(p_bmgeo->GetMylar1().Z());
    fpHisMylar12d->Fill(pos[0], pos[1]);
    pos=trk->PointAtLocalZ(p_bmgeo->GetMylar2().Z());
    fpHisMylar22d->Fill(pos[0], pos[1]);
    fpHisChi2Red->Fill(trk->GetChi2Red());
    fpHisNhitTrack->Fill(trk->GetNhit());
    fpHisNrejhitTrack->Fill(p_nturaw->GetHitsN()-trk->GetNhit());
    fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());
    if(trk->GetEffFittedPlane()>=0)
      fpHisTrackEff->Fill(trk->GetEffFittedPlane());
  }


  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:end of tracking"<<endl;

  fpNtuTrk->SetBit(kValid);
  return kTRUE;
}



void TABMactNtuTrack::ChargeLegendrePoly()
{
  TABMparCon* p_bmcon  = (TABMparCon*) fpBMCon->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: start charge legendre pol graphs"<<endl;
  fLegpolxsum->Reset("ICESM");
  fLegpolxsum->SetBins(p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(), p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());
  fLegpolysum->Reset("ICESM");
  fLegpolysum->SetBins(p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(), p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());

  Double_t tmp_double;
  for(Int_t i_h = 0; i_h < p_nturaw->GetHitsN(); ++i_h) {
    fpHit = p_nturaw->GetHit(i_h);
    if(fpHit->View()==1){ //on xz view
      for(Int_t k=1;k<fLegpolxsum->GetXaxis()->GetNbins();++k){
        tmp_double=fLegpolxsum->GetXaxis()->GetBinCenter(k);
        fLegpolxsum->Fill(tmp_double, fpHit->GetA0().X()-tmp_double*fpHit->GetA0().Z()+fpHit->Dist()*sqrt(tmp_double*tmp_double+1.));
        fLegpolxsum->Fill(tmp_double, fpHit->GetA0().X()-tmp_double*fpHit->GetA0().Z()-fpHit->Dist()*sqrt(tmp_double*tmp_double+1.));
      }
    }else{//on yz view
      for(Int_t k=1;k<fLegpolysum->GetXaxis()->GetNbins();++k){
        tmp_double=fLegpolysum->GetXaxis()->GetBinCenter(k);
        fLegpolysum->Fill(tmp_double, fpHit->GetA0().Y()-tmp_double*fpHit->GetA0().Z()+fpHit->Dist()*sqrt(tmp_double*tmp_double+1.));
        fLegpolysum->Fill(tmp_double, fpHit->GetA0().Y()-tmp_double*fpHit->GetA0().Z()-fpHit->Dist()*sqrt(tmp_double*tmp_double+1.));
      }
    }
  }

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::fLegpolysum and fLegpolxsum done"<<endl;

  //provv: adopted to save the legpolx/ysum th2d
  //~ TString tmp_tstring="fLegpolxsum_";
  //~ TString tmp_tstringy="fLegpolysum_";
  //~ Int_t tmp_int=gRandom->Uniform(0,100000);
  //~ tmp_tstring+=tmp_int;
  //~ tmp_tstringy+=tmp_int;
  //~ tmp_tstring+="x.root";
  //~ tmp_tstringy+="y.root";
  //~ fLegpolxsum->SaveAs(tmp_tstring.Data());
  //~ fLegpolysum->SaveAs(tmp_tstringy.Data());

return;
}


Int_t TABMactNtuTrack::FindLegendreBestValues(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: start"<<endl;

  Int_t first_maxbin,last_maxbin;
  //X values
  fBestMxbin=-1;
  Int_t rebinmx=0;
  do{
    first_maxbin=fLegpolxsum->FindFirstBinAbove(fLegpolxsum->GetMaximum()-1);
    last_maxbin=fLegpolxsum->FindLastBinAbove(fLegpolxsum->GetMaximum()-1);
    if(first_maxbin==last_maxbin){
      fBestMxbin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      fBestMxbin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      fLegpolxsum->RebinX();
      rebinmx++;
    }
  }while(fBestMxbin==-1);

  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: check on mx done, now I'll check on qx  fBestMxbin="<<fBestMxbin<<endl;

  //check on r
  fBestRxbin=-1;
  Int_t rebinrx=0;
  do{
    first_maxbin=fLegpolxsum->FindFirstBinAbove(fLegpolxsum->GetMaximum()-1,2);
    last_maxbin=fLegpolxsum->FindLastBinAbove(fLegpolxsum->GetMaximum()-1,2);
    if(first_maxbin==last_maxbin){
      fBestRxbin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      fBestRxbin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      fLegpolxsum->RebinY();
      rebinrx++;
    }
  }while(fBestRxbin==-1);

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: X done; check on r done  fBestRxbin="<<fBestRxbin<<"   bincontent="<<fLegpolxsum->GetBinContent(fBestMxbin,fBestRxbin)<<"  best m="<<fLegpolxsum->GetXaxis()->GetBinCenter(fBestMxbin)<<"  best q="<<fLegpolxsum->GetYaxis()->GetBinCenter(fBestRxbin)<<"   rebinmx="<<rebinmx<<"  rebinrx="<<rebinrx<<endl;

  // Y values
  fBestMybin=-1;
  Int_t rebinmy=0;
  do{
    first_maxbin=fLegpolysum->FindFirstBinAbove(fLegpolysum->GetMaximum()-1);
    last_maxbin=fLegpolysum->FindLastBinAbove(fLegpolysum->GetMaximum()-1);
    if(first_maxbin==last_maxbin){
      fBestMybin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      fBestMybin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      fLegpolysum->RebinX();
      rebinmy++;
    }
  }while(fBestMybin==-1);

  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: check on mx done, now I'll check on qx  fBestMybin="<<fBestMybin<<endl;

  //check on r
  fBestRybin=-1;
  Int_t rebinry=0;
  do{
    first_maxbin=fLegpolysum->FindFirstBinAbove(fLegpolysum->GetMaximum()-1,2);
    last_maxbin=fLegpolysum->FindLastBinAbove(fLegpolysum->GetMaximum()-1,2);
    if(first_maxbin==last_maxbin){
      fBestRybin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      fBestRybin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      fLegpolysum->RebinY();
      rebinry++;
    }
  }while(fBestRybin==-1);

  if(FootDebugLevel(2))
      cout<<"TABMactNtuTrack::FindLegendreBestValues: Y done; check on r done  fBestRybin="<<fBestRybin<<"   bincontent="<<fLegpolysum->GetBinContent(fBestMybin,fBestRybin)<<"  best m="<<fLegpolysum->GetXaxis()->GetBinCenter(fBestMybin)<<"  best q="<<fLegpolysum->GetYaxis()->GetBinCenter(fBestRybin)<<"   rebinmy="<<rebinmy<<"  rebinry="<<rebinry<<endl;

return rebinmx+rebinrx + 100*(rebinmy+rebinry);
}


//for Legendre polynomy tracking, set the associated hits
void TABMactNtuTrack::CheckAssHits(const Int_t asshitx, const Int_t asshity)
{
  TABMparCon* p_bmcon  = (TABMparCon*) fpBMCon->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  if(FootDebugLevel(2)){
    cout<<"TABMactNtuTrack::CheckAssHits: start"<<endl;
    TVector3 legPvers(fLegpolxsum->GetXaxis()->GetBinCenter(fBestMxbin),fLegpolysum->GetXaxis()->GetBinCenter(fBestMybin),1.);
    legPvers.SetMag(1.);
    cout<<"legPvers=("<<legPvers.X()<<", "<<legPvers.Y()<<", "<<legPvers.Z()<<")"<<" binwidthX="<<fLegpolxsum->GetXaxis()->GetBinWidth(fBestMxbin)<<"  binwidthY"<<fLegpolysum->GetXaxis()->GetBinWidth(fBestMybin)<<endl;
    cout<<"legR0="<<fLegpolxsum->GetYaxis()->GetBinCenter(fBestRxbin)<<", "<<fLegpolysum->GetYaxis()->GetBinCenter(fBestRybin)<<"  binwidthX="<<fLegpolxsum->GetYaxis()->GetBinWidth(fBestRxbin)<<"   binwidthY="<<fLegpolysum->GetYaxis()->GetBinWidth(fBestRybin)<<endl;
  }

  Int_t wireplane[12]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  Int_t selx=0, sely=0, cellplane;
  Double_t xvalue, yvalue, res, diff, tmp_double;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i) {
    fpHit = p_nturaw->GetHit(i);
    if(fpHit->View()==0){//yz plane
      xvalue=fLegpolysum->GetXaxis()->GetBinCenter(fBestMybin);
      yvalue=fpHit->GetA0().Y()-xvalue*fpHit->GetA0().Z()+fpHit->Dist()*sqrt(xvalue*xvalue+1.);
      cellplane=p_bmgeo->GetWirePlane(fpHit->GetCellid());
      res=(fpHit->GetSigma()>fLegpolysum->GetYaxis()->GetBinWidth(fBestRybin)) ? fpHit->GetSigma() : fLegpolysum->GetYaxis()->GetBinWidth(fBestRybin);
      // res=res*3.;
      diff=fabs(yvalue - fLegpolysum->GetYaxis()->GetBinCenter(fBestRybin));
      if(diff/res<p_bmcon->GetAsshitratio()){
        CheckPossibleHits(cellplane,wireplane,fLegpolysum,yvalue,fBestRybin,diff,res,sely,i);
      }else{
        yvalue=fpHit->GetA0().Y()-xvalue*fpHit->GetA0().Z()-fpHit->Dist()*sqrt(xvalue*xvalue+1.);
        diff=fabs(yvalue - fLegpolysum->GetYaxis()->GetBinCenter(fBestRybin));
        if(diff/res<p_bmcon->GetAsshitratio()){
          CheckPossibleHits(cellplane,wireplane,fLegpolysum,yvalue,fBestRybin,diff,res,sely,i);
        }else{
          if(FootDebugLevel(4))
            cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<i<<"  isFake="<<fpHit->GetIsFake()<<"  view="<<0<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegpolysum->GetYaxis()->GetBinCenter(fBestRybin)<<"  diff="<<diff<<"   res="<<res<<endl;
          fpHit->SetIsSelected(-1);
        }
      }
    }else{ //xz plane
      xvalue=fLegpolxsum->GetXaxis()->GetBinCenter(fBestMxbin);
      yvalue=fpHit->GetA0().X()-xvalue*fpHit->GetA0().Z()+fpHit->Dist()*sqrt(xvalue*xvalue+1.);
      cellplane=p_bmgeo->GetWirePlane(fpHit->GetCellid());
      res=(fpHit->GetSigma()>fLegpolxsum->GetYaxis()->GetBinWidth(fBestRxbin)) ? fpHit->GetSigma() : fLegpolxsum->GetYaxis()->GetBinWidth(fBestRxbin);
      // res=res*3;
      diff=fabs(yvalue - fLegpolxsum->GetYaxis()->GetBinCenter(fBestRxbin));
      if(diff/res<p_bmcon->GetAsshitratio()){
        CheckPossibleHits(cellplane,wireplane,fLegpolxsum,yvalue,fBestRxbin,diff,res,selx,i);
      }else{
        yvalue=fpHit->GetA0().X()-xvalue*fpHit->GetA0().Z()-fpHit->Dist()*sqrt(xvalue*xvalue+1.);
        diff=fabs(yvalue - fLegpolxsum->GetYaxis()->GetBinCenter(fBestRxbin));
        if(diff/res<p_bmcon->GetAsshitratio()){
          CheckPossibleHits(cellplane,wireplane,fLegpolxsum,yvalue,fBestRxbin,diff,res,selx,i);
        }else{
          if(FootDebugLevel(4))
            cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<i<<"  isFake="<<fpHit->GetIsFake()<<"  view="<<1<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegpolxsum->GetYaxis()->GetBinCenter(fBestRxbin)<<"  diff="<<fabs(yvalue - fLegpolxsum->GetYaxis()->GetBinCenter(fBestRxbin))<<"   binwidth="<<fLegpolxsum->GetYaxis()->GetBinWidth(fBestRxbin)<<endl;
          fpHit->SetIsSelected(-1);
        }
      }
    }
  }

  if((selx!=asshitx || sely!=asshity) && FootDebugLevel(1))
    cout<<"TABMactNtuTrack::CheckAssHits::WARNING!  charged hit is different from the estimate!  selx="<<selx<<"  asshitx="<<asshitx<<"  sely="<<sely<<"  asshity="<<asshity<<endl;

  //define the number of associated hits
  p_nturaw->SetNselhitx(selx);
  p_nturaw->SetNselhity(sely);

  if(FootDebugLevel(2))
      cout<<"TABMactNtuTrack::CheckAssHits: done"<<endl;

return;
}

//used in CheckAssHits
void TABMactNtuTrack::CheckPossibleHits(const Int_t cellplane, Int_t wireplane[], TH2D* legpolsum, Double_t yvalue, const Int_t best_rbin, Double_t diff, Double_t res, Int_t &selview, const Int_t hitnum)
{
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
  TABMntuHit*   p_doublehit;
  if(wireplane[cellplane]==-1){
    if(FootDebugLevel(4))
      cout<<"TABMactNtuTrack::CheckAssHits: hit selected hitnum="<<hitnum<<"  isFake="<<fpHit->GetIsFake()<<"  view="<<0<<"  cellid="<<fpHit->GetCellid()<<"  yvalue="<<yvalue<<"  bestvalue="<<legpolsum->GetYaxis()->GetBinCenter(best_rbin)<<"  diff="<<diff<<"   residual="<<diff/res<<endl;
    wireplane[cellplane]=hitnum;
    fpHit->SetIsSelected(1);
    fpHit->SetResidual(diff/res);
    p_nturaw->AddCellOccupyHit(fpHit->GetCellid());
    selview++;
  }else{
    p_doublehit=p_nturaw->GetHit(wireplane[cellplane]);
    if(p_doublehit->GetResidual()> diff/res){
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: hit replaced! selected= hitnum="<<hitnum<<"  isFake="<<fpHit->GetIsFake()<<"  view="<<fpHit->View()<<"  cellid="<<fpHit->GetCellid()<<"  yvalue="<<yvalue<<"  bestvalue="<<legpolsum->GetYaxis()->GetBinCenter(best_rbin)<<"  diff="<<diff<<"   residual="<<diff/res<<"  replace the hit num="<<wireplane[cellplane]<<endl;
      fpHit->SetIsSelected(1);
      fpHit->SetResidual(diff/res);
      wireplane[cellplane]=hitnum;
      p_nturaw->AddCellOccupyHit(fpHit->GetCellid());
      p_nturaw->RemoveCellOccupyHit(p_doublehit->GetCellid());
      p_doublehit->SetIsSelected(-1);
    }else{
      fpHit->SetIsSelected(-1);
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<hitnum<<"  isFake="<<fpHit->GetIsFake()<<"  view="<<fpHit->View()<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegpolysum->GetYaxis()->GetBinCenter(fBestRybin)<<"  diff="<<diff<<"   res="<<res<<endl;
    }
  }

  return;
}


//params[0]=pvers, params[1]=r0, params[3]=view
Double_t TABMactNtuTrack::EvaluateChi2(const double *params)
{
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  Double_t chi2=0, res;
  TVector3 vers,r0;
  Int_t nhit, myview=(Int_t) params[2];
  if(myview==0){//only y view
    vers.SetXYZ(0.,params[0], 1.);
    r0.SetXYZ(0.,params[1],0.);
  }else if(myview==1){//only x view
    vers.SetXYZ(params[0], 0., 1.);
    r0.SetXYZ(params[1], 0., 0.);
  }
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    fpHit = p_nturaw->GetHit(i);
    if( (fpHit->GetIsSelected()==1 && myview==2) || (myview==fpHit->View() && fpHit->GetIsSelected()==1) ){
      res=fpHit->Dist()-p_bmgeo->FindRdrift(r0, vers, fpHit->GetA0(), fpHit->GetWvers(), true);
      chi2+=res*res/fpHit->GetSigma()/fpHit->GetSigma();
    }
  }

  return chi2;
}

Double_t TABMactNtuTrack::EvaluateChi2Xview(const double *params)
{
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  Double_t chi2=0, res;
  TVector3 vers,r0;
  vers.SetXYZ(params[0], 0., 1.);
  r0.SetXYZ(params[1], 0., 0.);
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    fpHit = p_nturaw->GetHit(i);
    if( fpHit->View()==1 && fpHit->GetIsSelected()==1 ){
      res=fpHit->Dist()-p_bmgeo->FindRdrift(r0, vers, fpHit->GetA0(), fpHit->GetWvers(), true);
      chi2+=res*res/fpHit->GetSigma()/fpHit->GetSigma();
    }
  }

  return chi2;
}

Double_t TABMactNtuTrack::EvaluateChi2Yview(const double *params)
{
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  Double_t chi2=0, res;
  TVector3 vers,r0;
  vers.SetXYZ(0., params[0], 1.);
  r0.SetXYZ(0., params[1], 0.);
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    fpHit = p_nturaw->GetHit(i);
    if( fpHit->View()==0 && fpHit->GetIsSelected()==1 ){
      res=fpHit->Dist()-p_bmgeo->FindRdrift(r0, vers, fpHit->GetA0(), fpHit->GetWvers(), true);
      chi2+=res*res/fpHit->GetSigma()/fpHit->GetSigma();
    }
  }

  return chi2;
}


Int_t TABMactNtuTrack::NumericalMinimizationDouble()
{
   TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble start"<<endl;

  // Set the variables to be minimized
  fMinx->SetFunction(*fFunctorx);
  fMinx->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  fMinx->SetMaxIterations(p_bmcon->GetNumIte());
  fMinx->SetTolerance(p_bmcon->GetParMove());
  fMinx->SetPrintLevel(0);
  fMinx->SetVariable(0,"mx",0., p_bmcon->GetParMove());
  fMinx->SetVariable(1,"qx",0., p_bmcon->GetParMove());
  fMinx->SetVariableValue(0,fTmpTrackTr->GetPvers().X()/fTmpTrackTr->GetPvers().Z());
  fMinx->SetVariableValue(1,fTmpTrackTr->GetR0().X());

  fMiny->SetFunction(*fFunctory);
  fMiny->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  fMiny->SetMaxIterations(p_bmcon->GetNumIte());
  fMiny->SetTolerance(p_bmcon->GetParMove());
  fMiny->SetPrintLevel(0);
  fMiny->SetVariable(0,"my",0., p_bmcon->GetParMove());
  fMiny->SetVariable(1,"qy",0., p_bmcon->GetParMove());
  fMiny->SetVariableValue(0,fTmpTrackTr->GetPvers().Y()/fTmpTrackTr->GetPvers().Z());
  fMiny->SetVariableValue(1,fTmpTrackTr->GetR0().Y());

  fMinx->Minimize();
  fMiny->Minimize();
  Int_t status=fMinx->Status()+fMiny->Status();

  if(status==0){
    fTmpTrackTr->SetR0(fMinx->X()[1], fMiny->X()[1], 0.);
    TVector3 tmp_tvector3(fMinx->X()[0],fMiny->X()[0], 1.);
    fTmpTrackTr->SetPvers(tmp_tvector3);
    if(FootDebugLevel(1)){
      cout<<"TABMactNtuTrack:: NumericalMinimizationDouble done: The track parameters are:"<<endl;
      cout<<"Pvers=("<<fTmpTrackTr->GetPvers().X()<<", "<<fTmpTrackTr->GetPvers().Y()<<", "<<fTmpTrackTr->GetPvers().Z()<<")"<<endl;
      cout<<"R0=("<<fTmpTrackTr->GetR0().X()<<", "<<fTmpTrackTr->GetR0().Y()<<", "<<fTmpTrackTr->GetR0().Z()<<")"<<endl;
    }
    return 0;
  }else if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble: minimization failed: status="<<status<<endl;

  fMinx->Clear();
  fMiny->Clear();

  return status;
}



Double_t TABMactNtuTrack::EvaluateChi2GSIx(const double *params)
{
  TABMparCon* p_bmcon  = (TABMparCon*) fpBMCon->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  Double_t chi2=0, res, newrdrift, newresolution;
  TVector3 vers,r0;
  vers.SetXYZ(params[0], 0., 1.);
  r0.SetXYZ(params[1], 0., 0.);

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    fpHit = p_nturaw->GetHit(i);
    if(fpHit->GetIsSelected()==1 && fpHit->View()==1){
      newrdrift=p_bmcon->FirstSTrel(fpHit->Tdrift()+params[2]);
      newresolution=p_bmcon->ResoEval(newrdrift);
      res=newrdrift - p_bmgeo->FindRdrift(r0, vers, fpHit->GetA0(), fpHit->GetWvers(), true);
      chi2+=res*res/newresolution/newresolution;
    }
  }
  // cout<<"xview; t0="<<params[2]<<"  chi2="<<chi2<<endl;
  return chi2;
}

Double_t TABMactNtuTrack::EvaluateChi2GSIy(const double *params)
{
  TABMparCon* p_bmcon  = (TABMparCon*) fpBMCon->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
   
  Double_t chi2=0, res, newrdrift, newresolution;
  TVector3 vers,r0;
  vers.SetXYZ(0., params[0], 1.);
  r0.SetXYZ(0., params[1], 0.);

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    fpHit = p_nturaw->GetHit(i);
    if(fpHit->GetIsSelected()==1 && fpHit->View()==0){
      newrdrift=p_bmcon->FirstSTrel(fpHit->Tdrift()+params[2]);
      newresolution=p_bmcon->ResoEval(newrdrift);
      res=newrdrift - p_bmgeo->FindRdrift(r0, vers, fpHit->GetA0(), fpHit->GetWvers(), true);
      chi2+=res*res/newresolution/newresolution;
    }
  }
  // cout<<"yview; t0="<<params[2]<<"  chi2="<<chi2<<endl;
  return chi2;
}

Int_t TABMactNtuTrack::NumericalMinimizationGSI()
{
  TABMparCon* p_bmcon  = (TABMparCon*) fpBMCon->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: NumericalMinimizationGSI start"<<endl;

  Int_t t0lim=20;
  TVector3 bestpvers, bestr0, bestchi2(100000,100000,200000);
  Double_t bestt0=999, tmp_t0;

  for(Int_t i=0;i<t0lim;++i){
    tmp_t0=i-((Double_t)t0lim)/2.;
    fMinx->SetFunction(*fFunctorGSIx);
    fMinx->SetMaxFunctionCalls(p_bmcon->GetNumIte());
    fMinx->SetMaxIterations(p_bmcon->GetNumIte());
    fMinx->SetTolerance(p_bmcon->GetParMove());
    fMinx->SetPrintLevel(0);
    fMinx->SetVariable(0,"mx",0., p_bmcon->GetParMove());
    fMinx->SetVariable(1,"qx",0., p_bmcon->GetParMove());
    fMinx->SetVariable(2,"t0",0., 1.);
    // fMinx->SetVariableLimits(0,-0.2, 0.2);
    // fMinx->SetVariableLimits(1,-2, 2.);
    // fMinx->SetVariableLimits(2,-10, 10.);
    fMinx->FixVariable(2);
    fMinx->SetVariableValue(0,fTmpTrackTr->GetPvers().X()/fTmpTrackTr->GetPvers().Z());
    fMinx->SetVariableValue(1,fTmpTrackTr->GetR0().X());
    fMinx->SetVariableValue(2,tmp_t0);

    fMiny->SetFunction(*fFunctorGSIy);
    fMiny->SetMaxFunctionCalls(p_bmcon->GetNumIte());
    fMiny->SetMaxIterations(p_bmcon->GetNumIte());
    fMiny->SetTolerance(p_bmcon->GetParMove());
    fMiny->SetPrintLevel(0);
    fMiny->SetVariable(0,"my",0., p_bmcon->GetParMove());
    fMiny->SetVariable(1,"qy",0., p_bmcon->GetParMove());
    fMiny->SetVariable(2,"t0",0., 1.);
    // fMiny->SetVariableLimits(0,-0.2, 0.2);
    // fMiny->SetVariableLimits(1,-2, 2.);
    // fMiny->SetVariableLimits(2,-10, 10.);
    fMiny->FixVariable(2);
    fMiny->SetVariableValue(0,fTmpTrackTr->GetPvers().Y()/fTmpTrackTr->GetPvers().Z());
    fMiny->SetVariableValue(1,fTmpTrackTr->GetR0().Y());
    fMiny->SetVariableValue(2,tmp_t0);

    fMinx->Minimize();
    fMiny->Minimize();
    Int_t status=fMinx->Status()+fMiny->Status();
    if(status==0 && (fMinx->MinValue()+fMiny->MinValue())<bestchi2.Z()){
      bestchi2.SetXYZ(fMinx->MinValue(),fMiny->MinValue(),fMinx->MinValue()+fMiny->MinValue());
      bestpvers.SetXYZ(fMinx->X()[0],fMiny->X()[0],1.);
      bestr0.SetXYZ(fMinx->X()[1],fMiny->X()[1],0);
      bestt0=tmp_t0;
    }else if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack:: NumericalminGSIimizationGSI: t0="<<i<<" minGSIimization failed: status="<<status<<endl;

    fMinx->Clear();
    fMiny->Clear();
  }

  //set the best values
  if(bestt0!=999){
    fTmpTrackTr->SetR0(bestr0);
    fTmpTrackTr->SetPvers(bestpvers);
    for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
        fpHit=p_nturaw->GetHit(i);
        fpHit->SetTdrift(fpHit->Tdrift()+bestt0);
        fpHit->SetRdrift(p_bmcon->FirstSTrel(fpHit->Tdrift()));
        fpHit->SetSigma(p_bmcon->ResoEval(fpHit->Dist()));
      }
    if(ValidHistogram())
      fpHisTrigTime->Fill(bestt0);

    if(FootDebugLevel(1)){
      cout<<"TABMactNtuTrack:: NumericalminGSIimizationGSI done: The track parameters are:"<<endl;
      cout<<"Pvers=("<<fTmpTrackTr->GetPvers().X()<<", "<<fTmpTrackTr->GetPvers().Y()<<", "<<fTmpTrackTr->GetPvers().Z()<<")"<<endl;
      cout<<"R0=("<<fTmpTrackTr->GetR0().X()<<", "<<fTmpTrackTr->GetR0().Y()<<", "<<fTmpTrackTr->GetR0().Z()<<")"<<endl;
      cout<<"best chi2x="<<bestchi2.X()<<"  chi2y="<<bestchi2.Y()<<"  chi2tot="<<bestchi2.Z()<<endl;
      cout<<"bestt0="<<bestt0<<endl;
    }

    return 0;
  }else if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack:: NumericalminGSIimizationGSI: failed! "<<endl;



  return 1;
}


Bool_t TABMactNtuTrack::ComputeDataAll()
{
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  if(p_nturaw->GetNselhits()<5){
    cout<<"TABMactNtuTrack::Error: The number of associated hits is <6, no track is possible"<<p_nturaw->GetNselhits()<<endl;//something wrong that shouldn't happen
    return kTRUE;
  }
  Double_t res, chi2red=0.;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    fpHit=p_nturaw->GetHit(i);
    if(fpHit->GetIsSelected()==1){
      res=fpHit->Dist()-p_bmgeo->FindRdrift(fTmpTrackTr->GetR0(), fTmpTrackTr->GetPvers(), fpHit->GetA0(), fpHit->GetWvers(),true);
      fpHit->SetResidual(res);
      fpHit->SetChi2(res*res/fpHit->GetSigma()/fpHit->GetSigma());
      chi2red+=fpHit->GetChi2();
      if(ValidHistogram()) {
        fpResTot->Fill(res,fpHit->Dist());
        if(fpHit->GetIsFake()==0)
          fpHisTrackFakeHit->Fill(0);
        else if (fpHit->GetIsFake()==1 || fpHit->GetIsFake()==2)
          fpHisTrackFakeHit->Fill(1);
      }
    }else if(ValidHistogram()){
      if(fpHit->GetIsFake()==0)
        fpHisTrackFakeHit->Fill(-1);
      else
        fpHisTrackFakeHit->Fill(0);
    }
  }
  fTmpTrackTr->SetChi2Red(chi2red/((Double_t)(p_nturaw->GetNselhits()-4)));
  return kFALSE;
}
