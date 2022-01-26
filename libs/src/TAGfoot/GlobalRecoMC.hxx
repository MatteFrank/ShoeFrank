
#ifndef _GlobalRecoMC_HXX_
#define _GlobalRecoMC_HXX_

/*!
 \file GlobalRecoMC.hxx
 \brief   Declaration of GlobalRecoMC.
 */
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TTree.h"

#include "LocalRecoMC.hxx"

#include "TAGactKFitter.hxx"
#include "UpdatePDG.hxx"

#include "TAGFtrackingStudies.hxx"


class GlobalRecoMC : public LocalRecoMC
{
public:
  GlobalRecoMC(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "");
  
  virtual ~GlobalRecoMC();
  
  void BeforeEventLoop();

  void LoopEvent(Int_t nEvents, Int_t skipEvent);

  void AfterEventLoop();

  void OpenFileIn();

private:
  TTree* fTree; ///< tree

   ClassDef(GlobalRecoMC, 0); ///< Class def
};


#endif
