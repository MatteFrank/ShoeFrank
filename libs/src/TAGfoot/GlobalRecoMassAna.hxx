#ifndef _GlobalRecoMassAna_hxx_
#define _GlobalRecoMassAna_hxx_

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TString.h>
#include <TVectorD.h>
#include <TMatrixD.h>

#include "Math/Minimizer.h"

#include "TAGobject.hxx"
#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrunInfo.hxx"


using namespace std;


class GlobalRecoMassAna : public TObject {

  public:
    GlobalRecoMassAna();
    ~GlobalRecoMassAna();
    void Clear();
    void NewMass(Double_t inmom, Double_t inlength, Double_t intof, Double_t inekin, Double_t insigma_tof, Double_t insigma_mom, Double_t insigma_ekin);

    //getters:
    Double_t GetMassPb()  {return mass_pb;};
    Double_t GetMassPe()  {return mass_pe;};
    Double_t GetMassBe()  {return mass_be;};
    Double_t GetMassWavg()  {return mass_wavg;};
    Double_t GetMassErrPb()  {return mass_errpb;};
    Double_t GetMassErrPe()  {return mass_errpe;};
    Double_t GetMassErrBe()  {return mass_errbe;};
    Double_t GetMassErrWavg()  {return mass_errwavg;};
    Double_t GetMomMeas() 	{return mom_meas;};
    Double_t GetEkinMeas()	{return ekin_meas;};
    Double_t GetTofMeas() 	{return tof_meas;};

    Double_t GetMomBest() 	{return mom_best;};
    Double_t GetMomErrBest() 	{return mom_errbest;};
    Double_t GetEkinBest()	{return ekin_best;};
    Double_t GetEkinErrBest()	{return ekin_errbest;};
    Double_t GetTofBest() 	{return tof_best;};
    Double_t GetTofErrBest() 	{return tof_errbest;};
    Double_t GetMassBest() {return mass_best;};
    Double_t GetMassErrBest()  {return mass_errbest;};

    Double_t GetMomAlm() 	{return mom_alm;};
    Double_t GetMomErrAlm() 	{return mom_erralm;};
    Double_t GetEkinAlm()	{return ekin_alm;};
    Double_t GetEkinErrAlm()	{return ekin_erralm;};
    Double_t GetTofAlm() 	{return tof_alm;};
    Double_t GetTofErrAlm() 	{return tof_erralm;};
    Double_t GetMassAlm() {return mass_alm;};
    Double_t GetMassErrAlm()  {return mass_erralm;};

    Double_t GetMomChi() 	{return mom_chi;};
    Double_t GetMomErrChi()  {return mom_errchi;};
    Double_t GetEkinChi()	{return ekin_chi;};
    Double_t GetEkinErrChi()  {return ekin_errchi;};
    Double_t GetTofChi() 	{return tof_chi;};
    Double_t GetTofErrChi()  {return tof_errchi;};
    Double_t GetMassChi() {return mass_chi;};
    Double_t GetMassErrChi()  {return mass_errchi;};

    Double_t GetChiValue() {return chi2min;};
    Double_t GetAlmValue() {return almmin;};
    Int_t    GetInputStatus() {return input_status;};
    Int_t    GetAlmFitStatus() {return almfit_status;};
    Int_t    GetChiFitStatus() {return chifit_status;};

    void SetAlmPenalty(Double_t in_almpenalty){almPenalty=in_almpenalty;};
    Double_t GetAlmPenalty() {return almPenalty;};

    //chi2 and alm minimization methods
    void FitChiMass();
    Double_t EvaluateChi(const double *params); //used as functor for FitChiMass
    void FitAlmMass();
    Int_t MinimizeAlm();   //return 0 if ok
    Double_t EvaluateAlm(const double *params); //used as functor for FitAlmMass

    //other methods
    Double_t GetGamma(Double_t beta){return 1./sqrt(1.-beta*beta);};
    Double_t GetMassPB(Double_t mom, Double_t beta) {return mom*sqrt(1.-beta*beta)/beta/TAGgeoTrafo::AmuToGev();};
    Double_t GetMassBE(Double_t beta, Double_t ekin){return ekin/(GetGamma(beta)-1)/TAGgeoTrafo::AmuToGev();};
    Double_t GetMassPE(Double_t mom, Double_t ekin) {return (mom*mom-ekin*ekin)/2./ekin/TAGgeoTrafo::AmuToGev();};
    Double_t GetBetaFromTof(Double_t tof){return length_meas/tof/TAGgeoTrafo::GetLightVelocity();};
    Bool_t CombineChi2Alm();          //combine chi2 and alm resutls
    void     PrintAllValues();        //cout all the private values, useful for debug

  private:

