
#include "TMath.h"
#include "TGeoMedium.h"
#include "TGeoMaterial.h"

#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveViewer.h"
#include "TEveWindow.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGButton.h"
#include "TGComboBox.h"
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

//__________________________________________________________
TAEDbaseInterface::TAEDbaseInterface(Int_t type, const TString expName)
: TEveEventManager(),
  fExpName(expName),
  fType(type),
  fWorldSizeZ(120),
  fWorldSizeXY(25),
  fWorldName("World"),
  fWorldMedium(0x0),
  fTopVolume(0x0),
  fCurrentEventId(0),
  fMaxEnergy(1024),
  fMaxMomentum(30),
  fBoxDefWidth(0.02),
  fBoxDefHeight(0.02),
  fQuadDefWidth(0.1),
  fQuadDefHeight(0.1),
  fInfoView(0x0),
  fEventEntry(0x0),
  fRefreshButton(0),
  fQuadButton(0),
  fQuadMcButton(0),
  fLineButton(0),
  fGlbButton(0),
  fConsoleButton(0),
  fEventProgress(0),
  fHistoListBox(0),
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

//__________________________________________________________
TAEDbaseInterface::~TAEDbaseInterface()
{
   // default destructor
   delete fListOfCanvases;
   delete fSelHistoListBox;
   delete fSelHistoList;
   
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
 
   if (fQuadButton)
      fQuadButton->Delete();
   if (fQuadMcButton)
      fQuadMcButton->Delete();
   if (fLineButton)
      fLineButton->Delete();
   if (fGlbButton)
      fGlbButton->Delete();
   if (fConsoleButton)
      fConsoleButton->Delete();
   
   gTAGroot->EndEventLoop();
   delete fTopVolume;
}


//__________________________________________________________
void TAEDbaseInterface::BuildDefaultGeometry()
{
   // World
   TGeoMedium* med = gGeoManager->GetMedium("AIR");
   fTopVolume = gGeoManager->MakeBox(fWorldName.Data(),med,fWorldSizeZ, fWorldSizeXY, fWorldSizeXY);
   fTopVolume->SetInvisible();
   gGeoManager->SetTopVolume(fTopVolume);
}

//__________________________________________________________
void TAEDbaseInterface::FillDetectorNames()
{
   std::map<TString, int>::iterator itr = fVolumeNames.begin();
   
   while (itr != fVolumeNames.end()) {
      fDetectorMenu->AddEntry(itr->first.Data(), itr->second);
      fCameraMenu->AddEntry(itr->first.Data(), itr->second);
      fDetectorStatus[itr->second] = true;
      itr++;
   }
   
   fCameraMenu->AddEntry(fWorldName.Data(), kWorld);
   fDetectorMenu->AddEntry("ALL", kWorld);
}

//__________________________________________________________
void TAEDbaseInterface::DefineMaterial()
{
   //hall: fill with vacuum
   TGeoMaterial *mat  = new TGeoMaterial("Vacuum",0,0,0);
   TGeoMedium   *med  = new TGeoMedium("Vacuum",1,mat);
}


//__________________________________________________________
void TAEDbaseInterface::SetWordMedium(TString materialChoice)
{
   // search the material by its name
   TGeoMedium* med = gGeoManager->GetMedium(materialChoice.Data());
   if (med) fWorldMedium = med;
}

//__________________________________________________________
void TAEDbaseInterface::SetWorldSizeZ(Float_t sizeZ)
{
   fWorldSizeZ  = sizeZ;
}

