#ifndef _TABMMinuitFitter_HXX
#define _TABMMinuitFitter_HXX
/*!
  \file
  \brief   Declaration of TABMMinuitFitter.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGnamed.hxx"
#include "Math/Minimizer.h"
#include "TFitter.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TVector2.h"

struct HitPoint {
  Int_t    iHit;
  Double_t zwire;
  Double_t uwire;
  Double_t rDrift;
  Double_t OneOverSigma2;
};
  
class TABMparConf;
class TABMntuHit;
class TABMtrack;
class TH2F;

class TABMMinuitFitter : public TAGnamed {
public:

  explicit TABMMinuitFitter(const char* name=0, TABMparConf* p_bmcon=0);
  
  virtual ~TABMMinuitFitter();

  //! To stream
  virtual void ToStream(ostream& os=cout, Option_t* option="") const;

  void Prepare(TABMntuHit* p_nturaw, Int_t iTrackNum, Int_t iView);
  
  Int_t Minimize(Float_t rStart, Float_t mStart, TABMtrack* apTrack);
  
  void ComputeAll();
  
  Double_t EvaluateChi2(const double *params);
  
  static Double_t EvaluateRDrift(const TVector2 & pos, const TVector2 & vers, 
				 const TVector2 & wirePos);

  void EvaluatePulls(TH2F* his);

  // Getters and NO Setters!
  Int_t  Status() const { return status;};
  Double_t Qfit() const { return bestQValue;};
  Double_t Mfit() const { return bestMValue;};
  Double_t Chisq() const { return chisq;}
  Int_t    NCalls() const { return ncalls;}


protected: 
  
  void FillTrack(Double_t q, Double_t slope);  
  
private:

  TABMntuHit* fpnturaw;
  Int_t    fTrackNum;
  Int_t    fNowView;
  TABMtrack* fpTrack;     // adopted for reconstruction

  TABMparConf* fpbmcon;
   
  //chi2 minimization ROOT minuit2 based
  ROOT::Math::Minimizer* fpMinimizer;
  ROOT::Math::Functor* fpFunctor;
  // fit results
  Int_t    status;
  Double_t chisq;
  Double_t bestMValue;
  Double_t bestQValue;
  Int_t    ncalls;

  std::vector<HitPoint> hits;
  int nhits;
  
  ClassDef(TABMMinuitFitter,0)
};
 
#endif
