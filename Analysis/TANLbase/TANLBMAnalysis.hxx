#ifndef TANLBMAnalysis_h
#define TANLBMAnalysis_h

#include "TANLbaseAnalysis.hxx"
#include <list>

class TH1D;
class TH2D;

class TANLBMAnalysis : public TANLbaseAnalysis {
public:

  TANLBMAnalysis();

  virtual ~TANLBMAnalysis();

  //
  virtual void Setup();

  virtual void BeforeEventLoop();

  virtual void ProcessEvent();

  virtual void AfterEventLoop();

private:

  void Booking();

  enum hist1Name {ntracks,chix,chiy,nx,ny,probx,proby, prob3, prob4, prob5, prob6,
		  XposGlb,YposGlb,ThetaLoc,PhiLoc,ThetaGlb,PhiGlb,
		  last1d};
  enum hist2Name {onTargetLocal,
		  last2d};

  TH1D * h1v[last1d];
  TH1D * h2v[last2d];

};

#endif;
