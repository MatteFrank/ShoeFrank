
#ifndef _GlobalReco_HXX_
#define _GlobalReco_HXX_

#include "TString.h"
#include "LocalReco.hxx"


class GlobalReco : public LocalReco 
{
public:
   //! default constructor
   GlobalReco(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~GlobalReco();
   
   ClassDef(GlobalReco, 0); 
};


#endif
