
#ifndef _GlobalToeReco_HXX_
#define _GlobalToeReco_HXX_

#include "TString.h"
#include "TAGobject.hxx"
#include "BaseReco.hxx"


class GlobalToeReco  : public BaseReco
{
public:
   //! default constructor
   GlobalToeReco(TString expName, TString fileNameIn = "", TString fileNameout = "", Bool_t isMc = true);
   
   virtual ~GlobalToeReco();

   //! Create raw data action
   virtual void CreateRawAction();
   
   virtual void AddRawRequiredItem();
   
   //! Open File
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();

   //! Loop events
   virtual void LoopEvent(Int_t nEvents);

   //! Create branch in tree
   virtual void SetTreeBranches();

   //! Set run number
   void   SetRunNumber();

private:
   TAGdataDsc*           fpNtuMcEve;    // input data dsc

   TTree*                fTree;         // tree for local reconstruction
   TFile*                fActEvtReader; // file for local reconstruction
   
   ClassDef(GlobalToeReco, 1); 
};


#endif
