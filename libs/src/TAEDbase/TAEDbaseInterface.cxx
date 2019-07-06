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

#include "TMath.h"
#include "TGeoMedium.h"
#include "TGeoMaterial.h"

#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveWindow.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGButton.h"
#include "TGLViewer.h"
#include "TGLabel.h"
#include "TList.h"
#include "TMath.h"
#include "TNamed.h"
#include "TPad.h"
#include "TPaveLabel.h"
#include "TSystem.h"
#include "TRootEmbeddedCanvas.h"
#include "TVector3.h"
#include "TVirtualPad.h"
#include "Riostream.h"

#include "TAEDbaseInterface.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"
#include "TAGparTools.hxx"

Bool_t TAEDbaseInterface::fgIsGeoLoaded   = false;
Bool_t TAEDbaseInterface::fgIsDisplayed   = false;
Bool_t TAEDbaseInterface::fgGeoDone       = false;
Bool_t TAEDbaseInterface::fgGUIFlag       = true;
Bool_t TAEDbaseInterface::fgDisplayFlag   = true;
Int_t  TAEDbaseInterface::fgMaxHistosN   =  4;

ClassImp(TAEDbaseInterface)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TAEDbaseInterface::TAEDbaseInterface(const TString expName)
: TEveEventManager(),
  fExpName(expName),
  fWorldSizeZ(120),
  fWorldSizeXY(25),
  fWorldMedium(0x0),
  fTopVolume(0x0),
  fCurrentEventId(0),
  fFirstEventDone(false),
  fMaxEnergy(1024),
  fMaxMomentum(30),
  fBoxDefWidth(0.02),
  fBoxDefHeight(0.02),
  fQuadDefWidth(0.1),
  fQuadDefHeight(0.1),
  fInfoView(0x0),
  fEventEntry(0x0),
  fListOfCanvases(new TList())
{ 
  // default constructor

   fListOfCanvases->SetOwner(false);
   
   // check geometry manager
   if ( gGeoManager == NULL )  // a new Geo Manager is created if needed
      new TGeoManager(expName.Data(), "Creating Geometry");
   
   // define material
   DefineMaterial();

   // default parameter values
   TGeoMedium* med = gGeoManager->GetMedium("Vacuum");
	if (med) fWorldMedium = med;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TAEDbaseInterface::~TAEDbaseInterface()
{
   // default destructor
   delete fListOfCanvases;
   delete fSelecHistoList;
   delete fHistoList;
   
   if (fHistoListBox)
      delete fHistoListBox;
   if (fInfoView)
      delete fInfoView;
   if (fEventEntry)
      delete fEventEntry;
   if (fRefreshButton)
      fRefreshButton->Delete();
   if (fNumberEvent)
      fNumberEvent->Delete();
   if (frmMain)
      frmMain->Delete();
   
   gTAGroot->EndEventLoop();
   delete fTopVolume;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TAEDbaseInterface::BuildDefaultGeometry()
{
   // World
   TGeoMedium* med = gGeoManager->GetMedium("AIR");
   fTopVolume = gGeoManager->MakeBox("World",med,fWorldSizeZ, fWorldSizeXY, fWorldSizeXY);
   fTopVolume->SetInvisible();
   gGeoManager->SetTopVolume(fTopVolume);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TAEDbaseInterface::DefineMaterial()
{
   //hall: fill with vacuum
   TGeoMaterial *mat  = new TGeoMaterial("Vacuum",0,0,0);
   TGeoMedium   *med  = new TGeoMedium("Vacuum",1,mat);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TAEDbaseInterface::SetWordMedium(TString materialChoice)
{
   // search the material by its name
   TGeoMedium* med = gGeoManager->GetMedium(materialChoice.Data());
   if (med) fWorldMedium = med;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TAEDbaseInterface::SetWorldSizeZ(Float_t sizeZ)
{
   fWorldSizeZ  = sizeZ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TAEDbaseInterface::SetWorldSizeXY(Float_t sizeXY)
{
   fWorldSizeXY = sizeXY;
}

//__________________________________________________________
void TAEDbaseInterface::LoadGeometry(const Char_t* fileName)
{
   if (fgIsGeoLoaded)
      return;
   
   if ( !TGeoManager::Import(fileName) ) {
      cout << "Failed to load geometry" << endl;
   }
   fgIsGeoLoaded = true;
}

//__________________________________________________________
void TAEDbaseInterface::AddGeometry(TGeoVolume* volume, TGeoMatrix* transf)
{
   static Int_t nb = 0;
   TObjArray* list =  fTopVolume->GetNodes();
   if (list) {
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         TGeoNode* vol = (TGeoNode*)list->At(i);
         TString volName(vol->GetName());
         if (volName.Contains(volume->GetName())) {
            cout << Form("Geometry of %s already loaded", volume->GetName()) << endl;
            return;
         }
      }
   }
   
   fTopVolume->AddNode(volume, nb++, transf);
}

//__________________________________________________________
void TAEDbaseInterface::SetTransparency(Char_t  transparency)
{
   // refresh must be done by user
   if (!gGeoManager)
      return;
   TObjArray* list =  gGeoManager->GetListOfVolumes();
   if (!list) return;
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TGeoVolume* vol = (TGeoVolume*)list->At(i);
      vol->SetTransparency(transparency);
   }
}

//__________________________________________________________
void TAEDbaseInterface::ShowDisplay(const TString fileName)
{
   if (fgIsDisplayed) {
      gEve->FullRedraw3D(kTRUE);
      cout << "Display already open, just redraw canvas" << endl;
      return;
   }
   
   SetFileName(fileName);
   ReadParFiles();
   CreateRawAction();
   CreateRecAction();
   AddRequiredItem();
   
   OpenFile();
   
   BuildDefaultGeometry();
   
   TEveManager::Create();
   gEve->AddEvent(this);
   
   if (gGeoManager) {
      TGeoNode* topNode = gGeoManager->GetTopNode();
      TGeoManager::SetVerboseLevel(0);
      if (topNode == 0x0) {
         cout << "No top node in geometry" << endl;;
         return;
      } else {
         TEveGeoTopNode* out = new TEveGeoTopNode(gGeoManager, topNode);
         gEve->AddGlobalElement(out);
      }
   } else {
      cout << "No geometry loaded" << endl;
   }
   
   if (fgGUIFlag)
      MakeGUI();
   
   TGLViewer* v = gEve->GetDefaultGLViewer();
   
   v->SetClearColor(kWhite);
   
   gEve->FullRedraw3D(kTRUE);
   
   fgIsDisplayed = true;
   
   AddElements();
}

//__________________________________________________________
void TAEDbaseInterface::MakeGUI()
{
   TEveBrowser* browser = gEve->GetBrowser();
   browser->StartEmbedding(TRootBrowser::kLeft);
   
   frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
   frmMain->SetWindowName("XX GUI");
   frmMain->SetCleanup(kDeepCleanup);
   
   // event frame
   TGHorizontalFrame *eventFrame = new TGHorizontalFrame(frmMain);
   
   TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
   TGPictureButton* b = 0x0;
   
   b = new TGPictureButton(eventFrame, gClient->GetPicture(icondir + "GoForward.gif"));
   eventFrame->AddFrame(b);
   b->SetToolTipText("Next Event");
   b->Connect("Clicked()", "TAEDbaseInterface", this, "NextEvent()");
   
   b = new TGPictureButton(eventFrame, gClient->GetPicture(icondir + "ReloadPage.gif"));
   eventFrame->AddFrame(b);
   b->SetToolTipText("Loop Event");
   b->Connect("Clicked()", "TAEDbaseInterface", this, "LoopEvent()");
   
   fNumberEvent  = new TGNumberEntry(eventFrame, 0, 4, -1,
                                     TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative,
                                     TGNumberFormat::kNELLimitMinMax, 1, 1500);
   fNumberEvent->Resize(60,20);
   fNumberEvent->SetNumber(10);
   eventFrame->AddFrame(fNumberEvent, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   fRefreshButton = new TGCheckButton(eventFrame, "Refresh", 1);
   fRefreshButton->SetState(kButtonDown);
   eventFrame->AddFrame(fRefreshButton, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   frmMain->AddFrame(eventFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 5, 5));
   
   // info frame
   TGVerticalFrame *infoFrameView = new TGVerticalFrame(frmMain);
   
   // event entry
   TGLabel*  eventName = new TGLabel(infoFrameView, "Event:");
   infoFrameView->AddFrame(eventName, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 0, 5, 5));
   fEventEntry = new TGTextEntry(infoFrameView);
   infoFrameView->AddFrame(fEventEntry, new TGLayoutHints(kLHintsLeft | kLHintsCenterY  |
                                                          kLHintsExpandX, 2, 10, 0, 10));
   // progress bar
   TGLabel*  barName = new TGLabel(infoFrameView, "Events to Read:");
   infoFrameView->AddFrame(barName, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 0, 5, 5));
   fEventProgress = new TGHProgressBar(infoFrameView, TGProgressBar::kFancy);
   infoFrameView->AddFrame(fEventProgress, new TGLayoutHints(kLHintsLeft | kLHintsCenterY  |
                                                             kLHintsExpandX, 2, 10, 0, 10));
   // selection
   TGLabel*  infoName = new TGLabel(infoFrameView, "Selection:");
   infoFrameView->AddFrame(infoName, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 0, 5, 5));
   
   fInfoView = new TGTextView(infoFrameView, 300, 350);
   fInfoView->ShowBottom();
   infoFrameView->AddFrame(fInfoView, new TGLayoutHints(kLHintsLeft | kLHintsCenterY  |
                                                        kLHintsExpandX, 2, 10, 0, 0));
   // Clear info
   TGTextButton* clearInfo = new TGTextButton(infoFrameView,"&Clear");
   clearInfo->Connect("Clicked()","TAEDbaseInterface",this,"ClearInfoView()");
   clearInfo->SetToolTipText("Clear Info View");
   infoFrameView->AddFrame(clearInfo, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   // Reset histo
   TGTextButton* resetHisto = new TGTextButton(infoFrameView,"&Reset Histo");
   resetHisto->Connect("Clicked()","TAEDbaseInterface",this,"ResetHisto()");
   resetHisto->SetToolTipText("Reset selected histo");
   infoFrameView->AddFrame(resetHisto, new TGLayoutHints(kLHintsTop | kLHintsLeft, 55, 0, -20, 0));

   fHistoListBox = new TGListBox(infoFrameView, 200);
   fHistoListBox->SetMultipleSelections();
   fHistoListBox->Connect("Selected(Int_t)", "TAEDbaseInterface", this, "HistoSelected(Int_t)");

   infoFrameView->AddFrame(fHistoListBox, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 15, 5, 10, 4));
   
   fSelecHistoList = new TList();
   TList* list = gTAGroot->ListOfAction();
   Int_t hCnt = 0;
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TList* hlist = action->GetHistogrammList();
      if (hlist == 0x0) continue;
      for (Int_t j = 0; j < hlist->GetEntries(); ++j) {
         TH1* h = (TH1*)hlist->At(j);
         fHistoListBox->AddEntry(h->GetName(), hCnt);
      }
   }
   fHistoListBox->Resize(200, 130);

   fHistoList = new TList();
   fHistoList->SetOwner(false);
   
   frmMain->AddFrame(infoFrameView, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 5, 5));
   
   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();
   browser->StopEmbedding();
   browser->SetTabTitle("Info", 0);
   
   ConnectElements();
   CreateCanvases();
}

