/*!
 \file RecoMC.cxx
 \brief Reconstruction class from MC data
 */
/*------------------------------------------+---------------------------------*/


#include "TTree.h"

#include "RecoMC.hxx"

#include "TAGrecoManager.hxx"
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
RecoMC::RecoMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout),
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
   fActNtuHitCa(0x0),
   fActEvtReader(0x0)
{
   fFlagMC = true;
  
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
   fActEvtReader = new TAGactTreeReader("actEvtReader");

   if ( TAGrecoManager::GetPar()->IsRegionMc()) {
     fpNtuMcReg = new TAGdataDsc(TAMCntuRegion::GetDefParaName(), new TAMCntuRegion());
     if (TAGrecoManager::GetPar()->IsReadRootObj())
       fActEvtReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
     else
       fActNtuMcReg = new TAMCactNtuRegion("regActNtuMc", fpNtuMcReg, fEvtStruct);
   }
  
   fpNtuMcEvt    = new TAGdataDsc("evtMc", new TAMCntuEvent());
   if (TAGrecoManager::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcEvt,TAMCntuEvent::GetBranchName());
   else
     fActNtuMcEvt = new TAMCactNtuEvent("evtActNtuMc", fpNtuMcEvt, fEvtStruct);
  
   fpNtuMcTrk    = new TAGdataDsc("eveMc", new TAMCntuPart());
   if (TAGrecoManager::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcTrk,TAMCntuPart::GetBranchName());
   else
    fActNtuMcTrk = new TAMCactNtuPart("eveActNtuMc", fpNtuMcTrk, fEvtStruct);

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
      
      fpNtuHitSt = new TAGdataDsc("stRaw", new TASTntuHit());
      fActNtuHitSt = new TASTactNtuHitMC("stActNtu", fpNtuMcSt, fpNtuMcTrk, fpNtuHitSt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());

      fpNtuHitBm = new TAGdataDsc("bmRaw", new TABMntuHit());
      fActNtuHitBm = new TABMactNtuHitMC("bmActNtu", fpNtuMcBm, fpNtuMcTrk, fpNtuHitBm, fpParConfBm, fpParCalBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitBm->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
      
      fpNtuHitVtx = new TAGdataDsc("vtRaw", new TAVTntuHit());
      fActNtuHitVtx = new TAVTactNtuHitMC("vtActNtu", fpNtuMcVt, fpNtuMcTrk, fpNtuHitVtx, fpParGeoVtx, fpParConfVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
      
      fpNtuHitIt = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuHitIt = new TAITactNtuHitMC("itActNtu", fpNtuMcIt, fpNtuMcTrk, fpNtuHitIt, fpParGeoIt, fpParConfIt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
      
      fpNtuHitMsd = new TAGdataDsc("msdRaw", new TAMSDntuHit());
      fActNtuHitMsd = new TAMSDactNtuHitMC("msdActNtu", fpNtuMcMsd, fpNtuMcTrk, fpNtuHitMsd, fpParGeoMsd, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitMsd->CreateHistogram();
   }
   
   if(TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
      
      fpNtuHitTw   = new TAGdataDsc("twRaw", new TATWntuHit());

      fActNtuHitTw = new TATWactNtuHitMC("twActNtu", fpNtuMcTw, fpNtuMcSt,fpNtuHitSt, fpNtuMcTrk, fpNtuHitTw,  fpParConfTw, fpParCalTw, fpParGeoG,  fEvtStruct);

      if (fFlagHisto)
         fActNtuHitTw->CreateHistogram();
   }
   
   if(TAGrecoManager::GetPar()->IncludeCA()) {
     fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
     if (TAGrecoManager::GetPar()->IsReadRootObj())
       fActEvtReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
      
     fpNtuHitCa   = new TAGdataDsc("caRaw", new TACAntuHit());
     fActNtuHitCa = new TACAactNtuHitMC("caActNtu", fpNtuMcCa, fpNtuMcTrk, fpNtuHitCa, fpParGeoCa, fpParCalCa, fpParGeoG, fEvtStruct);
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
//! Add required MC data actions in list
void RecoMC::AddRawRequiredItem()
{
   fTAGroot->AddRequiredItem("actEvtReader");
   if (!TAGrecoManager::GetPar()->IsReadRootObj()) {
     fTAGroot->AddRequiredItem("eveActNtuMc");
     fTAGroot->AddRequiredItem("evtActNtuMc");
     if (TAGrecoManager::GetPar()->IsRegionMc() )
       fTAGroot->AddRequiredItem("regActNtuMc");
   }
}

//__________________________________________________________
//! Set tree branches for writing in output file
void RecoMC::SetTreeBranches()
{
   BaseReco::SetTreeBranches();
  
   if (!fgSaveMcFlag)
      return;
   
   fActEvtWriter->SetupElementBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
   fActEvtWriter->SetupElementBranch(fpNtuMcTrk, TAMCntuPart::GetBranchName());
  
  if (TAGrecoManager::GetPar()->IsRegionMc() )
    fActEvtWriter->SetupElementBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());


   if (TAGrecoManager::GetPar()->IncludeST()) {
      fActEvtWriter->SetupElementBranch(fpNtuHitSt, TASTntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fActEvtWriter->SetupElementBranch(fpNtuHitBm, TABMntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitVtx, TAVTntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitIt, TAITntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitMsd, TAMSDntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitTw, TATWntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitCa, TACAntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
   }
}

