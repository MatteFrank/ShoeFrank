
#ifndef _LocalReco_HXX_
#define _LocalReco_HXX_

#include "BaseLocalReco.hxx"

#include "TAGdataDsc.hxx"

#include "TASTactDatRaw.hxx"
#include "TASTactNtuRaw.hxx"
#include "TABMactVmeReader.hxx"
#include "TABMactDatRaw.hxx"
#include "TABMactNtuRaw.hxx"

#include "TAVTactVmeReader.hxx"
#include "TAVTactNtuRaw.hxx"

#include "TAITactNtuRaw.hxx"
//#include "TAMSDactNtuRaw.hxx"

//#include "TAMSDactDatRaw.hxx"

#include "TATWactDatRaw.hxx"
#include "TATWactNtuRaw.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

class LocalReco : public BaseLocalReco
{
public:
   //! default constructor
   LocalReco(TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~LocalReco();
   
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

   //! Loop events
   virtual void LoopEvent(Int_t nEvents);

public:
   //! Disable/Enable stand alone DAQ
   static void DisableStdAlone()   { fgStdAloneFlag = false;  }
   static void EnableStdAlone()    { fgStdAloneFlag = true;   }
   
private:
   TAGdataDsc*           fpDaqEvent;
 //  TAGdataDsc*           fpDatRawMsd;    // input data dsc

   TASTactDatRaw*        fActDatRawSt;  // action for ntu data
   TASTactNtuRaw*        fActNtuRawSt;  // action for ntu data
   
   TABMactVmeReader*     fActVmeReaderBm; // action for stand alone reader BM
   TABMactDatRaw*        fActDatRawBm;
   TABMactNtuRaw*        fActNtuRawBm;
   //   TAMSDactDatuRaw*      fActDatRawMsd;  // action for raw data

   TAVTactVmeReader*     fActVmeReaderVtx; // action for stand alone reader VTX
   TAVTactNtuRaw*        fActNtuRawVtx;  // action for ntu data
   TAITactNtuRaw*        fActNtuRawIt;  // action for ntu data
//   TAMSDactNtuRaw*       fActNtuRawMsd;  // action for ntu data

   TATWactDatRaw*        fActDatRawTw;
   TATWactNtuRaw*        fActNtuRawTw;

   TAGactDaqReader*      fActEvtReader; // reader for real data (DAQ)
  
private:
   static Bool_t         fgStdAloneFlag;   // flag for standalone DAQ

   ClassDef(LocalReco, 1); // Base class for event display
};


#endif
