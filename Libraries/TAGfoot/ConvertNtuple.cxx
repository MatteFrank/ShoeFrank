

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
}

//__________________________________________________________
//! default destructor
ConvertNtuple::~ConvertNtuple()
{
}

//__________________________________________________________
//! Actions before loop event
void ConvertNtuple::BeforeEventLoop()
{
   GlobalSettings();
   
   ReadParFiles();
   
   CreateRawAction();
   CreateRecAction();
   SetL0TreeBranches();
   
   CampaignChecks();
   
   AddRecRequiredItem();
   
   OpenFileIn();
   
   OpenFileOut();
   
   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
}

//__________________________________________________________
//! Loop over events
//!
//! \param[in] nEvents number of events to process
void ConvertNtuple::LoopEvent(Int_t nEvents)
{
   Int_t frequency = 1;
   
   if (nEvents >= 100000)      frequency = 10000;
   else if (nEvents >= 10000)  frequency = 1000;
   else if (nEvents >= 1000)   frequency = 100;
   else if (nEvents >= 100)    frequency = 10;
   else if (nEvents >= 10)     frequency = 1;
   
   
   if (fSkipEventsN > 0)
      printf(" Skipped Event: %d\n", fSkipEventsN);
   
   for (Int_t ientry = 0; ientry < nEvents; ientry++) {
      
      if(ientry % frequency == 0)
         printf(" Loaded Event: %d\n", ientry+fSkipEventsN);
      
      if (!fTAGroot->NextEvent()) break;
      FillTreeOut();
      ResetTreeOut();
   }
}

//__________________________________________________________
//! Actions after loop event
void ConvertNtuple::AfterEventLoop()
{
   fTAGroot->EndEventLoop();
   
   CloseFileOut();
   CloseFileIn();
}

//__________________________________________________________
//! Open input file
void ConvertNtuple::OpenFileIn()
{
   fActEvtReader->Open(GetName(), "READ", "tree");
}

//__________________________________________________________
//! Close input file
void ConvertNtuple::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
//!  Open output file
void ConvertNtuple::OpenFileOut()
{
   fActEvtWriter = new TFile("toto.root","RECREATE");
   fTreeOut      = new TTree("OutTree","Reco Event Tree");
   
   SetTreeBranches();
}

//__________________________________________________________
//! Close output file
void ConvertNtuple::CloseFileOut()
{
   // saving current run info
   TAGrecoManager::GetPar()->EnableFromLocalReco();
   TAGrunInfo info = TAGrecoManager::GetPar()->GetGlobalInfo();
   info.SetCampaignName(fExpName);
   info.SetRunNumber(fRunNumber);
   
   //add crossing map if enabled in input mc files
   if(fFlagMC){
      TAGrunInfo inputinfo = gTAGroot->CurrentRunInfo();
      if(inputinfo.GetGlobalPar().EnableRegionMc==true && info.GetGlobalPar().EnableRegionMc==true)
         info.ImportCrossMap(inputinfo);
   }
   gTAGroot->SetRunInfo(info);
   fActEvtWriter->Write();
   fActEvtWriter->Print();
   fActEvtWriter->Close();
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
   
   fpNtuClusVtx = new TAGdataDsc(new TAVTntuCluster());
   GetNtuClusterVtx()->SetParGeo(GetParGeoVtx());
}

//__________________________________________________________
//! Create ITR reconstruction containers
void ConvertNtuple::CreateRecActionIt()
{
   fpNtuClusIt = new TAGdataDsc(new TAITntuCluster());
   GetNtuClusterIt()->SetParGeo(GetParGeoIt());
   
   if (fFlagItrTrack && fFlagTrack)
      fpNtuTrackIt = new TAGdataDsc(new TAITntuTrack());
}

