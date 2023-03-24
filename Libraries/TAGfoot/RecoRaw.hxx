
#ifndef _RecoRaw_HXX_
#define _RecoRaw_HXX_

/*!
 \file RecoRaw.hxx
 \brief Reconstruction class from raw data
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "BaseReco.hxx"

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

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

class RecoRaw : public BaseReco
{
public:
   // default constructor
   RecoRaw(TString expName = "", Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "", Bool_t IsSubFile=false, Bool_t isMC = false);
   
   virtual ~RecoRaw();

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

   //! Set rum number from file
   void SetRunNumberFromFile();

   //! Disable SubFile processing
   virtual void DisableSubFileNumber()            { fSubFileFlag = false;      }
   //! Enable SubFile processing
   virtual void EnableSubFileNumber()             { fSubFileFlag = true;       }
   //! Check if subfile processing is enable
   virtual Bool_t IsSubFileEnabled()              { return fSubFileFlag;       }
   //! Set the number sub file for a given run number to be processed
   virtual void SetSubFileNumber(Int_t nsubfile)  { fSubFileNumber = nsubfile; }

public:
   //! Disable stand alone DAQ
   static void DisableStdAlone()                  { fgStdAloneFlag = false;    }
   //! Enable stand alone DAQ
   static void EnableStdAlone()                   { fgStdAloneFlag = true;     }
   //! Get stand alone DAQ
   static Bool_t IsStdAlone()                     { return fgStdAloneFlag;     }
   //! Set max number of file to be processed for stand alone DAQ
   static void SetStdAloneFiles(Int_t value)      { fgNumFileStdAlone = value; }

protected:
   TAGdataDsc*           fpNtuWDtrigInfo; ///< contains the WD trigger info
   TAGdataDsc*           fpDaqEvent;      ///< DAQ event
   TAGdataDsc*           fpNtuEvt;        ///< input data event dsc
   TAGdataDsc*           fpDatRawMsd;     ///< input raw data dsc for MSD
   TAGactWDreader*       fActWdRaw;       ///< action for WD decoding
   
   TAGactNtuEvent*       fActNtuEvt;      ///< action for trigger event

   TASTactNtuHit*        fActNtuHitSt;    ///< action for STC hits
   TABMactVmeReader*     fActVmeReaderBm; ///< action for stand alone reader BM
   TABMactNtuRaw*        fActDatRawBm;    ///< action for BM raw data
   TABMactNtuHit*        fActNtuHitBm;    ///< action for BM hits
   TAMSDactNtuRaw*       fActDatRawMsd;   ///< action for MSD raw data

   TAVTactVmeReader*     fActVmeReaderVtx; ///< action for stand alone reader VTX
   TAVTactNtuHit*        fActNtuHitVtx;    ///< action for VTX hits
   TAITactNtuHit*        fActNtuHitIt;     ///< action for ITR hits
   TAMSDactNtuHit*       fActNtuHitMsd;    ///< action for MSD hits
   TATWactNtuHit*        fActNtuHitTw;     ///< action for TW hits
   TACAactNtuHit*        fActNtuHitCa;     ///< action for CAL hits

   TAGactDaqReader*      fActEvtReader;    ///< reader for real data (DAQ)
  
   Int_t                 fSubFileNumber; ///< number of subfile to process
   Bool_t                fSubFileFlag;  ///< flag for subfile processing

private:
   static Bool_t         fgStdAloneFlag;   ///< flag for standalone DAQ
   static Int_t          fgNumFileStdAlone; ///< number of standalone file for WD

   ClassDef(RecoRaw, 1); ///< Base class for event display
};


#endif
