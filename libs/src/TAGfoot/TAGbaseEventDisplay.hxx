
#ifndef _TAGbaseEventDisplay_HXX_
#define _TAGbaseEventDisplay_HXX_

#include "TAEDbaseInterface.hxx"

/** TAGbaseEventDisplay FOOT class to work on event display
 
 */
#include <map>

#include "TAEDcluster.hxx"
#include "TAEDtrack.hxx"
#include "TAEDwire.hxx"
#include "TAEDglbTrack.hxx"
#include "TAEDglbTrackList.hxx"
#include "TAEDbaseInterface.hxx"
#include "BaseReco.hxx"

class TEveDigitSet;
class TGCheckButton;

class TAGbaseEventDisplay : public TAEDbaseInterface
{
protected:
   //! default constructor
   TAGbaseEventDisplay(const TString expName, Int_t runNumber, Int_t type);
   
public:   
   virtual ~TAGbaseEventDisplay();
         
   virtual void BuildDefaultGeometry();
   
   virtual void UpdateElements(); 
   
   //! Add required items
   virtual void AddRequiredItem();

   //! Create raw data action
   virtual void CreateRawAction();
   
   //! Create raw reco action
   virtual void CreateRecAction();
      
   //! Open File
   virtual void SetFileName(const TString fileName);
   
   //! Open File
   virtual void OpenFile();
   
   //! Add elements
   virtual void AddElements();
   
   //! Connect elements
   virtual void ConnectElements();
   
   void UpdateHitInfo(TEveDigitSet* qs, Int_t idx);
   void UpdateTrackInfo(TEveDigitSet* qs, Int_t idx);
   void UpdateTrackInfo(TEveStraightLineSet* ts, Int_t idx);
   void UpdateDriftCircleInfo(TEveDigitSet* qs, Int_t idx);
   
protected:
   void CreateRecActionBm();
   void CreateRecActionVtx();
   void CreateRecActionIt();
   void CreateRecActionMsd();
   void CreateRecActionTw();
   
   //! Add required items
   void AddRequiredRawItem();
   void AddRequiredRecItem();

   void ReadParFiles();
   void SetRecoOptions();
   
   void UpdateStcElements();
   void UpdateLayerElements();
   void UpdateBarElements();
   void UpdateCrystalElements();
   void UpdateStripElements();
   void UpdateQuadElements(const TString prefix);
   void UpdateTrackElements(const TString prefix);
   void UpdateGlbTrackElements();
   void UpdateElements(const TString prefix);
   
public:
   //! Set VTX Tracking algorithm
   static void SetVtxTrackingAlgo(char c)
   {
      switch (c) {
         case 'S':
            fgVtxTrackingAlgo = "Std";
            break;
         case 'F':
            fgVtxTrackingAlgo = "Full";
            break;
         default:
            printf("SetVtxTrackingAlgo: Wrongly set tracking algorithm");
      }
   }
   
   //! Set Itr Tracking algorithm
   static void SetItrTrackingAlgo(char c)
   {
      switch (c) {
         case 'S':
            fgItrTrackingAlgo = "Std";
            break;
         case 'F':
            fgItrTrackingAlgo = "Full";
            break;
         default:
            printf("SetItrTrackingAlgo: Wrongly set tracking algorithm");
      }
   }
   
   //! Set MSD Tracking algorithm
   static void SetMsdTrackingAlgo(char c)
   {
      switch (c) {
         case 'S':
            fgMsdTrackingAlgo = "Std";
            break;
         case 'F':
            fgMsdTrackingAlgo = "Full";
            break;
         default:
            printf("SetMsdTrackingAlgo: Wrongly set tracking algorithm");
      }
   }
   
   //! Disable/Enable stand alone DAQ
   static void DisableStdAlone()    { fgStdAloneFlag = false; }
   static void EnableStdAlone()     { fgStdAloneFlag = true;  }
   
   //! Disable/Enable stand alone DAQ
   static void DisableBmSelectHit() { fgBmSelectHit = false;  }
   static void EnableBmSelectHit()  { fgBmSelectHit = true;   }
   
   // M28 MT clustering flag
   static Bool_t IsM28ClusMT()      { return fgM28ClusMtFlag; }
   
   //! Disable/Enable M28 MT clustering
   static void DisableM28ClusMT()   { fgM28ClusMtFlag = false; }
   static void EnableM28lusMT()     { fgM28ClusMtFlag = true;  }
   
   // ITR tracking flag
   static Bool_t IsItrTracking()    { return BaseReco::IsItrTracking(); }

   //! Disable/Enable ITR tracking
   static void DisableItrTracking() { BaseReco::DisableItrTracking();   }
   static void EnableItrTracking()  { BaseReco::EnableItrTracking();    }
   
   // MSD tracking flag
   static Bool_t IsMsdTracking()    { return BaseReco::IsMsdTracking(); }
   
   //! Disable/Enable MSD tracking
   static void DisableMsdTracking() { BaseReco::DisableMsdTracking();   }
   static void EnableMsdTracking()  { BaseReco::EnableMsdTracking();    }

protected:
   BaseReco*       fReco;    // local reco
   
   //Display
   TAEDcluster*    fStClusDisplay;  // list of quad to display hits

   TAEDcluster*    fVtxClusDisplay;  // list of quad to display hits
   TAEDtrack*      fVtxTrackDisplay; // list of line to display tracks
   
   TAEDcluster*    fItClusDisplay;  // list of quad to display hits
   TAEDtrack*      fItTrackDisplay; // list of line to display tracks

   TAEDcluster*    fMsdClusDisplay;  // list of strip to display hits
   TAEDcluster*    fMsdPointDisplay;  // list of strip to display hits
   TAEDtrack*      fMsdTrackDisplay; // list of line to display tracks

   TAEDcluster*    fTwClusDisplay;  // list of quad to display hits
   
   TAEDcluster*    fCaClusDisplay;  // list of quad to display hits
   
   TAEDwire*       fBmClusDisplay;  // list of line to display wires
   TAEDtrack*      fBmTrackDisplay; // list of line to display tracks
   TEveBoxSet*     fBmDriftCircleDisplay;

   TAEDglbTrackList* fGlbTrackDisplay;  // list of global tracks to display
   TAEDtrack*      fIrTrackDisplay;   // list of line to display tracks
   Bool_t          fIrFlag;
   
   Bool_t          fFlagTrack;       // flag for tracking

   // TW
   map< pair<Int_t, Int_t>, Int_t > fFiredTofBar;       // list of fired bar per event
   
   // CA
   map< Int_t, Int_t >  fFiredCaCrystal;       // list of fired bar per event

   // GUI
   TGCheckButton*       fClusterButton;    // toggle clusters plots
   TGCheckButton*       fRawDataButton;    // toggle rawdata plots
   TGCheckButton*       fRateButton;       // toggle recompute parameters at each plane
   
protected:
   static Bool_t         fgStdAloneFlag;    // flag for standalone DAQ
   static TString        fgVtxTrackingAlgo; // tracking algorithm ("std" with BM, "Full" combinatory)
   static TString        fgItrTrackingAlgo; // tracking algorithm ("std" with BM, "Full" combinatory)
   static TString        fgMsdTrackingAlgo; // tracking algorithm ("std" with BM, "Full" combinatory)
   static Bool_t         fgBmSelectHit;     // flag BM selected hit
   static Bool_t         fgMsdsTrackFlag;    // flag for MSD tracking
   static Bool_t         fgM28ClusMtFlag;    // flag for MT clustering

   ClassDef(TAGbaseEventDisplay, 1); // Base class for event display
};


#endif
