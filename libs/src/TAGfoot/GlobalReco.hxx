
#ifndef _GlobalReco_HXX_
#define _GlobalReco_HXX_

/*!
 \file GlobalReco.hxx
 \brief Declaration of GlobalReco
 */
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "BaseReco.hxx"


class GlobalReco : public BaseReco
{
public:
   GlobalReco(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "", Bool_t isMC = true);
   
   virtual ~GlobalReco();
  
  //! Create raw data action
   virtual void CreateRawAction() { return; }
  
  // Open File
  virtual void OpenFileIn();
  
  // Close File in
  virtual void CloseFileIn();
  
  // Create L0 branch in tree
  void SetL0TreeBranches();
  
  // Create branch in tree
  void SetTreeBranches();

   ClassDef(GlobalReco, 0); ///< Class def
};

#endif