    //chi2 and alm minimization ROOT based
    ROOT::Math::Minimizer* fpChiMinimizer;
    ROOT::Math::Functor* fpChiFunctor;
    ROOT::Math::Minimizer* fpAlmMinimizer;
    ROOT::Math::Functor* fpAlmFunctor;

  	//measured input values
    Int_t input_status;  //input variable status:0=error, 1=no input, 2=Momentum ok, 3=Beta Ok, 5=Energy ok, other values=prod (e.g.: 30=all ok)

  	Double_t mom_meas;		//momentum from measurements
  	Double_t ekin_meas;		//ekin from measurements
  	Double_t tof_meas;		//tof from measurements
  	Double_t length_meas;	//length from measurements
  	Double_t beta_meas;		//beta from measurements
  	Double_t gamma_meas;	//gamma from measurements

    Double_t sigma_mom;   //momentum resolution
    Double_t sigma_ekin;  //energy resolution
    Double_t sigma_tof;  //tof resolution

    Double_t mass_pb;     //mass from momentum and beta (A0)
    Double_t mass_be;     //mass from beta and Ekin (A1)
    Double_t mass_pe;     //mass from momentum and ekin (A3)
    Double_t mass_wavg;   //first estimate of the mass with input measurements and weighted average, calculated from SimpleCombineMeas
    Double_t mass_errpb;     //mass from momentum and beta (A0)
    Double_t mass_errbe;     //mass from beta and Ekin (A1)
    Double_t mass_errpe;     //mass from momentum and ekin (A3)
    Double_t mass_errwavg;   //first estimate of the mass with input measurements and weighted average, calculated from SimpleCombineMeas

  	//chi2
  	Double_t mom_chi;	    	//momentum optimized from chi square method
  	Double_t mom_errchi;		//momentum error from chi square method
  	Double_t ekin_chi;		  //ekin optimized from chi square method
  	Double_t ekin_errchi;		//ekin error from chi square method
  	Double_t tof_chi;		    //beta optimized from chi square method
  	Double_t tof_errchi;		//beta error from chi square method
    Double_t mass_chi;      //mass estimate from chi2 optimization
    Double_t mass_errchi;   //mass error from chi2 optimization
    Int_t chifit_status;    //fitting status:-1=fit not done, 0=ok, otherwise check root minimizer status meaning (https://root-forum.cern.ch/t/is-fit-validity-or-minimizer-status-more-important/30637)

  	//augmented lagrangian
  	Double_t mom_alm;		//momentum optimized from augmented lagrangian method
  	Double_t mom_erralm;		//momentum optimized from augmented lagrangian method
  	Double_t ekin_alm;		//ekin optimized from augmented lagrangian method
  	Double_t ekin_erralm;		//ekin optimized from augmented lagrangian method
  	Double_t tof_alm;		//beta optimized from augmented lagrangian method
  	Double_t tof_erralm;		//beta optimized from augmented lagrangian method
    Double_t mass_alm;     //mass from alm optimization
    Double_t mass_erralm;     //mass from alm optimization
    Int_t almfit_status;    //fitting status:-1=fit not done, 0=ok, otherwise check root minimizer status meaning (https://root-forum.cern.ch/t/is-fit-validity-or-minimizer-status-more-important/30637)

  	//chi2 and auglag combined results
  	Double_t mom_best;		  //best momentum estimate
  	Double_t mom_errbest;		//best momentum error
  	Double_t ekin_best;	  	//best ekin estimate
  	Double_t ekin_errbest;	//best ekin error
  	Double_t tof_best;		  //best beta estimate
  	Double_t tof_errbest;	 	//best beta error
  	Double_t mass_best;		  //best mass estimate
  	Double_t mass_errbest;	//best mass error

  	Double_t chi2min;   //chi square value minimum value (default 999)
  	Double_t almmin;    //lagrangian minimum value (default 999)

    //fitting Parameters
    Int_t maxMinimizerIteChi;       //number of max iterations for the minuit function
    Int_t maxMinimizerFuncCallChi;  //max function calls for the minuit function
    Double_t toleranceChi;          //tolerance of chi2 minimization method
    Int_t almMaxIter;               //Max number of iterations for the alm algorithm
    Int_t maxMinimizerIteAlm;       //Max number of alm minimizer iterations for the alm minimizer function used in each alm iteration
    Int_t maxMinimizerFuncCallAlm;  //Max number of calls for the alm minimizer function used in each alm iteration
    Double_t toleranceAlm;          //tolerance of alm minimization method chi square difference
    TVector3 lagmult;               //Lagrangian multipliers for alm
    TVector3 convec;                //Lagrangian differences for alm
    Double_t almPenalty;            //Lagrangian penalty factor for alm

    //nuisance variables
    TMatrixD Bmat;     //used for chi2 minimization

};


#endif
