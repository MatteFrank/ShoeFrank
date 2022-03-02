#ifndef TAEDbaseInterface_h
#define TAEDbaseInterface_h 1

/*!
 \file TAEDbaseInterface.hxx
 \brief   Declaration of TAEDbaseInterface.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TEveEventManager.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TGNumberEntry.h"
#include "TGProgressBar.h"
#include "TGTextView.h"
#include "TGTextEntry.h"
#include "TGListBox.h"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGroot.hxx"

class TEveDigitSet;
class TGCheckButton;
class TGComboBox;
class TGMainFrame;
class TList;
class TGeoVolume;

class TGeoMedium;
class TAGgeoTrafo;

class TAEDbaseInterface : public TEveEventManager
{
public:
   //! enum detectors
   enum Detectors {
      kSTC, kBMN, kTGT, kVTX, kDIP, kITR, kMSD, kTOF, kCAL, kWorld
   };
   
public:
   TAEDbaseInterface(Int_t type, const TString expName = "", Int_t runNumber = -1);
   virtual ~TAEDbaseInterface();
   
   //! update elements
   virtual void UpdateElements() = 0;
   
   //! Add required items
   virtual void AddRequiredItem() = 0;
   
   //! Create raw data action
   virtual void CreateRawAction() = 0;
   
   //! Create raw reco action
   virtual void CreateRecAction() = 0;
   
   //! Read parameters files
   virtual void ReadParFiles() = 0;
   
   //! Open File
   virtual void SetFileName(const TString fileName) = 0;
   
   //! Open File
   virtual void OpenFile() = 0;
   
   //! Add elements
   virtual void AddElements() = 0;
   
   //! Connect elements
   virtual void ConnectElements() = 0;

   //! Loop over event
   virtual void LoopEvent(Int_t nEvts = 0);
   
   // Create canvases
   virtual void CreateCanvases();
   
   // Update normal Canvases
   virtual void UpdateDefCanvases();
   
   // reset list of histograms
   virtual void ResetAllHisto();
   
   //! MC virtual methods
   virtual Bool_t GetEntry(Int_t /*entry*/) { return true; }
   
   //! Skip event
   virtual Bool_t SkipEvent()               { return true; }

   //! Update MC elements
   virtual void UpdateMcElements()          { return;      }
   
   //! Connect MC elements
   virtual void ConnectMcElements()         { return;      }
   
   //! Add MC elements
   virtual void AddMcElements()             { return ;     }

   // Set transparency for volumes
   virtual void SetTransparency(Char_t transparency = 50); //*MENU*
   
   // Display
   virtual void ShowDisplay(const TString fileName);
   
   // Load geometry
   virtual void LoadGeometry(const Char_t* fileName);
   
   // Add geometry
   virtual void AddGeometry(TGeoVolume* volume, TGeoCombiTrans* matrix = 0x0);
   
   // Build default geometry
   virtual void BuildDefaultGeometry();
   
   // Next event
   virtual void NextEvent(); //*MENU*

   // Previous event
   virtual void PrevEvent(); //*MENU*

   // Go to event
   virtual void SetEvent(); //*MENU*

   // Toggel display detector
   virtual void ToggleDisplay(Int_t id);
   
   // Toggle quad display
   void         ToggleQuadDisplay();
   // Toggle line display
   void         ToggleLineDisplay();
   // Toggle global track display
   void         ToggleGlbDisplay();
   // Toggle MC quad display
   void         ToggleMcDisplay();
   // Toggle detector display
   void         ToggleDetector(Int_t id);
   // Toggle camera
   void         ToggleCamera(Int_t id);

   // Clear info view
   void         ClearInfoView();
   // Selected histogram
   void         HistoSelected(Int_t id);
   // Reset histogram
   void         ResetHisto();

   // Make gui
   void         MakeGUI();
   
   // Set world size in Z
   void         SetWorldSizeZ(Float_t sizeZ);
   // Set world size in XY
   void         SetWorldSizeXY(Float_t sizeXY);
   // Set world medium
	void         SetWordMedium(TString mat);
   
   //! Get world size in Z
   Float_t      GetWorldSizeZ()     const   { return fWorldSizeZ;     }
   //! Get world size in XY
   Float_t      GetWorldSizeXY()    const   { return fWorldSizeXY;    }
   //! Get world medium
   TGeoMedium*  GetWorldMediuml()           { return fWorldMedium;    }

   //! Get current event id
   Int_t        GetCurrentEventId() const   { return fCurrentEventId; }
   //! Set max energy
   void         SetMaxEnergy(Float_t e)     { fMaxEnergy = e;         }
   //! Get max energy
   Float_t      GetMaxEnergy()      const   { return fMaxEnergy;      }
   
   //! Set momentum max
   void         SetMaxMomentum(Float_t m)   { fMaxMomentum = m;       }
   //! Get momentum max
   Float_t      GetMaxMomentum()    const   { return fMaxMomentum;    }
   //! Get canvas list
   TList*       GetCanvasList()     const   { return fListOfCanvases; }
   
   //! Set width of box
   void         SetBoxDefWidth(Float_t w)   { fBoxDefWidth = w;       }
   //! Set height of box
   void         SetBoxDefHeight(Float_t h)  { fBoxDefHeight = h;      }
   
   //! Set width of quad
   void         SetQuadDefWidth(Float_t w)  { fQuadDefWidth = w;      }
   //! Set height of quad
   void         SetQuadDefHeight(Float_t h) { fQuadDefHeight = h;     }
   
   //! Get width of box
   Float_t      GetBoxDefWidth()    const   { return fBoxDefWidth;    }
   //! Get height of box
   Float_t      GetBoxDefHeight()   const   { return fBoxDefHeight;   }
   
   //! Get width of quad
   Float_t      GetQuadDefWidth()   const   { return fQuadDefWidth;   }
   //! Get height of quad
   Float_t      GetQuadDefHeight()  const   { return fQuadDefHeight;  }
   
