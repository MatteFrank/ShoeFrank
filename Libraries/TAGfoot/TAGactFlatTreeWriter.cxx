

/*!
 \file TAGactFlatTreeWriter.cxx
 \brief  Class for shoe root object to flat Ntuple
 */
/*------------------------------------------+---------------------------------*/

#include "TAGactFlatTreeWriter.hxx"

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


/*!
 \class TAGactFlatTreeWriter
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(TAGactFlatTreeWriter)

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] isMC tMC flag
TAGactFlatTreeWriter::TAGactFlatTreeWriter(const char* name)
:  TAGactTreeWriter(name),
   fFlagMC(false),
   fFlagItrTrack(false),
   fFlagMsdTrack(false)
{
   fFlagTrack    = TAGrecoManager::GetPar()->IsTracking();
}

//__________________________________________________________
//! default destructor
TAGactFlatTreeWriter::~TAGactFlatTreeWriter()
{
}

//__________________________________________________________
//! Loop over events
//!
Bool_t TAGactFlatTreeWriter::Process()
{
   FillTreeOut();
   ResetTreeOut();
   
   return true;
}

//------------------------------------------+-----------------------------------
//!  Set descriptors
//!
void TAGactFlatTreeWriter::SetDescriptors()
{
   fpFootGeo     = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   fpNtuHitSt    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TASTntuHit"),     "TASTntuHit"));
   fpNtuTrackBm  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TABMntuTrack"),   "TABMntuTrack"));
   fpNtuTrackVtx = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAVTntuTrack"),   "TAVTntuTrack"));
   fpNtuVtx      = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAVTntuVertex"),  "TAVTntuVertex"));
   fpNtuClusIt   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAITntuCluster"), "TAITntuCluster"));
   fpNtuTrackIt  = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAITntuTrack"),   "TAITntuTrack"));
   fpNtuRecMsd   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuPoint"),  "TAMSDntuPoint"));
   fpNtuTrackMsd = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuTrack"),  "TAMSDntuTrack"));
   fpNtuRecTw    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"),   "TATWntuPoint"));
   fpNtuClusCa   = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TACAntuCluster"), "TACAntuCluster"));
   fpNtuGlbTrack = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAGntuGlbTrack"), "TAGntuGlbTrack"));
   fpNtuMcTrk    = static_cast<TAGdataDsc*>(gTAGroot->FindDataDsc(FootActionDscName("TAMCntuPart"),    "TAMCntuPart"));
   
   fpGeoMapItr = static_cast<TAGparaDsc*>(gTAGroot->FindParaDsc(FootParaDscName("TAITparGeo"), "TAITparGeo"));
   fpGeoMapMsd = static_cast<TAGparaDsc*>(gTAGroot->FindParaDsc(FootParaDscName("TAMSDparGeo"), "TAMSDparGeo"));
   
   if (fpNtuTrackIt)
      fFlagItrTrack = true;
   else
      fFlagItrTrack = false;
   
   if (fpNtuTrackMsd)
      fFlagMsdTrack = true;
   else
      fFlagMsdTrack = false;
   
   if (fpNtuMcTrk)
      fFlagMC = true;
   else
      fFlagMC = false;
}


//------------------------------------------+-----------------------------------
//! Open file.
//!
//! \param[in] name action name
//! \param[in] option open file options
//! \param[in] treeName name of tree in file
//! \param[in] dscBranch flag for object descriptor
Int_t TAGactFlatTreeWriter::Open(const TString& name, Option_t* option, const TString treeName, Bool_t /*dscBranch*/)
{
   SetDescriptors();
   
   TDirectory* p_cwd = gDirectory;
   
   Int_t pos = name.Last('.');
   TString tmp(name(0, pos));
   tmp += "_FlatTree.root";
   
   fpFile = new TFile(tmp.Data(), option);
   fpTree = new TTree(treeName.Data(), "Reco Event Tree");
   
   SetTreeBranches();
   
   gDirectory = p_cwd;

   return 0;
}