//__________________________________________________________
//! Create MSD reconstruction containers
void ConvertNtuple::CreateRecActionMsd()
{
   fpNtuClusMsd = new TAGdataDsc(new TAMSDntuCluster());
   GetNtuClusterMsd()->SetParGeo(GetParGeoMsd());
   
   fpNtuRecMsd  = new TAGdataDsc(new TAMSDntuPoint());
   GetNtuPointMsd()->SetParGeo(GetParGeoMsd());
   
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
   const Char_t* name = FootActionDscName("TAGactTreeReader");
   fActEvtReader = new TAGactTreeReader(name);
   
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
//! Set tree branches for writing in output file
void ConvertNtuple::SetTreeBranches()
{
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fTreeOut->Branch("st_time",             &fTimeSt);
      fTreeOut->Branch("st_charge",           &fChargeSt);
      fTreeOut->Branch("st_hit_n",            &fHitsNst );
      fTreeOut->Branch("st_pos",              &fPosSt );
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM() && fFlagTrack) {
      fTreeOut->Branch("bm_trk_n",            &fTracksNbm );
      fTreeOut->Branch("bm_trk_chi2",         &fTrackChi2Bm );
      fTreeOut->Branch("bm_Pvers",            &fPversBm );
      fTreeOut->Branch("bm_R0",               &fR0Bm );
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT() && fFlagTrack) {
      fTreeOut->Branch("vertex_n",            &fVextexNvt );
      fTreeOut->Branch("vertex_Pos",          &fVertexPosVt );
      
      fTreeOut->Branch("vt_trk_n",            &fTracksNvt );
      fTreeOut->Branch("vt_trk_chi2",         &fTrackChi2Vt );
      fTreeOut->Branch("vt_trk_slopez",       &fTrackSlopezVt );
      fTreeOut->Branch("vt_trk_origin",       &fTrackOriginVt );
      fTreeOut->Branch("vt_trk_clus_pos_vec", &fTrackClusPosVecVt );
      fTreeOut->Branch("vt_trk_clus",         &fTrackClustersNvt );
      fTreeOut->Branch("vt_trk_clus_hits",    &fTrackClusHitsNvt);
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fTreeOut->Branch("it_sensor_id",        &fSensorIdIt );
      fTreeOut->Branch("it_clus_n",           &fClustersNit );
      fTreeOut->Branch("vt_clus_pos_vec",     &fClusPosVecIt );
      
      if(fFlagItrTrack && fFlagTrack) {
         fTreeOut->Branch("it_trk_n",            &fTracksNit );
         fTreeOut->Branch("it_trk_chi2",         &fTrackChi2It );
         fTreeOut->Branch("it_trk_slopez",       &fTrackSlopezIt );
         fTreeOut->Branch("it_trk_origin",       &fTrackOriginIt );
         fTreeOut->Branch("it_trk_clus_pos_vec", &fTrackClusPosVecIt );
         fTreeOut->Branch("it_trk_clus",         &fTrackClustersNit );
         fTreeOut->Branch("it_trk_clus_hits",    &fTrackClusHitsNit);
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fTreeOut->Branch("msd_station_id",      &fStationIdMsd);
      fTreeOut->Branch("msd_pt_n",            &fPointsNmsd);
      fTreeOut->Branch("msd_time",            &fTimeMsd);
      fTreeOut->Branch("msd_eloss1",          &fEnergyLoss1Msd);
      fTreeOut->Branch("msd_eloss2",          &fEnergyLoss2Msd);
      fTreeOut->Branch("msd_pos",             &fPosMsd );
      
      if(fFlagMsdTrack && fFlagTrack) {
         fTreeOut->Branch("msd_trk_n",            &fTracksNmsd );
         fTreeOut->Branch("msd_trk_chi2",         &fTrackChi2Msd );
         fTreeOut->Branch("msd_trk_slopez",       &fTrackSlopezMsd );
         fTreeOut->Branch("msd_trk_origin",       &fTrackOriginMsd );
         fTreeOut->Branch("msd_trk_clus_pos_vec", &fTrackClusPosVecMsd );
         fTreeOut->Branch("msd_trk_clus",         &fTrackClustersNmsd );
         fTreeOut->Branch("msd_trk_clus_hits",    &fTrackClusHitsNmsd);
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fTreeOut->Branch("tw_pt_n",             &fPointsNtw);
      fTreeOut->Branch("tw_time",             &fTimeTw);
      fTreeOut->Branch("tw_eloss",            &fEnergyLossTw);
      fTreeOut->Branch("tw_pos",              &fPosTw );
      fTreeOut->Branch("tw_chargeZ",          &fChargeZTw);
   }
   
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fTreeOut->Branch("ca_clus_n",           &fClustersNca);
      fTreeOut->Branch("tw_energy",           &fEnergyCa);
      fTreeOut->Branch("ca_pos",              &fPosCa);
   }
   
   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman() || TAGrecoManager::GetPar()->IncludeStraight()) {
      fTreeOut->Branch("glb_evt_number",      &fEvtNumberGlb);
      fTreeOut->Branch("glb_Pdg_id",          &fPdgIdGlb);
      fTreeOut->Branch("glb_length",          &fLengthGlb);
      fTreeOut->Branch("glb_chi2",            &fChi2Glb);
      fTreeOut->Branch("glb_nof",             &fNdofGlb);
      fTreeOut->Branch("glb_Pval",            &fPvalGlb);
      fTreeOut->Branch("glb_quality",         &fQualityGlb);
      
      fTreeOut->Branch("glb_mass",            &fMassGlb);
      fTreeOut->Branch("glb_mom_mod",         &fMomModuleGlb);
      fTreeOut->Branch("glb_tw_chg_z",        &fTwChargeZGlb);
      fTreeOut->Branch("glb_tw_tof",          &fTwTofGlb);
      fTreeOut->Branch("glb_cal_energy",      &fCalEnergyGlb);
      fTreeOut->Branch("glb_trk_id",          &fTrkIdGlb);
      
      fTreeOut->Branch("glb_fit_mass",        &fFitMassGlb);
      fTreeOut->Branch("glb_fit_charge_z",    &fFitChargeZGlb);
      fTreeOut->Branch("glb_fit_tof",         &fFitTofGlb);
      fTreeOut->Branch("glb_fit_enerhy_loss", &fFitEnergyLossGlb);
      fTreeOut->Branch("glb_fit_energy",      &fFitEnergyGlb);
      
      fTreeOut->Branch("glb_tgt_dir",        &fTgtDirGlb);
      fTreeOut->Branch("glb_tgt_pos",        &fTgtPosGlb);
      fTreeOut->Branch("glb_tgt_pos_err",    &fTgtPosErrorGlb);
      fTreeOut->Branch("glb_tgt_mom",        &fTgtMomGlb);
      fTreeOut->Branch("glb_tgt_mom_err",    &fTgtMomErrorGlb);
      
      fTreeOut->Branch("glb_tw_pos",         &fTwPosGlb);
      fTreeOut->Branch("glb_tw_pos_err",     &fTwPosErrorGlb);
      fTreeOut->Branch("glb_tw_mom",         &fTwMomGlb);
      fTreeOut->Branch("glb_tw_mom_err",     &fTwMomErrorGlb);
   }
   
   if (fFlagMC) {
      fTreeOut->Branch("mc_trk_n",            &fPartsNmc );
      fTreeOut->Branch("mc_trk_charge",       &fPartChargeMc );
      fTreeOut->Branch("mc_trk_mass",         &fPartMassMc );
      fTreeOut->Branch("mc_trk_tof",          &fPartTofMc );
      fTreeOut->Branch("mc_trk_length",       &fPartLengthMc );
      fTreeOut->Branch("mc_trk_intpos_x",     &fPartInPosxMc );
      fTreeOut->Branch("mc_trk_intpos_y",     &fPartInPosyMc );
      fTreeOut->Branch("mc_trk_intpos_z",     &fPartInPoszMc );
      fTreeOut->Branch("mc_trk_finpos_x",     &fPartOutPosxMc );
      fTreeOut->Branch("mc_trk_finpos_y",     &fPartOutPosyMc );
      fTreeOut->Branch("mc_trk_finpos_z",     &fPartOutPoszMc );
   }
}

