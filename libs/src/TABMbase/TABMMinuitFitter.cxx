/*!
  \file
  \brief   Implementation of TABMMinuitFitter.
*/
#include "TABMMinuitFitter.hxx"
#include "TABMparConf.hxx"
#include "TABMntuHit.hxx"
#include "TABMtrack.hxx"

#include "TVector2.h"
#include "TH2F.h"

using namespace std;
/*!
  \class TABMMinuitFitter TABMMinuitFitter.hxx "TABMMinuitFitter.hxx"
  \brief Track Fitter with Minuit for Beam Monitor. **
*/

ClassImp(TABMMinuitFitter);

TABMMinuitFitter::TABMMinuitFitter(const char *name, TABMparConf* p_bmcon) :
  TAGnamed(name,"Minuit Fitter - best fit on BM tracks ") {

  fpbmcon = p_bmcon;
  
  //new chi2 ROOT based
  fpFunctor= new ROOT::Math::Functor(this,&TABMMinuitFitter::EvaluateChi2,2);
  fpMinimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");
  fpMinimizer->SetFunction(*fpFunctor);
  fpMinimizer->SetMaxFunctionCalls(fpbmcon->GetNumIte());
  fpMinimizer->SetMaxIterations(fpbmcon->GetNumIte());
  fpMinimizer->SetTolerance(fpbmcon->GetParMove());
  fpMinimizer->SetPrintLevel(0);
}


TABMMinuitFitter::~TABMMinuitFitter(){
  delete fpFunctor;
  delete fpMinimizer;
}


//! To stream
void TABMMinuitFitter::ToStream(ostream& os, Option_t* option) const {
  os << "\nTABMMinuitFitter for view="<<fNowView<<", track="<<fTrackNum
     <<", hits considered: "<<nhits<<"\n";

  for(auto iter= hits.begin(); iter!=hits.end(); ++iter){
    os <<" Hit="<<iter->iHit<<" Z="<<iter->zwire<<" U="<<iter->uwire
       <<" rdrift="<<iter->rDrift
       <<" sigma="<<TMath::Sqrt(1./iter->OneOverSigma2)<<"\n";
  }

  os <<"  Slope term selected: value="<<bestMValue<<", error="
     <<TMath::Sqrt(fpMinimizer->CovMatrix(0,0))
     <<"\n  Constant term selected: value="<<bestQValue<<", error="
     <<TMath::Sqrt(fpMinimizer->CovMatrix(1,1))
     <<"\n  Chisq = "<<chisq<<"  calls:"<<ncalls<<"\n";

}


void TABMMinuitFitter::Prepare(TABMntuHit* p_nturaw, Int_t iTrackNum, 
			       Int_t iView){

  fTrackNum = iTrackNum;
  fNowView = iView;
  fpnturaw = p_nturaw;

  nhits=0;
  hits.clear();
  HitPoint aHit;
  for(Int_t i=0;i<fpnturaw->GetHitsN();++i){
    TABMhit* p_hit = fpnturaw->GetHit(i);
    if( p_hit->GetView()==iView && p_hit->GetIsSelected()==(iTrackNum+1) ){
      aHit.iHit = i;
      aHit.zwire = p_hit->GetWirePos().Z();
      aHit.uwire = (fNowView==0) ? p_hit->GetWirePos().Y() :  p_hit->GetWirePos().X();
      aHit.rDrift = p_hit->GetRdrift();
      aHit.OneOverSigma2 = 1./(p_hit->GetSigma()*p_hit->GetSigma());
      hits.push_back(aHit);
      nhits++;
    }
  }

}

  
Int_t TABMMinuitFitter::Minimize(Float_t qStart, Float_t mStart, TABMtrack* apTrack){

  fpTrack = apTrack;
  if(FootDebugLevel(2))
    cout<<"TABMMinuitFitter:: NumericalMinimizationDouble start, fNowView="<<fNowView<<"  fTrackNum="<<fTrackNum<<endl;

  // Set the variables to be minimized
  fpMinimizer->Clear();
  fpMinimizer->SetVariable(0,"m",mStart, 0.005);
  fpMinimizer->SetVariable(1,"q",qStart, 0.1);

  fpMinimizer->Minimize();
  status = fpMinimizer->Status();
  chisq = fpMinimizer->MinValue();
  ncalls = fpMinimizer->NCalls();

  if(status==0){
    bestMValue = fpMinimizer->X()[0];
    bestQValue = fpMinimizer->X()[1];
    FillTrack(bestQValue, bestMValue);
    fpTrack->SetIsConv(1);
    if(FootDebugLevel(1)){
      cout<<"TABMMinuitFitter:: NumericalMinimizationDouble done:  fNowView="<<fNowView<<"   The track parameters are:"<<endl;
      cout<<"Pvers=("<<fpTrack->GetSlope().X()<<", "<<fpTrack->GetSlope().Y()<<", "<<fpTrack->GetSlope().Z()<<")"<<endl;
      cout<<"R0=("<<fpTrack->GetOrigin().X()<<", "<<fpTrack->GetOrigin().Y()<<", "<<fpTrack->GetOrigin().Z()<<")"<<endl;
    }    
  } else {  
    FillTrack(qStart, mStart);
    if(FootDebugLevel(1))
      cout<<"TABMMinuitFitter:: NumericalMinimizationDouble: minimization failed: status="<<status<<endl;
  }
  return status;
}

  
void TABMMinuitFitter::ComputeAll(){

  TVector2 vers(fpMinimizer->X()[0],1.);
  TVector2 u0(fpMinimizer->X()[1],0.);
  	
  Double_t chi2sum=0.0, chi2=0.0, chi2red = 0.0, residual;
  for(std::vector<HitPoint>::const_iterator iter = hits.begin(); iter!= hits.end(); ++iter){
    TVector2 wirePos(iter->uwire, iter->zwire);
    residual = iter->rDrift - EvaluateRDrift(u0, vers, wirePos );
    chi2 =residual*residual*(iter->OneOverSigma2);
    TABMhit* p_hit=fpnturaw->GetHit(iter->iHit);
    p_hit->SetResidual(residual);
    p_hit->SetChi2(chi2);
    chi2red += chi2;
  }

  if(fNowView==0)
    fpTrack->SetChiSquareY(chi2red/(nhits-2.));
  else
    fpTrack->SetChiSquareX(chi2red/(nhits-2.));

  if (FootDebugLevel(2))
    cout<<"TABMMinuitFitter::ComputeDataAll finished: track has been reconstructed,  chi2red="<<chi2red/(nhits-2.)<<"  fNowView="<<fNowView<<endl;
}

  
Double_t TABMMinuitFitter::EvaluateChi2(const double *params){

  TVector2 vers(params[0],1.);
  TVector2 u0(params[1],0.);
  
  Double_t chi2=0.0;
  for(std::vector<HitPoint>::const_iterator iter = hits.begin(); 
      iter!= hits.end(); ++iter){
    TVector2 wirePos(iter->uwire, iter->zwire);
    Double_t residual = iter->rDrift - EvaluateRDrift(u0, vers, wirePos );
    chi2 += residual*residual*(iter->OneOverSigma2);
  }

  return chi2;
}

  
Double_t TABMMinuitFitter::EvaluateRDrift(const TVector2 & pos, const TVector2 & vers, const TVector2 & wirePos){
  TVector2 D0 = pos-wirePos;
  Double_t proj = (D0*vers) / vers.Mod();
  return sqrt(D0.Mod2()-proj*proj);
}


