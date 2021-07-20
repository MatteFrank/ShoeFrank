
#ifndef _GlobalReco_HXX_
#define _GlobalReco_HXX_

#include "TString.h"
#include "BaseReco.hxx"

/////////////////

#include "TAGdataDsc.hxx"
#include "TAGactWDreader.hxx"

#include "TAGactNtuEvent.hxx"
#include "TASTactNtuHit.hxx"
#include "TABMactVmeReader.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactNtuHit.hxx"

#include "TAVTactVmeReader.hxx"
#include "TAVTactNtuHit.hxx"

#include "TAITactNtuHit.hxx"
#include "TAMSDactNtuHit.hxx"

#include "TAMSDactNtuRaw.hxx"

#include "TATWactNtuHit.hxx"
#include "TACAactNtuHit.hxx"

////////////////

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

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


#include "TAGntuEvent.hxx"
#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuHit.hxx"
#include "TACAntuHit.hxx"

#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAMSDntuRaw.hxx"




class GlobalReco : public BaseReco
{
public:
   //! default constructor
   GlobalReco(TString expName, Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "", Bool_t isMc = true);
   
   virtual ~GlobalReco();
  
  //! Create raw data action
  virtual void CreateRawAction();

  void CreateRawAction_MC();
  void CreateRawAction_Raw();
  
  //! Open File
  virtual void OpenFileIn();
  
  //! Close File in
  virtual void CloseFileIn();
  
  //! Create L0 branch in tree
  void SetL0TreeBranches();
  
  //! Create branch in tree
  void SetTreeBranches();


private:

   TAGdataDsc*           fpDaqEvent;
   TAGdataDsc*           fpNtuEvt;    // input data dsc
   TAGdataDsc*           fpDatRawMsd;    // input data dsc
   TAGactWDreader*       fActWdRaw;  // action for WD decoding
  
   TAGactNtuEvent*       fActNtuEvt; // action for trigger event

   TAGaction*        fActNtuHitSt;  // action for ntu data
   TAGaction*        fActNtuHitBm;
   TAGaction*        fActNtuHitVtx;  // action for ntu data
   TAGaction*        fActNtuHitIt;  // action for ntu data
   TAGaction*        fActNtuHitMsd;  // action for ntu data
   TAGaction*        fActNtuHitTw;
   TAGaction*        fActNtuHitCa;

   TABMactVmeReader*     fActVmeReaderBm; // action for stand alone reader BM
   TABMactNtuRaw*        fActDatRawBm;
   
   TAMSDactNtuRaw*       fActDatRawMsd;  // action for raw data

   TAVTactVmeReader*     fActVmeReaderVtx; // action for stand alone reader VTX
   
   

   TAGactDaqReader*      fActEvtReaderDAQ; // reader for real data (DAQ)

   EVENT_STRUCT*         fEvtStruct;
  
   TAMCactNtuPart*       fActNtuMcTrk;
   TAMCactNtuRegion*     fActNtuMcReg;
   TAMCactNtuEvent*      fActNtuMcEvt;


   // TASTactNtuHit*        fActNtuHitSt;  // action for ntu data
   // TABMactNtuHit*        fActNtuHitBm;
   // TAVTactNtuHit*        fActNtuHitVtx;  // action for ntu data
   // TAITactNtuHit*        fActNtuHitIt;  // action for ntu data
   // TAMSDactNtuHit*       fActNtuHitMsd;  // action for ntu data
   // TATWactNtuHit*        fActNtuHitTw;
   // TACAactNtuHit*        fActNtuHitCa;

   // TASTactNtuHitMC*      fActNtuHitSt;  // action for ntu data
   // TABMactNtuHitMC*      fActNtuHitBm;  // action for ntu data
   // TAVTactNtuHitMC*      fActNtuHitVtx;  // action for ntu data
   // TAITactNtuHitMC*      fActNtuHitIt;  // action for ntu data
   // TAMSDactNtuHitMC*     fActNtuHitMsd;  // action for ntu data
   // TATWactNtuHitMC*      fActNtuHitTw;  // action for ntu data
   // TACAactNtuHitMC*      fActNtuHitCa;  // action for ntu data
  

   TAGactTreeReader*     	fActEvtReader; // file for MC

   bool         			fgStdAloneFlag;   // flag for standalone DAQ

   ClassDef(GlobalReco, 0); 


};


#endif
