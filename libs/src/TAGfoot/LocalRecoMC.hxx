
#ifndef _LocalRecoMC_HXX_
#define _LocalRecoMC_HXX_

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

#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCactNtuPart.hxx"
#include "TAMCactNtuRegion.hxx"
#include "TAMCactNtuEvent.hxx"

#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"

#include "TAMCflukaStruct.hxx"

class TTree;

class LocalRecoMC : public BaseReco
{
public:
   //! default constructor
   LocalRecoMC(TString expName = "", Int_t runNumber = 1, TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~LocalRecoMC();
   
   //! Add required items
   virtual void AddRawRequiredItem();

   //! Create raw data action
   virtual void CreateRawAction();
   
   //! Set raw histogram directory
   virtual void SetRawHistogramDir();
   
   //! Create branch in tree
   virtual void SetTreeBranches();
   
   //! Open File
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();
  
   // ! Global Checks
   virtual void GlobalChecks();
   
   //! Goto Event
   virtual Bool_t GoEvent(Int_t iEvent);

protected:
   EVENT_STRUCT*         fEvtStruct;
  
   TAMCactNtuPart*       fActNtuMcTrk;
   TAMCactNtuRegion*     fActNtuMcReg;
   TAMCactNtuEvent*      fActNtuMcEvt;
   TASTactNtuHitMC*      fActNtuHitSt;  // action for ntu data
   TABMactNtuHitMC*      fActNtuHitBm;  // action for ntu data
   TAVTactNtuHitMC*      fActNtuHitVtx;  // action for ntu data
   TAITactNtuHitMC*      fActNtuHitIt;  // action for ntu data
   TAMSDactNtuHitMC*     fActNtuHitMsd;  // action for ntu data
   TATWactNtuHitMC*      fActNtuHitTw;  // action for ntu data
   TACAactNtuHitMC*      fActNtuHitCa;  // action for ntu data
  
   TAGactTreeReader*     fActEvtReader; // file for MC

   ClassDef(LocalRecoMC, 1); // Base class for event display
};


#endif
