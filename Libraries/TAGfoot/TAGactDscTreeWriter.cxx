

/*!
 \file TAGactDscTreeWriter.cxx
 \brief  Class for shoe root object to flat Ntuple
 */
/*------------------------------------------+---------------------------------*/

#include "TAGactDscTreeWriter.hxx"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

// TGT
#include "TAGparGeo.hxx"

// ST
#include "TASTntuHit.hxx"

// BM
#include "TABMntuTrack.hxx"

//VTX
#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

//ITR
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"
#include "TAITntuCluster.hxx"
#include "TAITntuTrack.hxx"

//MSD
#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDparMap.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDntuTrack.hxx"

//TW
#include "TATWparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"

//CA
#include "TACAparGeo.hxx"
#include "TACAntuCluster.hxx"

// GLB
#include "TAGntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"
#include "TAGnameManager.hxx"
#include "TAGrecoManager.hxx"

//MC
#include "TAMCntuPart.hxx"
#include "TAMCntuPart.hxx"

#include "RecoRaw.hxx"

/*!
 \class TAGactDscTreeWriter
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(TAGactDscTreeWriter)

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] isMC tMC flag
TAGactDscTreeWriter::TAGactDscTreeWriter(const char* name, Bool_t isMC)
:  TAGactTreeWriter(name),
   fFlagMC(isMC),
   fFlagItrTrack(false),
   fFlagMsdTrack(false),
   fFlagRecCutter(false)
{
   fFlagTrack    = TAGrecoManager::GetPar()->IsTracking();
   fFlagHits     = TAGrecoManager::GetPar()->IsSaveHits();
   fStdAloneFlag = RecoRaw::IsStdAlone();
   fSaveMcFlag   = BaseReco::IsSaveMc();
}

//__________________________________________________________
//! default destructor
TAGactDscTreeWriter::~TAGactDscTreeWriter()
{
}

//------------------------------------------+-----------------------------------
//!  Set descriptors
//!
void TAGactDscTreeWriter::SetDescriptors()
{   
   fpNtuHitSt    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TASTntuHit")));
   
   fpNtuHitBm    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TABMntuHit")));
   fpNtuTrackBm  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TABMntuTrack")));
   
   fpNtuHitVtx   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAVTntuHit")));
   fpNtuClusVtx  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAVTntuCluster")));
   fpNtuTrackVtx = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAVTntuTrack")));
   fpNtuVtx      = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAVTntuVertex")));
   
   fpNtuHitIt    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAITntuHit")));
   fpNtuClusIt   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAITntuCluster")));
   fpNtuTrackIt  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAITntuTrack")));
   
   fpNtuHitMsd   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuHit")));
   fpNtuClusMsd  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuCluster")));
   fpNtuRecMsd   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuPoint")));
   fpNtuTrackMsd = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuTrack")));
   
   fpNtuHitTw    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TATWntuHit")));
   fpNtuRecTw    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint")));
   
   fpNtuHitCa    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TACAntuHit")));
   fpNtuClusCa   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TACAntuCluster")));
   
   fpNtuGlbTrack = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAGntuGlbTrack")));
   
   if (fFlagMC) {
      fpNtuMcTrk = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMCntuPart")));
      fpNtuMcEvt = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMCntuEvent")));
      fpNtuMcReg = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMCntuRegion")));
      fpNtuMcSt  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootDataDscMcName(kST)));
      fpNtuMcBm  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootDataDscMcName(kBM)));
      fpNtuMcVt  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootDataDscMcName(kVTX)));
      fpNtuMcIt  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootDataDscMcName(kITR)));
      fpNtuMcMsd = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootDataDscMcName(kMSD)));
      fpNtuMcTw  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootDataDscMcName(kTW)));
      fpNtuMcCa  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootDataDscMcName(kCAL)));
      
   } else {
      fpNtuEvt    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAGntuEvent")));
      fpWDtrigger = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAWDntuTrigger")));
      fpDatRawSt  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TASTntuRaw")));
      fpDatRawBm  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TABMntuRaw")));
      fpDatRawMsd = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuRaw")));
      fpDatRawTw  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TATWntuRaw")));
      fpDatRawCa  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TACAntuRaw")));
   }
   
   if (fpNtuTrackIt)
      fFlagItrTrack = true;
   else
      fFlagItrTrack = false;
   
   if (fpNtuTrackMsd)
      fFlagMsdTrack = true;
   else
      fFlagMsdTrack = false;
}


//------------------------------------------+-----------------------------------
//! Open file.
//!
//! \param[in] name action name
//! \param[in] option open file options
//! \param[in] treeName name of tree in file
//! \param[in] dscBranch flag for object descriptor
Int_t TAGactDscTreeWriter::Open(const TString& name, Option_t* option, const TString treeName, Bool_t /*dscBranch*/)
{
   if (TAGrecoManager::GetPar()->IsSaveTree()) {
     cout <<"savetree" << endl;
     SetDescriptors();
     cout <<"branches" << endl;
      SetTreeBranches();
   }

   cout <<"cazzzoaosdkos" << endl;
   TAGactTreeWriter::Open(name, option);
   
   return 0;
}
   
