
#ifndef _GlobalRecoMC_HXX_
#define _GlobalRecoMC_HXX_

#include "TString.h"

#include "LocalRecoMC.hxx"

#include "FootField.hxx"

#include "KFitter.hxx"
#include "UpdatePDG.hxx"

#include "GlobalTrackingStudies.hxx"


class GlobalRecoMC : public LocalRecoMC
{
public:
  //! default constructor
  GlobalRecoMC(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "");
  
  virtual ~GlobalRecoMC();
  
  void BeforeEventLoop();

  void LoopEvent(Int_t nEvents);

  void AfterEventLoop();

  ClassDef(GlobalRecoMC, 0);

private:
  KFitter* m_kFitter;
  GlobalTrackingStudies* m_globalTrackingStudies;
};


#endif
