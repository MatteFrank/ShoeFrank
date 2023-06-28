#include "TACEwaveDisplay.hxx"

#include "TH2F.h"
#include <TMath.h>
#include <TString.h>
#include <TVector2.h>
#include <TCanvas.h>

#include "TAPLparGeo.hxx"
#include "TACEparGeo.hxx"
#include "TAGactionFile.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGrecoManager.hxx"


ClassImp(TACEwaveDisplay)

TACEwaveDisplay* TACEwaveDisplay::fgInstance = 0x0;

//__________________________________________________________
TACEwaveDisplay* TACEwaveDisplay::Instance(const TString name, const TString expName, Int_t runNumber)
{
   if (fgInstance == 0x0)
      fgInstance = new TACEwaveDisplay(name, expName, runNumber);
   
   return fgInstance;
}

//_________________________________________________________________
TACEwaveDisplay::TACEwaveDisplay(const TString name, const TString expName, Int_t runNumber)
: TGFrame(gClient->GetRoot(), 1200, 600),
   fExpName(expName),
   fRunNumber(runNumber),
   fpDatRawSt(0x0),
   fpDatRawTw(0x0),
   fpDatRawPw(0x0),
   fpNtuRawSt(0x0),
   fpNtuRawTw(0x0),
   fpNtuRawPw(0x0),
   fpMapWc(0x0),
   fkMainWindow(gClient->GetRoot()),
   fMain(new TGMainFrame(gClient->GetRoot(), 1200, 600)),
   fEcanvas(0x0),
   fPlaneEntry(0x0),
   fNextPlaneEntry(0x0),
   fyEntry(0x0),
   fEventsN(0),
   fSTflag(0),
   fTWflag(0),
   fPWflag(0)
{
   fAGRoot = new TAGroot();
   
   // Par instance
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   
   // Campaign manager
   fCampManager = new TAGcampaignManager(expName);
   fCampManager->FromFile();

   CreateActions(name);
   // Create canvas widget
   Int_t width  = Int_t(1200*0.99);
   Int_t height = Int_t(600*0.99);
   
   fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain, width, height);
   
   // Create a horizontal frame widget with buttons
   TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,width,height/6);
   
   TGTextButton *exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
   hframe->AddFrame(exit, new TGLayoutHints(kLHintsLeft| kLHintsCenterY,15,5,3,4));
   
   TGTextButton *nextEvent = new TGTextButton(hframe,"&Next Event");
   nextEvent->Connect("Clicked()","TACEwaveDisplay",this,"NextEvent()");
   hframe->AddFrame(nextEvent, new TGLayoutHints(kLHintsLeft| kLHintsCenterY,15,5,3,4));
   
   fEventEntry = new TGTextEntry(hframe);
   fEventEntry->Resize(40,20);
   hframe->AddFrame(fEventEntry, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 5, 0, 5));
   
   TGTextButton *loopEvent = new TGTextButton(hframe,"&Loop Event");
   loopEvent->Connect("Clicked()","TACEwaveDisplay",this,"LoopEvent()");
   hframe->AddFrame(loopEvent, new TGLayoutHints(kLHintsLeft| kLHintsCenterY,5,5,3,4));
   
   fLoopEntry = new TGNumberEntry(hframe, 0, 4, -1,
                                  TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative,
                                  TGNumberFormat::kNELLimitMinMax, 1, 15);
   fLoopEntry->Resize(60,20);
   fLoopEntry->SetNumber(1);
   fLoopEntry->Associate(this);
   hframe->AddFrame(fLoopEntry, new TGLayoutHints(kLHintsLeft, 5, 0, 5, 0));
   
   Int_t i = 0;
   Char_t text[20];
   
   // reset button
   TGTextButton *reset = new TGTextButton(hframe,"&Reset");
   reset->Connect("Clicked()","TACEwaveDisplay",this,"Reset()");
   hframe->AddFrame(reset, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,10,5,3,4));
   
   // Save view
   TGTextButton* saveView = new TGTextButton(hframe,"&SaveView");
   saveView->Connect("Clicked()","TACEwaveDisplay",this,"SaveView()");
   saveView->SetToolTipText("Save View");
   hframe->AddFrame(saveView, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 0, 5, 0));
   
   fSaveViewEntry = new TGTextEntry(hframe);
   fSaveViewEntry->Resize(120, fSaveViewEntry->GetDefaultHeight());
   fSaveViewEntry->SetText("view.png");
   hframe->AddFrame(fSaveViewEntry, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,
                                                      2, 10, 0, 10));
   
   // log mesg
   TGHorizontalFrame *logFrame = new TGHorizontalFrame(fMain, width/2, height/6);
   
   TGLabel*  logName = new TGLabel(logFrame, "Log MSG:");
   logFrame->AddFrame(logName, new TGLayoutHints(kLHintsCenterX | kLHintsLeft | kLHintsCenterY, 10, 0, 2, 0));
   
   fLogMessage = new TGTextView(logFrame, width/2, 90);
   fLogMessage->ShowBottom();
   
   logFrame->AddFrame(fLogMessage, new TGLayoutHints(kLHintsLeft, 10, 0, 2, 0));
   
   // clear log mesg
   TGTextButton* clearLog = new TGTextButton(logFrame,"&Clear");
   clearLog->Connect("Clicked()","TACEwaveDisplay",this,"ClearLogMessage()");
   clearLog->SetToolTipText("Clear log message");
   logFrame->AddFrame(clearLog, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,15,5,3,4));
   
   // frame
   fMain->AddFrame(hframe, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,10,10));
   fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsCenterX,	10,10,10,10));
   fMain->AddFrame(logFrame, new TGLayoutHints(kLHintsLeft,2,2,2,10));
   
   // Set a name to the main frame
   fMain->SetWindowName("TACEwaveDisplay");
   
   // Map all subwindows of main frame
   fMain->MapSubwindows();
   
   // Initialize the layout algorithm
   fMain->Resize(fMain->GetDefaultSize());
   
   // Map main frame
   fMain->MapWindow();
   
   Int_t bins = 1024;
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fhOscSt = new TH1F("hOscSt", "ST Wave", bins, 0, 640);
      fhOscSt->SetStats(0);
      fLElineSt = new TLine();
      fFALLlineSt = new TLine();
      fSTflag++;
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fhOscTw = new TH1F("hOscTw", "TW Wave", bins, 0, 640);
      fhOscTw->SetStats(0);
      fLElineTw = new TLine();
      fFALLlineTw = new TLine();
      fTWflag++;
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fhOscPw = new TH1F("hOscPw", "PW Wave", bins, 0, 640);
      fhOscPw->SetStats(0);
      fLElinePw = new TLine();
      fFALLlinePw = new TLine();
      fPWflag++;
   }
   
   fEcanvas->GetCanvas()->Divide(fSTflag+fTWflag+fPWflag,1);
}

