#ifndef _TASTparConf_HXX
#define _TASTparConf_HXX
/*!
  \file   TASTparConf.hxx
  \brief   Declaration of TASTparConf, the start counter configuration class
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
class TASTparConf : public TAGparTools
{
  public:
                    TASTparConf();
    virtual         ~TASTparConf();

   
   Int_t    ApplyFFT()                const { return fFFTflag;}
   Int_t    ApplyLowPass()            const { return fApplyLowPassFilter;}
   Int_t    ApplySFCutoff()           const { return fApplySFFilter;}
   Int_t    ApplySFSmoothCutoff()     const { return fApplySFSmoothFilter;}

   Double_t  GetLowPassCutoff()        const{ return fLPFreqCutoff;}
   vector<Double_t>&  GetSFCutoff()          { return fSFFreqCutoff;}
   vector<Double_t>&  GetSFSmoothCutoff()    { return fSFSmoothFreqCutoff;}
  

   Bool_t    FromFile(const TString& name);

  virtual void      Clear(Option_t* opt="");
  virtual  void      ToStream(ostream& os = cout, Option_t* option = "") const;

private:

   TString   fkDefaultParName;
   Int_t     fFFTflag;
   Int_t     fApplyLowPassFilter;
   Int_t     fApplySFFilter;
   Int_t     fApplySFSmoothFilter;

   Double_t   fLPFreqCutoff;
   vector<Double_t> fSFFreqCutoff;
   vector<Double_t> fSFSmoothFreqCutoff;

   ClassDef(TASTparConf,1)
};

#endif
