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

  p_bmcon = (TABMparCon*) fpBMCon->Object();
  p_bmgeo = (TABMparGeo*) fpBMGeo->Object();
  p_tggeo = (TAGparGeo*)  fpTGgeo->Object();

  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  tmp_trackTr=new TABMntuTrackTr();

  //geometry parameters
  TVector3 tgCenter = geoTrafo->GetTGCenter();
  TVector3 bmCenter = geoTrafo->GetBMCenter();
  Float_t tgThick = p_tggeo->GetTargetPar().Size[2];
  TVector3 targetPos(tgCenter[0], tgCenter[1], -bmCenter[2]+tgCenter[2]+tgThick/2.);
  p_bmgeo->SetTarget(targetPos);

  //new chi2 ROOT based
  functorx= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2Xview,2);
  minx = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");
  // minx->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  // minx->SetMaxIterations(p_bmcon->GetNumIte());
  // minx->SetTolerance(p_bmcon->GetParMove());
  // minx->SetPrintLevel(0);
  // minx->SetFunction(*functorx);
  // minx->SetVariable(0,"mx",0., p_bmcon->GetParMove());
  // minx->SetVariable(1,"qx",0., p_bmcon->GetParMove());

  functory= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2Yview,2);
  miny = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");
  // miny->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  // miny->SetMaxIterations(p_bmcon->GetNumIte());
  // miny->SetTolerance(p_bmcon->GetParMove());
  // miny->SetPrintLevel(0);
  // miny->SetFunction(*functory);
  // miny->SetVariable(0,"my",0., p_bmcon->GetParMove());
  // miny->SetVariable(1,"qy",0., p_bmcon->GetParMove());

  functorGSIx= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2GSIx,3);
  functorGSIy= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2GSIy,3);
  //legendre
  legpolxsum=new TH2D("legpolxsum","Legendre polynomial space on xz plane; mx; q[cm]",p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(), p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());
  legpolysum=new TH2D("legpolysum","Legendre polynomial space on yz plane; my; q[cm]",p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(),  p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuTrack::~TABMactNtuTrack(){
delete tmp_trackTr;
delete minx;
delete miny;
delete functorx;
delete functory;
delete functorGSIx;
delete functorGSIy;
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
  p_nturaw = (TABMntuRaw*)   fpNtuHit->Object();

  p_ntutrk->Clear();
  Int_t i_nhit = p_nturaw->GetHitsN();
  Int_t firedx=0, firedy=0, tmp_int;
  //NB.: If the preselection reject the event no track will be saved

  if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack::Action::start"<<endl;

  //check the number of hits for each view
  for(Int_t i_h = 0; i_h < i_nhit; i_h++) {
    p_hit = p_nturaw->GetHit(i_h);
    if(FootDebugLevel(3))
      cout<<"hit="<<i_h<<" plane="<<p_hit->Plane()<<"  view="<<p_hit->View()<<"  cell="<<p_hit->Cell()<<"  wireplane="<<p_bmgeo->GetWirePlane(p_hit->Plane(),p_hit->View())<<"  rdrift="<<p_hit->Dist()<<"  isfake="<<p_hit->GetIsFake()<<endl;
    if(p_hit->View()==1)
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
    CheckAssHits((firedx<legpolxsum->GetMaximum() && tmp_int%100==0) ? firedx : legpolxsum->GetMaximum(), (firedy<legpolysum->GetMaximum() && tmp_int/100==0) ? firedy : legpolysum->GetMaximum()); //check the associated hits

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
      tmp_trackTr->Clean();
      tmp_trackTr->SetNhit(p_nturaw->GetNselhits());
      TVector3 tmp_tvector3(legpolxsum->GetXaxis()->GetBinCenter(best_mxbin),legpolysum->GetXaxis()->GetBinCenter(best_mybin),1.);
      tmp_trackTr->SetPvers(tmp_tvector3);
      tmp_trackTr->SetR0(legpolxsum->GetXaxis()->GetBinCenter(best_rxbin),legpolysum->GetXaxis()->GetBinCenter(best_rybin),0.);
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
  if(tmp_trackTr->GetChi2Red()<=p_bmcon->GetChi2Redcut()){//use only the track with chi2<chi2cut
    Int_t pivot[8]={0};
    Int_t probe[8]={0};
    Double_t efftot, effxview, effyview;
    p_nturaw->Efficiency_paoloni(pivot, probe,efftot, effxview, effyview);
    tmp_trackTr->SetEffFittedPlane(efftot);
  }

  //add the reconstructed track
  TABMntuTrackTr* trk = new((*(p_ntutrk->GetListOfTracks()))[p_ntutrk->GetTracksN()]) TABMntuTrackTr(*tmp_trackTr);
  p_ntutrk->GetTrackStatus()= (tmp_trackTr->GetChi2Red()>=p_bmcon->GetChi2Redcut())? 5:0;

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



void TABMactNtuTrack::ChargeLegendrePoly(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: start charge legendre pol graphs"<<endl;
  legpolxsum->Reset("ICESM");
  legpolxsum->SetBins(p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(), p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());
  legpolysum->Reset("ICESM");
  legpolysum->SetBins(p_bmcon->GetLegmbin(),-p_bmcon->GetLegmrange(),p_bmcon->GetLegmrange(), p_bmcon->GetLegrbin(),-p_bmcon->GetLegrrange(),p_bmcon->GetLegrrange());

  Double_t tmp_double;
  for(Int_t i_h = 0; i_h < p_nturaw->GetHitsN(); ++i_h) {
    p_hit = p_nturaw->GetHit(i_h);
    if(p_hit->View()==1){ //on xz view
      for(Int_t k=1;k<legpolxsum->GetXaxis()->GetNbins();++k){
        tmp_double=legpolxsum->GetXaxis()->GetBinCenter(k);
        legpolxsum->Fill(tmp_double, p_hit->GetA0().X()-tmp_double*p_hit->GetA0().Z()+p_hit->Dist()*sqrt(tmp_double*tmp_double+1.));
        legpolxsum->Fill(tmp_double, p_hit->GetA0().X()-tmp_double*p_hit->GetA0().Z()-p_hit->Dist()*sqrt(tmp_double*tmp_double+1.));
      }
    }else{//on yz view
      for(Int_t k=1;k<legpolysum->GetXaxis()->GetNbins();++k){
        tmp_double=legpolysum->GetXaxis()->GetBinCenter(k);
        legpolysum->Fill(tmp_double, p_hit->GetA0().Y()-tmp_double*p_hit->GetA0().Z()+p_hit->Dist()*sqrt(tmp_double*tmp_double+1.));
        legpolysum->Fill(tmp_double, p_hit->GetA0().Y()-tmp_double*p_hit->GetA0().Z()-p_hit->Dist()*sqrt(tmp_double*tmp_double+1.));
      }
    }
  }

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::legpolysum and legpolxsum done"<<endl;

  //provv: adopted to save the legpolx/ysum th2d
  //~ TString tmp_tstring="legpolxsum_";
  //~ TString tmp_tstringy="legpolysum_";
  //~ Int_t tmp_int=gRandom->Uniform(0,100000);
  //~ tmp_tstring+=tmp_int;
  //~ tmp_tstringy+=tmp_int;
  //~ tmp_tstring+="x.root";
  //~ tmp_tstringy+="y.root";
  //~ legpolxsum->SaveAs(tmp_tstring.Data());
  //~ legpolysum->SaveAs(tmp_tstringy.Data());

return;
}


Int_t TABMactNtuTrack::FindLegendreBestValues(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: start"<<endl;

  Int_t first_maxbin,last_maxbin;
  //X values
  best_mxbin=-1;
  Int_t rebinmx=0;
  do{
    first_maxbin=legpolxsum->FindFirstBinAbove(legpolxsum->GetMaximum()-1);
    last_maxbin=legpolxsum->FindLastBinAbove(legpolxsum->GetMaximum()-1);
    if(first_maxbin==last_maxbin){
      best_mxbin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      best_mxbin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      legpolxsum->RebinX();
      rebinmx++;
    }
  }while(best_mxbin==-1);

  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: check on mx done, now I'll check on qx  best_mxbin="<<best_mxbin<<endl;

  //check on r
  best_rxbin=-1;
  Int_t rebinrx=0;
  do{
    first_maxbin=legpolxsum->FindFirstBinAbove(legpolxsum->GetMaximum()-1,2);
    last_maxbin=legpolxsum->FindLastBinAbove(legpolxsum->GetMaximum()-1,2);
    if(first_maxbin==last_maxbin){
      best_rxbin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      best_rxbin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      legpolxsum->RebinY();
      rebinrx++;
    }
  }while(best_rxbin==-1);

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: X done; check on r done  best_rxbin="<<best_rxbin<<"   bincontent="<<legpolxsum->GetBinContent(best_mxbin,best_rxbin)<<"  best m="<<legpolxsum->GetXaxis()->GetBinCenter(best_mxbin)<<"  best q="<<legpolxsum->GetYaxis()->GetBinCenter(best_rxbin)<<"   rebinmx="<<rebinmx<<"  rebinrx="<<rebinrx<<endl;

  // Y values
  best_mybin=-1;
  Int_t rebinmy=0;
  do{
    first_maxbin=legpolysum->FindFirstBinAbove(legpolysum->GetMaximum()-1);
    last_maxbin=legpolysum->FindLastBinAbove(legpolysum->GetMaximum()-1);
    if(first_maxbin==last_maxbin){
      best_mybin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      best_mybin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      legpolysum->RebinX();
      rebinmy++;
    }
  }while(best_mybin==-1);

  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: check on mx done, now I'll check on qx  best_mybin="<<best_mybin<<endl;

  //check on r
  best_rybin=-1;
  Int_t rebinry=0;
  do{
    first_maxbin=legpolysum->FindFirstBinAbove(legpolysum->GetMaximum()-1,2);
    last_maxbin=legpolysum->FindLastBinAbove(legpolysum->GetMaximum()-1,2);
    if(first_maxbin==last_maxbin){
      best_rybin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<2){//value to optimize
      best_rybin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      legpolysum->RebinY();
      rebinry++;
    }
  }while(best_rybin==-1);

  if(FootDebugLevel(2))
      cout<<"TABMactNtuTrack::FindLegendreBestValues: Y done; check on r done  best_rybin="<<best_rybin<<"   bincontent="<<legpolysum->GetBinContent(best_mybin,best_rybin)<<"  best m="<<legpolysum->GetXaxis()->GetBinCenter(best_mybin)<<"  best q="<<legpolysum->GetYaxis()->GetBinCenter(best_rybin)<<"   rebinmy="<<rebinmy<<"  rebinry="<<rebinry<<endl;

return rebinmx+rebinrx + 100*(rebinmy+rebinry);
}


//for Legendre polynomy tracking, set the associated hits
void TABMactNtuTrack::CheckAssHits(const Int_t asshitx, const Int_t asshity) {

  if(FootDebugLevel(2)){
    cout<<"TABMactNtuTrack::CheckAssHits: start"<<endl;
    TVector3 legPvers(legpolxsum->GetXaxis()->GetBinCenter(best_mxbin),legpolysum->GetXaxis()->GetBinCenter(best_mybin),1.);
    legPvers.SetMag(1.);
    cout<<"legPvers=("<<legPvers.X()<<", "<<legPvers.Y()<<", "<<legPvers.Z()<<")"<<" binwidthX="<<legpolxsum->GetXaxis()->GetBinWidth(best_mxbin)<<"  binwidthY"<<legpolysum->GetXaxis()->GetBinWidth(best_mybin)<<endl;
    cout<<"legR0="<<legpolxsum->GetYaxis()->GetBinCenter(best_rxbin)<<", "<<legpolysum->GetYaxis()->GetBinCenter(best_rybin)<<"  binwidthX="<<legpolxsum->GetYaxis()->GetBinWidth(best_rxbin)<<"   binwidthY="<<legpolysum->GetYaxis()->GetBinWidth(best_rybin)<<endl;
  }

  Int_t wireplane[12]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  Int_t selx=0, sely=0, cellplane;
  Double_t xvalue, yvalue, res, diff, tmp_double;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i) {
    p_hit = p_nturaw->GetHit(i);
    if(p_hit->View()==0){//yz plane
      xvalue=legpolysum->GetXaxis()->GetBinCenter(best_mybin);
      yvalue=p_hit->GetA0().Y()-xvalue*p_hit->GetA0().Z()+p_hit->Dist()*sqrt(xvalue*xvalue+1.);
      cellplane=p_bmgeo->GetWirePlane(p_hit->GetCellid());
      res=(p_hit->GetSigma()>legpolysum->GetYaxis()->GetBinWidth(best_rybin)) ? p_hit->GetSigma() : legpolysum->GetYaxis()->GetBinWidth(best_rybin);
      // res=res*3.;
      diff=fabs(yvalue - legpolysum->GetYaxis()->GetBinCenter(best_rybin));
      if(diff/res<p_bmcon->GetAsshitratio()){
        CheckPossibleHits(cellplane,wireplane,legpolysum,yvalue,best_rybin,diff,res,sely,i);
      }else{
        yvalue=p_hit->GetA0().Y()-xvalue*p_hit->GetA0().Z()-p_hit->Dist()*sqrt(xvalue*xvalue+1.);
        diff=fabs(yvalue - legpolysum->GetYaxis()->GetBinCenter(best_rybin));
        if(diff/res<p_bmcon->GetAsshitratio()){
          CheckPossibleHits(cellplane,wireplane,legpolysum,yvalue,best_rybin,diff,res,sely,i);
        }else{
          if(FootDebugLevel(4))
            cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<i<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<0<<"  yvalue="<<yvalue<<"  bestvalue="<<legpolysum->GetYaxis()->GetBinCenter(best_rybin)<<"  diff="<<diff<<"   res="<<res<<endl;
          p_hit->SetIsSelected(-1);
        }
      }
    }else{ //xz plane
      xvalue=legpolxsum->GetXaxis()->GetBinCenter(best_mxbin);
      yvalue=p_hit->GetA0().X()-xvalue*p_hit->GetA0().Z()+p_hit->Dist()*sqrt(xvalue*xvalue+1.);
      cellplane=p_bmgeo->GetWirePlane(p_hit->GetCellid());
      res=(p_hit->GetSigma()>legpolxsum->GetYaxis()->GetBinWidth(best_rxbin)) ? p_hit->GetSigma() : legpolxsum->GetYaxis()->GetBinWidth(best_rxbin);
      // res=res*3;
      diff=fabs(yvalue - legpolxsum->GetYaxis()->GetBinCenter(best_rxbin));
      if(diff/res<p_bmcon->GetAsshitratio()){
        CheckPossibleHits(cellplane,wireplane,legpolxsum,yvalue,best_rxbin,diff,res,selx,i);
      }else{
        yvalue=p_hit->GetA0().X()-xvalue*p_hit->GetA0().Z()-p_hit->Dist()*sqrt(xvalue*xvalue+1.);
        diff=fabs(yvalue - legpolxsum->GetYaxis()->GetBinCenter(best_rxbin));
        if(diff/res<p_bmcon->GetAsshitratio()){
          CheckPossibleHits(cellplane,wireplane,legpolxsum,yvalue,best_rxbin,diff,res,selx,i);
        }else{
          if(FootDebugLevel(4))
            cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<i<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<1<<"  yvalue="<<yvalue<<"  bestvalue="<<legpolxsum->GetYaxis()->GetBinCenter(best_rxbin)<<"  diff="<<fabs(yvalue - legpolxsum->GetYaxis()->GetBinCenter(best_rxbin))<<"   binwidth="<<legpolxsum->GetYaxis()->GetBinWidth(best_rxbin)<<endl;
          p_hit->SetIsSelected(-1);
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
void TABMactNtuTrack::CheckPossibleHits(const Int_t cellplane, Int_t wireplane[], TH2D* legpolsum, Double_t yvalue, const Int_t best_rbin, Double_t diff, Double_t res, Int_t &selview, const Int_t hitnum){

  TABMntuHit*   p_doublehit;
  if(wireplane[cellplane]==-1){
    if(FootDebugLevel(4))
      cout<<"TABMactNtuTrack::CheckAssHits: hit selected hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<0<<"  cellid="<<p_hit->GetCellid()<<"  yvalue="<<yvalue<<"  bestvalue="<<legpolsum->GetYaxis()->GetBinCenter(best_rbin)<<"  diff="<<diff<<"   residual="<<diff/res<<endl;
    wireplane[cellplane]=hitnum;
    p_hit->SetIsSelected(1);
    p_hit->SetResidual(diff/res);
    p_nturaw->AddCellOccupyHit(p_hit->GetCellid());
    selview++;
  }else{
    p_doublehit=p_nturaw->GetHit(wireplane[cellplane]);
    if(p_doublehit->GetResidual()> diff/res){
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: hit replaced! selected= hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->View()<<"  cellid="<<p_hit->GetCellid()<<"  yvalue="<<yvalue<<"  bestvalue="<<legpolsum->GetYaxis()->GetBinCenter(best_rbin)<<"  diff="<<diff<<"   residual="<<diff/res<<"  replace the hit num="<<wireplane[cellplane]<<endl;
      p_hit->SetIsSelected(1);
      p_hit->SetResidual(diff/res);
      wireplane[cellplane]=hitnum;
      p_nturaw->AddCellOccupyHit(p_hit->GetCellid());
      p_nturaw->RemoveCellOccupyHit(p_doublehit->GetCellid());
      p_doublehit->SetIsSelected(-1);
    }else{
      p_hit->SetIsSelected(-1);
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->View()<<"  yvalue="<<yvalue<<"  bestvalue="<<legpolysum->GetYaxis()->GetBinCenter(best_rybin)<<"  diff="<<diff<<"   res="<<res<<endl;
    }
  }

  return;
}


//params[0]=pvers, params[1]=r0, params[3]=view
Double_t TABMactNtuTrack::EvaluateChi2(const double *params){

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
    p_hit = p_nturaw->GetHit(i);
    if( (p_hit->GetIsSelected()==1 && myview==2) || (myview==p_hit->View() && p_hit->GetIsSelected()==1) ){
      res=p_hit->Dist()-p_bmgeo->FindRdrift(r0, vers, p_hit->GetA0(), p_hit->GetWvers(), true);
      chi2+=res*res/p_hit->GetSigma()/p_hit->GetSigma();
    }
  }

  return chi2;
}

Double_t TABMactNtuTrack::EvaluateChi2Xview(const double *params){

  Double_t chi2=0, res;
  TVector3 vers,r0;
  vers.SetXYZ(params[0], 0., 1.);
  r0.SetXYZ(params[1], 0., 0.);
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    p_hit = p_nturaw->GetHit(i);
    if( p_hit->View()==1 && p_hit->GetIsSelected()==1 ){
      res=p_hit->Dist()-p_bmgeo->FindRdrift(r0, vers, p_hit->GetA0(), p_hit->GetWvers(), true);
      chi2+=res*res/p_hit->GetSigma()/p_hit->GetSigma();
    }
  }

  return chi2;
}

Double_t TABMactNtuTrack::EvaluateChi2Yview(const double *params){

  Double_t chi2=0, res;
  TVector3 vers,r0;
  vers.SetXYZ(0., params[0], 1.);
  r0.SetXYZ(0., params[1], 0.);
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    p_hit = p_nturaw->GetHit(i);
    if( p_hit->View()==0 && p_hit->GetIsSelected()==1 ){
      res=p_hit->Dist()-p_bmgeo->FindRdrift(r0, vers, p_hit->GetA0(), p_hit->GetWvers(), true);
      chi2+=res*res/p_hit->GetSigma()/p_hit->GetSigma();
    }
  }

  return chi2;
}


Int_t TABMactNtuTrack::NumericalMinimizationDouble(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble start"<<endl;

  // Set the variables to be minimized
  minx->SetFunction(*functorx);
  minx->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  minx->SetMaxIterations(p_bmcon->GetNumIte());
  minx->SetTolerance(p_bmcon->GetParMove());
  minx->SetPrintLevel(0);
  minx->SetVariable(0,"mx",0., p_bmcon->GetParMove());
  minx->SetVariable(1,"qx",0., p_bmcon->GetParMove());
  minx->SetVariableValue(0,tmp_trackTr->GetPvers().X()/tmp_trackTr->GetPvers().Z());
  minx->SetVariableValue(1,tmp_trackTr->GetR0().X());

  miny->SetFunction(*functory);
  miny->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  miny->SetMaxIterations(p_bmcon->GetNumIte());
  miny->SetTolerance(p_bmcon->GetParMove());
  miny->SetPrintLevel(0);
  miny->SetVariable(0,"my",0., p_bmcon->GetParMove());
  miny->SetVariable(1,"qy",0., p_bmcon->GetParMove());
  miny->SetVariableValue(0,tmp_trackTr->GetPvers().Y()/tmp_trackTr->GetPvers().Z());
  miny->SetVariableValue(1,tmp_trackTr->GetR0().Y());

  minx->Minimize();
  miny->Minimize();
  Int_t status=minx->Status()+miny->Status();

  if(status==0){
    tmp_trackTr->SetR0(minx->X()[1], miny->X()[1], 0.);
    TVector3 tmp_tvector3(minx->X()[0],miny->X()[0], 1.);
    tmp_trackTr->SetPvers(tmp_tvector3);
    if(FootDebugLevel(1)){
      cout<<"TABMactNtuTrack:: NumericalMinimizationDouble done: The track parameters are:"<<endl;
      cout<<"Pvers=("<<tmp_trackTr->GetPvers().X()<<", "<<tmp_trackTr->GetPvers().Y()<<", "<<tmp_trackTr->GetPvers().Z()<<")"<<endl;
      cout<<"R0=("<<tmp_trackTr->GetR0().X()<<", "<<tmp_trackTr->GetR0().Y()<<", "<<tmp_trackTr->GetR0().Z()<<")"<<endl;
    }
    return 0;
  }else if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble: minimization failed: status="<<status<<endl;

  minx->Clear();
  miny->Clear();

  return status;
}



Double_t TABMactNtuTrack::EvaluateChi2GSIx(const double *params){

  Double_t chi2=0, res, newrdrift, newresolution;
  TVector3 vers,r0;
  vers.SetXYZ(params[0], 0., 1.);
  r0.SetXYZ(params[1], 0., 0.);

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    p_hit = p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()==1 && p_hit->View()==1){
      newrdrift=p_bmcon->FirstSTrel(p_hit->Tdrift()+params[2]);
      newresolution=p_bmcon->ResoEval(newrdrift);
      res=newrdrift - p_bmgeo->FindRdrift(r0, vers, p_hit->GetA0(), p_hit->GetWvers(), true);
      chi2+=res*res/newresolution/newresolution;
    }
  }
  // cout<<"xview; t0="<<params[2]<<"  chi2="<<chi2<<endl;
  return chi2;
}

Double_t TABMactNtuTrack::EvaluateChi2GSIy(const double *params){

  Double_t chi2=0, res, newrdrift, newresolution;
  TVector3 vers,r0;
  vers.SetXYZ(0., params[0], 1.);
  r0.SetXYZ(0., params[1], 0.);

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    p_hit = p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()==1 && p_hit->View()==0){
      newrdrift=p_bmcon->FirstSTrel(p_hit->Tdrift()+params[2]);
      newresolution=p_bmcon->ResoEval(newrdrift);
      res=newrdrift - p_bmgeo->FindRdrift(r0, vers, p_hit->GetA0(), p_hit->GetWvers(), true);
      chi2+=res*res/newresolution/newresolution;
    }
  }
  // cout<<"yview; t0="<<params[2]<<"  chi2="<<chi2<<endl;
  return chi2;
}