//__________________________________________________________
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
void TAEDbaseInterface::AddGeometry(TGeoVolume* volume, TGeoCombiTrans* transf)
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
   AddMcElements();
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
   
   if (fType == 1 || fType == 2) {
      b = new TGPictureButton(eventFrame, gClient->GetPicture(icondir + "GoBack.gif"));
      eventFrame->AddFrame(b);
      b->SetToolTipText("Prevoius Event");
      b->Connect("Clicked()", "TAEDbaseInterface", this, "PrevEvent()");
   }
   
   b = new TGPictureButton(eventFrame, gClient->GetPicture(icondir + "ReloadPage.gif"));
   eventFrame->AddFrame(b);
   b->SetToolTipText("Loop Event");
   b->Connect("Clicked()", "TAEDbaseInterface", this, "LoopEvent()");
   
   if (fType == 1 || fType == 2) {
      b = new TGPictureButton(eventFrame, gClient->GetPicture(icondir + "GoHome.gif"));
      eventFrame->AddFrame(b);
      b->SetToolTipText("Go To Event");
      b->Connect("Clicked()", "TAEDbaseInterface", this, "SetEvent()");
   }
   
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
   
   // display frame
   TGHorizontalFrame *displayFrame = new TGHorizontalFrame(frmMain);
   
   // check Quad
   fQuadButton = new TGCheckButton(displayFrame, "Hits", 1);
   fQuadButton->SetState(kButtonDown);
   fQuadButton->SetToolTipText("Toggle hits display");
   fQuadButton->Connect("Clicked()", "TAEDbaseInterface", this, "ToggleQuadDisplay()");
   displayFrame->AddFrame(fQuadButton, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   // check Line
   fLineButton = new TGCheckButton(displayFrame, "Tracks", 1);
   fLineButton->SetState(kButtonDown);
   fLineButton->SetToolTipText("Toggle track display");
   fLineButton->Connect("Clicked()", "TAEDbaseInterface", this, "ToggleLineDisplay()");
   displayFrame->AddFrame(fLineButton, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   // check global Line
   fGlbButton = new TGCheckButton(displayFrame, "Glb", 1);
   fGlbButton->SetState(kButtonDown);
   fGlbButton->SetToolTipText("Toggle global track display");
   fGlbButton->Connect("Clicked()", "TAEDbaseInterface", this, "ToggleGlbDisplay()");
   displayFrame->AddFrame(fGlbButton, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));

   // check MC quad
   if (fType == 1) { // MC
      fQuadMcButton = new TGCheckButton(displayFrame, "MC", 1);
      fQuadMcButton->SetState(kButtonDown);
      fQuadMcButton->SetToolTipText("Toggle MC hits display");
      fQuadMcButton->Connect("Clicked()", "TAEDbaseInterface", this, "ToggleMcDisplay()");
      displayFrame->AddFrame(fQuadMcButton, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   }

   frmMain->AddFrame(displayFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 5, 5));

   // detector menu
   TGHorizontalFrame *detectorFrame = new TGHorizontalFrame(frmMain);
   TGLabel*  detectorName = new TGLabel(detectorFrame, "Detector drawn:");
   detectorFrame->AddFrame(detectorName, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 0, 5, 5));

   fDetectorMenu = new TGComboBox(detectorFrame, 10);
   fDetectorMenu->Resize(80, 20);
   fDetectorMenu->Connect("Selected(Int_t)", "TAEDbaseInterface", this, "ToggleDetector(Int_t)");
   detectorFrame->AddFrame(fDetectorMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   frmMain->AddFrame(detectorFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 5, 5));

   // camera center menu
   TGHorizontalFrame *cameraFrame = new TGHorizontalFrame(frmMain);
   TGLabel*  cameraName = new TGLabel(cameraFrame, "Camera center:");
   cameraFrame->AddFrame(cameraName, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 0, 5, 5));
   
   fCameraMenu = new TGComboBox(cameraFrame, 10);
   fCameraMenu->Resize(80, 20);
   fCameraMenu->Connect("Selected(Int_t)", "TAEDbaseInterface", this, "ToggleCamera(Int_t)");
   FillDetectorNames();
   fCameraMenu->Select(kWorld);
   fDetectorMenu->Select(kWorld);
   cameraFrame->AddFrame(fCameraMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   frmMain->AddFrame(cameraFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 5, 5));

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
   infoFrameView->AddFrame(infoName,  new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   // console
   fConsoleButton =  new TGCheckButton(infoFrameView, "Console", 1);
   fConsoleButton->SetState(kButtonUp);
   infoFrameView->AddFrame(fConsoleButton,  new TGLayoutHints(kLHintsTop | kLHintsLeft, 85, 0, -15, 0));
   
   fInfoView = new TGTextView(infoFrameView, 300, 300);
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
   
   fSelHistoListBox = new TList();
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

   fSelHistoList = new TList();
   fSelHistoList->SetOwner(false);
   
   frmMain->AddFrame(infoFrameView, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 5, 5));
   
   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();
   browser->StopEmbedding();
   browser->SetTabTitle("Info", 0);
   
   ConnectElements();
   ConnectMcElements();
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
      ResetAllHisto();
      
   for (Int_t i = 0; i < nEvts; ++i) {
      if (! GetEntry(fCurrentEventId)) return;
      if (!gTAGroot->NextEvent()) return;
      fCurrentEventId++;
      UpdateEventBar();
      
      if (!fRefreshButton->IsOn()) {
         UpdateMcElements();
         UpdateElements();
      }
   }
   
   UpdateMcElements();
   UpdateElements();
   
   gEve->FullRedraw3D(kFALSE);

   fEventProgress->Reset();
   fEventProgress->SetPosition(0.);
   
   UpdateDefCanvases();
}

