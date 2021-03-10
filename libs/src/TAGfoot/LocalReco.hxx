
#ifndef _LocalReco_HXX_
#define _LocalReco_HXX_

#include "BaseReco.hxx"

#include "TAGdataDsc.hxx"
#include "TAGactWDreader.hxx"

#include "TASTactNtuRaw.hxx"
#include "TABMactVmeReader.hxx"
#include "TABMactDatRaw.hxx"
#include "TABMactNtuHit.hxx"

#include "TAVTactVmeReader.hxx"
#include "TAVTactNtuHit.hxx"

#include "TAITactNtuHit.hxx"
//#include "TAMSDactNtuRaw.hxx"

//#include "TAMSDactDatRaw.hxx"

#include "TATWactNtuHit.hxx"
#include "TACAactNtuHit.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

class LocalReco : public BaseReco
{
public:
   //! default constructor
   LocalReco(TString expName = "", Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "");
   
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
   
public:
   //! Disable/Enable stand alone DAQ
   static void DisableStdAlone()   { fgStdAloneFlag = false;  }
   static void EnableStdAlone()    { fgStdAloneFlag = true;   }
   
private:
   TAGdataDsc*           fpDaqEvent;
 //  TAGdataDsc*           fpDatRawMsd;    // input data dsc

   TAGactWDreader*        fActWdRaw;  // action for WD decoding

   TASTactNtuRaw*        fActNtuRawSt;  // action for ntu data
   
   TABMactVmeReader*     fActVmeReaderBm; // action for stand alone reader BM
   TABMactDatRaw*        fActDatRawBm;
   TABMactNtuHit*        fActNtuRawBm;
   //   TAMSDactDatuRaw*      fActDatRawMsd;  // action for raw data

   TAVTactVmeReader*     fActVmeReaderVtx; // action for stand alone reader VTX
   TAVTactNtuHit*        fActNtuRawVtx;  // action for ntu data
   TAITactNtuHit*        fActNtuRawIt;  // action for ntu data
//   TAMSDactNtuRaw*       fActNtuRawMsd;  // action for ntu data

   TATWactNtuHit*        fActNtuRawTw;

   TACAactNtuHit*        fActNtuRawCa;

   TAGactDaqReader*      fActEvtReader; // reader for real data (DAQ)
  
private:
   static Bool_t         fgStdAloneFlag;   // flag for standalone DAQ

   ClassDef(LocalReco, 1); // Base class for event display
};


#endif
