/*!
 \file RecoMC.cxx
 \brief Reconstruction class from MC data
 */
/*------------------------------------------+---------------------------------*/


#include "TTree.h"

#include "RecoMC.hxx"

#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCflukaStruct.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"
#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuHit.hxx"

/*!
 \class RecoMC
 \brief Reconstruction class from MC data
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(RecoMC)

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
RecoMC::RecoMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout, isMC),
   fEvtStruct(0x0),
   fActNtuMcTrk(0x0),
   fActNtuMcReg(0x0),
   fActNtuMcEvt(0x0),
   fActNtuHitSt(0x0),
   fActNtuHitBm(0x0),
   fActNtuHitVtx(0x0),
   fActNtuHitIt(0x0),
   fActNtuHitMsd(0x0),
   fActNtuHitTw(0x0),
   fActNtuHitCa(0x0)
{
  if (!TAGrecoManager::GetPar()->IsReadRootObj())
    fEvtStruct = new EVENT_STRUCT;
}

//__________________________________________________________
RecoMC::~RecoMC()
{
   // default destructor
}

//__________________________________________________________
//! Create MC data actions
void RecoMC::CreateRawAction()
{

	if(!fFlagMC)	 return;
   const Char_t* name = FootActionDscName("TAGactTreeReader");
   fActEvtReader = new TAGactTreeReader(name);

   if ( TAGrecoManager::GetPar()->IsRegionMc()) {
     fpNtuMcReg = new TAGdataDsc(new TAMCntuRegion());
     if (TAGrecoManager::GetPar()->IsReadRootObj())
       fActEvtReader->SetupBranch(fpNtuMcReg);
     else {
        const Char_t* name = FootActionDscName("TAMCactNtuRegion");
        fActNtuMcReg = new TAMCactNtuRegion(name, fpNtuMcReg, fEvtStruct);
     }
   }
  
   fpNtuMcEvt    = new TAGdataDsc(new TAMCntuEvent());
   if (TAGrecoManager::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcEvt);
   else {
      const Char_t* name = FootActionDscName("TAMCactNtuEvent");
      fActNtuMcEvt = new TAMCactNtuEvent(name, fpNtuMcEvt, fEvtStruct);
   }
   
   fpNtuMcTrk    = new TAGdataDsc(new TAMCntuPart());
   if (TAGrecoManager::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcTrk);
   else {
      const Char_t* name = FootActionDscName("TAMCactNtuPart");
      fActNtuMcTrk = new TAMCactNtuPart(name, fpNtuMcTrk, fEvtStruct);
   }
   
   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuMcSt   = new TAGdataDsc(FootDataDscMcName(kST), new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcSt, FootBranchMcName(kST));
      
      fpNtuHitSt = new TAGdataDsc(new TASTntuHit());
      const Char_t* name = FootActionDscName("TASTactNtuHitMC");
      fActNtuHitSt = new TASTactNtuHitMC(name, fpNtuMcSt, fpNtuMcTrk, fpNtuHitSt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpNtuMcBm   = new TAGdataDsc(FootDataDscMcName(kBM), new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcBm, FootBranchMcName(kBM));

      fpNtuHitBm = new TAGdataDsc(new TABMntuHit());
      const Char_t* name = FootActionDscName("TABMactNtuHitMC");
      fActNtuHitBm = new TABMactNtuHitMC(name, fpNtuMcBm, fpNtuMcTrk, fpNtuHitBm, fpParConfBm, fpParCalBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitBm->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpNtuMcVt   = new TAGdataDsc(FootDataDscMcName(kVTX), new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcVt, FootBranchMcName(kVTX));
      
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      Int_t sensorsN = parGeo->GetSensorsN();
      
      fpNtuHitVtx = new TAGdataDsc(new TAVTntuHit(sensorsN));
      const Char_t* name = FootActionDscName("TAVTactNtuHitMC");
      fActNtuHitVtx = new TAVTactNtuHitMC(name, fpNtuMcVt, fpNtuMcTrk, fpNtuHitVtx, fpParGeoVtx, fpParConfVtx, fpParCalVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpNtuMcIt   = new TAGdataDsc(FootDataDscMcName(kITR), new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcIt, FootBranchMcName(kITR));
      
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      Int_t sensorsN = parGeo->GetSensorsN();

      fpNtuHitIt = new TAGdataDsc(new TAITntuHit(sensorsN));
      const Char_t* name = FootActionDscName("TAITactNtuHitMC");
      fActNtuHitIt = new TAITactNtuHitMC(name, fpNtuMcIt, fpNtuMcTrk, fpNtuHitIt, fpParGeoIt, fpParConfIt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpNtuMcMsd   = new TAGdataDsc(FootDataDscMcName(kMSD), new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcMsd, FootBranchMcName(kMSD));
      
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      Int_t sensorsN = parGeo->GetSensorsN();

      fpNtuHitMsd = new TAGdataDsc(new TAMSDntuHit(sensorsN));
      fActNtuHitMsd = new TAMSDactNtuHitMC("msdActNtu", fpNtuMcMsd, fpNtuMcTrk, fpNtuHitMsd, fpParGeoMsd, fpParCalMsd, fEvtStruct);

      if (fFlagHisto)
         fActNtuHitMsd->CreateHistogram();
   }
   
   if(TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuMcTw   = new TAGdataDsc(FootDataDscMcName(kTW), new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcTw, FootBranchMcName(kTW));
      
      fpNtuHitTw   = new TAGdataDsc(new TATWntuHit());
      const Char_t* name = FootActionDscName("TATWactNtuHitMC");
      fActNtuHitTw = new TATWactNtuHitMC(name, fpNtuMcTw, fpNtuMcSt,fpNtuHitSt, fpNtuMcTrk, fpNtuHitTw,  fpParConfTw, fpParCalTw, fpParGeoG,  fEvtStruct);

      if (fFlagHisto)
         fActNtuHitTw->CreateHistogram();
   }
   
   if(TAGrecoManager::GetPar()->IncludeCA()) {
     fpNtuMcCa   = new TAGdataDsc(FootDataDscMcName(kCAL), new TAMCntuHit());
     if (TAGrecoManager::GetPar()->IsReadRootObj())
       fActEvtReader->SetupBranch(fpNtuMcCa, FootBranchMcName(kCAL));
      
     fpNtuHitCa   = new TAGdataDsc(new TACAntuHit());
      const Char_t* name = FootActionDscName("TACAactNtuHitMC");
     fActNtuHitCa = new TACAactNtuHitMC(name, fpNtuMcCa, fpNtuMcTrk, fpNtuHitCa, fpParGeoCa, fpParCalCa, fpParGeoG, fEvtStruct);
     if (fFlagHisto)
       fActNtuHitCa->CreateHistogram();
   }
}


//__________________________________________________________
//! Go to a given event
//!
//! \param[in] iEvent event number to go
Bool_t RecoMC::GoEvent(Int_t iEvent)
{
  // only possible for MC data
  if (iEvent < fActEvtReader->NEvents()) {
    fSkipEventsN = iEvent;
    fActEvtReader->Reset(iEvent);
    return true;
  }
   
  return false;
}

//__________________________________________________________
//! Open input file
void RecoMC::OpenFileIn()
{
  if (TAGrecoManager::GetPar()->IsReadRootObj())
    fActEvtReader->Open(GetName(), "READ", "EventTree");
  else {
    fActEvtReader->Open(GetName(), "READ", "EventTree", false);
    TTree* tree = ((TAGactTreeReader*)fActEvtReader)->GetTree();
    TAMCflukaParser::FindBranches(tree, fEvtStruct);
  }
}

//__________________________________________________________
//! Close input file
void RecoMC::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
//! Generate output file name
TString RecoMC::GetFileOutName()
{
 
   TString tmp = GetName();
   Int_t pos = tmp.Last('/');
   
   if (pos == -1)
      pos = 0;
   
   Int_t pos1 = tmp.Last('_');
   
   TString name = tmp(pos+1, pos1-pos-1);
   
   name.Prepend("run_");
   
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
