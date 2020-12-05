
#ifndef _GlobalReco_HXX_
#define _GlobalReco_HXX_

#include "TString.h"
#include "BaseReco.hxx"


class GlobalReco : public BaseReco
{
public:
   //! default constructor
   GlobalReco(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "", Bool_t isMc = true);
   
   virtual ~GlobalReco();
  
  //! Create raw data action
  virtual void CreateRawAction();
  
  //! Open File
  virtual void OpenFileIn();
  
  //! Close File in
  virtual void CloseFileIn();
  
  //! Set run number
  void   SetRunNumber();

  
   ClassDef(GlobalReco, 0); 
};


#endif
