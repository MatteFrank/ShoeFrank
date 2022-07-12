#define GlobalRecoMassAna_cxx

#include <TVector3.h>
#include <TRandom.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "GlobalRecoMassAna.hxx"
#include "GlobalRecoAna.hxx"

using namespace std;


GlobalRecoMassAna::GlobalRecoMassAna(){
  chi2min=999;
  almmin=999;
  fpChiFunctor= new ROOT::Math::Functor(this,&GlobalRecoMassAna::EvaluateChi,4);
  fpChiMinimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2"); //minimization algorithm to be studied
  fpAlmFunctor= new ROOT::Math::Functor(this,&GlobalRecoMassAna::EvaluateAlm,4);
  fpAlmMinimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2"); //minimization algorithm to be studied

  //to be optimized
  maxMinimizerIteChi=1000;
  maxMinimizerFuncCallChi=1000;
  maxMinimizerIteAlm=1000;
  maxMinimizerFuncCallAlm=1000;
  toleranceChi=0.000001;
  toleranceAlm=0.000001;
  almMaxIter=1000;
  almPenalty=0.1;
  mass_wavg=-1;
  mass_pe=-1;
  mass_be=-1;
  mass_pb=-1;
  mass_wavg=-1,
  input_status=1;
  chifit_status=-1;
  almfit_status=-1;
  lagmult.SetXYZ(0.,0.,0.);
  convec.SetXYZ(0.,0.,0.);
}


GlobalRecoMassAna::~GlobalRecoMassAna(){
  delete fpChiFunctor;
  delete fpChiMinimizer;
  delete fpAlmFunctor;
  delete fpAlmMinimizer;
}

void GlobalRecoMassAna::Clear(){
  chi2min=999;
  almmin=999;
  lagmult.SetXYZ(0.,0.,0.);
  convec.SetXYZ(0.,0.,0.);
  input_status=1;
  chifit_status=-1;
  almfit_status=-1;
  return;
}

