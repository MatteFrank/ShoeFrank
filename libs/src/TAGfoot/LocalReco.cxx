

#include "LocalReco.hxx"

#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAVTntuRaw.hxx"
#include "TAITntuRaw.hxx"
#include "TAMSDntuRaw.hxx"
#include "TATWdatRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TACAntuRaw.hxx"

#include "TASTdatRaw.hxx"
#include "TABMdatRaw.hxx"
#include "TAMSDdatRaw.hxx"


ClassImp(LocalReco)

Bool_t  LocalReco::fgStdAloneFlag = false;

//__________________________________________________________
LocalReco::LocalReco(TString expName, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, fileNameIn, fileNameout),
   fpDaqEvent(0x0),
   fActWdRaw(0x0),
   fActDatRawBm(0x0),
   fActNtuRawBm(0x0),
   fActNtuRawVtx(0x0),
   fActNtuRawIt(0x0),
//   fActNtuRawMsd(0x0),
//   fpDatRawVtx(0x0),
//   fpDatRawIt(0x0),
//   fpDatRawMsd(0x0),
   fActEvtReader(0x0)
{
   SetRunNumber();
}

//__________________________________________________________
LocalReco::~LocalReco()
{
   // default destructor
   if (fActEvtReader) delete fActEvtReader;
}

//__________________________________________________________
void LocalReco::LoopEvent(Int_t nEvents)
{
   
   for (Int_t ientry = 0; ientry < nEvents; ientry++) {
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      if(GlobalPar::GetPar()->Debug())
         cout<<dec<<" Event:: "<<ientry<<endl;
      
      if (!fTAGroot->NextEvent()) break;;
   }
}

//__________________________________________________________
void LocalReco::CreateRawAction()
{
   if (!fgStdAloneFlag) {
      fpDaqEvent = new TAGdataDsc("daqEvt", new TAGdaqEvent());
      fActEvtReader = new TAGactDaqReader("daqActReader", fpDaqEvent);
   }

   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW() || GlobalPar::GetPar()->IncludeBM()) {
      fpDatRawSt   = new TAGdataDsc("stDat", new TASTdatRaw());
      fpDatRawTw   = new TAGdataDsc("twdDat", new TATWdatRaw());

      fActWdRaw  = new TAGactWDreader("wdActRaw", fpDaqEvent, fpDatRawSt, fpDatRawTw, fpParMapSt, fpParMapTw, fpParTimeSt, fpParTimeTw);
      if (fFlagHisto)
         fActWdRaw->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeST() ||GlobalPar::GetPar()->IncludeBM()) {
      fpNtuRawSt   = new TAGdataDsc("stNtu", new TASTntuRaw());
      fActNtuRawSt = new TASTactNtuRaw("stActNtu", fpDatRawSt, fpNtuRawSt, fpParMapSt);
      if (fFlagHisto)
         fActNtuRawSt->CreateHistogram();
   }

   if (GlobalPar::GetPar()->IncludeBM()) {
      fpDatRawBm = new TAGdataDsc("bmDat", new TABMdatRaw());
      fpNtuRawBm = new TAGdataDsc("bmNtu", new TABMntuRaw());
      
      if (fgStdAloneFlag) {
         fActVmeReaderBm  = new TABMactVmeReader("bmActNtu", fpDatRawBm, fpParMapBm, fpParConfBm, fpParGeoBm, fpNtuRawSt);
         if (fFlagHisto)
            fActVmeReaderBm->CreateHistogram();
         
      } else {
         fActDatRawBm = new TABMactDatRaw("bmActDat", fpDatRawBm, fpDaqEvent, fpParMapBm, fpParConfBm, fpParGeoBm, fpNtuRawSt);
         if (fFlagHisto)
            fActDatRawBm->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActDatRawBm->SetDebugLevel(1);
         
         fActNtuRawBm = new TABMactNtuRaw("bmActNtu", fpNtuRawBm, fpDatRawBm, fpParGeoBm, fpParConfBm);
         if (fFlagHisto)
            fActNtuRawBm->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActNtuRawBm->SetDebugLevel(1);
      }
   }

   if (GlobalPar::GetPar()->IncludeVertex()) {
      fpNtuRawVtx   = new TAGdataDsc("vtRaw", new TAVTntuRaw());
      
      if (fgStdAloneFlag) {
         fActVmeReaderVtx  = new TAVTactVmeReader("vtActNtu", fpNtuRawVtx, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
            fActVmeReaderVtx->CreateHistogram();
         
      } else {
         fActNtuRawVtx = new TAVTactNtuRaw("vtActNtu", fpNtuRawVtx, fpDaqEvent, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
         fActNtuRawVtx->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActNtuRawVtx->SetDebugLevel(1);
      }
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fpNtuRawIt   = new TAGdataDsc("itRaw", new TAITntuRaw());
      fActNtuRawIt = new TAITactNtuRaw("itActNtu", fpNtuRawIt, fpDaqEvent, fpParGeoIt, fpParConfIt, fpParMapIt);
      if (fFlagHisto)
         fActNtuRawIt->CreateHistogram();
   }
   
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      fpDatRawMsd   = new TAGdataDsc("msdDat", new TAVTdatRaw());
//      fpNtuRawMsd   = new TAGdataDsc("msdRaw", new TAMSDntuRaw());
//      fActDatRawMsd = new TAMSDactDaqRaw("msdAcDat", fpDatRawMsd, fpDaqEvent, fpParGeoMsd);
//      fActNtuRawMsd = new TAVTactNtuRaw("msdActNtu", fpNtuRawMsd, fpDatRawMsd, fpParGeoMsd);
//      if (fFlagHisto)
//         fActNtuRawMsd->CreateHistogram();
//   }

   cout<<"Is TW included??  "<<GlobalPar::GetPar()->IncludeTW()<<endl;
   
   if(GlobalPar::GetPar()->IncludeTW()) {
      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATWntuRaw());
      fActNtuRawTw = new TATWactNtuRaw("twActNtu", fpDatRawTw, fpNtuRawTw, fpParGeoTw, fpParMapTw, fpParCalTw);
      if(GlobalPar::GetPar()->Debug()) fActNtuRawTw->SetDebugLevel(1);
      if (fFlagHisto)
         fActNtuRawTw->CreateHistogram();
   }
   
   //   if(GlobalPar::GetPar()->IncludeCA()) {
   //      fpDatRawCa   = new TAGdataDsc("cadDat", new TACAdatRaw());
   //      fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuRaw());
   //      fActNtuRawCa = new TAVTactNtuRaw("caActNtu", fpNtuRawCa, fpNtuRawCa, fpParGeoCa);
   //      fActNtuRawCa->CreateHistogram();
   //   }

}

