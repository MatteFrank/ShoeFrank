#ifndef _TABMparCon_HXX
#define _TABMparCon_HXX
/*!
  \file
  \version $Id: TABMparCon.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TABMparCon.
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TVector3.h"
#include "TSpline.h"
#include "TF1.h"
#include "TH1.h"
#include "TRandom3.h"

#include <vector>

#include "TAGpara.hxx"
#include "TAGparTools.hxx"
#include "TABMparMap.hxx"


//##############################################################################
class TABMparCon : public TAGparTools {
  public:
                    TABMparCon();
    virtual         ~TABMparCon();

    //setters
    void SetBMdebug(Int_t deb_in){bm_debug=deb_in; return;};
    void SetIsMC(Bool_t ism){m_isMC=ism; return;};
    void SetRdriftCut(Float_t Rdcut){rdrift_cut=Rdcut; return;};
    void SetEnxcellcut(Float_t Encut){enxcell_cut=Encut; return;};
    void SetT0choice(Int_t in){t0choice=in;};
    
    //getters
    Bool_t   IsMC(){return m_isMC;};
    Float_t GetRdriftCut(){return rdrift_cut;};
    Float_t GetEnxcellcut(){return enxcell_cut;};
    Float_t GetChi2Redcut(){return chi2red_cut;};
    Int_t GetPlanehitcut(){return planehit_cut;};
    Int_t GetMinnhit_cut(){return minnhit_cut;};
    Int_t GetMaxnhit_cut(){return maxnhit_cut;};
    Int_t GetRejmaxcut(){return rejmax_cut;};
    Int_t GetFitterIndex(){return fitter_index;};
    Int_t GetPrefitEnable(){return prefit_enable;};
    Int_t GetBMdebug(){return bm_debug;};
    Int_t GetT0switch(){return t0_switch;};
    Float_t GetT0sigma(){return t0_sigma;};
    Float_t GetHitTimecut(){return hit_timecut;};
    Int_t GetNumIte(){return num_ite;};
    Float_t GetParMove(){return par_move;};
    Int_t GetSmearhits(){return smearhits;};
    Float_t GetFakehitsMean(){return fakehits_mean;};
    Float_t GetFakehitsSigmaLeft(){return fakehits_sigmaleft;};
    Float_t GetFakehitsSigmaRight(){return fakehits_sigmaright;};
    Float_t GetMCEffMean(){return mceff_mean;};
    Float_t GetMCEffSigma(){return mceff_sigma;};
    Int_t GetSmearrdrift(){return smearrdrift;};
    TRandom3* GetRand(){return rand;};
    Float_t GetRdrift_err(){return rdrift_err;};
    Int_t GetT0choice(){return t0choice;};

    //T0 stuff
    void        PrintT0s(TString , TString, Long64_t);
    Bool_t      loadT0s(TString filename); 
    void        SetT0s(vector<Float_t> t0s);
    void        SetT0(Int_t cha, Float_t t0in);   
    Float_t    GetT0(Int_t view, Int_t plane, Int_t cell){return GetT0(cell+view*3+plane*6);};
    Float_t    GetT0(Int_t index_in){return (index_in<36 && index_in>-1) ? v_t0s[index_in]:-1000;};
    void        CoutT0();
    
    //strel stuff
    void LoadSTrel(TString sF);
    Float_t FirstSTrel(Float_t tdrift);
    Float_t InverseStrel(Float_t rdrift);
    Float_t FirstSTrelMC(Float_t tdrift, Int_t mc_switch);    
  
    void LoadReso(TString sF);
    Float_t ResoEval(Float_t dist);

    Bool_t FromFile(const TString& name);
    Bool_t FromFileOld(const TString& name);

    virtual void    Clear(Option_t* opt="");

    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;

    TString         fkDefaultParName;  // default par file name

    ClassDef(TABMparCon,1)

  private:
    Bool_t m_isMC;
    Float_t rdrift_cut;
    Float_t enxcell_cut; //(Gev) if a hit has deposited energy less then energyxcell_cut the hit will be thrown (only for MC events)
    Float_t chi2red_cut;
    Int_t    planehit_cut;//minimum number of plane with at least 1 hit for each view (<=6 && >=3)
    Int_t    minnhit_cut; //cut on the minimum number of hit for the reconstruction algorithm
    Int_t    maxnhit_cut; //cut on the maximum number of hit for the reconstruction algorithm
    Int_t    rejmax_cut; //maximum number of hit that can be rejected during the reconstruction 
    Int_t    fitter_index;
    Int_t    bm_debug;//index for the bm_debug, (it is set at least to the global debug level)
    //~ string   datafile_name;//name of the input data file 
    Int_t    strel_switch;//flag to choose the st relations (1=garfield, 0=FIRST embedded)
    Int_t    prefit_enable;//flag to enable or disable the prefit
    Int_t    t0_switch;//0=t0 from the beginning of the tdc signal, 1=from the peak, 2=negative T0 enabled, 3=peak/2
    Float_t t0_sigma;//t0 with the gaussian shift for the negative T0 hits
    Float_t hit_timecut;//timecut on the lenght of the signal (ns)
    vector<Float_t> v_t0s;//T0 in ns
    vector<Float_t> adc_ped_mean;//pedestals mean 
    vector<Float_t> adc_ped_rms;//pedestals rms
    Int_t    num_ite;//number of iteration for the fit (only for FIRST fit)
    Float_t par_move;//change of the parameters for the FIRST fit
    Int_t    smearhits;//0=no smearhits on MC, 1=smear the number of hits
    Int_t    smearrdrift;//0=no smear rdrift, 1=gauss truncated 1sigma, 2=gaus 2sigma, 3=gaus 3sigma, 4=gaus no truncated, 5=uniform  
    Float_t fakehits_mean;//ratio between the left and the right tail of the hit distribution(only MC)
    Float_t fakehits_sigmaleft;//sigma for the fake hits generator on the left tail(only MC)
    Float_t fakehits_sigmaright;//sigma for the fake hits generator on the right tail (only MC)
    Float_t mceff_mean;//mean for the number of primary hits (only MC)
    Float_t mceff_sigma;//sigma for the number of primary hits (only MC)
    TRandom3 *rand;
    Float_t rdrift_err;  //rdrift default error (used if from parcon file the error isn't loaded)
    Int_t    t0choice; //0=wd, 1=internal
    
    //~ TF1* f_mypol;
    //~ TF1* f_mypol2;
    TSpline3 *m_mySpl;
    vector <TSpline3*> m_myVSpl;
    TF1 *m_myFunSpl;
    vector <TF1*> m_myVFunSpl;
    TH1D *my_hreso;
  
};

#endif
