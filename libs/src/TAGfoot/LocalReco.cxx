
/*!
 \file LocalReco.cxx
 \brief Implementation of LocalReco
 */
/*------------------------------------------+---------------------------------*/


#include "LocalReco.hxx"

#include "TAGntuEvent.hxx"
#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuHit.hxx"
#include "TACAntuHit.hxx"
#include "TAGWDtrigInfo.hxx"
#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAMSDntuRaw.hxx"

//! Class Imp
ClassImp(LocalReco)

/*!
 \class LocalReco
 \brief Reconstruction class from raw data
 */
/*------------------------------------------+---------------------------------*/

Bool_t  LocalReco::fgStdAloneFlag = false;
Int_t   LocalReco::fNumFileStdAlone=1;

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
LocalReco::LocalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout),
   fpDaqEvent(0x0),
   fpNtuEvt(0x0),
   fActWdRaw(0x0),
   fpNtuWDtrigInfo(0x0),
   fActDatRawBm(0x0),
   fActNtuHitBm(0x0),
   fActNtuHitVtx(0x0),
   fActNtuHitIt(0x0),
   fActNtuHitCa(0x0),
   fActNtuHitMsd(0x0),
   fpDatRawMsd(0x0),
   fActEvtReader(0x0)
{
}

//__________________________________________________________
//! default destructor
LocalReco::~LocalReco()
{
}