//__________________________________________________________
//! Reset items in tree out
void ConvertNtuple::ResetTreeOut()
{
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fHitsNst = 0;
      fChargeSt.clear();
      fTimeSt.clear();
      fPosSt.clear();
   }
   
   //BM
   if (TAGrecoManager::GetPar()->IncludeBM() && fFlagTrack) {
      fTracksNbm = 0;
      fTrackIdBm.clear();
      fPversBm.clear();
      fR0Bm.clear();
      fTrackChi2Bm.clear();
   }
   
   //VTX
   if (TAGrecoManager::GetPar()->IncludeVT() && fFlagTrack) {
      fTracksNvt = 0;
      fTrackIdVt.clear();
      fTrackClusPosVt.clear();
      fTrackSlopezVt.clear();
      fTrackOriginVt.clear();
      fTrackClusPosVecVt.clear();
      fTrackClustersNvt.clear();
      fTrackClusHitsNvt.clear();
      fTrackChi2Vt.clear();
      
      fVextexNvt = 0;
      fVertexPosVt.clear();
   }
   
   //ITR
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fSensorIdIt.clear();
      fClustersNit.clear();
      fClusPosIt.clear();
      fClusPosVecIt.clear();
      
      if(fFlagItrTrack && fFlagTrack) {
         fTracksNit = 0;
         fTrackIdIt.clear();
         fTrackClusPosIt.clear();
         fTrackSlopezIt.clear();
         fTrackOriginIt.clear();
         fTrackClusPosVecIt.clear();
         fTrackClustersNit.clear();
         fTrackClusHitsNit.clear();
         fTrackChi2It.clear();
      }
   }
   
   //MSD
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fStationIdMsd.clear();
      fPointsNmsd.clear();
      fEnergyLoss1Msd.clear();
      fEnergyLoss2Msd.clear();
      fTimeMsd.clear();
      fPosMsd.clear();
      
      if(fFlagMsdTrack && fFlagTrack) {
         fTracksNmsd = 0;
         fTrackIdMsd.clear();
         fTrackClusPosMsd.clear();
         fTrackSlopezMsd.clear();
         fTrackOriginMsd.clear();
         fTrackClusPosVecMsd.clear();
         fTrackClustersNmsd.clear();
         fTrackClusHitsNmsd.clear();
         fTrackChi2Msd.clear();
      }
   }
   
   //TW
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fPointsNtw = 0;
      fEnergyLossTw.clear();
      fTimeTw.clear();
      fPosTw.clear();
      fChargeZTw.clear();
   }
   
   //CA
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fClustersNca = 0;
      fEnergyCa.clear();
      fPosCa.clear();
   }
   
   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman() || TAGrecoManager::GetPar()->IncludeStraight()) {
      fEvtNumberGlb.clear();
      fPdgIdGlb.clear();
      fLengthGlb.clear();
      fChi2Glb.clear();
      fNdofGlb.clear();
      fPvalGlb.clear();
      fQualityGlb.clear();
      fMassGlb.clear();
      fMomModuleGlb.clear();
      fTwChargeZGlb.clear();
      fTwTofGlb.clear();
      fCalEnergyGlb.clear();
      fTrkIdGlb.clear();
      fFitMassGlb.clear();
      fFitChargeZGlb.clear();
      fFitTofGlb.clear();
      fFitEnergyLossGlb.clear();
      fFitEnergyGlb.clear();
      
      fTgtDirGlb.clear();
      fTgtPosGlb.clear();
      fTgtPosErrorGlb.clear();
      fTgtMomGlb.clear();
      fTgtMomErrorGlb.clear();
      
      fTwPosGlb.clear();
      fTwPosErrorGlb.clear();
      fTwMomGlb.clear();
      fTwMomErrorGlb.clear();
   }
   
   //MC
   if (fFlagMC) {
      fPartsNmc = 0;
      fPartChargeMc.clear();
      fPartMassMc.clear();
      fPartTofMc.clear();
      fPartLengthMc.clear();
      fPartInPosxMc.clear();
      fPartInPosyMc.clear();
      fPartInPoszMc.clear();
      fPartOutPosxMc.clear();
      fPartOutPosyMc.clear();
      fPartOutPoszMc.clear();
   }
}