Int_t TABMactNtuTrack::NumericalMinimizationGSI(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: NumericalMinimizationGSI start"<<endl;

  Int_t t0lim=20;
  TVector3 bestpvers, bestr0, bestchi2(100000,100000,200000);
  Double_t bestt0=999, tmp_t0;

  for(Int_t i=0;i<t0lim;++i){
    tmp_t0=i-((Double_t)t0lim)/2.;
    minx->SetFunction(*functorGSIx);
    minx->SetMaxFunctionCalls(p_bmcon->GetNumIte());
    minx->SetMaxIterations(p_bmcon->GetNumIte());
    minx->SetTolerance(p_bmcon->GetParMove());
    minx->SetPrintLevel(0);
    minx->SetVariable(0,"mx",0., p_bmcon->GetParMove());
    minx->SetVariable(1,"qx",0., p_bmcon->GetParMove());
    minx->SetVariable(2,"t0",0., 1.);
    // minx->SetVariableLimits(0,-0.2, 0.2);
    // minx->SetVariableLimits(1,-2, 2.);
    // minx->SetVariableLimits(2,-10, 10.);
    minx->FixVariable(2);
    minx->SetVariableValue(0,tmp_trackTr->GetPvers().X()/tmp_trackTr->GetPvers().Z());
    minx->SetVariableValue(1,tmp_trackTr->GetR0().X());
    minx->SetVariableValue(2,tmp_t0);

    miny->SetFunction(*functorGSIy);
    miny->SetMaxFunctionCalls(p_bmcon->GetNumIte());
    miny->SetMaxIterations(p_bmcon->GetNumIte());
    miny->SetTolerance(p_bmcon->GetParMove());
    miny->SetPrintLevel(0);
    miny->SetVariable(0,"my",0., p_bmcon->GetParMove());
    miny->SetVariable(1,"qy",0., p_bmcon->GetParMove());
    miny->SetVariable(2,"t0",0., 1.);
    // miny->SetVariableLimits(0,-0.2, 0.2);
    // miny->SetVariableLimits(1,-2, 2.);
    // miny->SetVariableLimits(2,-10, 10.);
    miny->FixVariable(2);
    miny->SetVariableValue(0,tmp_trackTr->GetPvers().Y()/tmp_trackTr->GetPvers().Z());
    miny->SetVariableValue(1,tmp_trackTr->GetR0().Y());
    miny->SetVariableValue(2,tmp_t0);

    minx->Minimize();
    miny->Minimize();
    Int_t status=minx->Status()+miny->Status();
    if(status==0 && (minx->MinValue()+miny->MinValue())<bestchi2.Z()){
      bestchi2.SetXYZ(minx->MinValue(),miny->MinValue(),minx->MinValue()+miny->MinValue());
      bestpvers.SetXYZ(minx->X()[0],miny->X()[0],1.);
      bestr0.SetXYZ(minx->X()[1],miny->X()[1],0);
      bestt0=tmp_t0;
    }else if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack:: NumericalminGSIimizationGSI: t0="<<i<<" minGSIimization failed: status="<<status<<endl;

    minx->Clear();
    miny->Clear();
  }

  //set the best values
  if(bestt0!=999){
    tmp_trackTr->SetR0(bestr0);
    tmp_trackTr->SetPvers(bestpvers);
    for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
        p_hit=p_nturaw->GetHit(i);
        p_hit->SetTdrift(p_hit->Tdrift()+bestt0);
        p_hit->SetRdrift(p_bmcon->FirstSTrel(p_hit->Tdrift()));
        p_hit->SetSigma(p_bmcon->ResoEval(p_hit->Dist()));
      }
    if(ValidHistogram())
      fpHisTrigTime->Fill(bestt0);

    if(FootDebugLevel(1)){
      cout<<"TABMactNtuTrack:: NumericalminGSIimizationGSI done: The track parameters are:"<<endl;
      cout<<"Pvers=("<<tmp_trackTr->GetPvers().X()<<", "<<tmp_trackTr->GetPvers().Y()<<", "<<tmp_trackTr->GetPvers().Z()<<")"<<endl;
      cout<<"R0=("<<tmp_trackTr->GetR0().X()<<", "<<tmp_trackTr->GetR0().Y()<<", "<<tmp_trackTr->GetR0().Z()<<")"<<endl;
      cout<<"best chi2x="<<bestchi2.X()<<"  chi2y="<<bestchi2.Y()<<"  chi2tot="<<bestchi2.Z()<<endl;
      cout<<"bestt0="<<bestt0<<endl;
    }

    return 0;
  }else if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack:: NumericalminGSIimizationGSI: failed! "<<endl;



  return 1;
}