protected:
   // Define material
   void         DefineMaterial();
   // Update event bar
   void         UpdateEventBar();
   // Fill detector name
   void         FillDetectorNames();

public:
   //! Disable GUI
   static void  DisableGUI()                { fgGUIFlag = false;      }
   
   //! Disable Display
   static void  DisableDisplay()            { fgDisplayFlag = false;  }
   
   //! Set maximum number of histograms per canvas
   static void  SetMaxHistosCanvas(Int_t m) { fgMaxHistosN = m;       }

protected:
   TString            fExpName;          ///< Experiment name
   Int_t              fType;             ///< type (MC/raw data)
   Int_t              fRunNumber;        ///< run number
   Float_t            fWorldSizeZ;       ///< World size Z
   Float_t            fWorldSizeXY;      ///< World size XY
   TString            fWorldName;        ///< World name
   TGeoMedium*        fWorldMedium;      ///< World medium
   
   TGeoVolume*        fTopVolume;        ///< top volume of geometry
   TAGgeoTrafo*       fpFootGeo;         ///< trafo prointer
   Int_t              fCurrentEventId;   ///< Current event id
   Float_t            fMaxEnergy;        ///< maximum energy fo palette
   Float_t            fMaxMomentum;      ///< maximum energy fo palette
   
   //Display
   Float_t            fBoxDefWidth;      ///< default width of track box
   Float_t            fBoxDefHeight;     ///< default height of track box
   Float_t            fQuadDefWidth;     ///< default width of hit quad
   Float_t            fQuadDefHeight;    ///< default height of hit quad
   
   // GUI
   TGMainFrame*       frmMain;           ///< main frame
   TGTextView*        fInfoView;         ///< text view for hit/track info
   TGTextEntry*       fEventEntry;       ///< text entry for event number
   TGNumberEntry*     fNumberEvent;      ///< number of events to loop
   TGCheckButton*     fRefreshButton;    ///< refresh display for each event otherwise superimpose events
   TGCheckButton*     fQuadButton;       ///< Toggle on/off clusters/hits
   TGCheckButton*     fQuadMcButton;     ///< Toggle on/off MC clusters/hits
   TGCheckButton*     fLineButton;       ///< Toggle on/off tracks
   TGCheckButton*     fGlbButton;        ///< Toggle on/off global tracks
   TGCheckButton*     fConsoleButton;    ///< Toggle on/off selection also on console
   TGHProgressBar*    fEventProgress;    ///< progress event bar
   TGComboBox*        fDetectorMenu;     ///< list of detector drawn
   TGComboBox*        fCameraMenu;       ///< list of camera centers
   TGListBox*         fHistoListBox;     ///< list of histograms
   TList*             fSelHistoListBox;  ///< list of selected histograms list in the box menu
   TList*             fSelHistoList;     ///< list of selected histograms
   
   // histos
   TList*             fListOfCanvases;     ///< list of canvases
   std::map<TString, int> fVolumeNames;    ///<  list of detector volume names
   Bool_t             fDetectorStatus[20]; ///< status detectors

protected:
   static Bool_t      fgIsGeoLoaded;       ///< flag if geometry loaded
   static Bool_t      fgIsDisplayed;       ///< flag if a display already opened
   
   static Bool_t      fgGeoDone;           ///< geometry done flag
   static Bool_t      fgGUIFlag;           ///< flag to disable or enable gui interface
   static Bool_t      fgDisplayFlag;       ///< do not display event and do not make clustering/tracking, define before running

   static Int_t       fgMaxHistosN;        ///< Maximum number per canvas;
   
   ClassDef(TAEDbaseInterface, 2); ///< Base class for event display

};


#endif