//__________________________________________________________
//! Add required reconstruction actions in list
void ConvertNtuple::AddRecRequiredItem()
{
   gTAGroot->AddRequiredItem(FootActionDscName("TAGactTreeReader"));
}

//__________________________________________________________
//! Fill tree Out
void ConvertNtuple::FillTreeOut()
{
   if (TAGrecoManager::GetPar()->IncludeST())
      FillTreeOutSt();
   
   if (TAGrecoManager::GetPar()->IncludeBM() && fFlagTrack)
      FillTreeOutBm();
   
   if (TAGrecoManager::GetPar()->IncludeVT() && fFlagTrack)
      FillTreeOutVt();
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      FillTreeOutIt();
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      FillTreeOutMsd();
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      FillTreeOutTw();
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      FillTreeOutCa();
   
   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman() || TAGrecoManager::GetPar()->IncludeStraight())
      FillTreeOutGlb();
      
   if (fFlagMC)
      FillTreeOutMc();
   
   fTreeOut->Fill();
}

//__________________________________________________________
//! Fill ST tree Out
void ConvertNtuple::FillTreeOutSt()
{
   TASTntuHit* stHits = (TASTntuHit*) fpNtuHitSt->Object();
   
   fHitsNst = stHits->GetHitsN();
   
   for (Int_t i = 0; i < fHitsNst; i++) {
      
      TASThit* hit = stHits->GetHit(i);
      Float_t charge  = hit->GetCharge();
      Float_t time    = hit->GetTime();
      
      TVector3 posHit(0,0,0); // center
      
      TVector3 posHitG = fpFootGeo->FromSTLocalToGlobal(posHit);
      
      fChargeSt.push_back(charge);
      fTimeSt.push_back(time);
      fPosSt.push_back(posHitG);
   }
}

