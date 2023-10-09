/*!
  \file
  \version $Id: TABMactNtuTrack.cxx,v 1.12 2003/07/08 18:58:35 mueller Exp $
  \brief   Implementation of TABMactNtuTrack, adopted to refine the BM track reconstruction made in TABMactNtuTrack
*/

#include "TABMactNtuTrack.hxx"

using namespace std;
/*!
  \class TABMactNtuTrack TABMactNtuTrack.hxx "TABMactNtuTrack.hxx"
  \brief Track builder for Beam Monitor, adopted to refine the BM track reconstruction made in TABMactNtuTrack
*/

ClassImp(TABMactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuTrack::TABMactNtuTrack(const char* name,
                                 TAGdataDsc* dscntutrk,
                                 TAGdataDsc* dscnturaw,
                                 TAGparaDsc* dscbmgeo,
                                 TAGparaDsc* dscbmcon,
                                 TAGparaDsc* dscbmcal)
  : TABMactBaseNtuTrack(name, dscntutrk, dscnturaw, dscbmgeo, dscbmcon, dscbmcal)
{
  if (FootDebugLevel(1))
    cout<<"TABMactNtuTrack::default constructor::Creating the Beam Monitor Track ntuplizer"<<endl;

  //new chi2 ROOT based
  fpFunctor= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2,2);
  fpMinimizer = ROOT::Math::Factory::CreateMinimizer("", "");

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuTrack::~TABMactNtuTrack(){
delete fpMinimizer;
delete fpFunctor;
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TABMactNtuTrack::CreateHistogram()
{
  TABMactBaseNtuTrack::CreateHistogram();

  fpResTot = new TH2F("bmTrackResidual","Residual vs Rdrift; Residual(measured-fitted drift distance) [cm]; Measured rdrift [cm]", 600, -0.3, 0.3, 100, 0., 1.);
  AddHistogram(fpResTot);
  fpHisMap = new TH2F("bmTrackTargetMap","BM - Position of the tracks at the target center; X[cm]; Y[cm]", 250, -3., 3.,250 , -3, 3);
  AddHistogram(fpHisMap);
  fpHisMapTW = new TH2F("bmTrackTWMap","BM - Position of the tracks at the TW center; X[cm]; Y[cm]", 500, -10., 10.,500 , -10, 10);
  AddHistogram(fpHisMapTW);
  fpHisMylar12d = new TH2F("bmTrackCenter","BM - Position of the tracks on the BM center plane; X[cm]; Y[cm]", 500, -3., 3.,500 , -3., 3.);
  AddHistogram(fpHisMylar12d);
  fpHisAngleX = new TH1F("bmTrackAngleX","BM track XZ Angular spread; XZ Angle [rad]; Events", 200, -0.3, 0.3);
  AddHistogram(fpHisAngleX);
  fpHisAngleY = new TH1F("bmTrackAngleY","BM track YZ Angular spread; YZ Angle [rad]; Events", 200, -0.3, 0.3);
  AddHistogram(fpHisAngleY);
  fpHisChi2Red = new TH1F("bmTrackChi2Red","total chi2red", 1000, 0., 100.);
  AddHistogram(fpHisChi2Red);
  fpHisChi2XZ = new TH1F("bmTrackChi2xzView","chi2red on xz view", 1000, 0., 100.);
  AddHistogram(fpHisChi2XZ);
  fpHisChi2YZ = new TH1F("bmTrackChi2yzView","chi2red on yz view", 1000, 0., 100.);
  AddHistogram(fpHisChi2YZ);
  fpTrackAngles = new TH1F("bmTrackMultiAngles","Angles btw reco tracks; deg; tracks", 500, 0, 10.);
  AddHistogram(fpTrackAngles);
  fpTrackSep = new TH1F("bmTrackMultiSeparation","Separation btw multi-tracks of the same event in the BM sys of ref. origin; cm; tracks", 500, 0, 5.);
  AddHistogram(fpTrackSep);

  //fitting studies
  fpFitIters = new TH1I("bmFitIters","Number of Minuit Iterations; Iterations; Entries",101,-0.5, 100.5);
  AddHistogram(fpFitIters);
  fpFitPulls = new TH2F("bmPulls","Approximate pulls;  pull distribution; distance [cm]", 100, -5.,5., 100,0.,1.);
  AddHistogram(fpFitPulls);
  fpFitQ = new TH1F("bmFitQ","Constant term; Constant term: fitted value [cm]; Entries",100,-1.5, 1.5);
  AddHistogram(fpFitQ);
  fpFitM = new TH1F("bmFitM","Slope variable; Slope: fitted value; Entries",100,-0.02, 0.02);
  AddHistogram(fpFitM);
  fpFitQdiff = new TH1F("bmFitQdiff","Constant term; Constant term: fit-initial value [cm]; Entries",100,-0.2, 0.2);
  AddHistogram(fpFitQdiff);
  fpFitMdiff = new TH1F("bmFitMdiff","Slope variable; Slope: fit-initial value; Entries",100,-0.02, 0.02);
  AddHistogram(fpFitMdiff);
  fpFitQinitial = new TH1F("bmFitQvalue","Constant term value; Initial constant term value [cm]; Entries", 100,-1.5, 1.5);
  AddHistogram(fpFitQinitial);
  fpFitMinitial = new TH1F("bmFitMvalue","Slope variable; initial slope; Entries",100,-0.1, 0.1);
  AddHistogram(fpFitMinitial);
  fpFitQFvsQ = new TH2F("bmFitQFvsQ","Constant term: fit value vs starting value; Q starting value [cm]; Q fit value [cm]",300,-1.5, 1.5, 300, -1.5, 1.5);
  AddHistogram(fpFitQFvsQ);
  fpFitMFvsM = new TH2F("bmFitMFvsM","Slope term: fit value vs starting value; M starting value []; M fit value []",200,-0.1, 0.1, 200, -0.1, 0.1);
  AddHistogram(fpFitMFvsM);
  fpFitMFvsQF = new TH2F("bmFitMFvsQF","Fit results: slope fit value vs Q constant term; M fit value [cm]; Q fit value []",200,-0.1, 0.1, 100, -1.5, 1.5);
  AddHistogram(fpFitMFvsQF);

  //STREL calibration
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  if(TAGrecoManager::GetPar()->CalibBM()>0){
    Int_t nbin=(int)(p_bmcon->GetHitTimeCut()/10.);
    fpResTimeTot = new TH2F("bmTrackTimeResidual","Residual vs Time; Residual [cm]; Time [ns]", 600, -0.3, 0.3,nbin, 0., p_bmcon->GetHitTimeCut());
    AddHistogram(fpResTimeTot);
    fpParNewSTrel = new TH1F("bmParNewSTrel","New space time relations; time [ns]; Space [cm]", nbin, 0, p_bmcon->GetHitTimeCut());
    AddHistogram(fpParNewSTrel);
    fpParNewTimeRes = new TH1F("bmParNewTimeRes","New resolution distribution; time [ns]; Resolution [cm]", nbin, 0, p_bmcon->GetHitTimeCut());
    AddHistogram(fpParNewTimeRes);
    fpNewStrelDiff = new TH1F("bmStrelDiff","Differences between old and new strel; Time [ns]; New-old strel [cm]", nbin, 0., p_bmcon->GetHitTimeCut());
    AddHistogram(fpNewStrelDiff);
    fpNewResoDistDiff = new TH1F("bmResDistDiff","Differences between old and new resolution; Drift distance [cm]; New-old Spatial resolution [cm]", 100, 0., 1.);
    AddHistogram(fpNewResoDistDiff);
    fpParNewDistRes = new TH1D("bmParNewDistRes","New resolution distribution; Drift distance [cm]; Resolution [cm]", 100, 0., 1.);
    AddHistogram(fpParNewDistRes);
    TH1F *RawResPlot;
    for(Int_t i=0;i<nbin;i++){
      TString title="bmTrackResTime_";
      title+=i;
      RawResPlot=new TH1F( title.Data(), "Residual;Residual [cm]; Number of hits", 600, -0.3, 0.3);
      fpResTimeBin.push_back(RawResPlot);
      AddHistogram(RawResPlot);
    }
    for(Int_t i=0;i<100;i++){
      TString title="bmTrackResDist_";
      title+=i;
      RawResPlot=new TH1F( title.Data(), "Residual;Residual [cm]; Number of hits", 600, -0.3, 0.3);
      fpResDistBin.push_back(RawResPlot);
      AddHistogram(RawResPlot);
    }
  }

   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuTrack::Action()
{
  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpParGeo->Object();

  p_ntutrk->Clear();

  if(LegendreFit())
    return kTRUE;

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:start to refine the tracking, number of tracks:"<<p_ntutrk->GetTracksN()<<endl;

  //loop for track reconstruction on each view
  for(Int_t i=0;i<p_ntutrk->GetTracksN();i++){
    fpTmpTrack=p_ntutrk->GetTrack(i);
    for(fNowView=0;fNowView<2;fNowView++){
      Int_t tmp_int= NumericalMinimizationDouble();
      if(tmp_int!=0){
        p_ntutrk->SetTrackStatus(8+tmp_int);
        if(ValidHistogram())
          fpFitIters->Fill(-1-tmp_int);
        return kTRUE;
      }

      //calculate the hit residuals and the track chi2
      if(fpTmpTrack->EvaluateResChi2(p_nturaw,p_bmgeo)){
        p_ntutrk->SetTrackStatus(8);//something is wrong, this should not happen!
        if(ValidHistogram()){
          fpFitIters->Fill(-8);
        }
        return kTRUE;
      }

      EvaluatePulls();
      // EvaluateMauroPulls();

      if (ValidHistogram()){
        if(fNowView==1)
          fpHisChi2XZ->Fill(fpTmpTrack->GetChiSquareX());
        else
          fpHisChi2YZ->Fill(fpTmpTrack->GetChiSquareY());
      }
    }
  }


  //fill plots
  if(ValidHistogram()){
    for(Int_t i=0;i<p_ntutrk->GetTracksN();++i){
      TABMtrack *savedtracktr=p_ntutrk->GetTrack(i);
      TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
      Float_t posZ = geoTrafo->FromGlobalToBMLocal(TVector3(0,0,0)).Z();
      TVector3 pos = savedtracktr->Intersection(posZ);
      fpHisMap->Fill(pos[0], pos[1]);
      pos = savedtracktr->Intersection(geoTrafo->FromGlobalToBMLocal(geoTrafo->FromTWLocalToGlobal(TVector3(0,0,0))).Z());
      fpHisMapTW->Fill(pos.X(), pos.Y());
      fpHisMylar12d->Fill(savedtracktr->GetOrigin().X(), savedtracktr->GetOrigin().Y());
      fpHisAngleX->Fill(savedtracktr->GetSlope().X()/savedtracktr->GetSlope().Z());
      fpHisAngleY->Fill(savedtracktr->GetSlope().Y()/savedtracktr->GetSlope().Z());
      fpHisChi2Red->Fill(savedtracktr->GetChiSquare());
      for(Int_t k=0;k<i;++k){
        fpTrackAngles->Fill(p_ntutrk->GetTrack(i)->GetSlope().Angle(p_ntutrk->GetTrack(k)->GetSlope())*TMath::RadToDeg());
        fpTrackSep->Fill((p_ntutrk->GetTrack(i)->GetOrigin()-p_ntutrk->GetTrack(k)->GetOrigin()).Mag());
      }
      for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
        TABMhit* p_hit=p_nturaw->GetHit(i);
        if(p_hit->GetIsSelected()==savedtracktr->GetTrackIdX() || p_hit->GetIsSelected()==savedtracktr->GetTrackIdY()){
          fpResTot->Fill(p_hit->GetResidual(),p_hit->GetRdrift());
          if(TAGrecoManager::GetPar()->CalibBM()){
            fpResTimeTot->Fill(p_hit->GetResidual(),p_hit->GetTdrift());
            if((int)(p_hit->GetTdrift()/10.)<fpResTimeBin.size())
            fpResTimeBin.at((int)(p_hit->GetTdrift()/10.))->Fill(p_hit->GetResidual());
            if((int)(p_hit->GetRdrift()*100)<fpResDistBin.size())
            fpResDistBin.at((int)(p_hit->GetRdrift()*100))->Fill(p_hit->GetResidual());
          }
        }
      }
    }
  }


  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:end of tracking"<<endl;

  return kTRUE;
}


Double_t TABMactNtuTrack::EvaluateChi2(const double *params){

  Double_t chi2=0, res;
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpParGeo->Object();

  Int_t trkidx=(fNowView==0 ? fpTmpTrack->GetTrackIdY():fpTmpTrack->GetTrackIdX());
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMhit* p_hit = p_nturaw->GetHit(i);
    if( p_hit->GetView()==fNowView && p_hit->GetIsSelected()==trkidx){
      res=p_hit->GetRdrift()-p_bmgeo->PointLineDist(params[0], params[1], fNowView==0 ? p_hit->GetWirePos().Y() : p_hit->GetWirePos().X(), p_hit->GetWirePos().Z());
      chi2+=res*res/p_hit->GetSigma()/p_hit->GetSigma();
    }
  }

  return chi2;
}


Int_t TABMactNtuTrack::NumericalMinimizationDouble(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble start, fNowView="<<fNowView<<endl;

  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();

  Double_t initQ=(fNowView==0) ? fpTmpTrack->GetOrigin().Y() : fpTmpTrack->GetOrigin().X();
  Double_t initM=((fNowView==0) ? fpTmpTrack->GetSlope().Y() : fpTmpTrack->GetSlope().X())/fpTmpTrack->GetSlope().Z();

  // Set the variables to be minimized
  fpMinimizer->SetFunction(*fpFunctor);
  fpMinimizer->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  fpMinimizer->SetMaxIterations(p_bmcon->GetNumIte());
  fpMinimizer->SetTolerance(p_bmcon->GetParToll());
  fpMinimizer->SetPrintLevel(0);
  fpMinimizer->SetVariable(0,"m",0., p_bmcon->GetParMoveM());
  fpMinimizer->SetVariable(1,"q",0., p_bmcon->GetParMoveQ());
  fpMinimizer->SetVariableValue(0,initM);
  fpMinimizer->SetVariableValue(1,initQ);

  fpMinimizer->Minimize();

 Int_t status=fpMinimizer->Status();
  if(status==0){
    if(ValidHistogram()){
      fpFitMinitial->Fill(initM);
      fpFitQinitial->Fill(initQ);
      fpFitM->Fill(fpMinimizer->X()[0]);
      fpFitQ->Fill(fpMinimizer->X()[1]);
      fpFitMdiff->Fill(fpMinimizer->X()[0]-initM);
      fpFitQdiff->Fill(fpMinimizer->X()[1]-initQ);
      fpFitMFvsM->Fill(initM,fpMinimizer->X()[0]);
      fpFitQFvsQ->Fill(initQ,fpMinimizer->X()[1]);
      fpFitMFvsQF->Fill(fpMinimizer->X()[0],fpMinimizer->X()[1]);
    }
    if(fNowView==0){
      fpTmpTrack->SetSlopeY(fpMinimizer->X()[0]);
      fpTmpTrack->SetOriginY(fpMinimizer->X()[1]);
      fpTmpTrack->SetErrCovY(fpMinimizer->CovMatrix(1,0));
      fpTmpTrack->SetSlopeYError(fpMinimizer->CovMatrix(0,0));
      fpTmpTrack->SetErrOriginY(fpMinimizer->CovMatrix(1,1));
    }else{
      fpTmpTrack->SetSlopeX(fpMinimizer->X()[0]);
      fpTmpTrack->SetOriginX(fpMinimizer->X()[1]);
      fpTmpTrack->SetErrCovX(fpMinimizer->CovMatrix(1,0));
      fpTmpTrack->SetSlopeXError(fpMinimizer->CovMatrix(0,0));
      fpTmpTrack->SetErrOriginX(fpMinimizer->CovMatrix(1,1));
    }

    fpTmpTrack->SetIsConv(1);
    if(ValidHistogram()){
      fpFitIters->Fill(fpMinimizer->NIterations());
    }

    if(FootDebugLevel(1)){
      cout<<"TABMactNtuTrack:: NumericalMinimizationDouble done:  fNowView="<<fNowView<<"   The track parameters are:"<<endl;
      cout<<"Pvers=("<<fpTmpTrack->GetSlope().X()<<", "<<fpTmpTrack->GetSlope().Y()<<", "<<fpTmpTrack->GetSlope().Z()<<")"<<endl;
      cout<<"R0=("<<fpTmpTrack->GetOrigin().X()<<", "<<fpTmpTrack->GetOrigin().Y()<<", "<<fpTmpTrack->GetOrigin().Z()<<")"<<endl;
    }

    fpMinimizer->Clear();
    return 0;
  }else if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble: minimization failed: status="<<status<<endl;

  fpMinimizer->Clear();
  return status;
}

void TABMactNtuTrack::EvaluatePulls(){
  Int_t trkidx=(fNowView==0 ? fpTmpTrack->GetTrackIdY():fpTmpTrack->GetTrackIdX());
  Double_t slope=(fNowView==0 ? fpTmpTrack->GetSlope().Y() : fpTmpTrack->GetSlope().X())/fpTmpTrack->GetSlope().Z();
  Double_t origin=(fNowView==0 ? fpTmpTrack->GetOrigin().Y() : fpTmpTrack->GetOrigin().X());
  Double_t Errslope=(fNowView==0 ? fpTmpTrack->GetErrSlope().Y() : fpTmpTrack->GetErrSlope().X());
  Double_t Errorigin=(fNowView==0 ? fpTmpTrack->GetErrOrigin().Y() : fpTmpTrack->GetErrOrigin().X());
  Double_t Errcov=(fNowView==0 ? fpTmpTrack->GetErrCov().Y() : fpTmpTrack->GetErrCov().X());
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMhit* p_hit = p_nturaw->GetHit(i);
    if( p_hit->GetView()==fNowView && p_hit->GetIsSelected()==trkidx){
      Double_t pos= (fNowView==0 ? p_hit->GetWirePos().Y() : p_hit->GetWirePos().X());
      Double_t droverdm=(-p_hit->GetWirePos().Z()-slope*(pos-slope*p_hit->GetWirePos().Z()-origin))/pow(1.+slope*slope,1.5);
      Double_t droverdq=-1./sqrt(1.+slope*slope);
      if((pos-slope*p_hit->GetWirePos().Z()-origin)<0){
        droverdm=-1.*droverdm;
        droverdq=-1.*droverdq;
      }
      Double_t fitErrQua=droverdm*droverdm*Errslope+droverdq*droverdq*Errorigin+2.*droverdm*droverdq*Errcov;
      Double_t sigmaQua=p_hit->GetSigma()*p_hit->GetSigma();
      if(sigmaQua>fitErrQua && ValidHistogram()){
        fpFitPulls->Fill(p_hit->GetResidual()/sqrt(sigmaQua-fitErrQua), p_hit->GetRdrift());
      }
    }
  }
  return;
}

