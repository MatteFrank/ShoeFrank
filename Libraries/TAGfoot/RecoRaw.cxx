
/*!
 \file RecoRaw.cxx
 \brief Implementation of RecoRaw
 */
/*------------------------------------------+---------------------------------*/


#include "RecoRaw.hxx"

#include "TAGnameManager.hxx"

#include "TAGntuEvent.hxx"
#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuHit.hxx"
#include "TACAntuHit.hxx"
#include "TAWDntuTrigger.hxx"
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
RecoRaw::RecoRaw(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t IsSubFile, Bool_t isMC)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout, isMC),
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
      fpDaqEvent = new TAGdataDsc(new TAGdaqEvent());
      const Char_t* name = FootActionDscName("TAGactDaqReader");
      fActEvtReader = new TAGactDaqReader(name, fpDaqEvent);
          
      fpNtuEvt = new TAGdataDsc(new TAGntuEvent());
      name = FootActionDscName("TAGactNtuEvent");
      fActNtuEvt = new TAGactNtuEvent(name, fpNtuEvt, fpDaqEvent);
     if (fFlagHisto)
       fActNtuEvt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeCA())
      fpDatRawCa      = new TAGdataDsc(new TACAntuRaw());

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || (TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag) || TAGrecoManager::GetPar()->IncludeCA()) {

      fpDatRawSt      = new TAGdataDsc(new TASTntuRaw());
      fpDatRawTw      = new TAGdataDsc(new TATWntuRaw());
      fpNtuWDtrigInfo = new TAGdataDsc(new TAWDntuTrigger());
      
      if (!fgStdAloneFlag){
	  TAWDparTime* parTimeWD = (TAWDparTime*) fpParTimeWD->Object();
	  TString parFileName = fCampManager->GetCurCalFile(FootBaseName("TASTparGeo"), fRunNumber, true);
	  parTimeWD->FromFileTcal(parFileName.Data());
      }
      
      if(TAGrecoManager::GetPar()->IncludeCA()){
        TACAparConf* parConf = (TACAparConf*)fpParConfCa->Object();
        if (parConf->GetAnalysisPar().EnableArduinoTemp)
          TAGactWDreader::EnableArduinoTempCA();
      }

      const Char_t* name = FootActionDscName("TAGactWDreader");
      fActWdRaw  = new TAGactWDreader(name, fpDaqEvent, fpDatRawSt, fpDatRawTw, fpDatRawCa, fpNtuWDtrigInfo, fpParMapWD,
                                      fpParTimeWD, fpParMapCa, fgStdAloneFlag);
      if (fgStdAloneFlag)
         fActWdRaw->SetMaxFiles(fgNumFileStdAlone);
      
      if(fFlagHisto)
         fActWdRaw->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeST() ||(TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag)) {
      fpNtuHitSt   = new TAGdataDsc(new TASTntuHit());
      const Char_t* name = FootActionDscName("TASTactNtuHit");
      fActNtuHitSt = new TASTactNtuHit(name, fpDatRawSt, fpNtuHitSt, fpParMapSt);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpDatRawBm = new TAGdataDsc(new TABMntuRaw());
      fpNtuHitBm = new TAGdataDsc(new TABMntuHit());

      const Char_t* name = FootActionDscName("TABMactNtuRaw");
      if (fgStdAloneFlag) {
         fActVmeReaderBm  = new TABMactVmeReader(name, fpDatRawBm, fpParMapBm, fpParCalBm, fpParGeoBm);
         if (fFlagHisto)
            fActVmeReaderBm->CreateHistogram();
      } else {
         fActDatRawBm = new TABMactNtuRaw(name, fpDatRawBm, fpDaqEvent, fpParMapBm, fpParCalBm, fpParGeoBm, fpNtuHitSt);
         if (fFlagHisto)
            fActDatRawBm->CreateHistogram();
      }
      name = FootActionDscName("TABMactNtuHit");
      fActNtuHitBm = new TABMactNtuHit(name, fpNtuHitBm, fpDatRawBm, fpParGeoBm, fpParConfBm, fpParCalBm);
      if (fFlagHisto)
        fActNtuHitBm->CreateHistogram();

   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      Int_t sensorsN = parGeo->GetSensorsN();
      fpNtuHitVtx   = new TAGdataDsc(new TAVTntuHit(sensorsN));
      const Char_t* name = FootActionDscName("TAVTactNtuHit");
      if (fgStdAloneFlag) {
         fActVmeReaderVtx  = new TAVTactVmeReader(name, fpNtuHitVtx, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
            fActVmeReaderVtx->CreateHistogram();

      } else {
         fActNtuHitVtx = new TAVTactNtuHit(name, fpNtuHitVtx, fpDaqEvent, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
      }
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {      
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      Int_t sensorsN = parGeo->GetSensorsN();
      
      fpNtuHitIt   = new TAGdataDsc(new TAITntuHit(sensorsN));
      const Char_t* name = FootActionDscName("TAITactNtuHit");
      fActNtuHitIt = new TAITactNtuHit(name, fpNtuHitIt, fpDaqEvent, fpParGeoIt, fpParConfIt, fpParMapIt);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()  && !fgStdAloneFlag) {
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      Int_t sensorsN = parGeo->GetSensorsN();

      fpDatRawMsd   = new TAGdataDsc(new TAMSDntuRaw(sensorsN));
      const Char_t* name = FootActionDscName("TAMSDactNtuRaw");
      fActDatRawMsd = new TAMSDactNtuRaw(name, fpDatRawMsd, fpDaqEvent, fpParMapMsd, fpParCalMsd, fpParGeoMsd, fpParConfMsd);
      if (fFlagHisto)
         fActDatRawMsd->CreateHistogram();
      
      fpNtuHitMsd   = new TAGdataDsc(new TAMSDntuHit(sensorsN));
      name = FootActionDscName("TAMSDactNtuHit");
      fActNtuHitMsd = new TAMSDactNtuHit(name, fpDatRawMsd, fpNtuHitMsd, fpParGeoMsd, fpParConfMsd, fpParCalMsd);
      if (fFlagHisto)
         fActNtuHitMsd->CreateHistogram();
   }

   if(TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuHitTw   = new TAGdataDsc(new TATWntuHit());

      if(TAGrecoManager::GetPar()->CalibTW()) {
        const Char_t* name = FootActionDscName("TATWactCalibTW");
        fActCalibTw = new TATWactCalibTW(name, fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
        fActCalibTw->CreateHistogram();

      } else {
         const Char_t* name = FootActionDscName("TATWactNtuHit");
         fActNtuHitTw = new TATWactNtuHit(name, fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
         if (fFlagHisto)
            fActNtuHitTw->CreateHistogram();
      }
   }

   if(TAGrecoManager::GetPar()->IncludeCA()) {
      fpNtuHitCa   = new TAGdataDsc(new TACAntuHit());
      const Char_t* name = FootActionDscName("TACAactNtuHit");
      fActNtuHitCa = new TACAactNtuHit(name, fpDatRawCa, fpNtuHitCa, fpParGeoCa, fpParMapCa, fpParCalCa);
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

   if (fFlagOut && !fFlagFlatTree)
      ((TAGactDscTreeWriter*)fActEvtWriter)->SetStdAlone(fgStdAloneFlag);
   
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

     Option_t* option = "";
     if(IsSubFileEnabled())
        option = "subFileNumber";
     else
        option = "chain";
      
     fActEvtReader->Open(GetName(),option);

     if (fSkipEventsN > 0)
       fActEvtReader->SkipEvents(fSkipEventsN);

   }
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

//__________________________________________________________
//! Generate output file name
TString RecoRaw::GetFileOutName()
{
   TString name = Form("run_%08d", fRunNumber);
   vector<TString> dec = TAGrecoManager::GetPar()->DectIncluded();
   
   Int_t detectorsN = 0;
   
   for (auto it : dec) {
      TString det = TAGrecoManager::GetDect3LetName(it);
      det.ToLower();
      if (det == "tgt") continue;
      detectorsN++;
   }
   
   if (detectorsN >= 7)
      name += "_all";
   else {
      for (auto it : dec) {
         TString det = TAGrecoManager::GetDect3LetName(it);
         det.ToLower();
         if (det == "tgt") continue;
         name += Form("_%s", det.Data());
      }
   }
   
   name += ".root";
   
   return name;
}