//__________________________________________________________
//! Create raw data actions
void LocalReco::CreateRawAction()
{
   if (!fgStdAloneFlag) {
      fpDaqEvent = new TAGdataDsc("daqEvt", new TAGdaqEvent());
      fActEvtReader = new TAGactDaqReader("daqActReader", fpDaqEvent);

      fpNtuEvt = new TAGdataDsc("evtNtu", new TAGntuEvent());
      fActNtuEvt = new TAGactNtuEvent("evtActNtu", fpNtuEvt, fpDaqEvent);
     if (fFlagHisto)
       fActNtuEvt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || (TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag) || TAGrecoManager::GetPar()->IncludeCA()) {

      fpDatRawSt      = new TAGdataDsc("stDat", new TASTntuRaw());
      fpDatRawTw      = new TAGdataDsc("twdDat", new TATWntuRaw());
      fpDatRawCa      = new TAGdataDsc("caDat", new TACAntuRaw());
      fpNtuWDtrigInfo = new TAGdataDsc("WDtrigInfo",new TAGWDtrigInfo());
      
      if (!fgStdAloneFlag){
         TAGbaseWDparTime* parTimeWD = (TAGbaseWDparTime*) fpParTimeWD->Object();
         TString parFileName = fCampManager->GetCurCalFile(TASTparGeo::GetBaseName(), fRunNumber, true);
         parTimeWD->FromFileTcal(parFileName.Data());
      }
      fActWdRaw  = new TAGactWDreader("wdActRaw", fpDaqEvent, fpDatRawSt, fpDatRawTw, fpDatRawCa, fpNtuWDtrigInfo, fpParMapWD,
                                      fpParTimeWD, fgStdAloneFlag);
      if (fgStdAloneFlag)
         fActWdRaw->SetMaxFiles(fNumFileStdAlone);
      
      if (fFlagHisto)
         fActWdRaw->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeST() ||(TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag)) {
      fpNtuHitSt   = new TAGdataDsc("stNtu", new TASTntuHit());
      fActNtuHitSt = new TASTactNtuHit("stActNtu", fpDatRawSt, fpNtuHitSt, fpParMapSt);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpDatRawBm = new TAGdataDsc("bmDat", new TABMntuRaw());
      fpNtuHitBm = new TAGdataDsc("bmNtu", new TABMntuHit());

      if (fgStdAloneFlag) {
         fActVmeReaderBm  = new TABMactVmeReader("bmActRaw", fpDatRawBm, fpParMapBm, fpParCalBm, fpParGeoBm);
         if (fFlagHisto)
            fActVmeReaderBm->CreateHistogram();
      } else {
         fActDatRawBm = new TABMactNtuRaw("bmActRaw", fpDatRawBm, fpDaqEvent, fpParMapBm, fpParCalBm, fpParGeoBm, fpNtuHitSt);
         if (fFlagHisto)
            fActDatRawBm->CreateHistogram();
      }
      fActNtuHitBm = new TABMactNtuHit("bmActNtu", fpNtuHitBm, fpDatRawBm, fpParGeoBm, fpParConfBm, fpParCalBm);
      if (fFlagHisto)
        fActNtuHitBm->CreateHistogram();

   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpNtuHitVtx   = new TAGdataDsc("vtRaw", new TAVTntuHit());

      if (fgStdAloneFlag) {
         fActVmeReaderVtx  = new TAVTactVmeReader("vtActNtu", fpNtuHitVtx, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
            fActVmeReaderVtx->CreateHistogram();

      } else {
         fActNtuHitVtx = new TAVTactNtuHit("vtActNtu", fpNtuHitVtx, fpDaqEvent, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
      }
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpNtuHitIt   = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuHitIt = new TAITactNtuHit("itActNtu", fpNtuHitIt, fpDaqEvent, fpParGeoIt, fpParConfIt, fpParMapIt);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpDatRawMsd   = new TAGdataDsc("msdDat", new TAMSDntuRaw());
      fActDatRawMsd = new TAMSDactNtuRaw("msdActRaw", fpDatRawMsd, fpDaqEvent, fpParMapMsd, fpParCalMsd, fpParGeoMsd, fpParConfMsd);
      if (fFlagHisto)
         fActDatRawMsd->CreateHistogram();

      fpNtuHitMsd   = new TAGdataDsc("msdRaw", new TAMSDntuHit());
      fActNtuHitMsd = new TAMSDactNtuHit("msdActNtu", fpDatRawMsd, fpNtuHitMsd, fpParGeoMsd, fpParConfMsd, fpParCalMsd);
      if (fFlagHisto)
         fActNtuHitMsd->CreateHistogram();
   }

   if(TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuHitTw   = new TAGdataDsc("twRaw", new TATWntuHit());

      if(TAGrecoManager::GetPar()->CalibTW()) {
        fActCalibTw = new TATWactCalibTW("twActCalib", fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
        fActCalibTw->CreateHistogram();

      } else {
	fActNtuHitTw = new TATWactNtuHit("twActNtu", fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
	if (fFlagHisto)
	  fActNtuHitTw->CreateHistogram();
      }
   }

   if(TAGrecoManager::GetPar()->IncludeCA()) {
     fpNtuHitCa   = new TAGdataDsc("caRaw", new TACAntuHit());
     fActNtuHitCa = new TACAactNtuHit("caActNtu", fpDatRawCa, fpNtuHitCa, fpParMapCa, fpParCalCa);
     if (fFlagHisto){
       fActNtuHitCa->CreateHistogram();
      }
    }
}

//__________________________________________________________
//! Skip a given number of events
//!
//! \param[in] iEvent event number to skip
Bool_t LocalReco::GoEvent(Int_t iEvent)
{
   fSkipEventsN = iEvent;
   
   return true;
}

//__________________________________________________________
//! Open input file
void LocalReco::OpenFileIn()
{
   if (fgStdAloneFlag) {
      if (TAGrecoManager::GetPar()->IncludeVT())
	fActVmeReaderVtx->Open(GetName());
      
      if (TAGrecoManager::GetPar()->IncludeBM())
	fActVmeReaderBm->Open(GetName());
      
      if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || TAGrecoManager::GetPar()->IncludeCA()){
	fActWdRaw->Open(GetName());
	fActWdRaw->SetInitName(GetName());
      }

   } else {
      fActEvtReader->Open(GetName());
      if (fSkipEventsN > 0)
         fActEvtReader->SkipEvents(fSkipEventsN);
   }
}

//__________________________________________________________
//! Set raw data histogram directory
void LocalReco::SetRawHistogramDir()
{
   // ST
   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TASTparGeo::GetBaseName());
      fActWdRaw->SetHistogramDir(subfolder);
   }

   if (TAGrecoManager::GetPar()->IncludeST()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TASTparGeo::GetBaseName());
      fActNtuHitSt->SetHistogramDir(subfolder);
   }

   // BM
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TABMparGeo::GetBaseName());
      fActNtuHitBm->SetHistogramDir(subfolder);
      TDirectory* subsubfolder = subfolder->mkdir("bmraw");
      if (fgStdAloneFlag)
        fActVmeReaderBm->SetHistogramDir(subsubfolder);
      else{
        fActDatRawBm->SetHistogramDir(subsubfolder);
      }
   }

   // VTX
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAVTparGeo::GetBaseName());
      fActNtuHitVtx->SetHistogramDir(subfolder);
   }

   // IT
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAITparGeo::GetBaseName());
      fActNtuHitIt->SetHistogramDir(subfolder);
   }

   // TW
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TATWparGeo::GetBaseName());
      if(TAGrecoManager::GetPar()->CalibTW()) {
         fActCalibTw->SetHistogramDir(subfolder);
      } else {
	fActNtuHitTw->SetHistogramDir(subfolder);
      }
   }

   // MSD
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAMSDparGeo::GetBaseName());
      fActDatRawMsd->SetHistogramDir(subfolder);
      fActNtuHitMsd->SetHistogramDir(subfolder);
   }

   // CA
    if (TAGrecoManager::GetPar()->IncludeCA()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TACAparGeo::GetBaseName());
      //fActWdRaw->SetHistogramDir(subfolder);
      fActNtuHitCa ->SetHistogramDir(subfolder);
    }
}