//__________________________________________________________
void TAEDbaseInterface::ClearInfoView()
{
   // clear log message
   fInfoView->Clear();
   fInfoView->ShowBottom();
}

//______________________________________________________________________________
void TAEDbaseInterface::UpdateEventBar()
{
   // display current event number with variable frequency
   Int_t frequency = 1;
   Float_t max     = fEventProgress->GetMax();;
   
   if( max > 100000)      frequency = 10000;
   else if( max > 10000)  frequency = 1000;
   else if( max > 1000)   frequency = 100;
   else if( max > 100)    frequency = 10;
   else if( max > 10)     frequency = 1;
   
   if( fCurrentEventId % frequency == 0) {
      fEventProgress->Increment(frequency);
   }
}

//__________________________________________________________
void TAEDbaseInterface::LoopEvent(Int_t nEvts)
{
   if (!fgGUIFlag) {
      printf("TAEDbaseInterface: No GUI available\n");
      return;
   }
   
   if (nEvts == 0)
      nEvts = fNumberEvent->GetIntNumber();
   
   fEventProgress->SetRange(0, nEvts);
   
   if (fRefreshButton->IsOn())
      ResetHistogram();
      
   for (Int_t i = 0; i < nEvts; ++i) {
      if (! GetEntry(fCurrentEventId)) return;
      if (!gTAGroot->NextEvent()) return;
      fCurrentEventId++;
      UpdateEventBar();
      
      if (!fRefreshButton->IsOn())
         UpdateElements();
   }
   
   UpdateElements();
   
   fEventProgress->Reset();
   fEventProgress->SetPosition(0.);
   
   UpdateDefCanvases();
   
   fFirstEventDone = true;     
}

