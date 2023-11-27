
#ifndef _TAGactDscTreeWriter_HXX_
#define _TAGactDscTreeWriter_HXX_

/*!
 \file TAGactDscTreeWriter.hxx
 \brief  Base class for reconstruction
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/


#include "TNamed.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

#include "TAGactTreeReader.hxx"
#include "TAGaction.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"


#include "TAGactTreeWriter.hxx"

using namespace std;

class TAGactDscTreeWriter : public TAGactTreeWriter
{
public:
   // default constructor
   TAGactDscTreeWriter(const char* name = 0, Bool_t isMC = false);
   
   // default destructor
   virtual ~TAGactDscTreeWriter();
      
   //! Open File In
   Int_t  Open(const TString& name, Option_t* option="RECREATE", const TString treeName="tree", Bool_t dscBranch = true);

   // Create branch in tree
   void   SetTreeBranches();
   
   //! Set cutter for TOE
   void   SetRecCutter(Bool_t c) { fFlagRecCutter = c; }

  //! Set Stdalone flag
  void   SetStdAlone(Bool_t c) {
    cout <<" set stdalone" << endl;
    fStdAloneFlag = c; }
        
protected:
   TAGdataDsc*     fpNtuHitSt;          ///< ST hit container
   TAGdataDsc*     fpNtuHitBm;          ///< BM hit container
   TAGdataDsc*     fpNtuTrackBm;        ///< BM track container
   TAGdataDsc*     fpNtuHitVtx;         ///< VT Hit container
   TAGdataDsc*     fpNtuClusVtx;        ///< VT cluster container
   TAGdataDsc*     fpNtuTrackVtx;       ///< VT vertex container
   TAGdataDsc*     fpNtuVtx;            ///< VT vertex container
   TAGdataDsc*     fpNtuHitIt;          ///< IT hit container
   TAGdataDsc*     fpNtuClusIt;         ///< IT cluster container
   TAGdataDsc*     fpNtuTrackIt;        ///< IT track container
   TAGdataDsc*     fpNtuHitMsd;         ///< MSD hit container
   TAGdataDsc*     fpNtuClusMsd;        ///< MSD cluster container
   TAGdataDsc*     fpNtuRecMsd;         ///< MSD point container
   TAGdataDsc*     fpNtuTrackMsd;       ///< MSD track container
   TAGdataDsc*     fpNtuHitTw;          ///< TW hit container
   TAGdataDsc*     fpNtuRecTw;          ///< TW rec point container
   TAGdataDsc*     fpNtuHitCa;          ///< TW hit container
   TAGdataDsc*     fpNtuClusCa;         ///< CA cluster container
   TAGdataDsc*     fpNtuGlbTrack;       ///< Glb track cointainers
   
   TAGdataDsc*     fpNtuMcTrk;          ///< MC tracks conatianers
   TAGdataDsc*     fpNtuMcEvt;          ///< MC event conatianers
   TAGdataDsc*     fpNtuMcReg;          ///< MC region conatianers
   TAGdataDsc*     fpNtuMcSt;           ///< MC ST hit conatianers
   TAGdataDsc*     fpNtuMcBm;           ///< MC BM hit conatianers
   TAGdataDsc*     fpNtuMcVt;           ///< MC VT hit conatianers
   TAGdataDsc*     fpNtuMcIt;           ///< MC IT hit conatianers
   TAGdataDsc*     fpNtuMcMsd;          ///< MC MSD hit conatianers
   TAGdataDsc*     fpNtuMcTw;           ///< MC TW hit conatianers
   TAGdataDsc*     fpNtuMcCa;           ///< MC CAL hit conatianers
   
   TAGdataDsc*     fpNtuEvt;            ///< Event  conatianers
   TAGdataDsc*     fpDatRawSt;          ///< ST raw hit conatianers
   TAGdataDsc*     fpWDtrigger;         ///< Info trigger conatianers
   TAGdataDsc*     fpDatRawBm;          ///< BM raw hit conatianers
   TAGdataDsc*     fpDatRawMsd;         ///< MSD raw hit conatianers
   TAGdataDsc*     fpDatRawTw;          ///< TW raw hit conatianers
   TAGdataDsc*     fpDatRawCa;          ///< CAL raw hit conatianers

   Bool_t          fFlagMC;             ///< MC flag
   Bool_t          fFlagHits;           ///< Flag hits
   Bool_t          fFlagTrack;          ///< tracking flag
   Bool_t          fFlagItrTrack;       ///< flag for ITR tracking
   Bool_t          fFlagMsdTrack;       ///< flag for MSD tracking
   Bool_t          fFlagRecCutter;      ///< Flag for Rec cutter (TOE)
   Bool_t          fStdAloneFlag;       ///< StandAlone flag (DAQ)
   Bool_t          fSaveMcFlag;         ///< Saving MC tree

 protected:
   // Set descriptors
   void SetDescriptors();

   ClassDef(TAGactDscTreeWriter, 1);        ///< Base class for reconstruction
};

#endif
