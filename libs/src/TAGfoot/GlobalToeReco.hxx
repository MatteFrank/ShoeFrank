
#ifndef _GlobalToeReco_HXX_
#define _GlobalToeReco_HXX_

#include "TString.h"
#include "TAGobject.hxx"
#include "BaseReco.hxx"


class GlobalToeReco  : public BaseReco
{
public:
   //! default constructor
   GlobalToeReco(TString expName, TString fileNameIn = "", TString fileNameout = "");
   
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

   //! Set run number
   void   SetRunNumber();

   
private:
  

protected:

   TAGdataDsc*           fpNtuMcEve;    // input data dsc

   TTree*                fTree;         // tree for MC
   TFile*                fActEvtReader; // file for MC
   
   ClassDef(GlobalToeReco, 1); 
};


#endif