//__________________________________________________________
void TAEDbaseInterface::NextEvent()
{   
   LoopEvent(1);
}

//__________________________________________________________
void TAEDbaseInterface::HistoSelected(Int_t /*id*/)
{
   // Fill histo list from selection
   fSelecHistoList->Clear();
   fHistoList->Clear();
   fHistoListBox->GetSelectedEntries(fSelecHistoList);
   Int_t nHisto = fSelecHistoList->GetEntries();
   
   TList* list = gTAGroot->ListOfAction();
   Int_t hCnt = 0;
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TList* hlist = action->GetHistogrammList();
      if (hlist == 0x0) continue;
      for (Int_t j = 0; j < hlist->GetEntries(); ++j) {
         TH1* h = (TH1*)hlist->At(j);
         
         for (Int_t k = 0; k < fSelecHistoList->GetEntries(); ++k) {
            
            TGTextLBEntry* t = (TGTextLBEntry*)fSelecHistoList->At(k);
            if (t == 0x0) continue;
            TString s(t->GetText()->GetString());
            if (s == h->GetName()) {
               fHistoList->Add(h);
            }
         }
      }
   }
   
   // Pads creation
   Int_t nCanvas = fListOfCanvases->GetEntries();
   if (nHisto > fgMaxHistosN*nCanvas) {
      Error("HistoSelected()", "Number of histogram out of limit %d (max: %d)", nHisto, fgMaxHistosN*nCanvas);
      return;
   }
   
   Int_t divX[nCanvas];
   Int_t divY[nCanvas];
   
   nCanvas = (nHisto - 1)/fgMaxHistosN + 1;

   divX[nCanvas-1] = ((nHisto - 1) % fgMaxHistosN)/2 + 1;
   divY[nCanvas-1] = 2;
   
   for (Int_t i = nCanvas-2; i >= 0; --i) {
      divX[i] = 2;
      divY[i] = 2;
   }

   for (Int_t i = 0; i < nCanvas; ++i) {
      TCanvas* canvas = (TCanvas*)fListOfCanvases->At(i);
      if (!canvas) continue;
      canvas->Clear();
      canvas->Divide(divX[i], divY[i]);
   }
}

//__________________________________________________________
void TAEDbaseInterface::ResetHisto()
{
   Int_t nHisto = fHistoList->GetEntries();
   
   for (Int_t k = 0; k < nHisto; ++k) {
      TH1* h = (TH1*)fHistoList->At(k);
      h->Reset();
   }
}



