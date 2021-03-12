


#include "LocalReco.hxx"

#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuHit.hxx"
#include "TACAntuHit.hxx"

#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAMSDdatRaw.hxx"


ClassImp(LocalReco)

Bool_t  LocalReco::fgStdAloneFlag = false;

//__________________________________________________________
LocalReco::LocalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout),
   fpDaqEvent(0x0),
   fActWdRaw(0x0),
   fActDatRawBm(0x0),
   fActNtuHitBm(0x0),
   fActNtuHitVtx(0x0),
   fActNtuHitIt(0x0),
   fActNtuHitCa(0x0),
//   fActNtuHitMsd(0x0),
//   fpDatRawMsd(0x0),
   fActEvtReader(0x0)
{
}

//__________________________________________________________
LocalReco::~LocalReco()
{
   // default destructor
}

//__________________________________________________________
void LocalReco::CreateRawAction()
{
   if (!fgStdAloneFlag) {
      fpDaqEvent = new TAGdataDsc("daqEvt", new TAGdaqEvent());
      fActEvtReader = new TAGactDaqReader("daqActReader", fpDaqEvent);
   }

   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW() || GlobalPar::GetPar()->IncludeBM() || GlobalPar::GetPar()->IncludeCA()) {
      fpDatRawSt   = new TAGdataDsc("stDat", new TASTntuRaw());
      fpDatRawTw   = new TAGdataDsc("twdDat", new TATWntuRaw());
      fpDatRawCa   = new TAGdataDsc("caDat", new TACAntuRaw());
      
      fActWdRaw  = new TAGactWDreader("wdActRaw", fpDaqEvent, fpDatRawSt, fpDatRawTw, fpDatRawCa, fpParMapWD, fpParTimeWD);
      if (fFlagHisto)
         fActWdRaw->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeST() ||GlobalPar::GetPar()->IncludeBM()) {
      fpNtuHitSt   = new TAGdataDsc("stNtu", new TASTntuHit());
      fActNtuHitSt = new TASTactNtuHit("stActNtu", fpDatRawSt, fpNtuHitSt, fpParMapSt);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }

   if (GlobalPar::GetPar()->IncludeBM()) {
      fpDatRawBm = new TAGdataDsc("bmDat", new TABMntuRaw());
      fpNtuHitBm = new TAGdataDsc("bmNtu", new TABMntuHit());
      
      if (fgStdAloneFlag) {
         fActVmeReaderBm  = new TABMactVmeReader("bmActNtu", fpDatRawBm, fpParMapBm, fpParCalBm, fpParGeoBm);
         if (fFlagHisto)
            fActVmeReaderBm->CreateHistogram();
         
      } else {
         fActDatRawBm = new TABMactNtuRaw("bmActDat", fpDatRawBm, fpDaqEvent, fpParMapBm, fpParCalBm, fpParGeoBm, fpNtuHitSt);
         if (fFlagHisto)
            fActDatRawBm->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActDatRawBm->SetDebugLevel(1);
         
         fActNtuHitBm = new TABMactNtuHit("bmActNtu", fpNtuHitBm, fpDatRawBm, fpParGeoBm, fpParConfBm, fpParCalBm);
         if (fFlagHisto)
            fActNtuHitBm->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActNtuHitBm->SetDebugLevel(1);
      }
   }

   if (GlobalPar::GetPar()->IncludeVT()) {
      fpNtuHitVtx   = new TAGdataDsc("vtRaw", new TAVTntuHit());
      
      if (fgStdAloneFlag) {
         fActVmeReaderVtx  = new TAVTactVmeReader("vtActNtu", fpNtuHitVtx, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
            fActVmeReaderVtx->CreateHistogram();
         
      } else {
         fActNtuHitVtx = new TAVTactNtuHit("vtActNtu", fpNtuHitVtx, fpDaqEvent, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActNtuHitVtx->SetDebugLevel(1);
      }
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      fpNtuHitIt   = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuHitIt = new TAITactNtuHit("itActNtu", fpNtuHitIt, fpDaqEvent, fpParGeoIt, fpParConfIt, fpParMapIt);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }
   
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      fpDatRawMsd   = new TAGdataDsc("msdDat", new TAVTdatRaw());
//      fpNtuHitMsd   = new TAGdataDsc("msdRaw", new TAMSDntuHit());
//      fActDatRawMsd = new TAMSDactDaqRaw("msdAcDat", fpDatRawMsd, fpDaqEvent, fpParGeoMsd);
//      fActNtuHitMsd = new TAVTactNtuHit("msdActNtu", fpNtuHitMsd, fpDatRawMsd, fpParGeoMsd);
//      if (fFlagHisto)
//         fActNtuHitMsd->CreateHistogram();
//   }

   if(GlobalPar::GetPar()->IncludeTW()) {
      fpNtuHitTw   = new TAGdataDsc("twRaw", new TATWntuHit());

      if(GlobalPar::GetPar()->CalibTW()) {
	fActCalibTw = new TATWactCalibTW("twActCalib", fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
	if(GlobalPar::GetPar()->Debug()) fActCalibTw->SetDebugLevel(1);
	fActCalibTw->CreateHistogram();
	
      } else {
	
	fActNtuHitTw = new TATWactNtuHit("twActNtu", fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
	if(GlobalPar::GetPar()->Debug()) fActNtuHitTw->SetDebugLevel(1);
	if (fFlagHisto)
	  fActNtuHitTw->CreateHistogram();
	
      }
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
     fpNtuHitCa   = new TAGdataDsc("caRaw", new TACAntuHit());
     fActNtuHitCa = new TACAactNtuHit("caActNtu", fpDatRawCa, fpNtuHitCa, fpParMapCa, NULL);
     //the calibration parameters have to be still defined!!! (gtraini)
     if (fFlagHisto){
	fActNtuHitCa->CreateHistogram();
      }
    }
   
}

//__________________________________________________________
void LocalReco::OpenFileIn()
{
   if (fgStdAloneFlag) {
      if (GlobalPar::GetPar()->IncludeVT())
         fActVmeReaderVtx->Open(GetName());
      
      if (GlobalPar::GetPar()->IncludeBM())
         fActVmeReaderBm->Open(GetName());
      
   } else
      fActEvtReader->Open(GetName());
}

//__________________________________________________________
void LocalReco::SetRawHistogramDir()
{
   // ST
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TASTparGeo::GetBaseName());
      fActWdRaw->SetHistogramDir(subfolder);
   }
   
   if (GlobalPar::GetPar()->IncludeST()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TASTparGeo::GetBaseName());
      fActNtuHitSt->SetHistogramDir(subfolder);
   }
   
   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TABMparGeo::GetBaseName());
      fActNtuHitBm->SetHistogramDir(subfolder);
      TDirectory* subsubfolder = subfolder->mkdir("bmdat");
      fActDatRawBm->SetHistogramDir(subsubfolder);
   }
   
   // VTX
   if (GlobalPar::GetPar()->IncludeVT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAVTparGeo::GetBaseName());
      fActNtuHitVtx->SetHistogramDir(subfolder);
   }
   
   // IT
   if (GlobalPar::GetPar()->IncludeIT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAITparGeo::GetBaseName());
      fActNtuHitIt->SetHistogramDir(subfolder);
   }

   // TW
   if (GlobalPar::GetPar()->IncludeTW()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TATWparGeo::GetBaseName());
      if(GlobalPar::GetPar()->CalibTW()) {
	fActCalibTw->SetHistogramDir(subfolder);
      } else {
	fActNtuHitTw->SetHistogramDir(subfolder);
      }
   }
   
   // MSD
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAMSDparGeo::GetBaseName());
//      fActDatRawMsd->SetHistogramDir(subfolder);
//      fActNtuHitMsd->SetHistogramDir(subfolder);
//   }


   // CA
   if (GlobalPar::GetPar()->IncludeCA()) {
     TDirectory* subfolder = fActEvtWriter->File()->mkdir(TACAparGeo::GetBaseName());
     fActWdRaw->SetHistogramDir(subfolder);
   }
   
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

   if (GlobalPar::GetPar()->IncludeVT()) {
      fTAGroot->AddRequiredItem("vtActNtu");
   }
   
