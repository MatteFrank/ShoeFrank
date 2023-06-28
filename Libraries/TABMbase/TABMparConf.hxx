#ifndef _TABMparConf_HXX
#define _TABMparConf_HXX
/*!
  \file   TABMparConf.hxx
  \brief   Declaration of TABMparConf, the Beam Monitor configuration class
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
#include "TAGrecoManager.hxx"


//##############################################################################
class TABMparConf : public TAGparTools {
  public:
                    TABMparConf();
    virtual         ~TABMparConf();

    void SetChi2Cut(Float_t invalue)               {fChi2Cut=invalue;};
    void SetPlaneHitCut(Int_t invalue)             {fPlaneHitCut=invalue;};
    void SetMinHitCut(Int_t invalue)               {fMinHitCut=invalue;};
    void SetMaxHitCut(Int_t invalue)               {fMaxHitCut=invalue;};
    void SetRejmaxCut(Int_t invalue)               {fRejMaxCut=invalue;};
    void SetHitTimeCut(Float_t invalue)            {fHitTimeCut=invalue;};
    void SetNumIte(Int_t invalue)                  {fNumIte=invalue;};
    void SetParMoveM(Float_t invalue)              {fParMoveM=invalue;};
    void SetParMoveQ(Float_t invalue)              {fParMoveQ=invalue;};
    void SetParToll(Float_t invalue)               {fParToll=invalue;};
    void SetSmearHits(Int_t invalue)               {fSmearHits=invalue;};
    void SetSmearRDrift(Int_t invalue)             {fSmearRDrift=invalue;};
    void SetRDriftErr(Float_t invalue)             {fRDriftErr=invalue;};
    void SetLegMBin(Int_t invalue)                 {fLegMBin=invalue;};
    void SetLegMRange(Float_t invalue)             {fLegMRange=invalue;};
    void SetLegRBin(Int_t invalue)                 {fLegRBin=invalue;};
    void SetLegRRange(Float_t invalue)             {fLegRRange=invalue;};
    void SetAssHitErr(Float_t invalue)             {fAssHitErr=invalue;};
    void SetInvertTrack(Int_t invalue)             {fInvertTrack=invalue;};
    void SetEnThresh(Float_t invalue)              {fEnThresh=invalue;};

    //!Get the reduced chi2 cut applied on the reconstructed tracks, this will be used for the track status flag of TABMtrack
    Float_t GetChi2Cut()                const{return fChi2Cut;};
    //!Get the minimum number of planes with at least 1 hit on each view per event requested by the tracking algorithm to proceed (3)
    Int_t GetPlaneHitCut()              const{return fPlaneHitCut;};
    //!Get the minimum number of hit per event requested by the tracking algorithm to proceed (6)
    Int_t GetMinHitCut()                const{return fMinHitCut;};
    //!Get the maximum number of hit per event requested by the tracking algorithm to proceed (20)
    Int_t GetMaxHitCut()                const{return fMaxHitCut;};
    //!Get the maximum number of hits that can be rejected by the tracking algorithm (10)
    Int_t GetRejmaxCut()                const{return fRejMaxCut;};
    //!Get the maximum drift time accepted for a hit (ns)
    Float_t GetHitTimeCut()             const{return fHitTimeCut;};
    //!Get the maximum number of iterations allowed for the chi2 minimization algorithm (100)
    Int_t GetNumIte()                   const{return fNumIte;};
    //!Get the stepsize adopted for the chi2 minimization algorithm (0.01)
    Float_t GetParMoveM()                const{return fParMoveM;};
    //!Get the stepsize adopted for the chi2 minimization algorithm (0.01)
    Float_t GetParMoveQ()                const{return fParMoveQ;};
    //!Get the stepsize adopted for the chi2 minimization algorithm (0.01)
    Float_t GetParToll()                const{return fParToll;};
    //!Get the flag for the smearing of the number of hits in the MC simulations (false)
    Int_t GetSmearHits()                const{return fSmearHits;};
    //!Get the flag for the smearing of the drift distances in the MC simulations (4=gaussian)
    Int_t GetSmearRDrift()              const{return fSmearRDrift;};
    //!Get the default drift distance uncertainty (150 microns), it is used if the input function isn't loaded
    Float_t GetRdriftErr()              const{return fRDriftErr;};
    //!Get the number of bins for the M parameter in the Legendre polynomy reconstruction algorithm (50)
    Int_t GetLegMBin()                  const{return fLegMBin;};
    //!Get the number of bins for the R parameter in the Legendre polynomy reconstruction algorithm (100)
    Int_t GetLegRBin()                  const{return fLegRBin;};
    //!Get the range for the M parameter in the Legendre polynomy reconstruction algorithm (0.05)
    Float_t GetLegMRange()              const{return fLegMRange;};
    //!Get the range for the R parameter in the Legendre polynomy reconstruction algorithm (2.)
    Float_t GetLegRRange()              const{return fLegRRange;};
    //!Get the number of sigma deviation accepted to associate a hit to a track hypothesis  (3.)
    Float_t GetAssHitErr()              const{return fAssHitErr;};
    //!Get the track inversion flag used to invert the BM tracks on X and/or Y view (0 by default, 2=y axis inversion for GSI2021)
    Int_t   GetInvertTrack()            const{return fInvertTrack;};
    //!Get the energy threshold in GeV for MC hits (0.000001)
    Float_t GetEnThresh()               const{return fEnThresh;};

    //! Reset the hit time cut, this is necessary for MC simulations
    void      ResetHitTimeCutMC(){fHitTimeCut=330;};

    Bool_t    FromFile(const TString& name);
    Bool_t    FromFileOld(const TString& name);
    Bool_t    CheckIsDeadCha(Int_t cha);

    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;

    TString         fkDefaultParName;  ///< default parconf file name ("./config/TABMdetector.cfg")

  private:

    Float_t   fChi2Cut;           ///< cut on the reconstructed track reduced chi2 value, this will be used for the TABMtrack trackstatus
    Int_t     fPlaneHitCut;       ///< minimum number of plane with at least 1 hit for each view (<=6 && >=3)
    Int_t     fMinHitCut;         ///< cut on the minimum number of hit for the reconstruction algorithm (>=6)
    Int_t     fMaxHitCut;         ///< cut on the maximum number of hit for the reconstruction algorithm (20)
    Int_t     fRejMaxCut;         ///< maximum number of hits that can be rejected during the reconstruction (10)
    Float_t   fHitTimeCut;        ///< timecut on the lenght of the signal in ns (320)

    // fitter parameters
    Int_t     fNumIte;            ///< number of iteration for the fit (100)
    Float_t   fParMoveM;           ///< change of parameters stepsize (0.001)
    Float_t   fParMoveQ;           ///< change of parameters stepsize (0.001)
    Float_t   fParToll;           ///< Minuit minimizer tollerance (0.001)
    Int_t     fLegMBin;           ///< number of bin in the m th2d legendre tracking
    Float_t   fLegMRange;         ///< half range for the m th2d in the legendre tracking
    Int_t     fLegRBin;           ///< number of bin in the r th2d legendre tracking
    Float_t   fLegRRange;         ///< half range for the r th2d in the legendre tracking
    Float_t   fAssHitErr;         ///< to select a hit for the reco in checkasshits (high value=more selected hits)
    Int_t     fInvertTrack;       ///< 0= do not invert the reconstructed tracks, 1=invert only X views, 2=invert only Y views, 3=invert both views

    // MC parameters
    Int_t     fSmearHits;         ///< smear the number of BM hits in MC simulations: 0=no fSmearHits, 1=add BM efficiency effect, 2=create also fake noisy hits
    Int_t     fSmearRDrift;       ///< smear the MC drift distances: 0=no smear, 1=gauss truncated 1sigma, 2=gaus 2sigma, 3=gaus 3sigma, 4=gaus no truncated, 5=uniform distribution
    Float_t   fEnThresh;          ///< Energy Threshold in GeV on the particle energy release

    // other parameters
    Float_t   fRDriftErr;         ///< rdrift default error (used if from parcon file the error isn't loaded)
    TArrayI   fDeadCha;           ///< BM dead channels array

   ClassDef(TABMparConf,1)
};

#endif
