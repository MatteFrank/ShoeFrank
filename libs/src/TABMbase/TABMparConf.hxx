#ifndef _TABMparConf_HXX
#define _TABMparConf_HXX
/*!
  \file
  \version $Id: TABMparConf.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TABMparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TVector3.h"
#include "TSpline.h"
#include "TF1.h"
#include "TH1.h"

#include <vector>

#include "TAGpara.hxx"
#include "TAGparTools.hxx"
#include "GlobalPar.hxx"


//##############################################################################
class TABMparConf : public TAGparTools {
  public:
                    TABMparConf();
    virtual         ~TABMparConf();

    //getters
    Float_t GetChi2Cut()                const{return fChi2Cut;};
    Int_t GetPlaneHitCut()              const{return fPlaneHitCut;};
    Int_t GetMinHitCut()                const{return fMinHitCut;};
    Int_t GetMaxHitCut()                const{return fMaxHitCut;};
    Int_t GetRejmaxCut()                const{return fRejMaxCut;};
    Float_t GetHitTimeCut()             const{return fHitTimeCut;};
    Int_t GetNumIte()                   const{return fNumIte;};
    Float_t GetParMove()                const{return fParMove;};
    Int_t GetSmearHits()                const{return fSmearHits;};
    Int_t GetSmearRDrift()              const{return fSmearRDrift;};
    Float_t GetRdriftErr()              const{return fRDriftErr;};
    Int_t GetLegMBin()                  const{return fLegMBin;};
    Int_t GetLegRBin()                  const{return fLegRBin;};
    Float_t GetLegMRange()              const{return fLegMRange;};
    Float_t GetLegRRange()              const{return fLegRRange;};
    Float_t GetAssHitErr()              const{return fAssHitErr;};


    //others
    Bool_t    FromFile(const TString& name);//read the config file
    Bool_t    FromFileOld(const TString& name);//read the old config file adopted in bm_calibration
    void      ResetHitTimeCutMC(){fHitTimeCut=330;}; //for MC studies

    //default
    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
    TString         fkDefaultParName;  // default par file name

    ClassDef(TABMparConf,1)

  private:

    //cuts
    Float_t   fChi2Cut;
    Int_t     fPlaneHitCut;       //minimum number of plane with at least 1 hit for each view (<=6 && >=3)
    Int_t     fMinHitCut;         //cut on the minimum number of hit for the reconstruction algorithm
    Int_t     fMaxHitCut;         //cut on the maximum number of hit for the reconstruction algorithm
    Int_t     fRejMaxCut;         //maximum number of hit that can be rejected during the reconstruction
    Float_t   fHitTimeCut;        //timecut on the lenght of the signal (ns)

    //fitter parameters
    Int_t     fNumIte;            //number of iteration for the fit
    Float_t   fParMove;           //change of parameters stepsize
    Int_t     fLegMBin;           //number of bin in the m th2d legendre tracking
    Float_t   fLegMRange;         //half range for the m th2d in the legendre tracking
    Int_t     fLegRBin;           //number of bin in the r th2d legendre tracking
    Float_t   fLegRRange;         //half range for the r th2d in the legendre tracking
    Float_t   fAssHitErr;         //to select a hit for the reco in checkasshits (high value=more selected hits)

    //MC parameters
    Int_t     fSmearHits;         //0=no fSmearHits on MC, 1=smear the number of hits
    Int_t     fSmearRDrift;       //0=no smear rdrift, 1=gauss truncated 1sigma, 2=gaus 2sigma, 3=gaus 3sigma, 4=gaus no truncated, 5=uniform

    //other parameters
    Float_t   fRDriftErr;         //rdrift default error (used if from parcon file the error isn't loaded)
};

#endif