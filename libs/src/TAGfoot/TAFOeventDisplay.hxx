
#ifndef _TAFOeventDisplay_HXX_
#define _TAFOeventDisplay_HXX_

#include "TAEDbaseInterface.hxx"

/** TAFOeventDisplay FOOT class to work on event display
 
 */
#include "TAEDcluster.hxx"
#include "TAEDtrack.hxx"
#include "TAEDwire.hxx"
#include "TAEDglbTrack.hxx"
#include "TAEDglbTrackList.hxx"
#include "TAEDbaseInterface.hxx"
#include "TADIeveTrackPropagator.hxx"

/** TAFOeventnDisplay FOOT class to work on event display
 
 */
#include <map>

#include "TVirtualMagField.h"

#include "TADIeveField.hxx"
#include "TADIgeoField.hxx"

#include "BaseReco.hxx"

class TEveDigitSet;
class TGCheckButton;

class TAFOeventDisplay : public TAEDbaseInterface
{
   
protected:
   //! default constructor
   TAFOeventDisplay(Int_t type, const TString expName);
   
protected:
   static TAFOeventDisplay* fgInstance; // static instance of class

public:
   //! Instance of class
   static TAFOeventDisplay* Instance(Int_t type = 0, const TString name = "");
   
   virtual ~TAFOeventDisplay();
      
   virtual void ResetHistogram();
   
   virtual void BuildDefaultGeometry();
   
   virtual void UpdateElements(); 

   //! Create canvases
   virtual void CreateCanvases();
   
   //! Update normal Canvases
   virtual void UpdateDefCanvases();
   
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
   
   //! Set local reconstruction
   virtual void SetLocalReco();
   
   //! Add elements
   virtual void AddElements();
   
   //! Connect elements
   virtual void ConnectElements();
   
   void UpdateHitInfo(TEveDigitSet* qs, Int_t idx);
   void UpdateTrackInfo(TEveDigitSet* qs, Int_t idx);
   void UpdateTrackInfo(TEveTrack* ts);
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
         case 'H':
            fgVtxTrackingAlgo = "Hough";
            break;
         default:
            printf("SetTrackingAlgo: Wrongly set tracking algorithm");
      }
   }
   
   //! Disable/Enable tracking
   static void DisableTracking()    { fgTrackFlag = false;    }
   static void EnableTracking()     { fgTrackFlag = true;     }

   //! Disable/Enable stand alone DAQ
   static void DisableStdAlone()    { fgStdAloneFlag = false; }
   static void EnableStdAlone()     { fgStdAloneFlag = true;  }

   //! Disable/Enable stand alone DAQ
   static void DisableBmSelectHit() { fgBmSelectHit = false;  }
   static void EnableBmSelectHit()  { fgBmSelectHit = true;   }

protected:
   BaseReco*       fReco;    // local reco
   
   //Display
   TAEDcluster*    fStClusDisplay;  // list of quad to display hits

   TAEDcluster*    fVtxClusDisplay;  // list of quad to display hits
   TAEDtrack*      fVtxTrackDisplay; // list of line to display tracks
   
   TAEDcluster*    fItClusDisplay;  // list of quad to display hits
   
   TAEDcluster*    fMsdClusDisplay;  // list of strip to display hits
   
   TAEDcluster*    fTwClusDisplay;  // list of quad to display hits
   
   TAEDcluster*    fCaClusDisplay;  // list of quad to display hits
   
   TAEDwire*       fBmClusDisplay;  // list of line to display wires
   TAEDtrack*      fBmTrackDisplay; // list of line to display tracks
   TEveBoxSet*     fBmDriftCircleDisplay;

   TAEDglbTrackList* fGlbTrackDisplay;  // list of global tracks to display
   TAEDtrack*      fIrTrackDisplay;   // list of line to display tracks
   Bool_t          fIrFlag;

   // Magnet
   TADIgeoField*         fFieldImpl;       // magnetic field implementation
   TADIeveField*         fField;           // Eve magnetic field
   TADIeveTrackPropagator* fGlbTrackProp;  // global track propagator
   
   // TW
   map< pair<Int_t, Int_t>, Int_t > fFiredTofBar;       // list of fired bar per event
   
   // CA
   map< Int_t, Int_t >  fFiredCaCrystal;       // list of fired bar per event

   // GUI
   TGCheckButton*       fClusterButton;    // toggle clusters plots
   TGCheckButton*       fRawDataButton;    // toggle rawdata plots
   TGCheckButton*       fRateButton;       // toggle recompute parameters at each plane
   
protected:
   static Bool_t         fgTrackFlag;       // flag for tracking
   static Bool_t         fgStdAloneFlag;    // flag for standalone DAQ
   static TString        fgVtxTrackingAlgo; // tracking algorithm ("std" with BM, "Full" combinatory and "Hough" Hough transformation)
   static Bool_t         fgBmSelectHit;     // flag BM selected hit

   ClassDef(TAFOeventDisplay, 1); // Base class for event display
};


#endif