void TABMactNtuTrack::EvaluateMauroPulls(){
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  Int_t trkidx=(fNowView==0 ? fpTmpTrack->GetTrackIdY():fpTmpTrack->GetTrackIdX());
  Double_t bestMValue=(fNowView==0 ? fpTmpTrack->GetSlope().Y() : fpTmpTrack->GetSlope().X())/fpTmpTrack->GetSlope().Z();
  Double_t bestQValue=(fNowView==0 ? fpTmpTrack->GetOrigin().Y() : fpTmpTrack->GetOrigin().X());
  Double_t Errslope=(fNowView==0 ? fpTmpTrack->GetErrSlope().Y() : fpTmpTrack->GetErrSlope().X());
  Double_t Errorigin=(fNowView==0 ? fpTmpTrack->GetErrOrigin().Y() : fpTmpTrack->GetErrOrigin().X());
  Double_t Errcov=(fNowView==0 ? fpTmpTrack->GetErrCov().Y() : fpTmpTrack->GetErrCov().X());
  TVector2 vers(bestMValue,1.);
  TVector2 u0(bestQValue,0.);

  Double_t chi2=0.0;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMhit* p_hit = p_nturaw->GetHit(i);
    if( p_hit->GetView()==fNowView && p_hit->GetIsSelected()==trkidx){
      TVector2 wirePos((fNowView==0 ? p_hit->GetWirePos().Y() : p_hit->GetWirePos().X()), p_hit->GetWirePos().Z());
      Double_t rDriftFit = p_hit->GetRdrift()-p_hit->GetResidual();
      // evaluate fit error
      Double_t projprime = (u0-wirePos)*vers/(1.+bestMValue*bestMValue); // not exactly a projection
      Double_t deltaX = (u0.X()-wirePos.X());
      Double_t drOverdq = (deltaX - projprime*bestMValue)/rDriftFit;
      Double_t drOverdm = (-projprime*deltaX);
      Double_t rFitError2 = drOverdm*drOverdm*Errslope +
        drOverdq*drOverdq*Errorigin +
        2.*drOverdm*drOverdq*Errcov;
      Double_t Sigma2 = p_hit->GetSigma()*p_hit->GetSigma();
      if( Sigma2 > rFitError2 ){
        Double_t rFitError = sqrt(rFitError2);
        Double_t pull = p_hit->GetResidual() / sqrt(Sigma2-rFitError2);
        fpFitPulls->Fill(pull, p_hit->GetRdrift());
        // cout<<"OK EvaluateMauroPulls"<<endl;
      } else {
        // cout<<"ERROR EvaluateMauroPulls: fit errors with problems! Sigma2="<<Sigma2<<" rFitError2="<<rFitError2<<"\n"
  	  // <<"drOverdm="<<drOverdm <<" drOverdq="<<drOverdq<<endl;
      }
    }
  }

