
#ifndef _GlobalRecoMC_HXX_
#define _GlobalRecoMC_HXX_

#include "TString.h"
#include "TTree.h"

#include "LocalRecoMC.hxx"

#include "TAGactKFitter.hxx"

#include "GlobalTrackingStudies.hxx"


class GlobalRecoMC : public LocalRecoMC
{
public:
  //! default constructor
  GlobalRecoMC(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "");
  
  virtual ~GlobalRecoMC();
  

  ClassDef(GlobalRecoMC, 0);

private:

};


#endif