//__________________________________________________________
//! Fill BM tree Out
void ConvertNtuple::FillTreeOutBm()
{
   ///Track in BM
   TABMntuTrack* bmTrack = (TABMntuTrack*) fpNtuTrackBm->Object();
   
   fTracksNbm = bmTrack->GetTracksN();
   
   for( Int_t iTrack = 0; iTrack < fTracksNbm; ++iTrack ) {
      
      fTrackIdBm.push_back(iTrack);
      
      TABMtrack* track = bmTrack->GetTrack(iTrack);
      TVector3 Pvers  = track->GetSlope();
      TVector3 R0     = track->GetOrigin();
      Double_t mychi2 = track->GetChiSquare();
      
      fTrackChi2Bm.push_back(mychi2);
         
      fPversBm.push_back(Pvers);
      fR0Bm.push_back(R0);
   } // end loop on tracks
}

//__________________________________________________________
//! Fill VTX tree Out
void ConvertNtuple::FillTreeOutVt()
{
   //  ///Vertex collection
   TAVTvertex*    vtxPD   = 0x0;//NEW
   TVector3 vtxPositionPD = TVector3(0.,0.,0.);
   
   TAVTntuVertex* vtVtx =  (TAVTntuVertex*)fpNtuVtx->Object();
   
   fVextexNvt = vtVtx->GetVertexN();
   for (Int_t iVtx = 0; iVtx < vtVtx->GetVertexN(); ++iVtx) {
      vtxPD = vtVtx->GetVertex(iVtx);
      
      if (vtxPD == 0x0) continue;
      vtxPositionPD = vtxPD->GetPosition();
      
      vtxPositionPD = fpFootGeo->FromVTLocalToGlobal(vtxPositionPD);
      fVertexPosVt.push_back(vtxPositionPD);
   }
   
   ///Vertex track collection
   TAVTntuTrack* vtTrack = (TAVTntuTrack*)fpNtuTrackVtx->Object();
   
   fTracksNvt = vtTrack->GetTracksN();
   for( Int_t iTrack = 0; iTrack < vtTrack->GetTracksN(); ++iTrack ) {
      TAVTtrack* track = vtTrack->GetTrack(iTrack);
      fTrackIdVt.push_back(iTrack);
      Float_t Chi2 = track-> GetChi2();
      fTrackChi2Vt.push_back(Chi2);
      
      TVector3 slopez = track->GetSlopeZ();
      fTrackSlopezVt.push_back(slopez);
      TVector3 origin = track->GetOrigin();
      fTrackOriginVt.push_back(origin);
      
      Float_t nCluster =  track->GetClustersN();
      fTrackClustersNvt.push_back(nCluster);
      
      Int_t TotHits = 0;
      for( Int_t iclus = 0; iclus < nCluster; ++iclus ) {
         TAVTcluster* clus = (TAVTcluster*)track->GetCluster(iclus);
         TVector3 p_clus = clus->GetPositionG();
         
         Int_t nHits = clus->GetPixelsN();
         TotHits += nHits;
         
         p_clus = fpFootGeo->FromVTLocalToGlobal(p_clus);
         fTrackClusPosVt.push_back(p_clus);
      }
      fTrackClusHitsNvt.push_back(TotHits);
      fTrackClusPosVecVt.push_back(fTrackClusPosVt);
      fTrackClusPosVt.clear();
   }
}

