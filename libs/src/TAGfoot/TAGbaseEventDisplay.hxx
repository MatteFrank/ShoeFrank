
#ifndef _TAGbaseEventDisplay_HXX_
#define _TAGbaseEventDisplay_HXX_

/*!
 \file TAGbaseEventDisplay.hxx
 \brief FOOT base class to work on event display
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAEDbaseInterface.hxx"

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
   
   //! Build default geometry
   virtual void BuildDefaultGeometry();
   
   //! Update elements
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
   
   //! Uodate hit info
   void UpdateHitInfo(TEveDigitSet* qs, Int_t idx);
   //! Uodate track info
   void UpdateTrackInfo(TEveDigitSet* qs, Int_t idx);
   //! Uodate global track info
   void UpdateTrackInfo(TEveStraightLineSet* ts, Int_t idx);
   //! Uodate BM info
   void UpdateDriftCircleInfo(TEveDigitSet* qs, Int_t idx);
   
protected:   
   //! Add raw reconstruction required items
   void AddRequiredRawItem();
   //! Add reconstruction required items
   void AddRequiredRecItem();

   //! Read parameter files
   void ReadParFiles();
   //! Set reconstruction options
   void SetRecoOptions();
   
   //! Update STC elements
   void UpdateStcElements();
   //! Update BM elements
   void UpdateLayerElements();
   //! Update TW elements
   void UpdateBarElements();
   //! Update CAL elements
   void UpdateCrystalElements();
   //! Update MSD elements
   void UpdateStripElements();
   //! Update quad elements
   void UpdateQuadElements(const TString prefix);
   //! Update track elements
   void UpdateTrackElements(const TString prefix);
   //! Update global track elements
   void UpdateGlbTrackElements();
   //! Update elements
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
   
   //! Disable stand alone DAQ
   static void DisableStdAlone()    { fgStdAloneFlag = false; }
   //! Enable stand alone DAQ
   static void EnableStdAlone()     { fgStdAloneFlag = true;  }
   
   //! Disable stand alone DAQ
   static void DisableBmSelectHit() { fgBmSelectHit = false;  }
   //! Enable stand alone DAQ
   static void EnableBmSelectHit()  { fgBmSelectHit = true;   }
   
   //! M28 MT clustering flag
   static Bool_t IsM28ClusMT()      { return fgM28ClusMtFlag; }
   
   //! Disable M28 MT clustering
   static void DisableM28ClusMT()   { fgM28ClusMtFlag = false; }
   //! Enable M28 MT clustering
   static void EnableM28lusMT()     { fgM28ClusMtFlag = true;  }
   
   //! ITR tracking flag
   static Bool_t IsItrTracking()    { return BaseReco::IsItrTracking(); }

   //! Disable ITR tracking
   static void DisableItrTracking() { BaseReco::DisableItrTracking();   }
   //! Enable ITR tracking
   static void EnableItrTracking()  { BaseReco::EnableItrTracking();    }
   
   //! MSD tracking flag
   static Bool_t IsMsdTracking()    { return BaseReco::IsMsdTracking(); }
   
   //! Disable MSD tracking
   static void DisableMsdTracking() { BaseReco::DisableMsdTracking();   }
   //! Enable MSD tracking
   static void EnableMsdTracking()  { BaseReco::EnableMsdTracking();    }

protected:
   BaseReco*       fReco;                   ///< Base reconstruction
   
   //Display
   TAEDcluster*    fStClusDisplay;          ///< list of quad to display STC clusters

   TAEDcluster*    fVtxClusDisplay;         ///< list of quad to display VTX clusters
   TAEDtrack*      fVtxTrackDisplay;        ///< list of line to display VTX tracks
   
   TAEDcluster*    fItClusDisplay;          ///< list of quad to display ITR clusters
   TAEDtrack*      fItTrackDisplay;         ///< list of line to display ITR tracks

   TAEDcluster*    fMsdClusDisplay;         ///< list of strip to display MSD clusters
   TAEDcluster*    fMsdPointDisplay;        ///< list of strip to display MSD points
   TAEDtrack*      fMsdTrackDisplay;        ///< list of line to display MSD tracks

   TAEDcluster*    fTwClusDisplay;          ///< list of quad to display TW clusters
   
   TAEDcluster*    fCaClusDisplay;          ///< list of quad to display CAL clusters
   
   TAEDwire*       fBmClusDisplay;          ///< list of line to display BM hits
   TAEDtrack*      fBmTrackDisplay;         ///< list of line to display BM tracks
   TEveBoxSet*     fBmDriftCircleDisplay;   ///< list of line to display BM wires

   TAEDglbTrackList* fGlbTrackDisplay;      ///< list of global tracks to display
   TAEDtrack*      fIrTrackDisplay;         ///< list of line to display tracks
   Bool_t          fIrFlag;                 ///< ITR flag
   Bool_t          fFlagTrack;              ///< flag for tracking

   ///< TW
   map< pair<Int_t, Int_t>, Int_t > fFiredTofBar;       ///< list of fired bar per event
   
   ///< CA
   map< Int_t, Int_t >  fFiredCaCrystal;     ///< list of fired bar per event

   ///< GUI
   TGCheckButton*       fClusterButton;      ///< toggle clusters plots
   TGCheckButton*       fRawDataButton;      ///< toggle rawdata plots
   TGCheckButton*       fRateButton;         ///< toggle recompute parameters at each plane
   
protected:
   static Bool_t         fgStdAloneFlag;     ///< flag for standalone DAQ
   static TString        fgVtxTrackingAlgo;  ///< tracking algorithm ("std" with BM, "Full" combinatory)
   static TString        fgItrTrackingAlgo;  ///< tracking algorithm ("std" with BM, "Full" combinatory)
   static TString        fgMsdTrackingAlgo;  ///< tracking algorithm ("std" with BM, "Full" combinatory)
   static Bool_t         fgBmSelectHit;      ///< flag BM selected hit
   static Bool_t         fgMsdsTrackFlag;    ///< flag for MSD tracking
   static Bool_t         fgM28ClusMtFlag;    ///< flag for MT clustering

   ClassDef(TAGbaseEventDisplay, 1); ///< Base class for event display
};


#endif