//__________________________________________________________
void TAEDbaseInterface::NextEvent()
{   
   LoopEvent(1);
}

//__________________________________________________________
void TAEDbaseInterface::SetEvent()
{
   Int_t nEvts = fNumberEvent->GetIntNumber();
   fCurrentEventId = nEvts;
   gTAGroot->SetEventNumber(nEvts-1);
   
   LoopEvent(1);
}

//__________________________________________________________
void TAEDbaseInterface::PrevEvent()
{
   fCurrentEventId -= 2;
   if (fCurrentEventId <= 0) fCurrentEventId = 0;
   gTAGroot->SetEventNumber(fCurrentEventId-1);
   
   LoopEvent(1);
}

//__________________________________________________________
void TAEDbaseInterface::ToggleDetector(Int_t id)
{
   const Char_t* name = fDetectorMenu->GetSelectedEntry()->GetTitle();   
   TGeoVolume* vol    = gGeoManager->FindVolumeFast(name);
   
   TString wName(name);
   if (wName.Contains("ALL")) {
      vol =  gGeoManager->FindVolumeFast(fWorldName.Data());
      
      Int_t nd = vol->GetNdaughters();
      for (Int_t i = 0; i < nd; ++i) {
         TGeoNode* node = vol->GetNode(i);
         TGeoVolume* volD = node->GetVolume();
         
         volD->InvisibleAll(false);
         if (volD->GetNdaughters() != 0)
            volD->SetVisibility(false);
      }
      
      for (Int_t i = 0; i < fDetectorMenu->GetNumberOfEntries(); ++i)
         fDetectorStatus[i] = true;
      
   } else if (vol) {
      if (fDetectorStatus[id] ) {
         vol->InvisibleAll();
         fDetectorStatus[id] = false;
      } else {
         vol->InvisibleAll(false);
         if (vol->GetNdaughters() != 0)
            vol->SetVisibility(false);
         fDetectorStatus[id] = true;
      }
   }
   
   gEve->FullRedraw3D(kFALSE);
}

//__________________________________________________________
void TAEDbaseInterface::ToggleCamera(Int_t id)
{
   const Char_t* name = fCameraMenu->GetSelectedEntry()->GetTitle();
   TGeoVolume* vol    = gGeoManager->FindVolumeFast(fWorldName.Data());

   TEveViewer *ev = gEve->GetDefaultViewer();
   TGLViewer  *gv = ev->GetGLViewer();
   
   TString wName(name);
   if (wName.Contains(fWorldName.Data())) {
      gv->ResetCurrentCamera();
      return;
   }
      
   Int_t nd = vol->GetNdaughters();
   TGeoNode* node = 0x0;
   
   for (Int_t i = 0; i < nd; ++i) {
      node = vol->GetNode(i);
      TString volName(node->GetName());
      if (volName.Contains(name)) {
         id = i;
         break;
      }
   }
   
   TGeoMatrix* mat     = node->GetMatrix();
   const Double_t *pos = mat->GetTranslation();
   
   gv->CurrentCamera().SetCenterVec(pos[0], pos[1], pos[2]);
}

//__________________________________________________________
void TAEDbaseInterface::ToggleQuadDisplay()
{
   ToggleDisplay(1);
}

//__________________________________________________________
void TAEDbaseInterface::ToggleLineDisplay()
{
   ToggleDisplay(2);
}

//__________________________________________________________
void TAEDbaseInterface::ToggleGlbDisplay()
{
   ToggleDisplay(3);
}

//__________________________________________________________
void TAEDbaseInterface::ToggleMcDisplay()
{
   if (fType != 1) return;
   ToggleDisplay(0);
}

//__________________________________________________________
void TAEDbaseInterface::ToggleDisplay(Int_t flag)
{
   TEveEventManager *mgr = gEve->GetCurrentEvent();

   for (TEveElement::List_i i=mgr->BeginChildren(); i!=mgr->EndChildren(); ++i) {
      TEveElement* el = ((TEveElement*)(*i));
      TString ename   = el->GetElementName();
      
      if ((ename.Contains("Cluster") || ename.Contains("Hit")) && !ename.Contains("MC") && flag == 1) {
         if (!fQuadButton->IsOn())
            el->SetRnrState(false);
         else
            el->SetRnrState(true);
         el->ElementChanged(true, true);
      }
      
      if (ename.Contains("Tracks") && !ename.Contains("Global") && flag == 2) {
         if (!fLineButton->IsOn())
            el->SetRnrState(false);
         else
            el->SetRnrState(true);
         el->ElementChanged(true, true);
      }
      
      if (ename.Contains("Global") && flag == 3) {
         if (!fGlbButton->IsOn())
            el->SetRnrState(false);
         else
            el->SetRnrState(true);
         el->ElementChanged(true, true);
      }
      
      if (ename.Contains("MC") && flag == 0) {
         if (!fQuadMcButton->IsOn())
            el->SetRnrState(false);
         else
            el->SetRnrState(true);
         el->ElementChanged(true, true);
      }
   }
}

