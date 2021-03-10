


#include "LocalReco.hxx"

#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWdatRaw.hxx"
#include "TATWntuHit.hxx"
#include "TACAntuHit.hxx"

#include "TASTdatRaw.hxx"
#include "TABMdatRaw.hxx"
#include "TAMSDdatRaw.hxx"


ClassImp(LocalReco)

Bool_t  LocalReco::fgStdAloneFlag = false;

//__________________________________________________________
LocalReco::LocalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout),
   fpDaqEvent(0x0),
   fActWdRaw(0x0),
   fActDatRawBm(0x0),
   fActNtuRawBm(0x0),
   fActNtuRawVtx(0x0),
   fActNtuRawIt(0x0),
   fActNtuRawCa(0x0),
//   fActNtuRawMsd(0x0),
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
      fpDatRawSt   = new TAGdataDsc("stDat", new TASTdatRaw());
      fpDatRawTw   = new TAGdataDsc("twdDat", new TATWdatRaw());
      fpDatRawCa   = new TAGdataDsc("caDat", new TACAdatRaw());
      
      fActWdRaw  = new TAGactWDreader("wdActRaw", fpDaqEvent, fpDatRawSt, fpDatRawTw, fpDatRawCa, fpParMapWD, fpParTimeWD);
      if (fFlagHisto)
         fActWdRaw->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeST() ||GlobalPar::GetPar()->IncludeBM()) {
      fpNtuRawSt   = new TAGdataDsc("stNtu", new TASTntuHit());
      fActNtuRawSt = new TASTactNtuHit("stActNtu", fpDatRawSt, fpNtuRawSt, fpParMapSt);
      if (fFlagHisto)
         fActNtuRawSt->CreateHistogram();
   }

   if (GlobalPar::GetPar()->IncludeBM()) {
      fpDatRawBm = new TAGdataDsc("bmDat", new TABMdatRaw());
      fpNtuRawBm = new TAGdataDsc("bmNtu", new TABMntuHit());
      
      if (fgStdAloneFlag) {
         fActVmeReaderBm  = new TABMactVmeReader("bmActNtu", fpDatRawBm, fpParMapBm, fpParCalBm, fpParGeoBm);
         if (fFlagHisto)
            fActVmeReaderBm->CreateHistogram();
         
      } else {
         fActDatRawBm = new TABMactDatRaw("bmActDat", fpDatRawBm, fpDaqEvent, fpParMapBm, fpParCalBm, fpParGeoBm, fpNtuRawSt);
         if (fFlagHisto)
            fActDatRawBm->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActDatRawBm->SetDebugLevel(1);
         
         fActNtuRawBm = new TABMactNtuHit("bmActNtu", fpNtuRawBm, fpDatRawBm, fpParGeoBm, fpParConfBm, fpParCalBm);
         if (fFlagHisto)
            fActNtuRawBm->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActNtuRawBm->SetDebugLevel(1);
      }
   }

   if (GlobalPar::GetPar()->IncludeVT()) {
      fpNtuRawVtx   = new TAGdataDsc("vtRaw", new TAVTntuHit());
      
      if (fgStdAloneFlag) {
         fActVmeReaderVtx  = new TAVTactVmeReader("vtActNtu", fpNtuRawVtx, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
            fActVmeReaderVtx->CreateHistogram();
         
      } else {
         fActNtuRawVtx = new TAVTactNtuHit("vtActNtu", fpNtuRawVtx, fpDaqEvent, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
         fActNtuRawVtx->CreateHistogram();
         if(GlobalPar::GetPar()->Debug()) fActNtuRawVtx->SetDebugLevel(1);
      }
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      fpNtuRawIt   = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuRawIt = new TAITactNtuHit("itActNtu", fpNtuRawIt, fpDaqEvent, fpParGeoIt, fpParConfIt, fpParMapIt);
      if (fFlagHisto)
         fActNtuRawIt->CreateHistogram();
   }
   
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      fpDatRawMsd   = new TAGdataDsc("msdDat", new TAVTdatRaw());
//      fpNtuRawMsd   = new TAGdataDsc("msdRaw", new TAMSDntuHit());
//      fActDatRawMsd = new TAMSDactDaqRaw("msdAcDat", fpDatRawMsd, fpDaqEvent, fpParGeoMsd);
//      fActNtuRawMsd = new TAVTactNtuHit("msdActNtu", fpNtuRawMsd, fpDatRawMsd, fpParGeoMsd);
//      if (fFlagHisto)
//         fActNtuRawMsd->CreateHistogram();
//   }

   if(GlobalPar::GetPar()->IncludeTW()) {
      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATWntuHit());

      if(GlobalPar::GetPar()->CalibTW()) {
	fActCalibTw = new TATWactCalibTW("twActCalib", fpDatRawTw, fpNtuRawTw, fpNtuRawSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
	if(GlobalPar::GetPar()->Debug()) fActCalibTw->SetDebugLevel(1);
	fActCalibTw->CreateHistogram();
	
      } else {
	
	fActNtuRawTw = new TATWactNtuHit("twActNtu", fpDatRawTw, fpNtuRawTw, fpNtuRawSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
	if(GlobalPar::GetPar()->Debug()) fActNtuRawTw->SetDebugLevel(1);
	if (fFlagHisto)
	  fActNtuRawTw->CreateHistogram();
	
      }
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
     fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuHit());
     fActNtuRawCa = new TACAactNtuHit("caActNtu", fpDatRawCa, fpNtuRawCa, fpParMapCa, NULL);
     //the calibration parameters have to be still defined!!! (gtraini)
     if (fFlagHisto){
	fActNtuRawCa->CreateHistogram();
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
      fActNtuRawSt->SetHistogramDir(subfolder);
   }
   
   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TABMparGeo::GetBaseName());
      fActNtuRawBm->SetHistogramDir(subfolder);
      TDirectory* subsubfolder = subfolder->mkdir("bmdat");
      fActDatRawBm->SetHistogramDir(subsubfolder);
   }
   
   // VTX
   if (GlobalPar::GetPar()->IncludeVT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAVTparGeo::GetBaseName());
      fActNtuRawVtx->SetHistogramDir(subfolder);
   }
   
   // IT
   if (GlobalPar::GetPar()->IncludeIT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAITparGeo::GetBaseName());
      fActNtuRawIt->SetHistogramDir(subfolder);
   }

   // TW
   if (GlobalPar::GetPar()->IncludeTW()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TATWparGeo::GetBaseName());
      if(GlobalPar::GetPar()->CalibTW()) {
	fActCalibTw->SetHistogramDir(subfolder);
      } else {
	fActNtuRawTw->SetHistogramDir(subfolder);
      }
   }
   
   // MSD
//   if (GlobalPar::GetPar()->IncludeMSD()) {
//      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAMSDparGeo::GetBaseName());
//      fActDatRawMsd->SetHistogramDir(subfolder);
//      fActNtuRawMsd->SetHistogramDir(subfolder);
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
       fActEvtWriter->SetupElementBranch(fpDatRawSt, TASTdatRaw::GetBranchName());
     }
     fActEvtWriter->SetupElementBranch(fpNtuRawSt, TASTntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpDatRawBm, TABMdatRaw::GetBranchName());
     fActEvtWriter->SetupElementBranch(fpNtuRawBm, TABMntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuRawVtx, TAVTntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuRawIt, TAITntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuRawMsd, TAMSDntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
     if (fFlagHits) {
         fActEvtWriter->SetupElementBranch(fpDatRawTw, TATWdatRaw::GetBranchName());
     }
     fActEvtWriter->SetupElementBranch(fpNtuRawTw, TATWntuHit::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuRawCa, TACAntuHit::GetBranchName());
   }
}