//__________________________________________________________
//! Fill ITR tree Out
void ConvertNtuple::FillTreeOutIt()
{
   TAITntuCluster* itClus = (TAITntuCluster*) fpNtuClusIt->Object();
   
   for (int iSensor = 0; iSensor < 32; ++iSensor) {
      
      Int_t nCluster = itClus->GetClustersN(iSensor);
      fClustersNit.push_back(nCluster);
      fSensorIdIt.push_back(iSensor);
      
      for( Int_t iclus = 0; iclus < nCluster; ++iclus ) {
         TAVTcluster* clus = (TAVTcluster*)itClus->GetCluster(iSensor, iclus);
         TVector3 p_clus = clus->GetPositionG();
         
         p_clus = fpFootGeo->FromITLocalToGlobal(p_clus);
         fClusPosIt.push_back(p_clus);
      }
      fClusPosVecIt.push_back(fClusPosIt);
      fClusPosIt.clear();
   }
   
   if(fFlagItrTrack && fFlagTrack) {
      TAITntuTrack* itTrack = (TAITntuTrack*)fpNtuTrackIt->Object();
      
      fTracksNit = itTrack->GetTracksN();
      for( Int_t iTrack = 0; iTrack < itTrack->GetTracksN(); ++iTrack ) {
         TAITtrack* track = itTrack->GetTrack(iTrack);
         fTrackIdIt.push_back(iTrack);
         Float_t Chi2 = track-> GetChi2();
         fTrackChi2It.push_back(Chi2);
         
         TVector3 slopez = track->GetSlopeZ();
         fTrackSlopezIt.push_back(slopez);
         TVector3 origin = track->GetOrigin();
         fTrackOriginIt.push_back(origin);
         
         Float_t nCluster =  track->GetClustersN();
         fTrackClustersNit.push_back(nCluster);
         
         Int_t TotHits = 0;
         for( Int_t iclus = 0; iclus < nCluster; ++iclus ) {
            TAITcluster* clus = (TAITcluster*)track->GetCluster(iclus);
            TVector3 p_clus = clus->GetPositionG();
            
            Int_t nHits = clus->GetPixelsN();
            TotHits += nHits;
            
            p_clus = fpFootGeo->FromVTLocalToGlobal(p_clus);
            fTrackClusPosIt.push_back(p_clus);
         }
         fTrackClusHitsNit.push_back(TotHits);
         fTrackClusPosVecIt.push_back(fTrackClusPosVt);
         fTrackClusPosIt.clear();
      }
   }
}

