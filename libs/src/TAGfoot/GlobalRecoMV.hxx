
#ifndef GlobalRecoMV_hxx
#define GlobalRecoMV_hxx

#include "GlobalRecoAna.hxx"
#include "TANLAnalysisManager.hxx"

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

  //useful analysis variables

private:

  TANLAnalysisManager methods;

};

#endif
