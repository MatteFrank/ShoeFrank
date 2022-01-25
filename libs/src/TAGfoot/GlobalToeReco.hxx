
#ifndef _GlobalToeReco_HXX_
#define _GlobalToeReco_HXX_

/*!
 \file GlobalToeReco.hxx
 \brief Global reconstruction class using TOE from L0 tree
 */
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TAGobject.hxx"
#include "TAGactTreeReader.hxx"
#include "BaseReco.hxx"


class GlobalToeReco  : public BaseReco
{
public:
   // default constructor
   GlobalToeReco(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "", Bool_t isMC = true, TString fileNameMcIn = "", TString treeNameMc = "EventTree");
   
   virtual ~GlobalToeReco();

   //! Create raw data action
   virtual void CreateRawAction() { return; }
  
   // Open File
   virtual void OpenFileIn();
   
   // Close File in
   virtual void CloseFileIn();

   // Read L0 branch in tree
   void SetL0TreeBranches();
  
   // Create branch in tree
   void SetTreeBranches();

   ClassDef(GlobalToeReco, 1); ///< Class def
};


#endif