//__________________________________________________________
//! Fill MSD tree Out
void ConvertNtuple::FillTreeOutMsd()
{
   TAMSDntuPoint* msPoint = (TAMSDntuPoint*) fpNtuRecMsd->Object();
   
   for (int iStation = 0; iStation < 3; ++iStation) {
      
      Int_t nPoint = msPoint->GetPointsN(iStation);
      fPointsNmsd.push_back(nPoint);
      fStationIdMsd.push_back(iStation);
      
      for (int i = 0; i < nPoint; i++) {
         
         TAMSDpoint* point = msPoint->GetPoint(iStation, i);
         TVector3 posG = point->GetPosition();
         
         posG = fpFootGeo->FromMSDLocalToGlobal(posG);
         
         Float_t edep1 = point->GetEnergyLoss1();
         Float_t edep2 = point->GetEnergyLoss2();
         Float_t time  = point->GetTime();
         
         fEnergyLoss1Msd.push_back(edep1);
         fEnergyLoss2Msd.push_back(edep2);
         fTimeMsd.push_back(time);
         fPosMsd.push_back(posG);
      }
   }
   
   if(fFlagMsdTrack && fFlagTrack) {
      TAMSDntuTrack* itTrack = (TAMSDntuTrack*)fpNtuTrackMsd->Object();
      
      fTracksNmsd = itTrack->GetTracksN();
      for( Int_t iTrack = 0; iTrack < itTrack->GetTracksN(); ++iTrack ) {
         TAMSDtrack* track = itTrack->GetTrack(iTrack);
         fTrackIdMsd.push_back(iTrack);
         Float_t Chi2 = track-> GetChi2();
         fTrackChi2Msd.push_back(Chi2);
         
         TVector3 slopez = track->GetSlopeZ();
         fTrackSlopezMsd.push_back(slopez);
         TVector3 origin = track->GetOrigin();
         fTrackOriginMsd.push_back(origin);
         
         Float_t nCluster =  track->GetClustersN();
         fTrackClustersNmsd.push_back(nCluster);
         
         Int_t TotHits = 0;
         for( Int_t iclus = 0; iclus < nCluster; ++iclus ) {
            TAMSDcluster* clus = (TAMSDcluster*)track->GetCluster(iclus);
            TVector3 p_clus = clus->GetPositionG();
            
            Int_t nHits = clus->GetStripsN();
            TotHits += nHits;
            
            p_clus = fpFootGeo->FromVTLocalToGlobal(p_clus);
            fTrackClusPosMsd.push_back(p_clus);
         }
         fTrackClusHitsNmsd.push_back(TotHits);
         fTrackClusPosVecMsd.push_back(fTrackClusPosVt);
         fTrackClusPosMsd.clear();
      }
   }
}

//__________________________________________________________
//! Fill TW tree Out
void ConvertNtuple::FillTreeOutTw()
{
   TATWntuPoint* twPoint = (TATWntuPoint*) fpNtuRecTw->Object();
   fPointsNtw = twPoint->GetPointsN();
   for (int i = 0; i < fPointsNtw; i++) {
      
      TATWpoint* point = twPoint->GetPoint(i);;
      TVector3 posG = point->GetPosition();
      
      posG = fpFootGeo->FromTWLocalToGlobal(posG);
      
      Float_t edep    = point->GetEnergyLoss();
      Float_t time    = point->GetToF();
      Float_t chargeZ = point->GetChargeZ();
      
      fEnergyLossTw.push_back(edep);
      fTimeTw.push_back(time);
      fPosTw.push_back(posG);
      fChargeZTw.push_back(chargeZ);
   }
}

//__________________________________________________________
//! Fill CAL tree Out
void ConvertNtuple::FillTreeOutCa()
{
   TACAntuCluster* cluster = (TACAntuCluster*) fpNtuClusCa->Object();
   
   fClustersNca =  cluster->GetClustersN();
   
   for( Int_t iclus = 0; iclus < fClustersNca; ++iclus ) {
      TACAcluster* clus = (TACAcluster*)cluster->GetCluster(iclus);
      
      TVector3 posG = clus->GetPosition();
      
      posG = fpFootGeo->FromCALocalToGlobal(posG);
      fPosCa.push_back(posG);
      
      Float_t edep = clus->GetEnergy();
      fEnergyCa.push_back(edep);
   }
}