//____________________________________________________________
TACEwaveDisplay::~TACEwaveDisplay()
{
   /// Clean up used widgets: frames, buttons, layouthints
   
   fMain->Cleanup();
   delete fMain;
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      delete fhOscSt;
      delete fLElineSt;
      delete fFALLlineSt;
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      delete fLElineTw;
      delete fhOscTw;
      delete fFALLlineTw;
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      delete fLElinePw;
      delete fhOscPw;
      delete fFALLlinePw;
      delete fFASTlinePw;
   }
}

//__________________________________________________________
void TACEwaveDisplay::CreateActions(const TString name)
{
   //------ defeinition of actions --------
   fpMapWc  = new TAGparaDsc("wcMap", new TAGbaseWCparMap());
   TAGbaseWCparMap* map = (TAGbaseWCparMap*) fpMapWc->Object();
   TString parFileName = fCampManager->GetCurMapFile(TAPLparGeo::GetBaseName(), fRunNumber);
   map->FromFile(parFileName.Data());

   if (TAGrecoManager::GetPar()->IncludeST())
      fpDatRawSt = new TAGdataDsc("stRaw", new TAPLntuRaw());
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      fpDatRawTw = new TAGdataDsc("twRaw", new TACEntuRaw());
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      fpDatRawPw = new TAGdataDsc("pwRaw", new TAPWntuRaw());
   
   fActEvtReader = new TAGactWCreader("wcFile", fpMapWc, fpDatRawSt, fpDatRawTw, fpDatRawPw);
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fpNtuRawSt = new TAGdataDsc("stNtu", new TAPLntuHit());
      fActNtuSt = new TAPLactNtuHit("stActNtu", fpNtuRawSt, fpDatRawSt);
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuRawTw = new TAGdataDsc("twNtu", new TACEntuHit());
      fActNtuTw = new TACEactNtuHit("twActNtu", fpNtuRawTw, fpDatRawTw);
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpNtuRawPw = new TAGdataDsc("pwNtu", new TAPWntuHit());
      fActNtuPw = new TAPWactNtuHit("pwActNtu", fpNtuRawPw, fpDatRawPw);
   }
   
   char path[400];
   strcpy(path, name.Data());
   fActEvtReader->Open(path);
   
   fAGRoot->AddRequiredItem("wcFile");
   
   if (TAGrecoManager::GetPar()->IncludeST())
      fAGRoot->AddRequiredItem("stActNtu");
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      fAGRoot->AddRequiredItem("twActNtu");
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      fAGRoot->AddRequiredItem("pwActNtu");
   
   fAGRoot->BeginEventLoop();
   fAGRoot->Print();
}

