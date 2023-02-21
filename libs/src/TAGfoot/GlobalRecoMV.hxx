
#ifndef GlobalRecoMV_hxx
#define GlobalRecoMV_hxx

#include "GlobalRecoAna.hxx"

#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"
#pragma GCC diagnostic warning "-Wformat-nonliteral"
#pragma GCC diagnostic warning "-Wcast-align"
#pragma GCC diagnostic warning "-Wpointer-arith"
#pragma GCC diagnostic warning "-Wmissing-declarations"
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wshadow"


using namespace std;

class GlobalRecoMV : public GlobalRecoAna {

  public:
  GlobalRecoMV(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, Int_t nTotEv);
  ~GlobalRecoMV();

  //BaseReco overwritten methds
  void BeforeEventLoop();
  void LoopEvent();
  void AfterEventLoop();

  //detector analysis
  void Booking();
  void SetupTree();

  void AnalyzeBM();
  void AnalyzeVTX();
  void MatchBMVTX();

  TVector3 extrapolate(Double_t z, const TVector3 & pos, const TVector3 & dir) const;

  //useful analysis variables

  enum hist1Name {ntracks,chix,chiy,nx,ny,probx,proby, prob3, prob4, prob5, prob6, 
		  nvtx, allDistances, matchDistance, sensors, xdiff, ydiff, theta, chi2xvt, chi2yvt,
		  last1d};

  TH1D * h1v[last1d];
  TH2D * h2match;
  TH2D * h2resvstx;
  TH2D * h2resvsty;

  std::vector<int> goodEvents;
};

#endif