//__________________________________________________________
//! Set tree branches for writing in output file
void TAGactFlatTreeWriter::SetTreeBranches()
{
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fpTree->Branch("st_time",             &fTimeSt);
      fpTree->Branch("st_charge",           &fChargeSt);
      fpTree->Branch("st_hit_n",            &fHitsNst );
      fpTree->Branch("st_pos",              &fPosSt );
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM() && fFlagTrack) {
      fpTree->Branch("bm_trk_n",            &fTracksNbm );
      fpTree->Branch("bm_trk_chi2",         &fTrackChi2Bm );
      fpTree->Branch("bm_Pvers",            &fPversBm );
      fpTree->Branch("bm_R0",               &fR0Bm );
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT() && fFlagTrack) {
      fpTree->Branch("vertex_n",            &fVextexNvt );
      fpTree->Branch("vertex_Pos",          &fVertexPosVt );
      
      fpTree->Branch("vt_trk_n",            &fTracksNvt );
      fpTree->Branch("vt_trk_chi2",         &fTrackChi2Vt );
      fpTree->Branch("vt_trk_slopez",       &fTrackSlopezVt );
      fpTree->Branch("vt_trk_origin",       &fTrackOriginVt );
      fpTree->Branch("vt_trk_clus_pos_vec", &fTrackClusPosVecVt );
      fpTree->Branch("vt_trk_clus",         &fTrackClustersNvt );
      fpTree->Branch("vt_trk_clus_hits",    &fTrackClusHitsNvt);
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpTree->Branch("it_sensor_id",        &fSensorIdIt );
      fpTree->Branch("it_clus_n",           &fClustersNit );
      fpTree->Branch("vt_clus_pos_vec",     &fClusPosVecIt );
      
      if(fFlagItrTrack && fFlagTrack) {
         fpTree->Branch("it_trk_n",            &fTracksNit );
         fpTree->Branch("it_trk_chi2",         &fTrackChi2It );
         fpTree->Branch("it_trk_slopez",       &fTrackSlopezIt );
         fpTree->Branch("it_trk_origin",       &fTrackOriginIt );
         fpTree->Branch("it_trk_clus_pos_vec", &fTrackClusPosVecIt );
         fpTree->Branch("it_trk_clus",         &fTrackClustersNit );
         fpTree->Branch("it_trk_clus_hits",    &fTrackClusHitsNit);
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpTree->Branch("msd_station_id",      &fStationIdMsd);
      fpTree->Branch("msd_pt_n",            &fPointsNmsd);
      fpTree->Branch("msd_time",            &fTimeMsd);
      fpTree->Branch("msd_eloss1",          &fEnergyLoss1Msd);
      fpTree->Branch("msd_eloss2",          &fEnergyLoss2Msd);
      fpTree->Branch("msd_pos",             &fPosMsd );
      
      if(fFlagMsdTrack && fFlagTrack) {
         fpTree->Branch("msd_trk_n",            &fTracksNmsd );
         fpTree->Branch("msd_trk_chi2",         &fTrackChi2Msd );
         fpTree->Branch("msd_trk_slopez",       &fTrackSlopezMsd );
         fpTree->Branch("msd_trk_origin",       &fTrackOriginMsd );
         fpTree->Branch("msd_trk_clus_pos_vec", &fTrackClusPosVecMsd );
         fpTree->Branch("msd_trk_clus",         &fTrackClustersNmsd );
         fpTree->Branch("msd_trk_clus_hits",    &fTrackClusHitsNmsd);
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fpTree->Branch("tw_pt_n",             &fPointsNtw);
      fpTree->Branch("tw_time",             &fTimeTw);
      fpTree->Branch("tw_eloss",            &fEnergyLossTw);
      fpTree->Branch("tw_pos",              &fPosTw );
      fpTree->Branch("tw_chargeZ",          &fChargeZTw);
   }
   
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpTree->Branch("ca_clus_n",           &fClustersNca);
      fpTree->Branch("tw_energy",           &fEnergyCa);
      fpTree->Branch("ca_pos",              &fPosCa);
   }
   
   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman() || TAGrecoManager::GetPar()->IncludeStraight()) {
      fpTree->Branch("glb_evt_number",      &fEvtNumberGlb);
      fpTree->Branch("glb_Pdg_id",          &fPdgIdGlb);
      fpTree->Branch("glb_length",          &fLengthGlb);
      fpTree->Branch("glb_chi2",            &fChi2Glb);
      fpTree->Branch("glb_nof",             &fNdofGlb);
      fpTree->Branch("glb_Pval",            &fPvalGlb);
      fpTree->Branch("glb_quality",         &fQualityGlb);
      
      fpTree->Branch("glb_mass",            &fMassGlb);
      fpTree->Branch("glb_mom_mod",         &fMomModuleGlb);
      fpTree->Branch("glb_tw_chg_z",        &fTwChargeZGlb);
      fpTree->Branch("glb_tw_tof",          &fTwTofGlb);
      fpTree->Branch("glb_cal_energy",      &fCalEnergyGlb);
      fpTree->Branch("glb_trk_id",          &fTrkIdGlb);
      
      fpTree->Branch("glb_fit_mass",        &fFitMassGlb);
      fpTree->Branch("glb_fit_charge_z",    &fFitChargeZGlb);
      fpTree->Branch("glb_fit_tof",         &fFitTofGlb);
      fpTree->Branch("glb_fit_enerhy_loss", &fFitEnergyLossGlb);
      fpTree->Branch("glb_fit_energy",      &fFitEnergyGlb);
      
      fpTree->Branch("glb_tgt_dir",        &fTgtDirGlb);
      fpTree->Branch("glb_tgt_pos",        &fTgtPosGlb);
      fpTree->Branch("glb_tgt_pos_err",    &fTgtPosErrorGlb);
      fpTree->Branch("glb_tgt_mom",        &fTgtMomGlb);
      fpTree->Branch("glb_tgt_mom_err",    &fTgtMomErrorGlb);
      
      fpTree->Branch("glb_tw_pos",         &fTwPosGlb);
      fpTree->Branch("glb_tw_pos_err",     &fTwPosErrorGlb);
      fpTree->Branch("glb_tw_mom",         &fTwMomGlb);
      fpTree->Branch("glb_tw_mom_err",     &fTwMomErrorGlb);
   }
   
   if (fFlagMC) {
      fpTree->Branch("mc_trk_n",            &fPartsNmc );
      fpTree->Branch("mc_trk_charge",       &fPartChargeMc );
      fpTree->Branch("mc_trk_mass",         &fPartMassMc );
      fpTree->Branch("mc_trk_tof",          &fPartTofMc );
      fpTree->Branch("mc_trk_length",       &fPartLengthMc );
      fpTree->Branch("mc_trk_intpos_x",     &fPartInPosxMc );
      fpTree->Branch("mc_trk_intpos_y",     &fPartInPosyMc );
      fpTree->Branch("mc_trk_intpos_z",     &fPartInPoszMc );
      fpTree->Branch("mc_trk_finpos_x",     &fPartOutPosxMc );
      fpTree->Branch("mc_trk_finpos_y",     &fPartOutPosyMc );
      fpTree->Branch("mc_trk_finpos_z",     &fPartOutPoszMc );
   }
}