//__________________________________________________________
Int_t  TACEwaveDisplay::GetHistoNumber(const Char_t* nameHisto)
{
   TString name(nameHisto);
   
   Int_t len  = name.Length();
   TString tmp = name(len-1, len);
   
   return tmp.Atoi();
}

//__________________________________________________________
//! Save view
void TACEwaveDisplay::SaveView()
{
   TString buf = fSaveViewEntry->GetDisplayText();
   
   fEcanvas->GetCanvas()->Print(buf.Data());
}

//__________________________________________________________
void TACEwaveDisplay::NextEvent()
{
   /// loop event
   LoopEvent(1);
}

//__________________________________________________________
void TACEwaveDisplay::LoopEvent(Int_t nEvents)
{
   if (TAGrecoManager::GetPar()->IncludeST())
      fhOscSt->Reset("ICE");
   if (TAGrecoManager::GetPar()->IncludeTW())
      fhOscTw->Reset("ICE");
   if (TAGrecoManager::GetPar()->IncludeCA())
      fhOscPw->Reset("ICE");
   
   if (nEvents == 0)
      nEvents = fLoopEntry->GetIntNumber();
   
   /// Next event
   for (Int_t i = 0; i < nEvents; ++i) {
      if (!fAGRoot->NextEvent()) return;
      fEventEntry->SetText(Form("%d", fEventsN));
      fEventsN++;
   }
   
   DrawMap();
}

