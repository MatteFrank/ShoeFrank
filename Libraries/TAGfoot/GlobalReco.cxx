
/*!
 \file GlobalReco.cxx
 \brief Implementation of GlobalReco
 */
/*------------------------------------------+---------------------------------*/

#include "BaseReco.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"
#include "GlobalReco.hxx"

#include "TAMCntuPart.hxx"

//! Class Imp
ClassImp(GlobalReco)

/*!
 \class GlobalReco.
 \brief Global reconstruction class using GenFit from L0 tree
 */
/*------------------------------------------+---------------------------------*/

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
//! \param[in] isMC MC flag
GlobalReco::GlobalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
: BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
  TAGrecoManager::GetPar()->IsFromLocalReco();
  fFlagMC = isMC;
}

//__________________________________________________________
//! default destructor
GlobalReco::~GlobalReco()
{
}

//__________________________________________________________
//! Open inout file
void GlobalReco::OpenFileIn()
{
  fActEvtReader->Open(GetName());
}

//__________________________________________________________
//! Close inout file
void GlobalReco::CloseFileIn()
{
  fActEvtReader->Close();
}

//__________________________________________________________
//! Set L0 reco tree branches
void GlobalReco::SetL0TreeBranches()
{
  BaseReco::SetL0TreeBranches();
  
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) {
    if (fFlagMC) {
      fpNtuMcTrk = new TAGdataDsc(new TAMCntuPart());
      fActEvtReader->SetupBranch(fpNtuMcTrk);
      
      fpNtuMcEvt = new TAGdataDsc(new TAMCntuEvent());
      fActEvtReader->SetupBranch(fpNtuMcEvt);
      
      if (TAGrecoManager::GetPar()->IsRegionMc()) {
        fpNtuMcReg = new TAGdataDsc(new TAMCntuRegion());
        fActEvtReader->SetupBranch(fpNtuMcReg);
      }
      
      if (TAGrecoManager::GetPar()->IncludeKalman() && TAGrecoManager::GetPar()->IsFromLocalReco()) {
        if (TAGrecoManager::GetPar()->IncludeST()) {
          fpNtuMcSt   = new TAGdataDsc(FootDataDscMcName(kST), new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcSt, FootBranchMcName(kST));
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
  }
}