void GlobalRecoMassAna::NewMass(Double_t inmom, Double_t inlength, Double_t intof, Double_t inekin, Double_t insigma_tof, Double_t insigma_mom, Double_t insigma_ekin){
  Clear();
  mom_meas=inmom;
  length_meas=inlength;
  tof_meas=intof;
  ekin_meas=inekin;
  sigma_tof=insigma_tof;
  sigma_mom=insigma_mom;
  sigma_ekin=insigma_ekin;
  beta_meas=GetBetaFromTof(intof);
  gamma_meas=GetGamma(beta_meas);

  if(mom_meas>0 && sigma_mom>0)  input_status*=2;
  if(beta_meas>0 && beta_meas<1 && sigma_tof>0)  input_status*=3;
  if(ekin_meas>0 && sigma_ekin>0)  input_status*=5;

  if(input_status%6==0){
    mass_pb=GetMassPB(mom_meas,beta_meas);
    mass_errpb=sqrt(sigma_tof*mom_meas*gamma_meas/tof_meas*sigma_tof*mom_meas*gamma_meas/tof_meas+sigma_mom/gamma_meas*sigma_mom/gamma_meas)/beta_meas/TAGgeoTrafo::AmuToGev();
  }
  if(input_status%15==0){
    mass_be=GetMassBE(beta_meas, ekin_meas);
    mass_errbe=sqrt(sigma_tof*ekin_meas*beta_meas*beta_meas*gamma_meas*gamma_meas*gamma_meas/(gamma_meas-1.)/tof_meas*sigma_tof*ekin_meas*beta_meas*beta_meas*gamma_meas*gamma_meas*gamma_meas/(gamma_meas-1.)/tof_meas+sigma_ekin*sigma_ekin)/(gamma_meas-1.)/TAGgeoTrafo::AmuToGev();
  }
  if(input_status%10==0){
    mass_pe=GetMassPE(mom_meas, ekin_meas);
    mass_errpe=sqrt(mom_meas*sigma_mom*mom_meas*sigma_mom+sigma_ekin*(ekin_meas*ekin_meas+mom_meas*mom_meas)/2./ekin_meas*sigma_ekin*(ekin_meas*ekin_meas+mom_meas*mom_meas)/2./ekin_meas)/ekin_meas/TAGgeoTrafo::AmuToGev();
  }

  if(input_status==6){
    mass_wavg=mass_pb;
    mass_errwavg=mass_errpb;
  }else if(input_status==10){
    mass_wavg=mass_pe;
    mass_errwavg=mass_errpe;
  }else if(input_status==15){
    mass_wavg=mass_be;
    mass_errwavg=mass_errbe;
  }else if(input_status==30){
    mass_errwavg=sqrt(1./(1./mass_errpe+1./mass_errpb+1./mass_errpe));
    mass_wavg=(mass_pb/mass_errpb+mass_pe/mass_errpe+mass_be/mass_errbe)/(1./mass_errpe+1./mass_errpb+1./mass_errbe);

    TMatrixD Cmat(3,3);
   //A1 (mass_pb with tof and mom)
    Cmat[0][0]=sigma_tof*mom_meas*gamma_meas/TAGgeoTrafo::AmuToGev()/beta_meas/tof_meas;
    Cmat[0][1]=sigma_mom/TAGgeoTrafo::AmuToGev()/beta_meas/gamma_meas;
    Cmat[0][2]=0.;
    //A2 (mass_be with tof and ekin)
    Cmat[1][0]=sigma_tof*ekin_meas*beta_meas*beta_meas*gamma_meas*gamma_meas*gamma_meas/(gamma_meas-1.)/(gamma_meas-1.)/tof_meas/TAGgeoTrafo::AmuToGev();
    Cmat[1][1]=0;
    Cmat[1][2]=sigma_ekin/TAGgeoTrafo::AmuToGev()/(gamma_meas-1.);
    //A3 (mass_pe with mom and ekin)
    Cmat[2][0]=0.;
    Cmat[2][1]=mom_meas/ekin_meas/TAGgeoTrafo::AmuToGev()*sigma_mom;
    Cmat[2][2]=sigma_ekin*(ekin_meas*ekin_meas+mom_meas*mom_meas)/2./ekin_meas/ekin_meas/TAGgeoTrafo::AmuToGev();
    TMatrixD CmatTrans(TMatrixD(TMatrixD::kTransposed, Cmat));
    Bmat.ResizeTo(3,3);
    Bmat=Cmat*CmatTrans;
    Bmat.Invert();
    if(!Bmat.IsValid()){
      input_status=0;
      if(FootDebugLevel(1))
        cout<<"GlobalRecoMassAna::NewMass: Error! Bmat is not valid! Mass minimization with chi2 not possible"<<endl;
    }
  }

  return ;
}


Double_t GlobalRecoMassAna::EvaluateChi(const double *params){ //params: 0=mom;1=ekin;2=tof;3=mass
  Double_t chi2=(params[0]-mom_meas)*(params[0]-mom_meas)/sigma_mom/sigma_mom
               +(params[1]-ekin_meas)*(params[1]-ekin_meas)/sigma_ekin/sigma_ekin
               +(params[2]-tof_meas)*(params[2]-tof_meas)/sigma_tof/sigma_tof;

  TVectorD Avec(3);
  Avec[0]=GetMassPB(params[0],GetBetaFromTof(params[2]))-params[3];
  Avec[1]=GetMassBE(GetBetaFromTof(params[2]), params[1])-params[3];
  Avec[2]=GetMassPE(params[0], params[1])-params[3];

  TVectorD tmpvec=Bmat * Avec;
  // cout<<chi2<<"  "<<tmpvec[0]*Avec[0]+tmpvec[1]*Avec[1]+tmpvec[2]*Avec[2]<<endl;
  return chi2+tmpvec[0]*Avec[0]+tmpvec[1]*Avec[1]+tmpvec[2]*Avec[2];

}