//   if (GlobalPar::GetPar()->IncludeIT()) {
//      fTAGroot->AddRequiredItem("itActNtu");
//   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
     if(GlobalPar::GetPar()->CalibTW()) {
       fTAGroot->AddRequiredItem("twActCalib");
     } else {
       fTAGroot->AddRequiredItem("twActNtu");
     }
   }

   
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      fTAGroot->AddRequiredItem("msdActDat");
//      fTAGroot->AddRequiredItem("msdActNtu");
//   }

   if (GlobalPar::GetPar()->IncludeCA()) {
     fTAGroot->AddRequiredItem("caActNtu");
   }

   
}

//__________________________________________________________
void LocalReco::SetTreeBranches()
{
   BaseReco::SetTreeBranches();
   
   if (GlobalPar::GetPar()->IncludeST()) {
     if (fFlagHits) {
       fActEvtWriter->SetupElementBranch(fpDatRawSt, TASTntuRaw::GetBranchName());
     }
     fActEvtWriter->SetupElementBranch(fpNtuHitSt, TASTntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpDatRawBm, TABMntuRaw::GetBranchName());
     fActEvtWriter->SetupElementBranch(fpNtuHitBm, TABMntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuHitVtx, TAVTntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuHitIt, TAITntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuHitMsd, TAMSDntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
     if (fFlagHits) {
         fActEvtWriter->SetupElementBranch(fpDatRawTw, TATWntuRaw::GetBranchName());
     }
     fActEvtWriter->SetupElementBranch(fpNtuHitTw, TATWntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuHitCa, TACAntuHit::GetBranchName());
   }
}
