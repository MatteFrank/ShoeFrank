#ifndef TANLBMAnalysis_h
#define TANLBMAnalysis_h

#include "TANLbaseAnalysis.hxx"
#include "TVector3.h"
#include <list>

class TH1D;
class TH2D;
class TABMntuTrack;
class TAGgeoTrafo;


class TANLBMAnalysis : public TANLbaseAnalysis {
public:

  TANLBMAnalysis();

  virtual ~TANLBMAnalysis();

  virtual void Setup(TAGgeoTrafo *aTrafo);

  virtual void BeforeEventLoop();

  virtual void ProcessEvent();

  virtual void AfterEventLoop();

private:

  //useful formulas
  TVector3 ProjectToZ(const TVector3 & Slope, const TVector3 & Pos0, 
		      Double_t FinalZ){
    return TVector3(Slope.X()/Slope.Z()*(FinalZ-Pos0.Z())+Pos0.X(),
		    Slope.Y()/Slope.Z()*(FinalZ-Pos0.Z())+Pos0.Y(), 
		    FinalZ);
  }


  void Booking();

  enum hist1Name {ntracks,chix,chiy,nx,ny,probx,proby, prob3, prob4, prob5, prob6,
		  XposGlb,YposGlb,ThetaLoc,PhiLoc,ThetaGlb,PhiGlb,
		  last1d};
  enum hist2Name {onTargetLocal,onTargetGlb,
		  last2d};

  TH1D * h1v[last1d];
  TH2D * h2v[last2d];

  TABMntuTrack* myBMTrackContainer;
};

#endif