//__________________________________________________________
//! Reset items in tree out
void TAGactFlatTreeWriter::ResetTreeOut()
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
//! Fill tree Out
void TAGactFlatTreeWriter::FillTreeOut()
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
   
   fpTree->Fill();
}

//__________________________________________________________
//! Fill ST tree Out
void TAGactFlatTreeWriter::FillTreeOutSt()
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
void TAGactFlatTreeWriter::FillTreeOutBm()
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
void TAGactFlatTreeWriter::FillTreeOutVt()
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
void TAGactFlatTreeWriter::FillTreeOutIt()
{
   TAITntuCluster* itClus  = (TAITntuCluster*) fpNtuClusIt->Object();
   TAITparGeo*     pGeoMap = (TAITparGeo*)     fpGeoMapItr->Object();
   
   for (int iSensor = 0; iSensor < pGeoMap->GetSensorsN(); ++iSensor) {
      
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
void TAGactFlatTreeWriter::FillTreeOutMsd()
{
   TAMSDntuPoint* msPoint = (TAMSDntuPoint*) fpNtuRecMsd->Object();
   TAMSDparGeo*   pGeoMap = (TAMSDparGeo*)   fpGeoMapMsd->Object();

   for (int iStation = 0; iStation < pGeoMap->GetStationsN(); ++iStation) {
      
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
void TAGactFlatTreeWriter::FillTreeOutTw()
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
void TAGactFlatTreeWriter::FillTreeOutCa()
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
void TAGactFlatTreeWriter::FillTreeOutGlb()
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
void TAGactFlatTreeWriter::FillTreeOutMc()
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