//__________________________________________________________
void LocalReco::OpenFileIn()
{
   if (fgStdAloneFlag) {
      if (GlobalPar::GetPar()->IncludeVertex())
         fActVmeReaderVtx->Open(GetName());
      
      if (GlobalPar::GetPar()->IncludeBM())
         fActVmeReaderBm->Open(GetName());
      
   } else
      fActEvtReader->Open(GetName());
}

// --------------------------------------------------------------------------------------
void LocalReco::SetRunNumber()
{
   // Done by hand shoud be given by DAQ header
   TString name = GetName();
   if (name.IsNull()) return;
   
   // protection about file name starting with .
   if (name[0] == '.')
      name.Remove(0,1);
      
   Int_t pos1   = name.First(".");
   Int_t len    = name.Length();
   
   TString tmp1 = name(pos1+1, len);
   Int_t pos2   = tmp1.First(".");
   TString tmp  = tmp1(0, pos2);
   
   gTAGroot->SetRunNumber(tmp.Atoi());
}

//__________________________________________________________
void LocalReco::SetRawHistogramDir()
{
   // ST
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()) {
      fActWdRaw->SetHistogramDir((TDirectory*)fActEvtWriter->File());

   }
   if (GlobalPar::GetPar()->IncludeST()) {
      fActNtuRawSt->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }
   
   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
      fActDatRawBm->SetHistogramDir((TDirectory*)fActEvtWriter->File());
      fActNtuRawBm->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }
   
   // VTX
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fActNtuRawVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }
   
   // IT
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fActNtuRawIt->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }

   // TW
   if (GlobalPar::GetPar()->IncludeTW()) {
      fActNtuRawTw->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }
   
   // MSD
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      fActDatRawMsd->SetHistogramDir((TDirectory*)fActEvtWriter->File());
//      fActNtuRawMsd->SetHistogramDir((TDirectory*)fActEvtWriter->File());
//   }
}

//__________________________________________________________
void LocalReco::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
void LocalReco::AddRawRequiredItem()
{
   if (!fgStdAloneFlag)
      fTAGroot->AddRequiredItem("daqActReader");

   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()) {
     fTAGroot->AddRequiredItem("wdActRaw");
   }

   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeBM()) {
      fTAGroot->AddRequiredItem("stActNtu");
   }

   if (GlobalPar::GetPar()->IncludeBM()) {
      fTAGroot->AddRequiredItem("bmActDat");
      fTAGroot->AddRequiredItem("bmActNtu");
   }

   if (GlobalPar::GetPar()->IncludeVertex()) {
      fTAGroot->AddRequiredItem("vtActNtu");
   }
   
//   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
//      fTAGroot->AddRequiredItem("itActNtu");
//   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      fTAGroot->AddRequiredItem("twActNtu");
   }

   
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      fTAGroot->AddRequiredItem("msdActDat");
//      fTAGroot->AddRequiredItem("msdActNtu");
//   }
}

//__________________________________________________________
void LocalReco::SetTreeBranches()
{
   BaseReco::SetTreeBranches();
   
   if (GlobalPar::GetPar()->IncludeST()) {
     if (fFlagHits) {
       fActEvtWriter->SetupElementBranch(fpDatRawSt, TASTdatRaw::GetBranchName());
     }
     fActEvtWriter->SetupElementBranch(fpNtuRawSt, TASTntuRaw::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpDatRawBm, TABMdatRaw::GetBranchName());
     fActEvtWriter->SetupElementBranch(fpNtuRawBm, TABMntuRaw::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawVtx, TAVTntuRaw::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawIt, TAITntuRaw::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawMsd, TAMSDntuRaw::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
     if (fFlagHits) {
         fActEvtWriter->SetupElementBranch(fpDatRawTw, TATWdatRaw::GetBranchName());
     }
     fActEvtWriter->SetupElementBranch(fpNtuRawTw, TATWntuRaw::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawCa, TACAntuRaw::GetBranchName());
   }
}