//__________________________________________________________
void TACEwaveDisplay::DrawMap()
{
   TAPLntuRaw* pDatRawSt = 0x0;
   TAPLntuHit* pNtuRawSt = 0x0;
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      pDatRawSt = (TAPLntuRaw*) fpDatRawSt->Object();
      pNtuRawSt = (TAPLntuHit*) fpNtuRawSt->Object();
   }
   
   TACEntuRaw* pDatRawTw = 0x0;
   TACEntuHit* pNtuRawTw = 0x0;
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      pDatRawTw = (TACEntuRaw*) fpDatRawTw->Object();
      pNtuRawTw = (TACEntuHit*) fpNtuRawTw->Object();
   }
   
   TAPWntuRaw* pDatRawPw = 0x0;
   TAPWntuHit* pNtuRawPw = 0x0;
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      pDatRawPw = (TAPWntuRaw*) fpDatRawPw->Object();
      pNtuRawPw = (TAPWntuHit*) fpNtuRawPw->Object();
   }
   
   static Int_t   samples = 0;
   static Float_t period  = 0;
   
   static bool first = false;
   if (!first) {
      if (TAGrecoManager::GetPar()->IncludeST()) {
         samples = pDatRawSt->GetSamplesN();
         period  = pDatRawSt->GetPeriod();
         fhOscSt->SetBins(1024, 0, TMath::Nint(samples*period*TAGgeoTrafo::PsToNs()));
      }
      if (TAGrecoManager::GetPar()->IncludeTW()) {
         samples = pDatRawTw->GetSamplesN();
         period  = pDatRawTw->GetPeriod();
         fhOscTw->SetBins(1024, 0, TMath::Nint(samples*period*TAGgeoTrafo::PsToNs()));
      }
      if (TAGrecoManager::GetPar()->IncludeCA()) {
         samples = pDatRawPw->GetSamplesN();
         period  = pDatRawPw->GetPeriod();
         fhOscPw->SetBins(1024, 0, TMath::Nint(samples*period*TAGgeoTrafo::PsToNs()));
      }
      first = true;
   }
   
   fLogMessage->AddLine(Form("Event %d\n", fEventsN));
   
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      TAPLrawHit* hitRawSt = pDatRawSt->GetHit();
      TAPLhit*    hitSt    = pNtuRawSt->GetHit();
      
      if (hitSt) {
         for (Int_t s = 0; s < samples; ++s)
            fhOscSt->Fill(hitRawSt->GetVectT(s), hitRawSt->GetVectA(s));
         
         fEcanvas->GetCanvas()->cd(fSTflag);
         fhOscSt->Draw("hist");
         fLElineSt->SetX1(hitSt->GetTimeLE());
         fLElineSt->SetX2(hitSt->GetTimeLE());
         fLElineSt->SetY1(fhOscSt->GetMaximum());
         fLElineSt->SetY2(fhOscSt->GetMinimum());
         fLElineSt->Draw();
         
         fFALLlineSt->SetX1(hitSt->GetGate(hitSt->kTfall));
         fFALLlineSt->SetX2(hitSt->GetGate(hitSt->kTfall));
         fFALLlineSt->SetY1(fhOscSt->GetMaximum());
         fFALLlineSt->SetY2(fhOscSt->GetMinimum());
         fFALLlineSt->Draw();
         
         fLogMessage->AddLine(Form("ST - timeLE %.1f timeCFD %.1f amplitude %.1f\n", pNtuRawSt->GetHit()->GetTimeLE(),
                                   pNtuRawSt->GetHit()->GetTimeCFD(), pNtuRawSt->GetHit()->GetAmplitude() ));
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      TACErawHit* hitRawTw = pDatRawTw->GetHit();
      TACEhit*    hitTw    = pNtuRawTw->GetHit();
      
      if (hitTw) {
         for (Int_t s = 0; s < samples; ++s)
            fhOscTw->Fill(hitRawTw->GetVectT(s), hitRawTw->GetVectA(s));
         
         fEcanvas->GetCanvas()->cd(fSTflag+fTWflag);
         fhOscTw->Draw("hist");
         fLElineTw->SetX1(hitTw->GetTimeLE());
         fLElineTw->SetX2(hitTw->GetTimeLE());
         fLElineTw->SetY1(fhOscTw->GetMaximum());
         fLElineTw->SetY2(fhOscTw->GetMinimum());
         fLElineTw->Draw();
         
         fFALLlineTw->SetX1(hitTw->GetGate(hitTw->kTfall));
         fFALLlineTw->SetX2(hitTw->GetGate(hitTw->kTfall));
         fFALLlineTw->SetY1(fhOscTw->GetMaximum());
         fFALLlineTw->SetY2(fhOscTw->GetMinimum());
         fFALLlineTw->Draw();
         
         fLogMessage->AddLine(Form("TW - timeLE %.1f timeCFD %.1f amplitude %.1f\n", pNtuRawTw->GetHit()->GetTimeLE(),
                                   pNtuRawTw->GetHit()->GetTimeCFD(), pNtuRawTw->GetHit()->GetAmplitude() ));
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      TAPWrawHit* hitRawPw = pDatRawPw->GetHit();
      TAPWhit*    hitPw    = pNtuRawPw->GetHit();
      
      if (hitPw) {
         for (Int_t s = 0; s < samples; ++s)
            fhOscPw->Fill(hitRawPw->GetVectT(s), hitRawPw->GetVectA(s));
         
         fEcanvas->GetCanvas()->cd(fSTflag+fTWflag+fPWflag);
         fhOscPw->Draw("hist");
         fLElinePw->SetX1(hitPw->GetTimeLE());
         fLElinePw->SetX2(hitPw->GetTimeLE());
         fLElinePw->SetY1(fhOscPw->GetMaximum());
         fLElinePw->SetY2(fhOscPw->GetMinimum());
         fLElinePw->Draw();
         
         fFALLlinePw->SetX1(hitPw->GetGate(hitPw->kTfall));
         fFALLlinePw->SetX2(hitPw->GetGate(hitPw->kTfall));
         fFALLlinePw->SetY1(fhOscPw->GetMaximum());
         fFALLlinePw->SetY2(fhOscPw->GetMinimum());
         fFALLlinePw->Draw();
         
         fFASTlinePw->SetX1(hitPw->GetGate(hitPw->kTfast));
         fFASTlinePw->SetX2(hitPw->GetGate(hitPw->kTfast));
         fFASTlinePw->SetY1(fhOscPw->GetMaximum());
         fFASTlinePw->SetY2(fhOscPw->GetMinimum());
         fFASTlinePw->Draw();
      }
   }
   fLogMessage->ShowBottom();
   
   fEcanvas->GetCanvas()->Update();
}

//__________________________________________________________
void TACEwaveDisplay::Reset()
{
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fhOscSt->Reset();
      fEcanvas->GetCanvas()->cd(fSTflag);
      fhOscSt->Draw();
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fhOscTw->Reset();
      fEcanvas->GetCanvas()->cd(fSTflag+fTWflag);
      fhOscTw->Draw();
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fhOscPw->Reset();
      fEcanvas->GetCanvas()->cd(fSTflag+fTWflag+fPWflag);
      fhOscPw->Draw();
   }
   
   fEcanvas->GetCanvas()->Update();
}

//__________________________________________________________
void TACEwaveDisplay::ClearLogMessage()
{
   /// clear log message
   fLogMessage->GetText()->Clear();
   fLogMessage->AddLine("ClearLogMessage: clear log messages:");
   fLogMessage->ShowBottom();
}

