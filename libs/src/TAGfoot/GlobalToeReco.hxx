
#ifndef _GlobalToeReco_HXX_
#define _GlobalToeReco_HXX_

#include "TString.h"
#include "LocalReco.hxx"


class GlobalToeReco : public LocalReco
{
public:
   //! default constructor
   GlobalToeReco(TString expName, TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~GlobalToeReco();
   
   ClassDef(GlobalToeReco, 0); 
};


#endif
