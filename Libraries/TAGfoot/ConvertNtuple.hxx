
#ifndef _ConvertNtuple_HXX_
#define _ConvertNtuple_HXX_

/*!
 \file ConvertNtuple.hxx
 \brief  Base class for reconstruction
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/


#include "TNamed.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

#include "TAGactTreeReader.hxx"
#include "TAGactFlatTreeWriter.hxx"
#include "TAGaction.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"


#include "BaseReco.hxx"

using namespace std;

class ConvertNtuple : public BaseReco // using TNamed for the in/out files
{
public:
   // default constructor
   ConvertNtuple(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC = false, TString fileNameMcIn = "", TString treeNameMc = "EventTree");
   
   // default destructor
   virtual ~ConvertNtuple();
      
   // Create raw action
   virtual void CreateRecAction();
   
   //! Create rec action
   virtual void CreateRawAction()      { return; }
   
   // Add required items
   virtual void AddRecRequiredItem();
   
   //! Open File In
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();
      
   // Open File Out
   virtual void OpenFileOut();
   
   // Close File Out
   virtual void CloseFileOut();
  
  // Create L0 branch in tree
  virtual void SetL0TreeBranches();
   
protected:
   TString               fExpName;        ///< Experiment name
 
   TAGactTreeReader*     fActEvtReader;  ///< Tree/event reader
   TAGactFlatTreeWriter* fActEvtWriter;  ///< Tree/event reader
   ///<
 protected:
   // Create reconstruction action for BM
   void CreateRecActionBm();
   // Create reconstruction action for VTX
   void CreateRecActionVtx();
   // Create reconstruction action for ITR
   void CreateRecActionIt();
   // Create reconstruction action for MSD
   void CreateRecActionMsd();
   // Create reconstruction action for TW
   void CreateRecActionTw();
   // Create reconstruction action for CAL
   void CreateRecActionCa();
   // Create reconstruction action for global tracks with TOE
   void CreateRecActionGlb();
   
   ClassDef(ConvertNtuple, 1);        ///< Base class for reconstruction
};


#endif
