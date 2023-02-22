#ifndef TANLBMVTmatchAnalysis_h
#define TANLBMVTmatchAnalysis_h

#include "TANLbaseAnalysis.hxx"
#include "TVector3.h"
#include <list>

class TH1D;
class TH2D;

class TANLBMVTmatchAnalysis : public TANLbaseAnalysis {
public:

  TANLBMVTmatchAnalysis();

  virtual ~TANLBMVTmatchAnalysis();

  //
  virtual void Setup();

  virtual void BeforeEventLoop();

  virtual void ProcessEvent();

  virtual void AfterEventLoop();
  
  TVector3 extrapolate(Double_t z, 
		       const TVector3 & pos, 
		       const TVector3 & dir);
  
private:

  enum hist1Name {nvtx, allDistances, matchDistance, sensors, xdiff, ydiff, theta, chi2xvt, chi2yvt,
		  last1d};
  enum hist2Name {h2match,resvstx,resvsty,
		  last2d};

  TH1D * h1v[last1d];
  TH1D * h2v[last2d];

};

#endif;