//__________________________________________________________
//! Set tree branches for writing in output file
void TAGactDscTreeWriter::SetTreeBranches()
{
   if (TAGrecoManager::GetPar()->IncludeST()) {

     SetupElementBranch(fpNtuHitSt);
     cout << "sto qui" << endl;
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      if (fFlagHits)
         SetupElementBranch(fpNtuHitBm);
      
      if (fFlagTrack)
         SetupElementBranch(fpNtuTrackBm);
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      if (fFlagHits)
         SetupElementBranch(fpNtuHitVtx);
      
      if (!fFlagTrack)
         SetupElementBranch(fpNtuClusVtx);
      else {
         SetupElementBranch(fpNtuClusVtx);
         SetupElementBranch(fpNtuTrackVtx);
         if (TAGrecoManager::GetPar()->IncludeTG())
            SetupElementBranch(fpNtuVtx);
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      if (fFlagHits)
         SetupElementBranch(fpNtuHitIt);
      
      SetupElementBranch(fpNtuClusIt);
      if (fFlagItrTrack && fFlagTrack)
         SetupElementBranch(fpNtuTrackIt);
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      if (fFlagHits)
         SetupElementBranch(fpNtuHitMsd);
      
      SetupElementBranch(fpNtuClusMsd);
      SetupElementBranch(fpNtuRecMsd);
      
      if (fFlagMsdTrack && fFlagTrack)
         SetupElementBranch(fpNtuTrackMsd);
   }
   if (TAGrecoManager::GetPar()->IncludeTW() && !TAGrecoManager::GetPar()->CalibTW()) {
      if (fFlagHits)
         SetupElementBranch(fpNtuHitTw);
      
      SetupElementBranch(fpNtuRecTw);
      cout << "sto qui2" << endl;
   }
   
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      if (fFlagHits)
         SetupElementBranch(fpNtuHitCa);
      
      SetupElementBranch(fpNtuClusCa);
   }
   
   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) {
      if (fFlagTrack)
         SetupElementBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());
   }
   
   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) {
      if (fFlagTrack && !fFlagRecCutter)
         SetupElementBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());
   }
         cout << "sto qui3" << endl;
   if (fFlagMC) {
           cout << "sono mc" << endl;
      if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) {
         if (fSaveMcFlag) {
            SetupElementBranch(fpNtuMcEvt);
            SetupElementBranch(fpNtuMcTrk);
            
            if (TAGrecoManager::GetPar()->IsRegionMc() )
               SetupElementBranch(fpNtuMcReg);
         }
         return;
      }
      
      if (!fSaveMcFlag)
         return;
      
      SetupElementBranch(fpNtuMcEvt);
      SetupElementBranch(fpNtuMcTrk);
      
      if (TAGrecoManager::GetPar()->IsRegionMc() )
         SetupElementBranch(fpNtuMcReg);
      
      if (TAGrecoManager::GetPar()->IncludeST())
         SetupElementBranch(fpNtuMcSt, FootBranchMcName(kST));
      
      if (TAGrecoManager::GetPar()->IncludeBM())
         SetupElementBranch(fpNtuMcBm, FootBranchMcName(kBM));
      
      if (TAGrecoManager::GetPar()->IncludeVT())
         SetupElementBranch(fpNtuMcVt, FootBranchMcName(kVTX));
      
      if (TAGrecoManager::GetPar()->IncludeIT())
         SetupElementBranch(fpNtuMcIt, FootBranchMcName(kITR));
      
      if (TAGrecoManager::GetPar()->IncludeMSD())
         SetupElementBranch(fpNtuMcMsd, FootBranchMcName(kMSD));
      
      if (TAGrecoManager::GetPar()->IncludeTW())
         SetupElementBranch(fpNtuMcTw, FootBranchMcName(kTW));
      
      if (TAGrecoManager::GetPar()->IncludeCA())
         SetupElementBranch(fpNtuMcCa, FootBranchMcName(kCAL));
      
   } else {
     cout << "stdalone::" << fStdAloneFlag << endl;
      if (!fStdAloneFlag)
         SetupElementBranch(fpNtuEvt);

      cout << "sto qui4" << endl;
      
      if (TAGrecoManager::GetPar()->IncludeST()) {
         if (fFlagHits)
            SetupElementBranch(fpDatRawSt);
         
	 SetupElementBranch(fpWDtrigger);
      }
      
      if (TAGrecoManager::GetPar()->IncludeBM())
         if (fFlagHits)
            SetupElementBranch(fpDatRawBm);
      
      if (TAGrecoManager::GetPar()->IncludeMSD())
         if (fFlagHits)
            SetupElementBranch(fpDatRawMsd);
      
      if (TAGrecoManager::GetPar()->IncludeTW())
         if (fFlagHits)
            SetupElementBranch(fpDatRawTw);
      
      if (TAGrecoManager::GetPar()->IncludeCA()) {
         if (fFlagHits) {
            SetupElementBranch(fpDatRawCa);
         }
      }
   }
}
