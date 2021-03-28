
#ifndef _GlobalToeReco_HXX_
#define _GlobalToeReco_HXX_

#include "TString.h"
#include "TAGobject.hxx"
#include "TAGactTreeReader.hxx"
#include "BaseReco.hxx"


class GlobalToeReco  : public BaseReco
{
public:
   //! default constructor
   GlobalToeReco(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "", Bool_t isMc = true);
   
   virtual ~GlobalToeReco();

   //! Create raw data action
   virtual void CreateRawAction();
  
   //! Open File
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();

   //! Create L0 branch in tree
   void SetL0TreeBranches();
   
   ClassDef(GlobalToeReco, 1); 
};


#endif
