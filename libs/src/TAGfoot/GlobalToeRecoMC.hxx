
#ifndef _GlobalToeRecoMC_HXX_
#define _GlobalToeRecoMC_HXX_

#include "TString.h"

#include "LocalRecoMC.hxx"


class GlobalToeRecoMC : public LocalRecoMC
{
public:
  //! default constructor
  GlobalToeRecoMC(TString expName, TString fileNameIn = "", TString fileNameout = "");
  
  virtual ~GlobalToeRecoMC();
  
  ClassDef(GlobalToeRecoMC, 0);
};


#endif