return;
}


void TABMactNtuTrack::FitWriteCalib(TF1 *newstrel, TF1 *resofunc, Double_t &meanTimeReso, Double_t &meanDistReso){

  TF1* gaus=new TF1("gaus","gaus",-0.1,0.1);
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparCal* p_bmcal = (TABMparCal*) fpParCal->Object();

  //strel evaluation
  for(Int_t i=0;i<fpResTimeBin.size();i++){
    if(fpResTimeBin.at(i)->GetEntries()>300){
      gaus->SetParameters(fpResTimeBin.at(i)->GetEntries(),fpResTimeBin.at(i)->GetMean(),fpResTimeBin.at(i)->GetStdDev());
      fpResTimeBin.at(i)->Fit(gaus,"QR+");
      fpParNewSTrel->SetBinContent(i+1,p_bmcal->STrelEval(fpParNewSTrel->GetXaxis()->GetBinCenter(i+1))-gaus->GetParameter(1));
      fpParNewTimeRes->SetBinContent(i+1,gaus->GetParameter(2));
      fpNewStrelDiff->SetBinContent(i+1,gaus->GetParameter(1));
    }else{
      fpParNewSTrel->SetBinContent(i+1,p_bmcal->STrelEval(fpParNewSTrel->GetXaxis()->GetBinCenter(i+1)));
      fpParNewTimeRes->SetBinContent(i+1,p_bmcal->ResoEval(p_bmcal->STrelEval(fpParNewTimeRes->GetXaxis()->GetBinCenter(i+1))));
      fpNewStrelDiff->SetBinContent(i+1,0.);
    }
  }
  TH1D* p_ResProj=(TH1D*)fpResTimeTot->ProjectionX();
  gaus->SetParameters(p_ResProj->GetEntries(),p_ResProj->GetMean(),p_ResProj->GetStdDev());
  p_ResProj->Fit(gaus,"QR+");
  meanTimeReso=gaus->GetParameter(2);

  //Spatial resolution evaluation
  for(Int_t i=0;i<fpResDistBin.size();i++){
    if(fpResDistBin.at(i)->GetEntries()>300){
      gaus->SetRange(-1.5,(i<3) ? 0. : 1.5);
      gaus->SetParameters(fpResDistBin.at(i)->GetEntries(),fpResDistBin.at(i)->GetMean(),fpResDistBin.at(i)->GetStdDev());
      fpResDistBin.at(i)->Fit(gaus,"QR+");
      fpParNewDistRes->SetBinContent(i+1,gaus->GetParameter(2));
      fpNewResoDistDiff->SetBinContent(i+1,p_bmcal->ResoEval(fpParNewDistRes->GetXaxis()->GetBinCenter(i+1))-gaus->GetParameter(2));
    }else{
      fpParNewDistRes->SetBinContent(i+1,p_bmcal->ResoEval(fpParNewDistRes->GetXaxis()->GetBinCenter(i+1)));
      fpNewResoDistDiff->SetBinContent(i+1,0.);
    }
  }
  p_ResProj=(TH1D*)fpResTot->ProjectionX();
  gaus->SetParameters(p_ResProj->GetEntries(),p_ResProj->GetMean(),p_ResProj->GetStdDev());
  p_ResProj->Fit(gaus,"QR+");
  meanDistReso=gaus->GetParameter(2);

  //fit the new strel and spatial resolutions
  newstrel->FixParameter(0,0.);
  fpParNewSTrel->Fit(newstrel,"QBR+");
  fpParNewDistRes->Fit(resofunc,"QR+");

return;
}


void TABMactNtuTrack::ToStream(ostream& os,  Option_t* /*option*/) const
{
  os<<"fpHisNhitTotTrack=  "<<fpHisNhitTotTrack->GetMean()<<"  fpHisNhitXTrack= "<<fpHisNhitXTrack->GetMean()<<"  fpHisNhitYTrack="<<fpHisNhitYTrack->GetMean()<<"  fpHisNrejhitTrack= "<<fpHisNrejhitTrack->GetMean()<<endl;
  os<<"fpNtotTrack= "<< fpNtotTrack->GetMean()<<"    fpNtotTrack(bin1)= "<<fpNtotTrack->GetBinContent(2)<<endl;
  os<<"fpFitIters=  "<<fpFitIters->GetMean()<<"   fpHisChi2Red= "<< fpHisChi2Red->GetMean()<<endl<<endl;

return;
}