void GlobalRecoMassAna::FitChiMass(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoMassAna::FitChiMass start chi2 minimization"<<endl;

    // PrintAllValues();
  fpChiMinimizer->SetFunction(*fpChiFunctor);
  fpChiMinimizer->SetMaxFunctionCalls(maxMinimizerFuncCallChi);  //not optimized yet!!!
  fpChiMinimizer->SetMaxIterations(maxMinimizerIteChi);
  fpChiMinimizer->SetTolerance(toleranceChi);           //not optimized yet!!!
  fpChiMinimizer->SetPrintLevel(0);
  fpChiMinimizer->SetVariable(0,"mom",0., toleranceChi);  //the numerical value indicates the variable step, not optimized yet!!
  fpChiMinimizer->SetVariable(1,"ekin",0., toleranceChi);
  fpChiMinimizer->SetVariable(2,"tof",0., toleranceChi);
  fpChiMinimizer->SetVariable(3,"mass",0., toleranceChi);
  fpChiMinimizer->SetVariableValue(0,mom_meas);
  fpChiMinimizer->SetVariableValue(1,ekin_meas);
  fpChiMinimizer->SetVariableValue(2,tof_meas);
  fpChiMinimizer->SetVariableValue(3,mass_wavg);

  fpChiMinimizer->Minimize();

  chifit_status=fpChiMinimizer->Status();
  if(chifit_status==0){
    mom_chi=fpChiMinimizer->X()[0];
    mom_errchi=fpChiMinimizer->Errors()[0];
    ekin_chi=fpChiMinimizer->X()[1];
    ekin_errchi=fpChiMinimizer->Errors()[1];
    tof_chi=fpChiMinimizer->X()[2];
    tof_errchi=fpChiMinimizer->Errors()[2];
    mass_chi=fpChiMinimizer->X()[3];
    mass_errchi=fpChiMinimizer->Errors()[3];
    // chi2min=fpChiMinimizer->MinValue();
    double param[4]={mom_chi, ekin_chi, tof_chi, mass_chi};
    chi2min=EvaluateChi(param);

    if(FootDebugLevel(1))
      PrintAllValues();

  }else if(FootDebugLevel(1))
    cout<<"GlboalRecoMassAna:: FitChiMass failed; status="<<chifit_status<<endl;

  fpChiMinimizer->Clear();

  return;
}

Double_t GlobalRecoMassAna::EvaluateAlm(const double *params){ //params: 0=mom;1=ekin;2=tof;3=mass
  Double_t newmasspb=GetMassPB(params[0],GetBetaFromTof(params[2]));
  Double_t newmassbe=GetMassBE(GetBetaFromTof(params[2]), params[1]);
  Double_t newmasspe=GetMassPE(params[0],params[1]);

  return (params[0]-mom_meas)*(params[0]-mom_meas)/sigma_mom/sigma_mom
               +(params[1]-ekin_meas)*(params[1]-ekin_meas)/sigma_ekin/sigma_ekin
               +(params[2]-tof_meas)*(params[2]-tof_meas)/sigma_tof/sigma_tof
               + lagmult[0]*(newmasspb-params[3])
               + lagmult[1]*(newmassbe-params[3])
               + lagmult[2]*(newmasspe-params[3])
               +0.5/almPenalty*((newmasspb-params[3])*(newmasspb-params[3]) + (newmassbe-params[3])*(newmassbe-params[3]) + (newmasspe-params[3])*(newmasspe-params[3]));
}

