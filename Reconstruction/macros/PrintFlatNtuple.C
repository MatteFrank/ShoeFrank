#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>

#include "TAMCntuPart.hxx"
#include "TAMCntuHit.hxx"

#include "TASTparGeo.hxx"
#include "TASTntuHit.hxx"

#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TABMntuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAITparGeo.hxx"
#include "TAITntuCluster.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"

#include "TATWparGeo.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"

#include "TACAparGeo.hxx"
#include "TACAntuCluster.hxx"

#include "TAGntuGlbTrack.hxx"

#include "TAGgeoTrafo.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGnameManager.hxx"

#endif

static int IncludeMC;
static int IncludeDI;
static int IncludeSC;
static int IncludeBM;
static int IncludeTG;
static int IncludeVT;
static int IncludeIT;
static int IncludeMSD;
static int IncludeTW;
static int IncludeCA;

// main
void PrintFlatNtuple(TString expName = "12C_200", Int_t runNumber = 1, TString nameFile = "", Int_t nentries = 0)

{
	//  bool refittrack = false;
	// bool  printntuple = true;

	TAGroot gTAGroot;

	TAGcampaignManager *campManager = new TAGcampaignManager(expName);
	campManager->FromFile();

	IncludeMC = campManager->GetCampaignPar(campManager->GetCurrentCamNumber()).McFlag;
	cout << "Include MC: " << IncludeMC << endl;
	IncludeDI = campManager->IsDetectorOn("DI");
	IncludeSC = campManager->IsDetectorOn("ST");
	IncludeBM = campManager->IsDetectorOn("BM");
	IncludeTG = campManager->IsDetectorOn("TG");
	IncludeVT = campManager->IsDetectorOn("VT");
	IncludeIT = campManager->IsDetectorOn("IT");
	IncludeMSD = campManager->IsDetectorOn("MSD");
	IncludeTW = campManager->IsDetectorOn("TW");
	IncludeCA = campManager->IsDetectorOn("CA");

	//! it is fundamental to ask the include

	TAGgeoTrafo *geoTrafo = new TAGgeoTrafo();
	TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
	geoTrafo->FromFile(parFileName);

	// TAGparaDsc* parGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
	// TASTparGeo* stparGeo = (TASTparGeo*)parGeoSt->Object();
	// parFileName = campManager->GetCurGeoFile(TASTparGeo::GetBaseName(), runNumber);
	// stparGeo->FromFile(parFileName);

	// TAGparaDsc* parGeoBm = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
	// TABMparGeo* bmparGeo = (TABMparGeo*)parGeoBm->Object();
	// parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
	// bmparGeo->FromFile(parFileName);

	TAGparaDsc *parGeoVtx = new TAGparaDsc(new TAVTparGeo());
	// TAGparaDsc *parGeoVtx = new TAGparaDsc(FootParaDscName("TAVTparGeo"), new TAVTparGeo());
	TAVTparGeo *vtparGeo = (TAVTparGeo *)parGeoVtx->Object();
	parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
	vtparGeo->FromFile(parFileName);

	// TAGparaDsc* parGeoIt = new TAGparaDsc(TAITparGeo::GetDefParaName(), new TAITparGeo());
	// TAITparGeo* itparGeo = (TAITparGeo*)parGeoIt->Object();
	// parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
	// itparGeo->FromFile(parFileName);

	TAGparaDsc *parGeoMsd = new TAGparaDsc(new TAMSDparGeo());
	// TAGparaDsc* parGeoMsd = new TAGparaDsc(FootParaDscName("TAMSDparGeo"), new TAMSDparGeo());
	TAMSDparGeo *msdparGeo = (TAMSDparGeo *)parGeoMsd->Object();
	parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
	msdparGeo->FromFile(parFileName);

	TAGparaDsc *parGeoTw = new TAGparaDsc(new TATWparGeo());
	// TAGparaDsc *parGeoTw = new TAGparaDsc(FootParaDscName("TATWparGeo"), new TATWparGeo());
	TATWparGeo *twparGeo = (TATWparGeo *)parGeoTw->Object();
	parFileName = campManager->GetCurGeoFile(TATWparGeo::GetBaseName(), runNumber);
	twparGeo->FromFile(parFileName);

	TTree *tree = 0;
	TFile *f = new TFile(nameFile.Data());
	tree = (TTree *)f->Get("tree");

	// TASTntuHit *stHit = new TASTntuHit();
	// tree->SetBranchAddress(TASTntuHit::GetBranchName(), &stHit);
	// tree->SetBranchAddress("stNtu", &stHit);

	// TABMntuHit*  bmHit = new TABMntuHit();
	// tree->SetBranchAddress(TABMntuHit::GetBranchName(), &bmHit);

	// TABMntuTrack*  bmTrack = new TABMntuTrack();
	// tree->SetBranchAddress(TABMntuTrack::GetBranchName(), &bmTrack);

	TAVTntuCluster *vtClus = new TAVTntuCluster();
	vtClus->SetParGeo(vtparGeo);
	tree->SetBranchAddress(TAGnameManager::GetBranchName(vtClus->ClassName()), &vtClus);
	// tree->SetBranchAddress(FootBranchName("TAVTntuCluster"), &vtClus);
	// tree->SetBranchAddress(FootBranchName("TAVTntuCluster"), &vtClus);

	//    TAITntuCluster *itClus = new TAITntuCluster();
	//    tree->SetBranchAddress(TAITntuCluster::GetBranchName(), &itClus);

	TAMSDntuCluster *msdClus = new TAMSDntuCluster();
	tree->SetBranchAddress(TAGnameManager::GetBranchName(msdClus->ClassName()), &msdClus);
	// tree->SetBranchAddress(FootBranchName("TAMSDntuCluster"), &msdClus);

	TAMSDntuPoint *msPoint = new TAMSDntuPoint();
	tree->SetBranchAddress(TAGnameManager::GetBranchName(msPoint->ClassName()), &msPoint);
	// tree->SetBranchAddress(FootBranchName("TAMSDntuPoint"), &msdPoint);

	// TATW_ContainerPoint *twpoint = new TATW_ContainerPoint();
	// tree->SetBranchAddress(TATW_ContainerPoint::GetBranchName(), &twpoint);

	TATWntuPoint *twpoint = new TATWntuPoint;
	tree->SetBranchAddress(TAGnameManager::GetBranchName(twpoint->ClassName()), &twpoint);
	// tree->SetBranchAddress(FootBranchName("TATWntuPoint"), &twpoint);

	TACAntuCluster *caClus = new TACAntuCluster;
	tree->SetBranchAddress(TAGnameManager::GetBranchName(caClus->ClassName()), &caClus);

	TATWntuHit *twrh = new TATWntuHit;
	tree->SetBranchAddress(TAGnameManager::GetBranchName(twrh->ClassName()), &twrh);
	// tree->SetBranchAddress(FootBranchName("TATWntuHit"), &twhit);

	TAVTntuVertex *vtx = new TAVTntuVertex();
	tree->SetBranchAddress(TAGnameManager::GetBranchName(vtx->ClassName()), &vtx);
	// tree->SetBranchAddress(FootBranchName("TAVTntuVertex"), &vtx);

	// TAVTntuHit* vthit      = new TAVTntuHit();
	// tree->SetBranchAddress(TAVTntuHit::GetBranchName(), &vthit);

	TAVTntuTrack *vttrack = new TAVTntuTrack();
	tree->SetBranchAddress(TAGnameManager::GetBranchName(vttrack->ClassName()), &vttrack);
	// tree->SetBranchAddress(FootBranchName("TAVTntuTrack"), &vttrack);
	TAGdataDsc *track = new TAGdataDsc("vttrack", vttrack);

	TAMCntuPart *eve = new TAMCntuPart();
	if ((IncludeMC))
	{
		tree->SetBranchAddress(FootBranchName("TAMCntuPart"), &eve);
		// tree->SetBranchAddress("TAMCntuPart", &eve);
		//  tree->SetBranchAddress(TAMCntuPart::GetBranchName(), &eve);
	}
	// tree->SetBranchAddress(FootBranchName("TAMCntuPart"), &eve);

	// TAMCntuHit* mctwhit = new TAMCntuHit();
	// tree->SetBranchAddress(FootBranchMcName(kTW),&mctwhit);

	//    TAMCntuHit *vtMc = new TAMCntuHit();
	//    tree->SetBranchAddress(TAMCntuHit::GetVtxBranchName(), &vtMc);

	if (nentries == 0)
		nentries = tree->GetEntriesFast();
	// printf("Processed Events: %d\n", ev);

	// KFitter* m_kFitter = new KFitter();

	// vector<float> st_charge, st_time, st_pos_x, st_pos_y, st_pos_z;
	// vector<TVector3> st_pos;

	// vector<int> bm_trkind;
	// vector<TVector3> bm_Pvers, bm_R0;
	// vector<float> bm_Pvers_x, bm_Pvers_y, bm_Pvers_z;
	// vector<float> bm_R0_x, bm_R0_y, bm_R0_z, bmtrk_chi2;

	Int_t n_ev;
	vector<TVector3> vtx_coll;
	vector<float> vtx_x, vtx_y, vtx_z;

	// vector<double> mc_trk_charge, mc_trk_mass, mc_trk_tof, mc_trk_length;
	// vector<float> mc_trk_intpos_x,  mc_trk_intpos_y,  mc_trk_intpos_z;
	// vector<float> mc_trk_finpos_x,  mc_trk_finpos_y,  mc_trk_finpos_z;

	vector<int> trk_ind;
	vector<TVector3> trk_vtx_clus, trk_slopez, trk_origin, trk_projTW;
	vector<vector<TVector3>> trk_vtx_clus_2;
	vector<int> vt_trk_n, trk_vtx_clus_n, trk_vtx_clus_tothit;
	// if (IncludeMC)
	// {
	vector<int> trk_vtx_clus_MCId;
	// }
	vector<float> trk_vtx_clus_x, trk_vtx_clus_y, trk_vtx_clus_z, trk_chi2;

	vector<int> vtx_clus_n, vtx_clus_tothit;
	// if (IncludeMC){
	vector<int> vtx_clus_MCId;
	// }
	vector<float> vtx_clus_x, vtx_clus_y, vtx_clus_z;
	// if ((IncludeMC))
	// {
	vector<float> vtx_clus_MC;
	// }

	Int_t TWPoints = 0;
	vector<Int_t> TWChargePoint;
	// if (IncludeMC)
	// {
	vector<Int_t> TATW_MCID_1, TATW_MCID_2;
	// }
	vector<Double_t> TWDe1Point;
	vector<Double_t> TWDe2Point;
	vector<Double_t> TWXPoint;
	vector<Double_t> TWYPoint;
	vector<Double_t> TWZPoint;
	// vector<TVector3> TWhitX_pos;
	// vector<TVector3> TWhitY_pos;
	vector<Double_t> TWTOF, TWTOF1, TWTOF2;

	Int_t TWHit = 0;
	vector<Double_t> TWDeHit;
	vector<Double_t> TWbarHit;
	vector<Double_t> TWlayerHit;
	vector<Double_t> TWTOFHit;
	// if (IncludeMC)
	// {
	vector<Int_t> TW_McID_Hit;
	// }

	// vector<int> vtx_hit_line, vtx_hit_col, vtx_hit_frame;

	// vector<double> tw_eloss, tw_time, tw_pos, tw_chargeA, tw_chargeB, tw_timeA, tw_timeB, tw_chargeCOM;
	// vector<int> tw_bar, tw_layer;

	// Int_t nsthit;
	// Int_t nbmtrack;
	Int_t nvtx;
	Int_t nvttrack;
	Int_t nvtclus;
	Int_t nvthit;
	Int_t nCaClus;
	// Int_t mctrack;
	// Int_t ntwhit;

	// MSD
	vector<int> fStationIdMsd;		///< MSD station id
	vector<int> fPointsNmsd;		///< MSD point number
	vector<double> fEnergyLoss1Msd; ///< MSD point energy loss position
	vector<double> fEnergyLoss2Msd; ///< MSD point energy loss position
	vector<TVector3> fPosMsd;		///< MSD point position

	vector<int> fClusStationIdMsd;
	// if (IncludeMC)
	// {
	vector<int> fPointMsd_MCId;
	// } ///< MSD station id
	vector<int> fClusNmsd;
	// if (IncludeMC)
	// {
	vector<int> fClusMsd_MCId; ///< MSD point number
	// }
	vector<double> fClusEnergyLossMsd; ///< MSD point energy loss position
	vector<TVector3> fPosClusMsd;	   ///< MSD point position

	///////////////// MC generated particle characteristics
	// if (IncludeMC)
	// {
	vector<Int_t> MotherID;
	vector<Int_t> Dead_region;
	vector<Int_t> Generation_region;
	vector<Int_t> FlukaID;
	vector<Int_t> BaryonN;
	vector<Double32_t> Mass;
	vector<Int_t> Ptype;
	vector<Int_t> Charge;
	vector<Double_t> TOF;
	vector<Double_t> Track_Length;
	vector<TVector3> InitPos; ///< initial position
	vector<float> InitPos_x, InitPos_y, InitPos_z;
	vector<TVector3> FinalPos; ///< final position
	vector<float> FinalPos_x, FinalPos_y, FinalPos_z;
	vector<TVector3> InitMom; ///< initial momentum
	vector<float> InitMom_x, InitMom_y, InitMom_z;
	vector<TVector3> FinalMom;
	vector<float> FinalMom_x, FinalMom_y, FinalMom_z;
	// }
	////////////////////////////////////////////

	Int_t pos1 = nameFile.Last('.');
	TString nameOut = nameFile(0, pos1);
	// string nameOut = nameFile(0, pos1);
	//  cout << " name out 0 " << nameOut << endl;
	nameOut.Append("_ntu.root");

	// Int_t pos2 = nameOut.Last('/');
	// Int_t end = nameOut.Length();
	// nameOut = nameOut(pos2+1,end); //uncomment tu run on GSI file

	TFile *file_out = new TFile(nameOut, "RECREATE");
	TTree *tree_out = new TTree("OuTree", "Reco Event Tree");
	// tree_out->Branch("st_time",             &st_time);
	// tree_out->Branch("st_charge",           &st_charge);
	// tree_out->Branch("st_hit_n",            &nsthit   );
	// tree_out->Branch("st_pos_x",            &st_pos_x );
	// tree_out->Branch("st_pos_y",            &st_pos_y );
	// tree_out->Branch("st_pos_z",            &st_pos_z );
	// tree_out->Branch("bm_trk_n",            &nbmtrack );
	// tree_out->Branch("bm_trk_chi2",         &bmtrk_chi2 );
	// tree_out->Branch("bm_Pvers_x",          &bm_Pvers_x );
	// tree_out->Branch("bm_Pvers_y",          &bm_Pvers_y );
	// tree_out->Branch("bm_Pvers_z",          &bm_Pvers_z );
	// tree_out->Branch("bm_R0_x",             &bm_R0_x );
	// tree_out->Branch("bm_R0_y",             &bm_R0_y );
	// tree_out->Branch("bm_R0_z",             &bm_R0_z );
	tree_out->Branch("ev", &n_ev);
	tree_out->Branch("vertex_n", &nvtx);
	tree_out->Branch("vertex_x", &vtx_x);
	tree_out->Branch("vertex_y", &vtx_y);
	tree_out->Branch("vertex_z", &vtx_z);
	tree_out->Branch("vt_trk_n", &vt_trk_n);
	tree_out->Branch("vt_trk_chi2", &trk_chi2);
	tree_out->Branch("vt_trk_slopez", &trk_slopez);
	tree_out->Branch("vt_trk_origin", &trk_origin);
	tree_out->Branch("vt_trk_projTW", &trk_projTW);
	if (IncludeMC)
	{
		tree_out->Branch("vt_trk_clus_n", &trk_vtx_clus_n);
		tree_out->Branch("trk_vtx_clus_MCId", &trk_vtx_clus_MCId);
	}
	tree_out->Branch("vt_trk_clus_tot_hits", &trk_vtx_clus_tothit);
	tree_out->Branch("vt_trk_clus_x", &trk_vtx_clus_x);
	tree_out->Branch("vt_trk_clus_y", &trk_vtx_clus_y);
	tree_out->Branch("vt_trk_clus_z", &trk_vtx_clus_z);

	tree_out->Branch("vt_clus_n", &vtx_clus_n);
	if (IncludeMC)
	{
		tree_out->Branch("vtx_clus_MCId", &vtx_clus_MCId);
	}

	tree_out->Branch("vt_clus_tot_hits", &vtx_clus_tothit);
	tree_out->Branch("vt_clus_x", &vtx_clus_x);
	tree_out->Branch("vt_clus_y", &vtx_clus_y);
	tree_out->Branch("vt_clus_z", &vtx_clus_z);
	if (IncludeMC)
	{
		tree_out->Branch("vt_clus_MC", &vtx_clus_MC);
	}

	tree_out->Branch("msd_station_id", &fStationIdMsd);
	tree_out->Branch("msd_pt_n", &fPointsNmsd);
	if (IncludeMC)
	{
		tree_out->Branch("msd_pt_MCId", &fPointMsd_MCId);
	}
	tree_out->Branch("msd_eloss1", &fEnergyLoss1Msd);
	tree_out->Branch("msd_eloss2", &fEnergyLoss2Msd);
	tree_out->Branch("msd_pos", &fPosMsd);

	tree_out->Branch("msd_station_clus_id", &fClusStationIdMsd);
	tree_out->Branch("msd_clus_n", &fClusNmsd);
	if (IncludeMC)
	{
		tree_out->Branch("msd_clus_MCId", &fClusMsd_MCId);
	}
	tree_out->Branch("msd_eloss", &fClusEnergyLossMsd);
	tree_out->Branch("msd_clus_pos", &fPosClusMsd);

	tree_out->Branch("TWPoints", &TWPoints);
	tree_out->Branch("TWChargePoint", &TWChargePoint);

	if (IncludeMC)
	{
		tree_out->Branch("TATW_MCID_1", &TATW_MCID_1);
		tree_out->Branch("TATW_MCID_2", &TATW_MCID_2);
	}
	tree_out->Branch("TWDe1Point", &TWDe1Point);
	tree_out->Branch("TWDe2Point", &TWDe2Point);
	tree_out->Branch("TWXPoint", &TWXPoint);
	tree_out->Branch("TWYPoint", &TWYPoint);
	tree_out->Branch("TWZPoint", &TWZPoint);

	tree_out->Branch("TWHit", &TWHit);
	tree_out->Branch("TWDeHit", &TWDeHit);
	if (IncludeMC)
	{
		tree_out->Branch("TW_MCID_hit", &TW_McID_Hit);
	}
	tree_out->Branch("TWTOFHit", &TWTOFHit);
	tree_out->Branch("TWbarHit", &TWbarHit);
	tree_out->Branch("TWlayerHit", &TWlayerHit);

	// tree_out->Branch("TWhitX_pos", &TWhitX_pos);
	// tree_out->Branch("TWhitY_pos", &TWhitY_pos);

	tree_out->Branch("TWTOF", &TWTOF);
	tree_out->Branch("TWTOF1", &TWTOF1);
	tree_out->Branch("TWTOF2", &TWTOF2);

	// tree_out->Branch("vt_hit_sensor",       &nvthit );
	// tree_out->Branch("vt_hit_line",         &vtx_hit_line );
	// tree_out->Branch("vt_hit_col",          &vtx_hit_col );
	// tree_out->Branch("vt_hit_frame",        &vtx_hit_frame );
	// tree_out->Branch("vt_hit_sensor",       &vtx_hit_sensor );

	// tree_out->Branch("tw_time",             &tw_time);
	// tree_out->Branch("tw_eloss",            &tw_eloss);
	// tree_out->Branch("tw_hit_n",            &ntwhit   );
	// tree_out->Branch("tw_bar",              &tw_bar );
	// tree_out->Branch("tw_layer",            &tw_layer );
	// tree_out->Branch("tw_pos",              &tw_pos );
	// tree_out->Branch("tw_timeA",            &tw_timeA);
	// tree_out->Branch("tw_timeB",            &tw_timeB);
	// tree_out->Branch("tw_chargeA",          &tw_chargeA);
	// tree_out->Branch("tw_chargeB",          &tw_chargeB);
	// tree_out->Branch("tw_chargeCOM",        &tw_chargeCOM);
	// tree_out->Branch("mc_trk_n",            &mctrack );
	// tree_out->Branch("mc_trk_charge",       &mc_trk_charge );
	// tree_out->Branch("mc_trk_mass",         &mc_trk_mass );
	// tree_out->Branch("mc_trk_tof",          &mc_trk_tof );
	// tree_out->Branch("mc_trk_length",       &mc_trk_length );
	// tree_out->Branch("mc_trk_intpos_x",     &mc_trk_intpos_x );
	// tree_out->Branch("mc_trk_intpos_y",     &mc_trk_intpos_y );
	// tree_out->Branch("mc_trk_intpos_z",     &mc_trk_intpos_z );
	// tree_out->Branch("mc_trk_finpos_x",     &mc_trk_finpos_x );
	// tree_out->Branch("mc_trk_finpos_y",     &mc_trk_finpos_y );
	// tree_out->Branch("mc_trk_finpos_z",     &mc_trk_finpos_z );

	/////////////// MC generated particle branches
	///////////////////////////////////////////////////
	if (IncludeMC)
	{
		tree_out->Branch("MC_Dead_region", &Dead_region);
		tree_out->Branch("MC_Generation_region", &Generation_region);
		tree_out->Branch("MC_FlukaID", &FlukaID);
		tree_out->Branch("MC_MotherID", &MotherID);
		tree_out->Branch("MC_BaryonN", &BaryonN);
		tree_out->Branch("MC_Mass", &Mass);
		tree_out->Branch("MC_Ptype", &Ptype);
		tree_out->Branch("MC_Charge", &Charge);
		tree_out->Branch("MC_TOF", &TOF);
		tree_out->Branch("MC_Track_Length", &Track_Length);

		tree_out->Branch("MC_InitPos_x", &InitPos_x);
		tree_out->Branch("MC_InitPos_y", &InitPos_y);
		tree_out->Branch("MC_InitPos_z", &InitPos_z);

		tree_out->Branch("MC_FinalPos_x", &FinalPos_x);
		tree_out->Branch("MC_FianlPos_y", &FinalPos_y);
		tree_out->Branch("MC_FinalPos_z", &FinalPos_z);

		tree_out->Branch("MC_InitMom_x", &InitMom_x);
		tree_out->Branch("MC_InitMom_y", &InitMom_y);
		tree_out->Branch("MC_InitMom_z", &InitMom_z);

		tree_out->Branch("MC_FinalMom_x", &FinalMom_x);
		tree_out->Branch("MC_FinalMom_y", &FinalMom_y);
		tree_out->Branch("MC_FinalMom_z", &FinalMom_z);
	}
	////////////////////////////////////////////////////

	for (Int_t ev = 0; ev < nentries; ++ev)
	{
		if (ev % 10000 == 0)
			printf("Processed Events: %d\n", ev);
		n_ev = ev;

		////////////////////////////////////////////////////////////
		// MC generated particle characteristics !!!!!!
		if (IncludeMC)
		{
			Dead_region.clear();
			Generation_region.clear();
			FlukaID.clear();
			MotherID.clear();
			BaryonN.clear();
			Mass.clear();
			Ptype.clear();
			Charge.clear();
			TOF.clear();
			Track_Length.clear();
			InitPos.clear();
			InitPos_x.clear();
			InitPos_y.clear();
			InitPos_z.clear();

			FinalPos.clear();
			FinalPos_x.clear();
			FinalPos_y.clear();
			FinalPos_z.clear();

			InitMom.clear();
			InitMom_x.clear();
			InitMom_y.clear();
			InitMom_z.clear();

			FinalMom.clear();
			FinalMom_x.clear();
			FinalMom_y.clear();
			FinalMom_z.clear();
		}
		/////////////////////////////////////

		// stHit->Clear();
		// bmHit->Clear();
		// bmTrack->Clear();

		vtClus->Clear();
		// itClus->Clear();
		msdClus->Clear();
		msPoint->Clear();

		twrh->Clear();
		vttrack->Clear();
		vtx->Clear();
		// vtclus->Clear();
		//  vthit->Clear();

		// vtMc->Clear();
		// if (IncludeMC == 1){
		//   eve->Clear();
		// }

		TWPoints = -1;
		TWChargePoint.clear();
		TWDe1Point.clear();
		TWDe2Point.clear();
		TWXPoint.clear();
		TWYPoint.clear();
		TWZPoint.clear();
		if (IncludeMC)
		{
			TATW_MCID_1.clear();
			TATW_MCID_2.clear();
		}
		// TWhitX_pos.clear();
		// TWhitY_pos.clear();
		TWTOF.clear();
		TWTOF1.clear();
		TWTOF2.clear();
		TWHit = -1;
		TWDeHit.clear();
		TWbarHit.clear();
		TWlayerHit.clear();
		TWTOFHit.clear();
		if (IncludeMC)
		{
			TW_McID_Hit.clear();
		}

		tree->GetEntry(ev);

		// st_charge.clear();
		// st_time.clear();
		// st_pos.clear();
		// st_pos_x.clear();
		// st_pos_y.clear();
		// st_pos_z.clear();

		// bm_trkind.clear();
		// bmtrk_chi2.clear();
		// bm_Pvers.clear();
		// bm_R0.clear();
		// bm_Pvers_x.clear();
		// bm_Pvers_y.clear();
		// bm_Pvers_z.clear();
		// bm_R0_x.clear();
		// bm_R0_y.clear();
		// bm_R0_z.clear();

		vtx_coll.clear();
		vtx_x.clear();
		vtx_y.clear();
		vtx_z.clear();

		// mc_trk_charge.clear();
		// mc_trk_mass.clear();
		// mc_trk_tof.clear();
		// mc_trk_length.clear();
		// mc_trk_intpos_x.clear();
		// mc_trk_intpos_y.clear();
		// mc_trk_intpos_z.clear();
		// mc_trk_finpos_x.clear();
		// mc_trk_finpos_y.clear();
		// mc_trk_finpos_z.clear();

		// ! tutti i termini con "FlukaId" sono da mettere in un if se MC è acceso

		trk_ind.clear();
		trk_vtx_clus.clear();
		trk_vtx_clus_2.clear();
		vt_trk_n.clear();
		trk_vtx_clus_n.clear();
		trk_vtx_clus_tothit.clear();
		trk_chi2.clear();
		trk_slopez.clear();
		trk_origin.clear();
		trk_projTW.clear();

		if ((IncludeMC))
		{
			trk_vtx_clus_MCId.clear();
		}

		trk_vtx_clus_x.clear();
		trk_vtx_clus_y.clear();
		trk_vtx_clus_z.clear();

		vtx_clus_n.clear();
		vtx_clus_tothit.clear();

		if ((IncludeMC))
		{
			vtx_clus_MCId.clear();
			vtx_clus_MC.clear();
		}

		vtx_clus_x.clear();

		vtx_clus_y.clear();
		vtx_clus_z.clear();

		fStationIdMsd.clear();
		fPointsNmsd.clear();

		if ((IncludeMC))
		{
			fPointMsd_MCId.clear();
		}

		fEnergyLoss1Msd.clear();
		fEnergyLoss2Msd.clear();
		fPosMsd.clear();

		fClusStationIdMsd.clear();
		fClusNmsd.clear();
		if ((IncludeMC))
		{
			fClusMsd_MCId.clear();
		}
		fClusEnergyLossMsd.clear();
		fPosClusMsd.clear();

		// vtx_hit_line.clear();
		// vtx_hit_col.clear();
		// vtx_hit_frame.clear();
		// vtx_hit_sensor.clear();

		// tw_eloss.clear();
		// tw_time.clear();
		// tw_pos.clear();
		// tw_bar.clear();
		// tw_layer.clear();
		// tw_chargeA.clear();
		// tw_chargeB.clear();
		// tw_timeA.clear();
		// tw_timeB.clear();
		// tw_chargeCOM.clear();

		//////////////////////////////////////
		///////// MC generated particle characteristics !!!!!!
		if (IncludeMC)
		{
			for (Int_t i = 0; i < eve->GetTracksN(); ++i)
			{
				TAMCpart *particle = eve->GetTrack(i);
				Int_t reg = particle->GetRegion();
				Generation_region.push_back(reg);
				Int_t dead = particle->GetDead();
				Dead_region.push_back(dead);

				Int_t baryon = particle->GetBaryon();
				BaryonN.push_back(baryon);
				Int_t fluka = particle->GetFlukaID();
				FlukaID.push_back(fluka);
				Int_t mother = particle->GetMotherID();
				MotherID.push_back(mother);
				Double32_t m = particle->GetMass();
				Mass.push_back(m);
				Int_t type = particle->GetType();
				Ptype.push_back(type);
				Int_t z = particle->GetCharge();
				Charge.push_back(z);
				Double_t Tof = particle->GetTof(); // time of flight
				TOF.push_back(Tof);
				Double_t TrkLength = particle->GetTrkLength();
				Track_Length.push_back(TrkLength);

				TVector3 pos_in = particle->GetInitPos();
				float pos_in_x = pos_in.x();
				float pos_in_y = pos_in.y();
				float pos_in_z = pos_in.z();
				InitPos_x.push_back(pos_in_x);
				InitPos_y.push_back(pos_in_y);
				InitPos_z.push_back(pos_in_z);

				TVector3 pos_fin = particle->GetFinalPos();
				float pos_fin_x = pos_fin.x();
				float pos_fin_y = pos_fin.y();
				float pos_fin_z = pos_fin.z();
				FinalPos_x.push_back(pos_fin_x);
				FinalPos_y.push_back(pos_fin_y);
				FinalPos_z.push_back(pos_fin_z);

				TVector3 mom_in = particle->GetInitP();
				float mom_in_x = mom_in.x();
				float mom_in_y = mom_in.y();
				float mom_in_z = mom_in.z();
				InitMom_x.push_back(mom_in_x);
				InitMom_y.push_back(mom_in_y);
				InitMom_z.push_back(mom_in_z);

				TVector3 mom_fin = particle->GetFinalP();
				float mom_fin_x = mom_fin.x();
				float mom_fin_y = mom_fin.y();
				float mom_fin_z = mom_fin.z();
				FinalMom_x.push_back(mom_fin_x);
				FinalMom_y.push_back(mom_fin_y);
				FinalMom_z.push_back(mom_fin_z);
			}
		}
		Int_t nPlanes = vtparGeo->GetSensorsN();
		// cout << "ev :"<< ev << endl;
		// cout << " n° of planes of vt clusters:  " <<   nPlanes  << endl;

		for (Int_t iPlane = 0; iPlane < nPlanes; iPlane++)
		{

			Int_t nclus = vtClus->GetClustersN(iPlane);
			vtx_clus_n.push_back(nclus);

			// totClus += nclus;
			// cout << "plane: " << iPlane <<endl;
			// cout << " n° of clus: " << nclus << endl;
			if (nclus == 0)
				continue;

			for (Int_t iClus = 0; iClus < nclus; ++iClus)
			{
				TAVTcluster *clus = vtClus->GetCluster(iPlane, iClus);
				// cout << "MCParticle Id" << clus->GetMcTrackIdx(0) <<endl;
				TAMCpart *track;
				if ((IncludeMC))
				{
					track = eve->GetTrack(clus->GetMcTrackIdx(0));
					// cout << "Z: "<< track->GetCharge() << " Mid: " << track->GetMotherID() << " posZ: " << track->GetInitPos().Z() << " momentum: " <<track->GetInitP().Mag() << endl;
				}

				if (!clus->IsValid())
					continue;

				if ((IncludeMC))
				{
					vtx_clus_MC.push_back(clus->GetMcTrackIdx(0));	// pos 0: MC ID
					vtx_clus_MC.push_back(track->GetCharge());		// pos 1: MC charge
					vtx_clus_MC.push_back(track->GetMotherID());	// pos 2: MID
					vtx_clus_MC.push_back(track->GetInitPos().Z()); // pos 3: init pos Z
					vtx_clus_MC.push_back(track->GetInitP().Mag()); // pos 4: init momentum

					vtx_clus_MCId.push_back(clus->GetMcTrackIdx(0));
				}
				TVector3 pos = clus->GetPositionG();
				pos = geoTrafo->FromVTLocalToGlobal(pos);

				vtx_clus_x.push_back(pos.X());
				vtx_clus_y.push_back(pos.Y());
				vtx_clus_z.push_back(pos.Z());

				Int_t nHits = clus->GetPixelsN();
				vtx_clus_tothit.push_back(nHits);
			}
		}

		// Int_t       nstHits  = stHit->GetHitsN();
		// cout << "n  sthits  " << nstHits << endl;
		//  //hits

		// double time = stHit->GetTriggerTime();
		// double charge = stHit->GetCharge();
		// //cout << "timest  " << timetest << endl;
		// st_charge.push_back(charge);
		// st_time.push_back(time);
		// for (Int_t i = 0; i < nstHits; i++) {

		//   TASThit* hit = stHit->GetHit(i);
		//   Float_t charge  = hit->GetCharge();
		//   Float_t time    = hit->GetTime();

		//   TVector3 posHit(0,0,0); // center

		//   TVector3 posHitG = geoTrafo->FromSTLocalToGlobal(posHit);

		//   //  cout << " time  " << time << endl;
		//   // cout << " charge  " << charge << endl;
		//   // cout << " posHitG x " <<  posHitG.X() << endl;
		//   // cout << " posHitG y " <<  posHitG.Y() << endl;
		//   // cout << " posHitG z " <<  posHitG.Z() << endl;

		//   // st_charge.push_back(charge);
		//   // st_time.push_back(time);
		//   st_pos.push_back(posHitG);
		// }

		// Int_t       nbmHits  = bmHit->GetHitsN();

		// // cout << " nbmHits   " << nbmHits  << endl;
		// // //hits
		// for (Int_t i = 0; i < nbmHits; i++) {
		//   TABMhit* hit = bmHit->GetHit(i);

		//   Int_t view  = hit->GetView();
		//   Int_t lay   = hit->GetPlane();
		//   Int_t cell  = hit->GetCell();

		//   Float_t x = bmparGeo->GetWireX(bmparGeo->GetSenseId(cell),lay,view);
		//   Float_t y = bmparGeo->GetWireY(bmparGeo->GetSenseId(cell),lay,view);
		//   Float_t z = bmparGeo->GetWireZ(bmparGeo->GetSenseId(cell),lay,view);

		//   TVector3 posHit(x, y, z);

		//   // cout << "poshit0 " << posHit.X()  << endl;
		//   // cout << "poshit1 " << posHit.Y()  << endl;
		//   // cout << "poshit2 " << posHit.Z()  << endl;

		// }

		/// Track in BM
		// int  Nbmtrack = bmTrack->GetTracksN();
		// // cout << " ntrack  BM  " << Nbmtrack  << endl;
		// if( bmTrack->GetTracksN() > 0 ) {
		//   for( Int_t iTrack = 0; iTrack < bmTrack->GetTracksN(); ++iTrack ) {

		//     bm_trkind.push_back(iTrack);

		//     TABMtrack* track = bmTrack->GetTrack(iTrack);

		//     Int_t nHits = track->GetHitsNtot();
		//     // cout << " nHits  BM  " <<  nHits << endl;

		//     TVector3 Pvers = track->GetSlope();  //direction of the track from mylar1_pos to mylar2_pos
		//     TVector3 R0 = track->GetOrigin();              //position of the track on the xy plane at z=0

		//     Double_t mychi2 = track->GetChiSquare();
		//     //      cout << " mychi2Red   " << mychi2  << endl;

		//     bmtrk_chi2.push_back(mychi2);

		//     bm_Pvers.push_back(Pvers);
		//     bm_R0.push_back(R0);

		//   } // end loop on tracks

		// } // nTracks > 0

		//  ///Vertex collection
		TAVTvertex *vtxPD = 0x0; // NEW
		TVector3 vtxPositionPD = TVector3(0., 0., 0.);
		// cout << " vtx->GetVertexN() " << vtx->GetVertexN() <<endl;

		for (Int_t iVtx = 0; iVtx < vtx->GetVertexN(); ++iVtx)
		{
			vtxPD = vtx->GetVertex(iVtx);

			if (vtxPD == 0x0)
				continue;
			vtxPositionPD = vtxPD->GetPosition();

			// cout << " vtxPositionPD  local " << vtxPositionPD[2]  << endl;
			vtxPositionPD = geoTrafo->FromVTLocalToGlobal(vtxPositionPD);
			// cout << " vtxPositionPD  global " << vtxPositionPD[2]  << endl;

			vtx_coll.push_back(vtxPositionPD);
			vt_trk_n.push_back(vtxPD->GetTracksN());

			for (Int_t iTrack = 0; iTrack < vtxPD->GetTracksN(); ++iTrack)
			{
				TAVTtrack *track = vtxPD->GetTrack(iTrack);
				trk_ind.push_back(iTrack);
				Float_t Chi2 = track->GetChi2();
				trk_chi2.push_back(Chi2);

				TVector3 slopez = track->GetSlopeZ();
				trk_slopez.push_back(slopez);
				TVector3 origin = track->GetOrigin();
				trk_origin.push_back(origin);

				// projection of vtx track to TW
				Float_t posZtw = geoTrafo->FromTWLocalToGlobal(TVector3(0, 0, 0)).Z();
				posZtw = geoTrafo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();
				TVector3 A3 = track->Intersection(posZtw);
				TVector3 A4 = geoTrafo->FromVTLocalToGlobal(A3);
				trk_projTW.push_back(A4);

				Float_t nCluster = track->GetClustersN();
				// cout << " n cluster of vertex tracks " <<  nCluster <<endl;
				Int_t TotHits = 0;
				for (Int_t iclus = 0; iclus < nCluster; ++iclus)
				{
					TAVTcluster *clus = (TAVTcluster *)track->GetCluster(iclus);
					TVector3 p_clus = clus->GetPositionG();
					if ((IncludeMC))
					{
						Int_t clus_MCId = clus->GetMcTracksN();
						trk_vtx_clus_MCId.push_back(clus_MCId);
					}
					Int_t nHits = clus->GetPixelsN();
					// cout << " n  nHits   " <<   nHits <<endl;
					TotHits += nHits;

					// for (Int_t j = 0; j < nHits; ++j) {
					//   TAVThit* hit = clus->GetPixel(j);
					//   for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
					//     Int_t id = hit->GetMcTrackIdx(k);
					//     Int_t idx = hit->GetMcIndex(k);
					//     cout << " TrackMcIdx  " <<   id   <<endl;
					//     cout << "  McIndex   " <<   idx    <<endl;
					//     // printf("TrackMcId %d ", id);
					//     // printf("McIndex   %d ", idx);
					//     TAMCpart* track = eve->GetHit(id);
					//     printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
					//     // TAMChit* mcHit = vtMc->GetHit(idx);
					//     // TVector3 pos = mcHit->GetPosition();
					//     // printf("MC pos (%.4f %.4f %.4f)\n", pos[0], pos[1], pos[2]);

					//   }
					// }

					// cout << " p_clus.z() local   i  " << iclus <<  "  z  " << p_clus.z()<< endl;
					p_clus = geoTrafo->FromVTLocalToGlobal(p_clus);
					// cout << " p_clus.z() global  i  " << iclus <<  "  z  " << p_clus.z()<< endl;

					trk_vtx_clus.push_back(p_clus);
					trk_vtx_clus_tothit.push_back(nHits); //! changed for every cluster
				}
				// cout << " TotHits   " <<   TotHits  <<endl;

				trk_vtx_clus_2.push_back(trk_vtx_clus);
				trk_vtx_clus.clear();
			}
		}

		// cout << " vtx->GetVertexN() " << vtx->GetVertexN() <<endl;
		nvthit = 0;

		// for (Int_t iSensor = 0; iSensor < 4 ; iSensor++){
		//   nvthit += vtxhit->GetPixelsN(iSensor);
		//   for (Int_t iVtxHit = 0; iVtxHit < vtxhit->GetPixelsN(iSensor); ++iVtxHit) {
		//     TAVThit* pixel = vtxhit->GetPixel(iSensor, iVtx);
		//     if (pixel == 0x0) continue;
		//     for (Int_t iFrame = 0; iFrame < 3; iFrame++){
		//       if (pixel->GetFrameON()[iFrame] == true){
		//         vtx_hit_line.push_back(pixel->GetPixelLine());
		//         vtx_hit_col.push_back(pixel->GetPixelColumn());
		//         vtx_hit_frame.push_back(iFrame);
		//         vtx_hit_sensor.push_back(iSensor);

		//       }

		//     }

		//   }
		// }

		TWHit = twrh->GetHitN();
		if (twrh->GetHitN() > 0)
		{
			for (int i = 0; i < twrh->GetHitN(); i++)
			{

				TATWhit *hit = twrh->Hit(i);
				Double_t eloss = hit->GetEnergyLoss();
				Double_t tof = hit->GetToF();
				Int_t bar = hit->GetBar();
				Int_t layer = hit->GetLayer();

				// posG = fpFootGeo->FromTWLocalToGlobal(posG);

				TWbarHit.push_back(bar);
				TWlayerHit.push_back(layer);
				TWDeHit.push_back(eloss);
				TWTOFHit.push_back(tof);
				if (IncludeMC)
				{
					TW_McID_Hit.push_back(hit->GetMcTrackIdx(0));
				}
			}
		}
		// cout << " posG  x  "<<  posG.X()  << endl;
		// cout << " posG  y  "<<  posG.Y()  << endl;
		// cout << " posG  z  "<<  posG.Z()  << endl;

		//     Float_t edep1  = point->GetEnergyLoss1();
		//     Float_t edep2  = point->GetEnergyLoss2();
		//     Float_t edep   = point->GetEnergyLoss();
		//     Float_t timeTW = point->GetTime();

		//   // cout << " bar    "<<  bar  << endl;
		//   // cout << " layer  "<< layer   << endl;

		//   Double_t eloss = hit->GetEnergyLoss();
		//   Double_t twtime = hit->GetTime();
		//   Double_t pos = hit->GetPosition();
		//   TVector3 postw(0,0,0);
		//   TVector3 postwglob = geoTrafo->FromTWLocalToGlobal(postw);

		//   // cout << " eloss    "<<  eloss  << endl;
		//   // cout << " timeTW  "<< twtime   << endl;
		//   // cout << " pos  "<< pos   << endl;

		//   // cout << " postwglob x   "<<  postwglob.x()  << endl;
		//   // cout << " postwglob y   "<<  postwglob.y()  << endl;
		//   // cout << " postwglob z   "<<  postwglob.z()  << endl;

		//   Double_t  chargeA = hit->GetChargeChA();
		//   Double_t  chargeB = hit->GetChargeChB();
		//   Double_t  timeA = hit->GetChargeTimeA();
		//   Double_t  timeB = hit->GetChargeTimeB();
		//   Double_t  chargeCOM = hit->GetCOM();

		//   //	cout << " chargeA    "<< chargeA   << endl;
		//   // cout << " chargeB  "<< chargeB  << endl;
		//   // cout << " timeA  "<< timeA   << endl;
		//   // cout << " timeB  "<< timeB   << endl;

		//   // Float_t posdetframe = hit->GetHitCoordinate_detectorFrame();
		//   // Float_t poszdetframe = hit->GetHitZ_detectorFrame();

		//   tw_eloss.push_back(eloss);
		//   tw_time.push_back(twtime);
		//   tw_pos.push_back(pos);
		//   tw_bar.push_back(bar);
		//   tw_layer.push_back(layer);
		//   tw_chargeA.push_back(chargeA);
		//   tw_timeA.push_back(timeA);
		//   tw_chargeB.push_back(chargeB);
		//   tw_timeB.push_back(timeB);
		//   tw_chargeCOM.push_back(chargeCOM);

		// }

		// nsthit = st_pos.size();

		// for (Int_t i = 0; i < nsthit; i++) {
		//   st_pos_x.push_back(st_pos.at(i).X());
		//   st_pos_y.push_back(st_pos.at(i).Y());
		//   st_pos_z.push_back(st_pos.at(i).Z());

		// }

		// nbmtrack = bm_trkind.size();

		// for (Int_t i = 0; i < nbmtrack; i++) {
		//   bm_Pvers_x.push_back(bm_Pvers.at(i).X());
		//   bm_Pvers_y.push_back(bm_Pvers.at(i).Y());
		//   bm_Pvers_z.push_back(bm_Pvers.at(i).Z());

		//   bm_R0_x.push_back(bm_R0.at(i).X());
		//   bm_R0_y.push_back(bm_R0.at(i).Y());
		//   bm_R0_z.push_back(bm_R0.at(i).Z());

		// }

		// filling

		nvtx = vtx_coll.size();
		for (Int_t i = 0; i < nvtx; i++)
		{
			vtx_x.push_back(vtx_coll.at(i).X());
			vtx_y.push_back(vtx_coll.at(i).Y());
			vtx_z.push_back(vtx_coll.at(i).Z());
		}

		// // cout<<  "ntrack   " << trk_ind.size() << endl;
		nvttrack = trk_ind.size();
		for (Int_t i = 0; i < nvttrack; i++)
		{

			trk_vtx_clus_n.push_back(trk_vtx_clus_2.at(i).size());
			// cout<<  "trk_vtx_clus_n " << trk_vtx_clus_n.at(i)  << endl;

			for (Int_t j = 0; j < trk_vtx_clus_n.at(i); j++)
			{

				// cout << "j " << j << endl;
				// cout << " trk_vtx_clus_2.at(i).at(j).X()   " << trk_vtx_clus_2.at(i).at(j).X() << endl;
				trk_vtx_clus_x.push_back(trk_vtx_clus_2.at(i).at(j).X());
				trk_vtx_clus_y.push_back(trk_vtx_clus_2.at(i).at(j).Y());
				trk_vtx_clus_z.push_back(trk_vtx_clus_2.at(i).at(j).Z());
			}
		}

		Int_t nPoint = twpoint->GetPointsN();
		TWPoints = nPoint;
		//   cout<<"nPoints::"<<nPoint<<endl;

		for (int ipt = 0; ipt < nPoint; ipt++)
		{

			TATWpoint *ptr = twpoint->GetPoint(ipt);

			TATWhit *hitX = ptr->GetRowHit();
			TATWhit *hitY = ptr->GetColumnHit();

			if (IncludeMC)
			{
				TATW_MCID_1.push_back(hitX->GetMcTrackIdx(0));
				TATW_MCID_2.push_back(hitY->GetMcTrackIdx(0));
			}
			//     cout<<"time2::"<<ptr->GetTof2()<<"  time1::"<<ptr->GetTof1()<<endl;
			//     cout<<"tof2::"<<hitY->GetToF()<<"  tof1::"<<hitX->GetToF()<<endl;

			Int_t Z = ptr->GetChargeZ();
			TWChargePoint.push_back(Z);
			TWDe1Point.push_back(ptr->GetEnergyLoss1());
			TWDe2Point.push_back(ptr->GetEnergyLoss2());

			TVector3 posTW_glb = ptr->GetPositionG();
			TVector3 poshitX_glb(0., hitX->GetPosition(), hitX->GetHitZ_detectorFrame());
			TVector3 poshitY_glb(hitY->GetPosition(), 0., hitY->GetHitZ_detectorFrame());

			posTW_glb = geoTrafo->FromTWLocalToGlobal(posTW_glb);
			poshitX_glb = geoTrafo->FromTWLocalToGlobal(poshitX_glb);
			poshitY_glb = geoTrafo->FromTWLocalToGlobal(poshitY_glb);

			// cout<<"Z::"<<Z<<"  x::"<<posTW_glb.x()<<"  y::"<<posTW_glb.y()<<"  z::"<<posTW_glb.z()<<endl;

			// TVector3 posTG_fromBM_glb = mapBMprojOnTG[ev];
			TWXPoint.push_back(posTW_glb.X());
			TWYPoint.push_back(posTW_glb.Y());
			TWZPoint.push_back(posTW_glb.Z());
			TWTOF.push_back(ptr->GetToF());
			TWTOF1.push_back(ptr->GetTof1());
			TWTOF2.push_back(ptr->GetTof2());

			// TWhitX_pos.push_back(poshitX_glb);
			// TWhitY_pos.push_back(poshitY_glb);
		}
		// ntwhit = tw_pos.size();

		// MSD Point collection

		for (int iStation = 0; iStation < msdparGeo->GetStationsN(); ++iStation)
		{
			Int_t nPoint = msPoint->GetPointsN(iStation);
			fPointsNmsd.push_back(nPoint);
			fStationIdMsd.push_back(iStation);

			for (int i = 0; i < nPoint; i++)
			{

				TAMSDpoint *point = msPoint->GetPoint(iStation, i);
				TVector3 posG = point->GetPositionG();
				posG = geoTrafo->FromMSDLocalToGlobal(posG);

				Int_t MCiD = point->GetMcTrackIdx(0); //! it is the ID only of the first cluster

				Float_t edep1 = point->GetEnergyLoss1();
				Float_t edep2 = point->GetEnergyLoss2();

				fEnergyLoss1Msd.push_back(edep1);
				fEnergyLoss2Msd.push_back(edep2);
				fPosMsd.push_back(posG);

				if (IncludeMC)
				{
					fPointMsd_MCId.push_back(MCiD);
				}
			}
		}

		// MSD Cluster collection
		// cout << "strips: " << msdparGeo->GetStripsN() << endl;
		for (int iSensor = 0; iSensor < (msdparGeo->GetStationsN() * 2); ++iSensor)
		{
			Int_t nClus = msdClus->GetClustersN(iSensor);
			fClusNmsd.push_back(nClus);
			fClusStationIdMsd.push_back(iSensor);
			for (int i = 0; i < nClus; i++)
			{

				TAMSDcluster *clus = msdClus->GetCluster(iSensor, i);
				TVector3 posG = clus->GetPositionG();
				posG = geoTrafo->FromMSDLocalToGlobal(posG);
				Float_t edep = clus->GetEnergyLoss();
				fClusEnergyLossMsd.push_back(edep);
				fPosClusMsd.push_back(posG);

				if (IncludeMC)
				{
					fClusMsd_MCId.push_back(clus->GetMcTrackIdx(0));
				}
			}
		}

		tree_out->Fill();

	} // Loop on event

	file_out->Write();
	file_out->Close();
}
