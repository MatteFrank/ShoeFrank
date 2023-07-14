

/*!
 \file ConvertNtuple.cxx
 \brief  Class for shoe root object to flat Ntuple
 */
/*------------------------------------------+---------------------------------*/

#include "ConvertNtuple.hxx"

#include "TAGroot.hxx"

#include "TAGactTreeReader.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGnameManager.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class ConvertNtuple
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(ConvertNtuple)

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
ConvertNtuple::ConvertNtuple(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, TString fileNameMcIn, TString treeNameMc)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
   fFlagMC = isMC;
   
   if (!fileNameMcIn.IsNull() && isMC) {
      DisableSaveMc();
      fFriendFileName = fileNameMcIn;
      fFriendTreeName = treeNameMc;
   }
   
   const Char_t* name = FootActionDscName("TAGactTreeReader");
   fActEvtReader = new TAGactTreeReader(name);
}

//__________________________________________________________
//! default destructor
ConvertNtuple::~ConvertNtuple()
{
}

//__________________________________________________________
//! Open input file
void ConvertNtuple::OpenFileIn()
{
   SetL0TreeBranches();
   fActEvtReader->Open(GetName(), "READ", "tree");
}

//__________________________________________________________
//! Close input file
void ConvertNtuple::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
//! Create reconstruction containers
void ConvertNtuple::CreateRecAction()
{
   if (TAGrecoManager::GetPar()->IncludeBM())
      CreateRecActionBm();
   
   if (TAGrecoManager::GetPar()->IncludeVT())
      CreateRecActionVtx();
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      CreateRecActionIt();
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      CreateRecActionMsd();
   
   if (TAGrecoManager::GetPar()->IncludeTW() && !TAGrecoManager::GetPar()->CalibTW())
      CreateRecActionTw();
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      CreateRecActionCa();
   
   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman() || TAGrecoManager::GetPar()->IncludeStraight())
      CreateRecActionGlb();
}

//__________________________________________________________
//! Create BM reconstruction containers
void ConvertNtuple::CreateRecActionBm()
{
   if(fFlagTrack)
      fpNtuTrackBm = new TAGdataDsc(new TABMntuTrack());
}

//__________________________________________________________
//! Create VTX reconstruction containers
void ConvertNtuple::CreateRecActionVtx()
{
   if(fFlagTrack) {
      fpNtuTrackVtx = new TAGdataDsc(new TAVTntuTrack());
      if (TAGrecoManager::GetPar()->IncludeTG())
         fpNtuVtx = new TAGdataDsc(new TAVTntuVertex());
   }
   Int_t sensorsN = GetParGeoVtx()->GetSensorsN();
   fpNtuClusVtx = new TAGdataDsc(new TAVTntuCluster(sensorsN));
}

//__________________________________________________________
//! Create ITR reconstruction containers
void ConvertNtuple::CreateRecActionIt()
{
   Int_t sensorsN = GetParGeoIt()->GetSensorsN();
   fpNtuClusIt = new TAGdataDsc(new TAITntuCluster(sensorsN));
   
   if (fFlagItrTrack && fFlagTrack)
      fpNtuTrackIt = new TAGdataDsc(new TAITntuTrack());
}

//__________________________________________________________
//! Create MSD reconstruction containers
void ConvertNtuple::CreateRecActionMsd()
{
   TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
   Int_t sensorsN = parGeo->GetSensorsN();
   Int_t stationsN = parGeo->GetStationsN();

   fpNtuClusMsd = new TAGdataDsc(new TAMSDntuCluster(sensorsN));   
   fpNtuRecMsd  = new TAGdataDsc(new TAMSDntuPoint(stationsN));
   
   if (fFlagMsdTrack && fFlagTrack)
      fpNtuTrackMsd = new TAGdataDsc(new TAMSDntuTrack());
}

//__________________________________________________________
//! Create TW reconstruction containers
void ConvertNtuple::CreateRecActionTw()
{
   fpNtuRecTw = new TAGdataDsc(new TATWntuPoint());
}

//__________________________________________________________
//! Create CAL reconstruction containers
void ConvertNtuple::CreateRecActionCa()
{
   fpNtuClusCa = new TAGdataDsc(new TACAntuCluster());
}

//__________________________________________________________
//! Create global track reconstruction containers
void ConvertNtuple::CreateRecActionGlb()
{
   if(fFlagTrack)
      fpNtuGlbTrack = new TAGdataDsc(new TAGntuGlbTrack());
}

