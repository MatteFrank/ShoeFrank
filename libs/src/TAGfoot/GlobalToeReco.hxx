
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
   
   //! Create rec data action
   virtual void CreateRecAction();
   
   //! Open File
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();

   //! Loop events
   virtual void LoopEvent(Int_t nEvents);

   //! Set run number
   void   SetRunNumber();

private:
   TTree*                fTree;         // tree for local reconstruction
   TAGactTreeReader*     fActEvtReaderFile; // file for local reconstruction
   
   ClassDef(GlobalToeReco, 1); 
};


#endif