Bool_t TABMactNtuTrack::ComputeDataAll(){

  if(p_nturaw->GetNselhits()<5){
    cout<<"TABMactNtuTrack::Error: The number of associated hits is <6, no track is possible"<<p_nturaw->GetNselhits()<<endl;//something wrong that shouldn't happen
    return kTRUE;
  }
  Double_t res, chi2red=0.;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    p_hit=p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()==1){
      res=p_hit->Dist()-p_bmgeo->FindRdrift(tmp_trackTr->GetR0(), tmp_trackTr->GetPvers(), p_hit->GetA0(), p_hit->GetWvers(),true);
      p_hit->SetResidual(res);
      p_hit->SetChi2(res*res/p_hit->GetSigma()/p_hit->GetSigma());
      chi2red+=p_hit->GetChi2();
      if(ValidHistogram()) {
        fpResTot->Fill(res,p_hit->Dist());
        if(p_hit->GetIsFake()==0)
          fpHisTrackFakeHit->Fill(0);
        else if (p_hit->GetIsFake()==1 || p_hit->GetIsFake()==2)
          fpHisTrackFakeHit->Fill(1);
      }
    }else if(ValidHistogram()){
      if(p_hit->GetIsFake()==0)
        fpHisTrackFakeHit->Fill(-1);
      else
        fpHisTrackFakeHit->Fill(0);
    }
  }
  tmp_trackTr->SetChi2Red(chi2red/((Double_t)(p_nturaw->GetNselhits()-4)));
  return kFALSE;
}