//__________________________________________________________
//! Close input file
void LocalReco::CloseFileIn()
{
  if (fgStdAloneFlag && TAGrecoManager::GetPar()->IncludeBM())
    fActVmeReaderBm->Close();
  else if (fgStdAloneFlag && (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || TAGrecoManager::GetPar()->IncludeCA()))
    fActWdRaw->Close();
  else if(!fgStdAloneFlag)
    fActEvtReader->Close();
}

//__________________________________________________________
//! Add required raw data actions in list
void LocalReco::AddRawRequiredItem()
{
   if (!fgStdAloneFlag) {
      fTAGroot->AddRequiredItem("daqActReader");
      fTAGroot->AddRequiredItem("evtActNtu");
   }

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW()) {
     fTAGroot->AddRequiredItem("wdActRaw");
   }

   if (TAGrecoManager::GetPar()->IncludeST() || (TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag)) {
      fTAGroot->AddRequiredItem("stActNtu");
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fTAGroot->AddRequiredItem("bmActRaw");
      fTAGroot->AddRequiredItem("bmActNtu");
   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fTAGroot->AddRequiredItem("vtActNtu");
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fTAGroot->AddRequiredItem("itActNtu");
   }

   if (TAGrecoManager::GetPar()->IncludeTW()) {
     if(TAGrecoManager::GetPar()->CalibTW()) {
       fTAGroot->AddRequiredItem("twActCalib");
     } else {
       fTAGroot->AddRequiredItem("twActNtu");
     }
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fTAGroot->AddRequiredItem("msdActRaw");
      fTAGroot->AddRequiredItem("msdActNtu");
   }

   if (TAGrecoManager::GetPar()->IncludeCA()) {
     fTAGroot->AddRequiredItem("caActNtu");
   }


}

//__________________________________________________________
//! Set tree branches for writing in output file
void LocalReco::SetTreeBranches()
{
   BaseReco::SetTreeBranches();

   if (!fgStdAloneFlag) {
     fActEvtWriter->SetupElementBranch(fpNtuEvt, TAGntuEvent::GetBranchName());
   }

   if (TAGrecoManager::GetPar()->IncludeST()) {
     if (fFlagHits) {
       fActEvtWriter->SetupElementBranch(fpDatRawSt, TASTntuRaw::GetBranchName());
       fActEvtWriter->SetupElementBranch(fpNtuHitSt, TASTntuHit::GetBranchName());
     }
     fActEvtWriter->SetupElementBranch(fpNtuWDtrigInfo, TAGWDtrigInfo::GetBranchName());
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      if (fFlagHits) {
         fActEvtWriter->SetupElementBranch(fpDatRawBm, TABMntuRaw::GetBranchName());
         fActEvtWriter->SetupElementBranch(fpNtuHitBm, TABMntuHit::GetBranchName());
      }
   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuHitVtx, TAVTntuHit::GetBranchName());
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {
     if (fFlagHits)
       fActEvtWriter->SetupElementBranch(fpNtuHitIt, TAITntuHit::GetBranchName());
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
     if (fFlagHits) {
         fActEvtWriter->SetupElementBranch(fpDatRawMsd, TAMSDntuRaw::GetBranchName());
         fActEvtWriter->SetupElementBranch(fpNtuHitMsd, TAMSDntuHit::GetBranchName());
     }
   }

   if (TAGrecoManager::GetPar()->IncludeTW()) {
     if (fFlagHits) {
         fActEvtWriter->SetupElementBranch(fpDatRawTw, TATWntuRaw::GetBranchName());
         fActEvtWriter->SetupElementBranch(fpNtuHitTw, TATWntuHit::GetBranchName());
     }
   }

   if (TAGrecoManager::GetPar()->IncludeCA()) {
      if (fFlagHits) {
        fActEvtWriter->SetupElementBranch(fpDatRawCa, TACAntuRaw::GetBranchName());
        fActEvtWriter->SetupElementBranch(fpNtuHitCa, TACAntuHit::GetBranchName());
      }
   }
}