//__________________________________________________________
void TAEDbaseInterface::HistoSelected(Int_t /*id*/)
{
   // Fill histo list from selection
   fSelHistoListBox->Clear();
   fSelHistoList->Clear();
   fHistoListBox->GetSelectedEntries(fSelHistoListBox);
   Int_t nHisto = fSelHistoListBox->GetEntries();
   
   TList* list = gTAGroot->ListOfAction();
   Int_t hCnt = 0;
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TList* hlist = action->GetHistogrammList();
      if (hlist == 0x0) continue;
      for (Int_t j = 0; j < hlist->GetEntries(); ++j) {
         TH1* h = (TH1*)hlist->At(j);
         
         for (Int_t k = 0; k < fSelHistoListBox->GetEntries(); ++k) {
            
            TGTextLBEntry* t = (TGTextLBEntry*)fSelHistoListBox->At(k);
            if (t == 0x0) continue;
            TString s(t->GetText()->GetString());
            if (s == h->GetName()) {
               fSelHistoList->Add(h);
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
   Int_t nHisto = fSelHistoList->GetEntries();
   
   for (Int_t k = 0; k < nHisto; ++k) {
      TH1* h = (TH1*)fSelHistoList->At(k);
      h->Reset();
   }
}

//__________________________________________________________
void TAEDbaseInterface::ResetAllHisto()
{
   TList* list = gTAGroot->ListOfAction();
   Int_t hCnt = 0;
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TList* hlist = action->GetHistogrammList();
      if (hlist == 0x0) continue;
      for (Int_t j = 0; j < hlist->GetEntries(); ++j) {
         TH1* h = (TH1*)hlist->At(j);
         if (h) h->Reset();
      }
   }
}

//__________________________________________________________
void TAEDbaseInterface::UpdateDefCanvases()
{
   Int_t nCanvas = fListOfCanvases->GetEntries();
   Int_t nHisto = fSelHistoList->GetEntries();
   
   for (Int_t k = 0; k < nHisto; ++k) {
      
      Int_t iCanvas = k / fgMaxHistosN;
      if (iCanvas > 2) continue;
      TCanvas* canvas = (TCanvas*)fListOfCanvases->At(iCanvas);
      if (!canvas) continue;
      
      TH1* h = (TH1*)fSelHistoList->At(k);
      Int_t iCd = k % fgMaxHistosN + 1;
      
      if (nHisto == 1)
         canvas->cd();
      else
         canvas->cd(iCd);
      h->Draw();
      
      canvas->Update();
   }
}

//__________________________________________________________
void TAEDbaseInterface::CreateCanvases()
{
   // GUI
   // histo
   TCanvas* canvas = 0x0;
   TVirtualPad* pad    = 0x0;
   TEveWindowSlot* slot0 = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
   TEveWindowTab*  tab0 = slot0->MakeTab();
   tab0->SetElementName("Histograms");
   tab0->SetShowTitleBar(kFALSE);
   
   // canvas tab
   slot0 = tab0->NewSlot();
   TRootEmbeddedCanvas* eCanvas00 = new TRootEmbeddedCanvas();
   TEveWindowFrame* frame00 = slot0->MakeFrame(eCanvas00);
   frame00->SetElementName("Histograms 1");
   canvas = eCanvas00->GetCanvas();
   canvas->SetName("HistoCanvas 1");
   canvas->Resize();
   fListOfCanvases->Add(canvas);
   
   slot0 = tab0->NewSlot();
   TRootEmbeddedCanvas* eCanvas01 = new TRootEmbeddedCanvas();
   TEveWindowFrame* frame01 = slot0->MakeFrame(eCanvas01);
   frame01->SetElementName("Histograms 2");
   canvas = eCanvas01->GetCanvas();
   canvas->SetName("HistoCanvas 2");
   canvas->Resize();
   fListOfCanvases->Add(canvas);
   
   slot0 = tab0->NewSlot();
   TRootEmbeddedCanvas* eCanvas02 = new TRootEmbeddedCanvas();
   TEveWindowFrame* frame02 = slot0->MakeFrame(eCanvas02);
   frame02->SetElementName("Histograms 3");
   canvas = eCanvas02->GetCanvas();
   canvas->SetName("HistoCanvas 3");
   canvas->Resize();
   fListOfCanvases->Add(canvas);
   
   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();
}