void TABMMinuitFitter::FillTrack(Double_t q, Double_t slope){
  if(fNowView==0){
    fpTrack->SetSlopeY(slope);
    fpTrack->SetOriginY(q);
    fpTrack->SethitsNy(nhits);
    if( fpTrack->GetIsConv()==1 )
      fpTrack->SetCovarianceY(fpMinimizer->CovMatrix(0,0), 
			      fpMinimizer->CovMatrix(1,1), 
			      fpMinimizer->CovMatrix(1,0));
  } else {
    fpTrack->SetSlopeX(slope);
    fpTrack->SetOriginX(q);
    fpTrack->SethitsNx(nhits);
    if( fpTrack->GetIsConv()==1 )
      fpTrack->SetCovarianceX(fpMinimizer->CovMatrix(0,0), 
			      fpMinimizer->CovMatrix(1,1), 
			      fpMinimizer->CovMatrix(1,0));
  }
}


void TABMMinuitFitter::EvaluatePulls(TH2F* his){

  TVector2 vers(bestMValue,1.);
  TVector2 u0(bestQValue,0.);
  
  Double_t chi2=0.0;
  for(std::vector<HitPoint>::const_iterator iter = hits.begin(); 
      iter!= hits.end(); ++iter){
    TVector2 wirePos(iter->uwire, iter->zwire);
    Double_t rDriftFit = EvaluateRDrift(u0, vers, wirePos );
    Double_t residual = iter->rDrift - EvaluateRDrift(u0, vers, wirePos );
    // evaluate fit error
    Double_t projprime = (u0-wirePos)*vers/(1.+bestMValue*bestMValue); // not exactly a projection
    Double_t deltaX = (u0.X()-wirePos.X());
    Double_t drOverdq = (deltaX - projprime*bestMValue)/rDriftFit;
    Double_t drOverdm = (-projprime*deltaX);
    Double_t rFitError2 = drOverdm*drOverdm*fpMinimizer->CovMatrix(0,0) +
      drOverdq*drOverdq*fpMinimizer->CovMatrix(1,1) +
      2.*drOverdm*drOverdq*fpMinimizer->CovMatrix(1,0);
    Double_t Sigma2 = 1./iter->OneOverSigma2;
    if( Sigma2 > rFitError2 ){
      Double_t rFitError = sqrt(rFitError2);
      Double_t pull = residual / sqrt(Sigma2-rFitError2);
      his->Fill(pull, iter->rDrift);
    } else {
      cout<<"Pulls: fit errors with problems! Sigma2="<<Sigma2<<" rFitError2="<<rFitError2<<"\n"
	  <<"drOverdm="<<drOverdm <<" drOverdq="<<drOverdq
	  <<" C00="<<fpMinimizer->CovMatrix(0,0)
	  <<" C11="<<fpMinimizer->CovMatrix(1,1)
	  <<" C01="<<fpMinimizer->CovMatrix(0,1)<<"\n";
      ToStream();
    }
  }

}
