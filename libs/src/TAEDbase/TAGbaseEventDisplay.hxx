//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef TAGbaseEventDisplay_h
#define TAGbaseEventDisplay_h 1

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
class TGMainFrame;
class TList;
class TGeoVolume;

class TGeoMedium;
class TAGgeoTrafo;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TAGbaseEventDisplay : public TEveEventManager
{
public:
   TAGbaseEventDisplay(const TString expName = "");
   virtual ~TAGbaseEventDisplay();
   
public:
   //! reset list of histograms
   virtual void ResetHistogram() = 0;
   
   //! update elements
   virtual void UpdateElements() = 0;
   
   //! Create canvases
   virtual void CreateCanvases() = 0;
   
   //! Update normal Canvases
   virtual void UpdateDefCanvases() = 0;
   
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
   
   //! Add elements
   virtual void ConnectElements() = 0;

   virtual void LoopEvent(Int_t nEvts = 0);
   
   virtual Bool_t GetEntry(Int_t /*entry*/) { return true; }
   
   virtual void SetTransparency(Char_t transparency = 50); //*MENU*
   
   virtual void ShowDisplay(const TString fileName);
   
   virtual void LoadGeometry(const Char_t* fileName);
   
   virtual void AddGeometry(TGeoVolume* volume, TGeoMatrix* matrix = gGeoIdentity);
   
   virtual void BuildDefaultGeometry();
   
   virtual void NextEvent(); //*MENU*
   
   void         ClearInfoView();
   void         HistoSelected(Int_t id);
   void         ResetHisto();

   void         MakeGUI();
   
   void         SetWorldSizeZ(Float_t sizeZ);
   void         SetWorldSizeXY(Float_t sizeXY);
	void         SetWordMedium(TString mat);
   
   Float_t      GetWorldSizeZ()     const   { return fWorldSizeZ;     }
   Float_t      GetWorldSizeXY()    const   { return fWorldSizeXY;    }
   
   TGeoMedium*  GetWorldMediuml()           { return fWorldMedium;    }

   Int_t        GetCurrentEventId() const   { return fCurrentEventId; }
   
   void         SetMaxEnergy(Float_t e)     { fMaxEnergy = e;         }
   Float_t      GetMaxEnergy()      const   { return fMaxEnergy;      }
   
   void         SetMaxMomentum(Float_t m)   { fMaxMomentum = m;       }
   Float_t      GetMaxMomentum()    const   { return fMaxMomentum;    }
   
   TList*       GetCanvasList()     const   { return fListOfCanvases; }
   
   //! Set width/height of box
   void         SetBoxDefWidth(Float_t w)   { fBoxDefWidth = w;       }
   void         SetBoxDefHeight(Float_t h)  { fBoxDefHeight = h;      }
   
   //! Set width/height of quad
   void         SetQuadDefWidth(Float_t w)  { fQuadDefWidth = w;      }
   void         SetQuadDefHeight(Float_t h) { fQuadDefHeight = h;     }
   
   //! Get width/height of box
   Float_t      GetBoxDefWidth()    const   { return fBoxDefWidth;    }
   Float_t      GetBoxDefHeight()   const   { return fBoxDefHeight;   }
   
   //! Get width/height of quad
   Float_t      GetQuadDefWidth()   const   { return fQuadDefWidth;   }
   Float_t      GetQuadDefHeight()  const   { return fQuadDefHeight;  }
   
protected:
   void         DefineMaterial();
   void         UpdateEventBar();
  
public:
   //! Disable GUI
   static void  DisableGUI()                { fgGUIFlag = false;      }
   
   //! Disable Display
   static void  DisableDisplay()            { fgDisplayFlag = false;  }
   
   //! Set maximum number of histograms per canvas
   static void  SetMaxHistosCanvas(Int_t m) { fgMaxHistosN = m;       }

protected:
   TString            fExpName;
	Float_t            fWorldSizeZ;
   Float_t            fWorldSizeXY;
   TGeoMedium*        fWorldMedium;
   
   TGeoVolume*        fTopVolume;          // top volume of geometry
   TAGgeoTrafo*       fpFootGeo;           // trafo prointer
   Int_t              fCurrentEventId;     // Current event id
   Bool_t             fFirstEventDone;     // flag for first event processed
   Float_t            fMaxEnergy;          // maximum energy fo palette
   Float_t            fMaxMomentum;        // maximum energy fo palette
   
   //Display
   Float_t            fBoxDefWidth;      // default width of track box
   Float_t            fBoxDefHeight;     // default height of track box
   Float_t            fQuadDefWidth;     // default width of hit quad
   Float_t            fQuadDefHeight;    // default height of hit quad
   
   // GUI
   TGMainFrame*       frmMain;           // main frame
   TGTextView*        fInfoView;         // text view for hit/track info
   TGTextEntry*       fEventEntry;       // text entry for event number
   TGNumberEntry*     fNumberEvent;      // number of events to loop
   TGCheckButton*     fRefreshButton;    // refresh display for each event otherwise superimpose events
   TGHProgressBar*    fEventProgress;    // progress event bar
   TGListBox*         fHistoListBox;     // list of histograms
   TList*             fSelecHistoList;   // list of slected histograms
   TList*             fHistoList;        // list of slected histograms
   
   //histos
   TList*             fListOfCanvases;   // list of canvases

protected:
   static Bool_t      fgIsGeoLoaded;       // flag if geometry loaded
   static Bool_t      fgIsDisplayed;       // flag if a display already opened
   
   static Bool_t      fgGeoDone;           // geometry done flag
   static Bool_t      fgGUIFlag;           // flag to disable or enable gui interface
   static Bool_t      fgDisplayFlag;       // do not display event and do not make clustering/tracking, define before running

   static Int_t       fgMaxHistosN;       // Maximum number per canvas;
   
   ClassDef(TAGbaseEventDisplay, 1); // Base class for event display

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

