#ifndef _RecoMC_HXX_
#define _RecoMC_HXX_

/*!
 \file RecoMC.hxx
 \brief Reconstruction class from MC data
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "BaseReco.hxx"

#include "TFile.h"

#include "TAGdataDsc.hxx"
#include "TAGactTreeReader.hxx"
#include "TASTactNtuHitMC.hxx"
#include "TABMactNtuHitMC.hxx"
#include "TAVTactNtuHitMC.hxx"
#include "TAITactNtuHitMC.hxx"
#include "TAMSDactNtuHitMC.hxx"
#include "TATWactNtuHitMC.hxx"
#include "TACAactNtuHitMC.hxx"

#include "TAMCactNtuPart.hxx"
#include "TAMCactNtuRegion.hxx"
#include "TAMCactNtuEvent.hxx"

class RecoMC : public BaseReco
{
public:
   // default constructor
   RecoMC(TString expName = "", Int_t runNumber = 1, TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~RecoMC();
   
   // Add required items
   virtual void AddRawRequiredItem();

   // Create raw data action
   virtual void CreateRawAction();
   
   // Create branch in tree
   virtual void SetTreeBranches();
   
   // Open File
   virtual void OpenFileIn();
   
   // Close File in
   virtual void CloseFileIn();
  
   // Goto Event
   virtual Bool_t GoEvent(Int_t iEvent);

protected:
   EVENT_STRUCT*         fEvtStruct;   ///< Fluka structure
  
   TAMCactNtuPart*       fActNtuMcTrk;  ///< action for MC tracks
   TAMCactNtuRegion*     fActNtuMcReg;  ///< action for MC region
   TAMCactNtuEvent*      fActNtuMcEvt;  ///< action for MC events
   TASTactNtuHitMC*      fActNtuHitSt;  ///< action for STC MC hits
   TABMactNtuHitMC*      fActNtuHitBm;  ///< action for BM MC hits
   TAVTactNtuHitMC*      fActNtuHitVtx; ///< action for VTX MC hits
   TAITactNtuHitMC*      fActNtuHitIt;  ///< action for ITR MC hits
   TAMSDactNtuHitMC*     fActNtuHitMsd; ///< action for MSD MC hits
   TATWactNtuHitMC*      fActNtuHitTw;  ///< action for TW MC hits
   TACAactNtuHitMC*      fActNtuHitCa;  ///< action for CAL MC hits
  
   TAGactTreeReader*     fActEvtReader; ///< file reader for MC

   ClassDef(RecoMC, 1); ///< Base class for event display
};


#endif