Int_t GlobalRecoMassAna::MinimizeAlm(){
  fpAlmMinimizer->SetFunction(*fpAlmFunctor);
  fpAlmMinimizer->SetMaxFunctionCalls(maxMinimizerFuncCallAlm);  //not optimized yet!!!
  fpAlmMinimizer->SetMaxIterations(maxMinimizerIteAlm);
  fpAlmMinimizer->SetTolerance(toleranceAlm);           //not optimized yet!!!
  fpAlmMinimizer->SetPrintLevel(0);
  // fpAlmMinimizer->SetLimitedVariable(0,"mom",mom_meas, toleranceAlm,0.,mom_meas*5);  //the numerical value indicates the variable step, not optimized yet!!
  // fpAlmMinimizer->SetLimitedVariable(1,"ekin",ekin_meas, toleranceAlm,0., ekin_meas*5);
  // fpAlmMinimizer->SetLimitedVariable(2,"tof",tof_meas, toleranceAlm, 0., 1.);
  // fpAlmMinimizer->SetLimitedVariable(3,"mass",mass_wavg, toleranceAlm,0.,mass_wavg*2);
  fpAlmMinimizer->SetVariable(0,"mom",0., toleranceAlm);  //the numerical value indicates the variable step, not optimized yet!!
  fpAlmMinimizer->SetVariable(1,"ekin",0., toleranceAlm);
  fpAlmMinimizer->SetVariable(2,"tof",0., toleranceAlm);
  fpAlmMinimizer->SetVariable(3,"mass",0., toleranceAlm);
  fpAlmMinimizer->SetVariableValue(0,mom_alm);
  fpAlmMinimizer->SetVariableValue(1,ekin_alm);
  fpAlmMinimizer->SetVariableValue(2,tof_alm);
  fpAlmMinimizer->SetVariableValue(3,mass_alm);

  fpAlmMinimizer->Minimize();

  convec.SetX(GetMassPB(fpAlmMinimizer->X()[0], GetBetaFromTof(fpAlmMinimizer->X()[2])) - fpAlmMinimizer->X()[3]);
  convec.SetY(GetMassBE(GetBetaFromTof(fpAlmMinimizer->X()[2]), fpAlmMinimizer->X()[1]) - fpAlmMinimizer->X()[3]);
  convec.SetZ(GetMassPE(fpAlmMinimizer->X()[0], fpAlmMinimizer->X()[1]) - fpAlmMinimizer->X()[3]);

  mom_alm=fpAlmMinimizer->X()[0];
  mom_erralm=fpAlmMinimizer->Errors()[0];
  ekin_alm=fpAlmMinimizer->X()[1];
  ekin_erralm=fpAlmMinimizer->Errors()[1];
  tof_alm=fpAlmMinimizer->X()[2];
  tof_erralm=fpAlmMinimizer->Errors()[2];
  mass_alm=fpAlmMinimizer->X()[3];
  mass_erralm=fpAlmMinimizer->Errors()[3];
  almmin=fpAlmMinimizer->MinValue();



  return fpAlmMinimizer->Status();
}

void GlobalRecoMassAna::FitAlmMass(){

  Int_t nalmiter;
  Double_t oldchi2=-1000;

  if(FootDebugLevel(1))
    cout<<"GlobalRecoMassAna::FitAlmMass start alm minimization"<<endl;

  //set initial alm values:
  mom_alm=mom_meas;
  ekin_alm=ekin_meas;
  tof_alm=tof_meas;
  mass_alm=mass_wavg;

  for(nalmiter=0;nalmiter<almMaxIter;nalmiter++){
    almfit_status=MinimizeAlm();
    if(almfit_status!=0){
      fpAlmMinimizer->Clear();
      if(FootDebugLevel(1))
        cout<<"GlboalRecoMassAna:: FitAlmMass failed; almfit_status="<<almfit_status<<"  iteration="<<nalmiter<<endl;
      break;
    }
    if(FootDebugLevel(1))
      cout<<"GlboalRecoMassAna:: FitAlmMass iteration number:"<<nalmiter<<"/"<<almMaxIter<<"; almfit_status="<<almfit_status<<"  new minimum="<<fpAlmMinimizer->MinValue()<<endl;
    if(fabs(oldchi2-fpAlmMinimizer->MinValue())<toleranceAlm){
      mom_alm=fpAlmMinimizer->X()[0];
      mom_erralm=fpAlmMinimizer->Errors()[0];
      ekin_alm=fpAlmMinimizer->X()[1];
      ekin_erralm=fpAlmMinimizer->Errors()[1];
      tof_alm=fpAlmMinimizer->X()[2];
      tof_erralm=fpAlmMinimizer->Errors()[2];
      mass_alm=fpAlmMinimizer->X()[3];
      mass_erralm=fpAlmMinimizer->Errors()[3];
      almmin=fpAlmMinimizer->MinValue();
      break;
    }else{
      oldchi2=fpAlmMinimizer->MinValue();
      for(Int_t i=0;i<3;i++)
        lagmult[i] = lagmult[i] + convec[i]/almPenalty;
    }
  }

  if(FootDebugLevel(1))
    PrintAllValues();

  if(FootDebugLevel(1))
   cout<<"GlboalRecoMassAna:: FitAlmMass done; status="<<almfit_status<<endl;

  return ;
}


