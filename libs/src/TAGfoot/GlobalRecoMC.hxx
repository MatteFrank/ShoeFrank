
#ifndef _GlobalRecoMC_HXX_
#define _GlobalRecoMC_HXX_

#include "TString.h"

#include "LocalRecoMC.hxx"

class GlobalRecoMC : public LocalRecoMC
{
public:
   //! default constructor
   GlobalRecoMC(TString expName, TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~GlobalRecoMC();
   
   ClassDef(GlobalRecoMC, 0);
};


#endif
