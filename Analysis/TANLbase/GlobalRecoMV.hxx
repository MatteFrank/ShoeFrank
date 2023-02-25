#ifndef GlobalRecoMV_hxx
#define GlobalRecoMV_hxx

#include "LocalReco.hxx"
#include "TAGactTreeReader.hxx"
#include "TANLAnalysisManager.hxx"

// #pragma GCC diagnostic warning "-Wall"
// #pragma GCC diagnostic warning "-Wextra"
// #pragma GCC diagnostic warning "-Wformat-nonliteral"
// #pragma GCC diagnostic warning "-Wcast-align"
// #pragma GCC diagnostic warning "-Wpointer-arith"
// #pragma GCC diagnostic warning "-Wmissing-declarations"
// #pragma GCC diagnostic warning "-Wundef"
// #pragma GCC diagnostic warning "-Wshadow"

using namespace std;

class GlobalRecoMV : public LocalReco {

  public:
  GlobalRecoMV(TString expName, Int_t runNumber, TString fileNameIn, 
	       TString fileNameout, Bool_t isMC, Int_t nTotEv);
  ~GlobalRecoMV();

  //BaseReco overwritten methods
  
  void BeforeEventLoop();
  void LoopEvent();
  void AfterEventLoop();

  //detector analysis
  void Booking();
  void SetupTree();

private:
  
  Int_t nTotEv;  //total number of events (-nev flag)
  Int_t nSkipEvt; // events to skip
  Int_t currEvent;
  bool  isMC;

  //useful variables
  TFile *file_out;
  TAGactTreeReader* myReader;

  TANLAnalysisManager methods;

};

#endif