Bool_t GlobalRecoMassAna::CombineChi2Alm(){
  if(chi2min!=999 && almmin!=999){
    mom_best=(mom_chi/mom_errchi+mom_alm/mom_erralm)/(1./mom_errchi+1./mom_erralm);
    mom_errbest=sqrt(1./(1./mom_errchi+1./mom_erralm));
    ekin_best=(ekin_chi/ekin_errchi+ekin_alm/ekin_erralm)/(1./ekin_errchi+1./ekin_erralm);
    ekin_errbest=sqrt(1./(1./ekin_errchi+1./ekin_erralm));
    tof_best=(tof_chi/tof_errchi+tof_alm/tof_erralm)/(1./tof_errchi+1./tof_erralm);
    tof_errbest=sqrt(1./(1./tof_errchi+1./tof_erralm));
    mass_best=(mass_chi/mass_errchi+mass_alm/mass_erralm)/(1./mass_errchi+1./mass_erralm);
    mass_errbest=sqrt(1./(1./mass_errchi+1./mass_erralm));
  }else if(chi2min!=999 && almmin==999){
    mom_best=mom_chi;
    mom_errbest=mom_errchi;
    ekin_best=ekin_chi;
    ekin_errbest=ekin_errchi;
    tof_best=tof_chi;
    tof_errbest=tof_errchi;
    mass_best=mass_chi;
    mass_errbest=mass_errchi;
  }else if(chi2min==999 && almmin!=999){
    mom_best=mom_alm;
    mom_errbest=mom_erralm;
    ekin_best=ekin_alm;
    ekin_errbest=ekin_erralm;
    tof_best=tof_alm;
    tof_errbest=tof_erralm;
    mass_best=mass_alm;
    mass_errbest=mass_erralm;
  }else{
    cout<<"GlobalRecoAna::CombineCh2Alm::Error! chi2min=999 and almmin=999, the mass calculation has failed!"<<endl;
    return -3;
  }
  return 0;
}


void GlobalRecoMassAna::PrintAllValues(){

  cout<<"GlobalRecoMassAna measured values:"<<endl;
  cout<<"mom_meas="<<mom_meas<<"  ekin_meas="<<ekin_meas<<"  tof_meas="<<tof_meas<<"  length_meas="<<length_meas<<"  beta_meas="<<beta_meas<<"  gamma_meas="<<gamma_meas<<"  sigma_mom="<<sigma_mom<<"  sigma_ekin="<<sigma_ekin<<"  sigma_tof="<<sigma_tof<<endl;
  cout<<"mass estimated values:"<<endl;
  cout<<"mass_pb="<<mass_pb<<"  mass_be="<<mass_be<<"  mass_pe="<<mass_pe<<"  mass_wavg="<<mass_wavg<<endl;
  cout<<"Chi2 estimated values:"<<endl;
  cout<<"mom_chi="<<mom_chi<<"  mom_errchi="<<mom_errchi<<"  ekin_chi="<<ekin_chi<<"  ekin_errchi"<<ekin_errchi<<"  tof_chi="<<tof_chi<<"  tof_errchi="<<tof_errchi<<"  mass_chi="<<mass_chi<<"  mass_errchi="<<mass_errchi<<"  chi2min="<<chi2min<<endl;
  cout<<"Augmented Lagrangian estimated values:"<<endl;
  cout<<"mom_alm="<<mom_alm<<"  mom_erralm="<<mom_erralm<<"  ekin_alm="<<ekin_alm<<"  ekin_erralm="<<ekin_erralm<<"  tof_alm="<<tof_alm<<"  tof_erralm="<<tof_erralm<<"  mass_alm="<<mass_alm<<"  mass_erralm="<<mass_erralm<<"  almmin="<<almmin<<endl;
  cout<<"Combined estimated values:"<<endl;
  cout<<"mom_best="<<mom_best<<"  mom_errbest="<<mom_errbest<<"  ekin_best="<<ekin_best<<"  ekin_errbest="<<ekin_errbest<<"  tof_best="<<tof_best<<"  tof_errbest="<<tof_errbest<<"  mass_best="<<mass_best<<endl;

  return;
}