//__________________________________________________________
//! Set L0 tree branches for reading back
void ConvertNtuple::SetL0TreeBranches()
{
   if (!fgSaveMcFlag)
      if (!fFriendFileName.IsNull() && !fFriendTreeName.IsNull()) {
         fActEvtReader->AddFriendTree(fFriendFileName,fFriendTreeName);
         Info("SetL0TreeBranches()", "\n Open file %s for friend tree %s\n", fFriendFileName.Data(), fFriendTreeName.Data());
      }
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fpNtuHitSt   = new TAGdataDsc(new TASTntuHit());
      fActEvtReader->SetupBranch(fpNtuHitSt);
   }

   if (TAGrecoManager::GetPar()->IncludeBM() && fFlagTrack)
      fActEvtReader->SetupBranch(fpNtuTrackBm);
   
   if (TAGrecoManager::GetPar()->IncludeVT() && fFlagTrack) {
      fActEvtReader->SetupBranch(fpNtuTrackVtx);
      fActEvtReader->SetupBranch(fpNtuClusVtx);
      fActEvtReader->SetupBranch(fpNtuVtx);
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fActEvtReader->SetupBranch(fpNtuClusIt);
      if (fFlagItrTrack && fFlagTrack)
         fActEvtReader->SetupBranch(fpNtuTrackIt);
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fActEvtReader->SetupBranch(fpNtuClusMsd);
      fActEvtReader->SetupBranch(fpNtuRecMsd);
      if (fFlagMsdTrack && fFlagTrack)
         fActEvtReader->SetupBranch(fpNtuTrackMsd);
   }
   
   if(TAGrecoManager::GetPar()->IncludeTW())
      fActEvtReader->SetupBranch(fpNtuRecTw);
   
   if(TAGrecoManager::GetPar()->IncludeCA())
      fActEvtReader->SetupBranch(fpNtuClusCa);
   
   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman() || TAGrecoManager::GetPar()->IncludeStraight())
      fActEvtReader->SetupBranch(fpNtuGlbTrack,  TAGntuGlbTrack::GetBranchName());

   if (fFlagMC) {
      fpNtuMcTrk = new TAGdataDsc(new TAMCntuPart());
      fActEvtReader->SetupBranch(fpNtuMcTrk);
      
      fpNtuMcEvt = new TAGdataDsc(new TAMCntuEvent());
      fActEvtReader->SetupBranch(fpNtuMcEvt);
      
      if (TAGrecoManager::GetPar()->IsRegionMc()) {
         fpNtuMcReg = new TAGdataDsc(new TAMCntuRegion());
         fActEvtReader->SetupBranch(fpNtuMcReg);
      }
   }
   
   if (fReadL0Hits && fFlagMC) {
      if (TAGrecoManager::GetPar()->IncludeST()) {
         fpNtuMcSt   = new TAGdataDsc(FootDataDscMcName(kST), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcSt,FootBranchMcName(kST));
      }
      
      if (TAGrecoManager::GetPar()->IncludeBM()) {
         fpNtuMcBm   = new TAGdataDsc(FootDataDscMcName(kBM), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcBm,FootBranchMcName(kBM));
      }
      
      if (TAGrecoManager::GetPar()->IncludeVT()) {
         fpNtuMcVt   = new TAGdataDsc(FootDataDscMcName(kVTX), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcVt,FootBranchMcName(kVTX));
      }
      
      if (TAGrecoManager::GetPar()->IncludeIT()) {
         fpNtuMcIt   = new TAGdataDsc(FootDataDscMcName(kITR), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcIt,FootBranchMcName(kITR));
      }
      
      if (TAGrecoManager::GetPar()->IncludeMSD()) {
         fpNtuMcMsd   = new TAGdataDsc(FootDataDscMcName(kMSD), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcMsd,FootBranchMcName(kMSD));
      }
      
      if(TAGrecoManager::GetPar()->IncludeTW()) {
         fpNtuMcTw   = new TAGdataDsc(FootDataDscMcName(kTW), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcTw,FootBranchMcName(kTW));
      }
      
      if(TAGrecoManager::GetPar()->IncludeCA()) {
         fpNtuMcCa   = new TAGdataDsc(FootDataDscMcName(kCAL), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcCa,FootBranchMcName(kCAL));
      }
   }
}

//__________________________________________________________
//! Add required reconstruction actions in list
void ConvertNtuple::AddRecRequiredItem()
{
   gTAGroot->AddRequiredItem(FootActionDscName("TAGactTreeReader"));
   gTAGroot->AddRequiredItem(FootActionDscName("TAGactFlatTreeWriter"));
}