//__________________________________________________________
//! Fill GLB tree Out
void ConvertNtuple::FillTreeOutGlb()
{
   // Glb Track
   TAGntuGlbTrack* glbTrack = (TAGntuGlbTrack*) fpNtuGlbTrack->Object();
   
   for( Int_t iTrack = 0; iTrack < glbTrack->GetTracksN(); ++iTrack ) {
      
      TAGtrack* track = glbTrack->GetTrack(iTrack);
      
      Int_t    trkId     = iTrack;
      fEvtNumberGlb.push_back(trkId);
      
      Int_t    pdgId     = track->GetPdgID();
      fPdgIdGlb.push_back(pdgId);
      
      Long64_t evtNb     = track->GetEvtNumber();
      fEvtNumberGlb.push_back(evtNb);
      
      Double_t length    = track->GetLength();
      fLengthGlb.push_back(length);
      
      Double_t chi2      = track->GetChi2();
      fChi2Glb.push_back(chi2);
      
      Int_t    ndof      = track->GetNdof();
      fNdofGlb.push_back(ndof);
      
      Double_t pVal      = track->GetPval();
      fPvalGlb.push_back(pVal);

      Double_t qual      = track->GetQuality();
      fQualityGlb.push_back(qual);

      
      Double_t mass      = track->GetMass();
      fMassGlb.push_back(mass);

      Double_t momMod    = track->GetMomentum();
      fMomModuleGlb.push_back(momMod);

      Double_t twTof = track->GetTwTof();
      fTwTofGlb.push_back(twTof);

      Double_t chgTwZ    = track->GetTwChargeZ();
      fTwChargeZGlb.push_back(chgTwZ);

      Double_t enCal     = track->GetCalEnergy();
      fTwTofGlb.push_back(enCal);


      Double_t fitMass   = track->GetFitMass();
      fFitMassGlb.push_back(fitMass);

      Double_t fitChgZ   = track->GetFitChargeZ();
      fFitChargeZGlb.push_back(fitChgZ);

      Double_t fitTof   = track->GetFitTof();
      fFitTofGlb.push_back(fitTof);

      Double_t fitEloss  = track->GetFitEnergyLoss();
      fQualityGlb.push_back(fitEloss);

      Double_t fitEn     = track->GetFitEnergy();
      fQualityGlb.push_back(fitEn);

      TVector3 tgtDir    = track->GetTgtDirection();
      fTgtDirGlb.push_back(tgtDir);
      
      TVector3 tgtPos    = track->GetTgtPosition();
      fTgtPosGlb.push_back(tgtPos);

      TVector3 tgtPosErr = track->GetTgtPosError();
      fTgtPosErrorGlb.push_back(tgtPosErr);


      TVector3 tgtMom    = track->GetTgtMomentum();
      fTgtMomGlb.push_back(tgtMom);

      TVector3 tgtMonErr = track->GetTgtMomError();
      fTgtMomErrorGlb.push_back(tgtMonErr);


      TVector3 twPos     = track->GetTwPosition();
      fTwPosGlb.push_back(twPos);

      TVector3 twPosErr  = track->GetTwPosError();
      fTwPosErrorGlb.push_back(twPosErr);

      TVector3 twMom     = track->GetTwMomentum();
      fTwMomGlb.push_back(twMom);

      TVector3 twMonErr  = track->GetTwMomError();
      fTwMomErrorGlb.push_back(twMonErr);
   } // end loop on tracks
}

//__________________________________________________________
//! Fill MC tree Out
void ConvertNtuple::FillTreeOutMc()
{
   //truth track coll
   TAMCntuPart* eve = (TAMCntuPart*)fpNtuMcTrk->Object();
   fPartsNmc = eve->GetTracksN();
   
   for( Int_t iTrack = 0; iTrack < eve->GetTracksN(); ++iTrack ) {
      TAMCpart* track = eve->GetTrack(iTrack);
      
      Double_t Charge = track->GetCharge();
      Double_t Mass   = track->GetMass();
      
      fPartChargeMc.push_back(Charge);
      fPartMassMc.push_back(Mass);
      
      TVector3 InitPos = track->GetInitPos();
      fPartInPosxMc.push_back(InitPos.x());
      fPartInPosyMc.push_back(InitPos.y());
      fPartInPoszMc.push_back(InitPos.z());
      
      TVector3 FinalPos = track->GetFinalPos();
      fPartOutPosxMc.push_back(FinalPos.x());
      fPartOutPosyMc.push_back(FinalPos.y());
      fPartOutPoszMc.push_back(FinalPos.z());
      
      Double_t tof       =  track->GetTof();
      Double_t trkLength =  track->GetTrkLength();
      
      fPartTofMc.push_back(tof);
      fPartLengthMc.push_back(trkLength);
   }
}
