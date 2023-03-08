
/*!
 \file RecoRaw.cxx
 \brief Implementation of RecoRaw
 */
/*------------------------------------------+---------------------------------*/


#include "RecoRaw.hxx"

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
ClassImp(RecoRaw)

/*!
 \class RecoRaw
 \brief Reconstruction class from raw data
 */
/*------------------------------------------+---------------------------------*/

Bool_t  RecoRaw::fgStdAloneFlag    = false;
Int_t   RecoRaw::fgNumFileStdAlone = 1;

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
//! \param[in] IsSubFile flag to enable or disable the subfile of only the input file
RecoRaw::RecoRaw(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t IsSubFile)
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
   fActEvtReader(0x0),
   fSubFileFlag(IsSubFile)
  {
     if (fRunNumber == -1)  // if not set from outside, take from name
        SetRunNumberFromFile();
}

//__________________________________________________________
//! default destructor
RecoRaw::~RecoRaw()
{
}

//__________________________________________________________
//! Create raw data actions
void RecoRaw::CreateRawAction()
{
   if (!fgStdAloneFlag) {
      fpDaqEvent = new TAGdataDsc("daqEvt", new TAGdaqEvent());
      fActEvtReader = new TAGactDaqReader("daqActReader", fpDaqEvent);
          
      fpNtuEvt = new TAGdataDsc("evtNtu", new TAGntuEvent());
      fActNtuEvt = new TAGactNtuEvent("evtActNtu", fpNtuEvt, fpDaqEvent);
     if (fFlagHisto)
       fActNtuEvt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeCA())
      fpDatRawCa      = new TAGdataDsc("caDat", new TACAntuRaw());

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || (TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag) || TAGrecoManager::GetPar()->IncludeCA()) {

      fpDatRawSt      = new TAGdataDsc("stDat", new TASTntuRaw());
      fpDatRawTw      = new TAGdataDsc("twdDat", new TATWntuRaw());
      fpNtuWDtrigInfo = new TAGdataDsc("WDtrigInfo",new TAGWDtrigInfo());
      
      if (!fgStdAloneFlag){
         TAGbaseWDparTime* parTimeWD = (TAGbaseWDparTime*) fpParTimeWD->Object();
         TString parFileName = fCampManager->GetCurCalFile(TASTparGeo::GetBaseName(), fRunNumber, true);
         parTimeWD->FromFileTcal(parFileName.Data());
      }
      
      TACAparConf* parConf = (TACAparConf*)fpParConfCa->Object();
      if (parConf->GetAnalysisPar().EnableArduinoTemp)
         TAGactWDreader::EnableArduinoTempCA();

      fActWdRaw  = new TAGactWDreader("wdActRaw", fpDaqEvent, fpDatRawSt, fpDatRawTw, fpDatRawCa, fpNtuWDtrigInfo, fpParMapWD,
                                      fpParTimeWD, fpParMapCa, fgStdAloneFlag);
      if (fgStdAloneFlag)
         fActWdRaw->SetMaxFiles(fgNumFileStdAlone);
      
      if(fFlagHisto)
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

   if (TAGrecoManager::GetPar()->IncludeMSD()  && !fgStdAloneFlag) {
      
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
      fActNtuHitCa = new TACAactNtuHit("caActNtu", fpDatRawCa, fpNtuHitCa, fpParGeoCa, fpParMapCa, fpParCalCa);
      if (fFlagHisto){
         fActNtuHitCa->CreateHistogram();
      }
   }
}

//__________________________________________________________
//! Skip a given number of events
//!
//! \param[in] iEvent event number to skip
Bool_t RecoRaw::GoEvent(Int_t iEvent)
{
   fSkipEventsN = iEvent;
   
   return true;
}

//__________________________________________________________
//! Open input file
void RecoRaw::OpenFileIn()
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

     if(IsSubFileEnabled()) {

       Option_t* option = "subFileNumber";
       fActEvtReader->Open(GetName(),option);

     }

     else
       fActEvtReader->Open(GetName());
     
     if (fSkipEventsN > 0){
      
       fActEvtReader->SkipEvents(fSkipEventsN);
     }
   }
}

//__________________________________________________________
//! Set raw data histogram directory
void RecoRaw::SetRawHistogramDir()
{
  // WD
  if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || TAGrecoManager::GetPar()->IncludeCA()) {
    TDirectory* subfolder = fActEvtWriter->File()->mkdir("WD");
    fActWdRaw->SetHistogramDir(subfolder);
  }

  //ST
   if (TAGrecoManager::GetPar()->IncludeST()) {
     TDirectory* subfolder = fActEvtWriter->File()->mkdir(TASTparGeo::GetBaseName());
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
      fActNtuHitCa ->SetHistogramDir(subfolder);
   }

   //DAQ
   TDirectory* subfolder = fActEvtWriter->File()->mkdir("Event");
   fActNtuEvt ->SetHistogramDir(subfolder);



   
}

//__________________________________________________________
//! Close input file
void RecoRaw::CloseFileIn()
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
void RecoRaw::AddRawRequiredItem()
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
void RecoRaw::SetTreeBranches()
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

// --------------------------------------------------------------------------------------
void RecoRaw::SetRunNumberFromFile()
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
   fRunNumber = tmp.Atoi();
   
   Warning("SetRunNumber()", "Run number not set!, taking number from file: %d", fRunNumber);
   
   gTAGroot->SetRunNumber(fRunNumber);
}