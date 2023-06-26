/*!
 \file TAVTactBaseNtuTrack.cxx
 \brief   Base class to NTuplize VTX tracks
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVector2.h"

#include "TAMCparTools.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TAGnameManager.hxx"

//BM
#include "TABMntuTrack.hxx"

// First
#include "TAGgeoTrafo.hxx"

//TAG
#include "TAGroot.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparCal.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseNtuTrack.hxx"
#include <algorithm>
#include <TStopwatch.h>

/*!
 \class TAVTactBaseNtuTrack
 \brief Base class to NTuplize VTX tracks
 */

//! Class Imp
ClassImp(TAVTactBaseNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus  cluster container descriptor
//! \param[out] pNtuTrack  track container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pCalib calibration parameter descriptor
//! \param[in] pBMntuTrack input BM track container descriptor
TAVTactBaseNtuTrack::TAVTactBaseNtuTrack(const char* name,
										 TAGdataDsc* pNtuClus,TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig,
										 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAVTactBaseTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib),
  fpBMntuTrack(pBMntuTrack),
  fBmTrackOk(false),
  fBmTrack(0x0),
  flagMC(false),
  fBmTrackPos(0,0,0)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuTrack::~TAVTactBaseNtuTrack()
{
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuTrack::CreateHistogram()
{
   TAVTactBaseTrack::CreateHistogram();

   TAVTbaseParGeo* pGeoMap  = GetParGeo();

   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisPixel[i] = new TH1F(Form("%sTrackedClusPix%d", fPrefix.Data(), i+1), Form("%s - # pixels per tracked clusters of sensor %d", fTitleDev.Data(), i+1), 100, -0.5, 99.5);
      AddHistogram(fpHisPixel[i]);
   }

   fpHisPixelTot = new TH1F(Form("%sTrackedClusPixTot", fPrefix.Data()), Form("%s - Total # pixels per tracked clusters", fTitleDev.Data()), 100, -0.5, 99.5);
   AddHistogram(fpHisPixelTot);

   fpHisClusLeftPix = new TH2F(Form("%sClusLeftPix", fPrefix.Data()), Form("%s - Number of pixels for left clusters vs sensor", fTitleDev.Data()),
                               10, 0, 9, 50, 1, 50);
   fpHisClusLeftPix->SetStats(kFALSE);
   AddHistogram(fpHisClusLeftPix);

   // TG
   fpHiVtxTgResX = new TH1F(Form("%sTgResX", fPrefix.Data()), Form("%s - Resolution at target in X", fTitleDev.Data()), 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResX);

   fpHiVtxTgResY = new TH1F(Form("%sTgResY", fPrefix.Data()), Form("%s - Resolution at target in Y", fTitleDev.Data()), 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResY);

   // BM
   fpHisBmBeamProf = new TH2F(Form("%sbmBeamProf", fPrefix.Data()), Form("%s - BM Beam Profile", fTitleDev.Data()),
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2.,
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2.);
   fpHisBmBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBmBeamProf);

   fpHisVtxResX = new TH2F(Form("%sBmResX", fPrefix.Data()), Form("%s - Vertex position correlationX BM/VT", fTitleDev.Data()), 200, -5, 5, 200, -5, 5);
   AddHistogram(fpHisVtxResX);

   fpHisVtxResY = new TH2F(Form("%sBmResY", fPrefix.Data()), Form("%s - Vertex position correlationY BM/VT", fTitleDev.Data()), 200, -5, 5, 200, -5, 5);
   AddHistogram(fpHisVtxResY);

   fpHisBmChi2 = new TH1F(Form("%sbmChi2", fPrefix.Data()), Form("%s - BM Chi2 of tracks", fTitleDev.Data()), 200, 0, 1000);
   AddHistogram(fpHisBmChi2);

   fpHisClusLeft = new TH1F(Form("%sClusLeft", fPrefix.Data()), Form("%s - Clusters left per sensor", fTitleDev.Data()), 8, 1, 8);
   AddHistogram(fpHisClusLeft);

   if (GetFlagMC()){
      cout << "entro nei plot" << endl;
      Int_t primary_charge = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber;
      fpHisTrackMultiplicity_frag = new TH1F(Form("%sTrackMultiplicity_frag", fPrefix.Data()), Form("%s - Track multiplicity of clusters when there if fragmentation (MC)", fTitleDev.Data()), 10, -0.5, 9.5);
      AddHistogram(fpHisTrackMultiplicity_frag );
      fpHisTrackMultiplicity_primary  = new TH1F(Form("%sTrackMultiplicity_primary", fPrefix.Data()), Form("%s - Track multiplicity of clusters if no fragmentation (MC)", fTitleDev.Data()), 10, -0.5, 9.5);
      AddHistogram(fpHisTrackMultiplicity_primary);
      fpReconstructedTracks = new TH1F(Form("%sReconstructedTracks", fPrefix.Data()), "Counts of Reconstructed tracks; Z; events;", primary_charge, 0.5, primary_charge+0.5);
      AddHistogram(fpReconstructedTracks);
      fpTrackEfficiency = new TH1F(Form("%sTrackEfficiency", fPrefix.Data()), "Efficiency of reconstructed tracks; Z; eff;", primary_charge, 0.5, primary_charge+0.5);
      AddHistogram(fpTrackEfficiency);
      fpTrackEfficiencyFake = new TH1F(Form("%sTrackEfficiencyFake", fPrefix.Data()), "Efficiency of reconstructed tracks out of the selected ones; Z; eff;", primary_charge, 0.5, primary_charge+0.5);
      AddHistogram(fpTrackEfficiencyFake);
      fpTrackPurityTrack = new TH1F(Form("%sTrackPurityTrack", fPrefix.Data()), "Purity of reconstructed tracks out of the selected ones; Z; purity;", primary_charge, 0.5, primary_charge+0.5);
      AddHistogram(fpTrackPurityTrack);

          fpTrackPurity = new TH1F(Form("%sTrackPurityCluster", fPrefix.Data()), "Purity of clusters in track reconstruction; Z; purity;", primary_charge, 0.5, primary_charge + 0.5);
      AddHistogram(fpTrackPurity);
      fpTrackClones = new TH1F(Form("%sTrackClones", fPrefix.Data()), "Track clones among all the reconstructed; Z; eff;", primary_charge, 0.5, primary_charge + 0.5);
      AddHistogram(fpTrackClones);

      fpBadTrackPosZ = new TH1F(Form("%sBadTrackPosZ", fPrefix.Data()), "Origin in Z axis of bad tracks; posizion Z; events;", 1600, -500, 300);
      AddHistogram(fpBadTrackPosZ);
      fpBadTrackCharge = new TH1F(Form("%sBadTrackCharge", fPrefix.Data()), "Charge of the bad track particle; Z; events;", primary_charge, 0.5, primary_charge + 0.5);
      AddHistogram(fpBadTrackCharge);
      fpBadTrackFlukaID = new TH1F(Form("%sBadTrackFlukaID", fPrefix.Data()), "Fluka Id of the bad track particle; id; events;", 35, -10+0.5, 25 + 0.5);
      AddHistogram(fpBadTrackFlukaID);
      fpBadTrackMothID = new TH1F(Form("%sBadTrackMothID", fPrefix.Data()), "MOther Id of the bad track particle; id; events;", 22, -2+ 0.5, 20 + 0.5);
      AddHistogram(fpBadTrackMothID);
      fpBadTrackInitP = new TH1F(Form("%sBadTrackInitP", fPrefix.Data()), "Magnitude of momentum of bad track particle; p (mag); events;", 1200, 0, 400);
      AddHistogram(fpBadTrackInitP);
            
            
      //angular resolution and efficiency,purity
      for (Int_t i = 0; i < primary_charge; ++i)
      {
         fpHisThetaRes[i] = new TH1F(Form("%sTrackThetaRes%d", fPrefix.Data(), i + 1), Form("%s - theta track resolution for Z= %d", fTitleDev.Data(), i + 1), 200, -1, 1);
         AddHistogram(fpHisThetaRes[i]);
         fpHisPhiRes[i] = new TH1F(Form("%sTrackPhiRes%d", fPrefix.Data(), i + 1), Form("%s - phi track resolution for Z= %d", fTitleDev.Data(), i + 1), 20000, -10, 10);
         AddHistogram(fpHisPhiRes[i]);

         fpTrackAngularEfficiency[i] = new TH1F(Form("%sTrackAngularEfficiencyZ%d", fPrefix.Data(), i + 1), Form("Angular Efficiency of reconstructed tracks for Z= %d; Angle [deg]; eff;", i + 1), 41, -0.5, 40.5);
      AddHistogram(fpTrackAngularEfficiency[i]);

      fpTrackAngularPurityTrack[i] = new TH1F(Form("%sTrackAngularPurityTrackZ%d", fPrefix.Data(), i + 1), Form("Angular Purity of reconstructed tracks out of the selected ones for Z = %d; Angle [deg]; purity;", i + 1), 41, -0.5, 40.5);
      AddHistogram(fpTrackAngularPurityTrack[i]);
      }

      //position res of the clusters
      for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i)
      {
         fpTrackClusResX[i] = new TH1F(Form("%sTrackedClusResX%d", fPrefix.Data(), i + 1), Form("%s -Position X resolution of the claster of sensor %d; X [cm]; ;", fTitleDev.Data(), i + 1), 1000, -0.01, 0.01);
         AddHistogram(fpTrackClusResX[i]);
         fpTrackClusResY[i] = new TH1F(Form("%sTrackedClusResY%d", fPrefix.Data(), i + 1), Form("%s -Position  Y resolution of the claster of sensor %d; Y [cm];;", fTitleDev.Data(), i + 1), 1000, -0.01, 0.01);
         AddHistogram(fpTrackClusResY[i]);
      }
      

   }

   SetValidHistogram(kTRUE);
   return;
}

//_____________________________________________________________________________
//! Action
Bool_t TAVTactBaseNtuTrack::Action()
{
   // BM tracks
   if (fpBMntuTrack)
	  CheckBM();


   // VTX
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();

   // TStopwatch watch;
   // watch.Start();
   //FindTiltedTracks();


   //looking straight
   FindStraightTracks();

   //looking inclined line
       if (!FindTiltedTracks())
   { // it is never false with TAVTactNtuTrackF::FindTiltedTracks()
     if (ValidHistogram())
		 FillHistogramm();
	  fpNtuTrack->SetBit(kValid);
	  return true;
   }

   if (FindVertices()){    //it is always false
	  FindTiltedTracks();
   }

   // if (pNtuTrack->GetTracksN() >2)
   //   cout << watch.RealTime() << endl;

   if(FootDebugLevel(1)) {
	  printf("%s %d tracks found\n", this->GetName(), pNtuTrack->GetTracksN()); //print name of action since it's used for all trackers
	  for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
		 TAVTtrack* track = pNtuTrack->GetTrack(i);
		 printf("   with # clusters %d\n", track->GetClustersN());
	  }
   }

   if ((GetFlagMC() && TAGrecoManager::GetPar()->IsRegionMc()))
     EvaluateTrack(); // study of the purity of the reconstructed tracks

   if (ValidHistogram())
	  FillHistogramm();

   // Set charge probability
   SetChargeProba();

   fpNtuTrack->SetBit(kValid);
   return true;
}


//_____________________________________________________________________________
//! Check BM tracks
void TAVTactBaseNtuTrack::CheckBM()
{
   // BM info
   TAVTbaseParConf* pConfig = GetParConf();

   Float_t zDiff = 0;
   TABMntuTrack* pBMtrack = 0x0;
   fBmTrack = 0x0;
   fBmTrackOk    = false;
   pBMtrack = (TABMntuTrack*) fpBMntuTrack->Object();

	 if (pBMtrack->GetTracksN() == 1)
	  fBmTrack = pBMtrack->GetTrack(0);

   if (fBmTrack && fpFootGeo) {
	  fBmTrackPos  = fBmTrack->Intersection(fpFootGeo->GetVTCenter().Z()-fpFootGeo->GetBMCenter().Z());
	  Float_t chi2 = fBmTrack->GetChiSquare();
	  if (ValidHistogram())
		 fpHisBmChi2->Fill(chi2);

	//  if (chi2 < pConfig->GetAnalysisPar().BmTrackChi2Limit && chi2 > 0) // for the moment
		 fBmTrackOk = true;
   }
}

//_____________________________________________________________________________
//! Find straight tracks
Bool_t TAVTactBaseNtuTrack::FindStraightTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance;

   TVector3 lineOrigin;    // origin in the tracker system
   TVector3 lineSlope;     // slope along z-axis in tracker system
   TVector3 pos ;

   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);

   TAVTbaseParGeo*  pGeoMap = GetParGeo();
   TAVTbaseParConf* pConfig = GetParConf();

   TList array;
   array.SetOwner(false);
   array.Clear();

   Int_t nPlane = pGeoMap->GetSensorsN()-1;
   Int_t curPlane = nPlane;

   while (curPlane >= fRequiredClusters-1) {
	  // Get the last reference plane
	  curPlane = nPlane--;
	  Int_t nClusters = GetClustersN(curPlane);
	  if ( nClusters == 0) continue;

	  // Loop on all clusters of the first plane
	  // and try to make a track out of each one
	  for( Int_t iLastClus = 0; iLastClus < nClusters; ++iLastClus) { // loop on cluster of last plane,

		 if( GetTracksN() >= pConfig->GetAnalysisPar().TracksMaximum ) break; // if max track number reach, stop

		 TAGcluster* cluster = GetCluster(curPlane, iLastClus);
		 if (cluster->Found()) continue;
		 TAGbaseTrack* track = NewTrack();
		 array.Clear();
		 track->AddCluster(cluster);
		 array.Add(cluster);

		 lineOrigin.SetXYZ(cluster->GetPosition()[0], cluster->GetPosition()[1], 0); // parallel lines
		 lineOrigin = pGeoMap->Sensor2Detector(curPlane, lineOrigin);
		 lineSlope.SetXYZ(0, 0, 1);

		 track->SetLineValue(lineOrigin, lineSlope);

		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on planes
			Int_t nClusters1 = GetClustersN(iPlane);
			if (nClusters1 == 0) continue; //empty planes

			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance;
			TAGcluster* bestCluster = 0x0;

			for( Int_t iClus = 0; iClus < nClusters1; ++iClus ) { // loop on plane clusters
			   TAGcluster* aCluster = GetCluster(iPlane, iClus );

			   if( aCluster->Found()) continue; // skip cluster already found

			   aDistance = aCluster->Distance(track);

			   if( aDistance < minDistance ) {
				  minDistance = aDistance;
				  bestCluster = aCluster;
			   }
			} // end loop on plane clusters

			// if a cluster has been found, add the cluster
			if( bestCluster ) {
			   bestCluster->SetFound();
			   cluster->SetFound();
			   track->AddCluster(bestCluster);
			   array.Add(bestCluster);
			   if (fgRefit)
				  UpdateParam(track);
			}

		 } // end loop on planes

		 // Apply cuts
		 if (AppyCuts(track)) {
			track->SetTrackIdx(GetTracksN());
			//track->MakeChiSquare();    //i put it in update param
			track->SetType(0);
         AddNewTrack(track);
			TVector3 orig(0,0,0);
			SetBeamPosition(orig);

			if (ValidHistogram()) {
			   FillHistogramm(track);
			}

			delete track;
		 } else { // reset clusters
			for (Int_t i = 0; i < array.GetEntries(); ++i) {
			   TAVTcluster*  cluster1 = (TAVTcluster*)array.At(i);
			   cluster1->SetFound(false);
			}
			delete track;
			array.Clear();
		 }

	  } // end loop on first clusters

   } // while


   return true;
}

//_____________________________________________________________________________
//! Set charge probability through calibration parameters
void TAVTactBaseNtuTrack::SetChargeProba()
{
   if (fpCalib == 0x0) return;
   TAVTbaseParCal* pCalib   = (TAVTbaseParCal*) fpCalib->Object();

   Int_t nTrack = GetTracksN();
   if (nTrack == 0) return;

   for (Int_t iTrack = 0; iTrack < nTrack; ++iTrack) {

      Int_t nPixelsTot = 0;
      Float_t nPixelsMean;
      TAGbaseTrack* track = GetTrack(iTrack);
      Int_t nClus = track->GetClustersN();

      for (Int_t i=0;i<nClus;i++) {
         TAVTcluster* clus = (TAVTcluster*)track->GetCluster(i);
         nPixelsTot += clus->GetPixelsN();
      }
      nPixelsMean = nPixelsTot/nClus;
      const TArrayF* proba =  pCalib->GetChargeProba(nPixelsMean);
      track->SetChargeProba(proba);
      track->SetChargeMaxProba(pCalib->GetChargeMaxProba());
      track->SetChargeWithMaxProba(pCalib->GetChargeWithMaxProba());

      proba =  pCalib->GetChargeProbaNorm(nPixelsMean);
      track->SetChargeProbaNorm(proba);
      track->SetChargeMaxProbaNorm(pCalib->GetChargeMaxProbaNorm());
      track->SetChargeWithMaxProbaNorm(pCalib->GetChargeWithMaxProbaNorm());
   }
}

//_____________________________________________________________________________
//! Fill histograms per track
//! \param[in] track a given track
void TAVTactBaseNtuTrack::FillHistogramm(TAGbaseTrack* track)
{
   TAVTactBaseTrack::FillHistogramm(track);

   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*)fpGeoMap->Object();

   fpHisTrackClus->Fill(track->GetClustersN());
   for (Int_t i = 0; i < track->GetClustersN(); ++i) {
      TAGcluster* cluster = (TAGcluster*)track->GetCluster(i);
      Int_t idx = cluster->GetSensorIdx();
      fpHisPixelTot->Fill(cluster->GetElementsN());
      fpHisPixel[idx]->Fill(cluster->GetElementsN());
   }
}

//_____________________________________________________________________________
//! Fill histograms
void TAVTactBaseNtuTrack::FillHistogramm()
{
   TAVTbaseParGeo* pGeoMap   = (TAVTbaseParGeo*) fpGeoMap->Object();

   fpHisTrackEvt->Fill(GetTracksN());
   if (GetTracksN() == 0)
      fpHisClusSensor->Fill(0);

   for (Int_t iPlane = 0; iPlane < pGeoMap->GetSensorsN(); ++iPlane) {

      Int_t nClusters = GetClustersN(iPlane);
      if ( nClusters == 0) continue;

      Int_t left = 0;
      for( Int_t iClus = 0; iClus < nClusters; ++iClus) {

         TAGcluster* cluster = GetCluster(iPlane, iClus);
         if (!cluster->Found()) {
            fpHisClusLeftPix->Fill(iPlane+1, cluster->GetElementsN());
            left++;
         }
      }
      static Float_t mean[36];
      Int_t evtNumber = gTAGroot->CurrentEventNumber();
      mean[iPlane] = evtNumber/(evtNumber+1.)*mean[iPlane] + 1./(evtNumber+1.)*(left/nClusters);
      fpHisClusLeft->SetBinContent(iPlane+1, mean[iPlane]*100);
   }
}

//_____________________________________________________________________________
//! Fill BM histograms per track
//! \param[in] bmTrackPos vector for BM track position in VTX frame
void TAVTactBaseNtuTrack::FillBmHistogramm(TVector3 bmTrackPos)
{
   bmTrackPos  = fpFootGeo->FromBMLocalToGlobal(bmTrackPos);
   fpHisBmBeamProf->Fill(bmTrackPos.X(), bmTrackPos.Y());

   Float_t posZtg = fpFootGeo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
   posZtg = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtg)).Z();

   for (Int_t i = 0; i < GetTracksN(); ++i) {
	  TAGbaseTrack* track  = GetTrack(i);
     TVector3      origin = track->Intersection(posZtg);

	  origin  = fpFootGeo->FromVTLocalToGlobal(origin);
	  TVector3 res = origin - bmTrackPos;
	  fpHisVtxResX->Fill(origin.X(), bmTrackPos.Y());
	  fpHisVtxResY->Fill(origin.Y(), bmTrackPos.Y());
   }
}


//_____________________________________________________________________________
//! Study of the purity of the track and efficiency
void TAVTactBaseNtuTrack::EvaluateTrack()
{  
   bool good_clus = true; 
   TAVTntuTrack *pNtuTrack = (TAVTntuTrack *)fpNtuTrack->Object();
   pNtuEve = static_cast<TAMCntuPart *>(gTAGroot->FindDataDsc(FootActionDscName("TAMCntuPart"), "TAMCntuPart")->Object());
   vector<int> id_vec; // vector of MC_ID of every cluster
   std::map<std::string, int> m; //map of the multiplicity of different cluster in a track


   //from MC: study the fragmentation of the primary

   double_t target_sizeZ = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetTargetPar().Size.Z();
   Int_t target_region = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetRegTarget();
   Int_t primary_charge = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber;
   Bool_t isFragment = false;


   TAMCpart *primary_MC = pNtuEve->GetTrack(0); // mcparticle primary
   if ( primary_MC->GetMotherID() == -1  // if primary of the beam
    && primary_MC->GetInitPos().Z() < -100  // if the particle is born at the beginning of the setup
    && (primary_MC->GetFinalPos().Z() >= -(target_sizeZ/2.) && primary_MC->GetFinalPos().Z() <= (target_sizeZ/2.)) // if the particle goes into fragmentation inside the target
   ) isFragment = true;
 
//----- loop on every reconstructed track
   m_nClone.clear();
   for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i)   //for every track
   {  
      id_vec.clear();
      m.clear();
      //cout << "track n°" << i << endl;
         TAVTtrack *track = pNtuTrack->GetTrack(i);
     for (Int_t j = 0; j < track->GetClustersN(); ++j){     // for every cluster
         TAVTcluster *clus = (TAVTcluster *)track->GetCluster(j);
         good_clus = true;
         // if (clus->GetMcTracksN()>1){   // i study only tracks with clusters well reconstructed   TO CHANGE HERE
         //    good_clus = false;
         //    break;
         // }
         //id_vec.push_back(clus->GetMcTrackIdx(0));

      for (Int_t i = 0; i < clus->GetMcTracksN();i++)     //nb: every cluster can be done of pixels of different particle_id
         {
            id_vec.push_back(clus->GetMcTrackIdx(i));
         }
            

     }
     if (good_clus == false) continue;
     
     for (auto &element : id_vec)
     {
         ++m[to_string(element)];   //moltiplicity of different particle ID in the track
     }
     
     if (isFragment)
     fpHisTrackMultiplicity_frag->Fill(m.size());
     else 
     fpHisTrackMultiplicity_primary->Fill(m.size());

     // retrieve the MCId of the most frequent particle
     int max = -1;
     Int_t max_id = -1;
         for (auto &x : m)
     { 
         if (x.second > max){
            max = x.second;
            //if (x.second >= 3)
            max_id = stoi(x.first); //string to int
         }
     }

     

     // define the charge of the track as the one of the most probable MCparticle     TO BE CHANGED 
     int charge_track = -1;
     Int_t theta = -1;
     if (max_id == -666) {
      charge_track = -1; // if the cluster is made of noisy pixels (id = -666)
      theta = -1;
     }
     else{
      charge_track = pNtuEve->GetTrack(max_id)->GetCharge();
      theta = (int) round(pNtuEve->GetTrack(max_id)->GetInitP().Theta() * (180 / TMath::Pi())) ;
     }
     m_nRecoTracks[charge_track]++;
     m_nClone[charge_track][max_id]++;
     if (m_nClone[charge_track][max_id]>1){
         n_clones[charge_track] = m_nClone[charge_track][max_id];
     }
     m_nRecoTracks_angle[charge_track][theta]++;   

     //TAMCpart *particle = pNtuEve->GetTrack(max_id);
     if (isVTMatched(max_id)){
         m_nRecoTracks_matched[charge_track]++;
         m_nRecoTracks_matched_angle[charge_track][theta]++;
         m_nCorrectClus[charge_track]+=m[to_string(max_id)];
         m_nTotalClus[charge_track] += track->GetClustersN();

         // -------------------------study of angular resolution

         //angle of track from vtx frame to global frame in deg
         Float_t track_theta = fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ()).Theta() * TMath::RadToDeg();
         Float_t track_phi = fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ()).Phi() * TMath::RadToDeg();

         //angleof the MCparticle after crossing the target
         TAMCpart *mcpart = pNtuEve->GetTrack(max_id);
         Int_t charge = mcpart->GetCharge();
         pNtuReg = static_cast<TAMCntuRegion *>(gTAGroot->FindDataDsc(FootActionDscName("TAMCntuRegion"), "TAMCntuRegion")->Object());
         Int_t nCross = pNtuReg->GetRegionsN();
         Float_t mcpart_theta = -1 ; 
         Float_t mcpart_phi = -1; 
         for (int iCross = 0; iCross < nCross; iCross++)
         {
            TAMCregion *cross = pNtuReg->GetRegion(iCross);
            Int_t OldReg = cross->GetOldCrossN();
            Int_t NewReg = cross->GetCrossN();
            Int_t target_region = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetRegTarget();
            TString regairone("AIR1");
            Int_t after_target_region = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetCrossReg(regairone);

            if (((cross->GetTrackIdx() - 1) == max_id) && OldReg == target_region && NewReg == after_target_region)
            { // if it is not the particle I want to check, continue the loop
            mcpart_theta = cross->GetMomentum().Theta() * TMath::RadToDeg();
            mcpart_phi = cross->GetMomentum().Phi() * TMath::RadToDeg();

            break;
            } else
            continue;
         }
          if (charge > 0 && charge < 9 && mcpart_theta>-1){
             fpHisThetaRes[charge - 1]->Fill(track_theta - mcpart_theta);
             fpHisPhiRes[charge - 1]->Fill(track_phi - mcpart_phi);
          }

         
         // //clus position resolution
          pNtuHit = static_cast<TAMCntuHit *>(gTAGroot->FindDataDsc(FootDataDscMcName(kVTX), "TAMCntuHit")->Object());
           for (Int_t i = 0; i < track->GetClustersN(); ++i)
           {
              TAGcluster *cluster = (TAGcluster *)track->GetCluster(i);
              Int_t idx = cluster->GetSensorIdx();

              TVector3 clus_pos = fpFootGeo->FromVTLocalToGlobal(cluster->GetPositionG());
              Int_t hit_id = cluster->GetMcIndex(0); // i take the first hit
              TAMChit *hit = pNtuHit->GetHit(hit_id);
              TVector3 hit_pos = hit->GetInPosition();
              Int_t hit_idx = hit->GetSensorId();

              fpTrackClusResX[idx]->Fill(clus_pos.X() - hit_pos.X());
              fpTrackClusResY[idx]->Fill(clus_pos.Y() - hit_pos.Y());
          }
    

     } else{ // study of bad reco tracks
              // cout << "track " << i << "(out of " << pNtuTrack->GetTracksN() << " ) of event " <<gTAGroot->CurrentEventNumber() << "is fake "<< endl;
              // for (auto &x : m)
              // {
              //    cout << "id = "<< x.first << " n° "<< x.second << endl;
              //    TAMCpart *bad_particle = pNtuEve->GetTrack(stoi(x.first));
              //    cout << "charge: " << bad_particle->GetCharge() << endl;
              //    cout << "Init pos Z: " << bad_particle->GetInitPos().Z() << endl;
              //    cout << "fluka id :"<< bad_particle ->GetFlukaID() << endl;
              //    cout << "mother id :"<< bad_particle->GetMotherID() << endl;
              //    cout << "final position : "<< bad_particle ->GetFinalPos().Z() << endl;
              // }

              if (max_id == -666)
                 fpBadTrackCharge->Fill(-1); // if it is from simulated random noise
              else {
         TAMCpart *bad_particle = pNtuEve->GetTrack(max_id);
         Int_t charge =  bad_particle -> GetCharge();
         TVector3 Init_pos = bad_particle->GetInitPos();
         fpBadTrackPosZ->Fill(Init_pos.Z());
         fpBadTrackCharge->Fill(charge);
         fpBadTrackFlukaID->Fill(bad_particle->GetFlukaID());
         fpBadTrackMothID->Fill(bad_particle->GetMotherID());
         fpBadTrackInitP->Fill(bad_particle->GetInitP().Mag() );
         }
      }
   }
   m_nClone.clear(); // refresh at every event



   //----- loop on every MCpart for efficiency in Z
   for (Int_t i = 0; i < pNtuEve->GetTracksN(); ++i) // for every mc part
   {
     TAMCpart *particle = pNtuEve->GetTrack(i);
      if (isVTMatched(i)) {
         m_nMCTracks[particle->GetCharge()]++;
         Int_t theta = (int) round(particle->GetInitP().Theta() * (180 / TMath::Pi()));  // in deg)
         //cout << "theta : " << particle->GetInitP().Theta() * (180 / TMath::Pi()) << " rounded to : " << theta << endl;
         m_nMCTracks_angle[particle->GetCharge()][theta]++;
      }
   }









   }

//_____________________________________________________________________________
//! \brief Finalize all the needed histograms for studies
//!
//! Save control plots and calculate resolution. Called from outside, at the end of the event cycle
void TAVTactBaseNtuTrack::Finalize()
{
   if (GetFlagMC() &&  TAGrecoManager::GetPar()->IsRegionMc()){
      PrintEfficiency();
      PrintAngularEfficiency();
   }
}

void TAVTactBaseNtuTrack::PrintEfficiency()
{
     float totalNum = 0.;
     float totalDen = 0.;
     for (int i = 1; i <= ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber; i++){
     fpReconstructedTracks->SetBinContent(i, m_nRecoTracks[i]);

     float n_goodreco = (float)m_nRecoTracks_matched[i];
     float n_badreco = (float)m_nRecoTracks[i] - (float)m_nRecoTracks_matched[i];
     float n_ref = (float)m_nMCTracks[i];
     float n_CorrectClus = (float)m_nCorrectClus[i];
     float n_TotalClus = (float)m_nTotalClus[i];
     //cout << "Z="<<i<<endl<<"n_goodreco: "<< n_goodreco << " n_badreco " << n_badreco << " n_ref " << n_ref << " n_clus "<<n_clus << endl;

     // recognition efficiency: reconstructed tracks in the set of the MC possible one
     float eff = 0;     
     float eff_err = 0;
     if (n_ref >0) eff =(float) n_goodreco / n_ref;
     if (n_ref > 0) eff_err = TMath::Sqrt(eff * abs(1 - eff) / n_ref); // associate error as sqrt(eff(1 - eff) / Ntot) -- binomial distribution
     fpTrackEfficiency->SetBinContent(i, eff);
     fpTrackEfficiency->SetBinError(i, eff_err);

     // fake yield
     float eff_fake = 0;
     float eff_fake_err =0;
     if ((float)m_nRecoTracks[i]>0) eff_fake = n_badreco / (float)m_nRecoTracks[i];
     if ((float)m_nRecoTracks[i]>0) eff_fake_err = TMath::Sqrt(eff_fake * abs(1 - eff_fake) / (float)m_nRecoTracks[i]);

     fpTrackEfficiencyFake->SetBinContent(i, eff_fake);
     fpTrackEfficiencyFake->SetBinError(i, eff_fake_err);

     // track purity
     float purity_track = 0;
     float purity_track_err = 0;
     if ((float)m_nRecoTracks[i] > 0)
         purity_track = n_goodreco / (float)m_nRecoTracks[i];
     if ((float)m_nRecoTracks[i] > 0)
         purity_track_err = TMath::Sqrt(purity_track * abs(1 - purity_track) / (float)m_nRecoTracks[i]);

     fpTrackPurityTrack->SetBinContent(i, purity_track);
     fpTrackPurityTrack->SetBinError(i, purity_track_err);

     // purity of clusters
     float purity = 0;
     float purity_err = 0;
     if ((float)m_nRecoTracks_matched[i]>0) purity = n_CorrectClus / n_TotalClus;
     if ((float)m_nRecoTracks_matched[i]>0) purity_err = TMath::Sqrt(purity * abs(1 - purity) / n_TotalClus);   //! hard coded the total number of cluster in a track
     fpTrackPurity->SetBinContent(i, purity);
     fpTrackPurity->SetBinError(i, purity_err);

     // clones
     float clones_ratio = 0;
     float clones_err = 0;
     if ((float)m_nRecoTracks[i]>0) clones_ratio = n_clones[i] / (float)m_nRecoTracks[i];
     if ((float)m_nRecoTracks[i]>0) clones_err = TMath::Sqrt(clones_ratio * abs(1 - clones_ratio) / (float)m_nRecoTracks[i]);
     cout << "clones ratio: " << i << " " << clones_ratio << " of " << n_clones[i] << " / " << m_nRecoTracks[i] <<endl;
     fpTrackClones->SetBinContent(i, clones_ratio);
     fpTrackClones->SetBinError(i, clones_err);
   }
}

void TAVTactBaseNtuTrack::PrintAngularEfficiency()
{

     for (int i = 1; i <= ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber; i++){
     for (int j = 0; j<=40; j++){

     float n_goodreco = (float)m_nRecoTracks_matched_angle[i][j];
     float n_badreco = (float)m_nRecoTracks_angle[i][j] - (float)m_nRecoTracks_matched_angle[i][j];
     float n_allreco = (float)m_nRecoTracks_angle[i][j];
     float n_ref = (float)m_nMCTracks_angle[i][j];
     

     cout << "Z=" << i << " theta = "<< j<< " good reco: " << n_goodreco << " bad reco:  "<< n_badreco << " tot: " << n_allreco << " ref: " << n_ref << endl;
     // efficiency: reconstructed tracks in the set of the MC possible one
     float eff = 0;     
     float eff_err = 0;
     if (n_ref >0) eff =(float) n_goodreco / n_ref;
     if (n_ref > 0) eff_err = TMath::Sqrt(eff * abs(1 - eff) / n_ref); // associate error as sqrt(eff(1 - eff) / Ntot) -- binomial distribution
     fpTrackAngularEfficiency[i-1]->SetBinContent(j+1, eff);
     fpTrackAngularEfficiency[i-1]->SetBinError(j+1, eff_err);

     // track purity
     float purity_track = 0;
     float purity_track_err = 0;
     if (n_allreco > 0) purity_track = n_goodreco / n_allreco;
     if (n_allreco >0)  purity_track_err = TMath::Sqrt(purity_track * abs(1 - purity_track) / n_allreco);
     fpTrackAngularPurityTrack[i-1]->SetBinContent(j+1, purity_track);
     fpTrackAngularPurityTrack[i-1]->SetBinError(j+1, purity_track_err);

   }
   }
}


bool TAVTactBaseNtuTrack::isVTMatched(Int_t Id_part){ // check if the particle goes throughout the detector
      pNtuReg = static_cast<TAMCntuRegion *>(gTAGroot->FindDataDsc(FootActionDscName("TAMCntuRegion"), "TAMCntuRegion")->Object());
      Int_t nCross = pNtuReg->GetRegionsN();

      for (int iCross = 0; iCross < nCross; iCross++)
      {
     TAMCregion *cross = pNtuReg->GetRegion(iCross);     // Gets the i-crossing
     TVector3 crosspos = cross->GetPosition();           // Get CROSSx, CROSSy, CROSSz
     Int_t OldReg = cross->GetOldCrossN();
     Int_t NewReg = cross->GetCrossN();
     Double_t time_cross = cross->GetTime();
     TVector3 mom_cross = cross->GetMomentum();                                                                                // retrieves P at crossing
     Double32_t Mass = cross->GetMass();                                                                                       // retrieves Mass of track
     Double_t ekin_cross = pow(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2) + pow(Mass, 2), 0.5) - Mass; // Kinetic energy at crossing
     Double_t beta_cross = pow(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2), 0.5) / (ekin_cross + Mass);

     if ((cross->GetTrackIdx() - 1) != Id_part){   // if it is not the particle I want to check, continue the loop
      continue;
     }

     TAMCpart *particle = pNtuEve->GetTrack(cross->GetTrackIdx() - 1); // retrievs TrackID
     Int_t particle_ID = cross->GetTrackIdx() - 1;                     // TrackID
     Int_t target_region = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetRegTarget();
     Int_t primary_charge = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber;
     TString regvtxname("VTXP3");
     Int_t last_vtx_plane_region = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetCrossReg(regvtxname);
     TString regname("AIR1");
     Int_t RegAirAfterVtx = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetCrossReg(regname);
     auto Mid = particle->GetMotherID();
     double mass = particle->GetMass(); // Get TRpaid-1
     auto Reg = particle->GetRegion();
     auto finalPos = particle->GetFinalPos();
     int baryon = particle->GetBaryon();
     TVector3 initMom = particle->GetInitP();
     double InitPmod = sqrt(pow(initMom(0), 2) + pow(initMom(1), 2) + pow(initMom(2), 2));
     Float_t Ek_tr_tot = (sqrt(pow(InitPmod, 2) + pow(mass, 2)) - mass);
     Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
     Float_t Ek_true = Ek_tr_tot / (double)baryon;                          // MeV/n
     Float_t theta_tr = particle->GetInitP().Theta() * (180 / TMath::Pi()); // in deg
     Float_t charge_tr = particle->GetCharge();

     if (
         (particle_ID == 0                // if the particle is a primary OR
         || (Mid == 0 && Reg == target_region) // if the particle is a primary fragment born in the target
         ) &&  particle->GetCharge() > 0 &&
         particle->GetCharge() <= primary_charge && // with reasonable charge
         // Ek_true > 100 &&                           // with enough initial energy to go beyond the vtx    //! is it true?
         // theta_tr <= 30. && // inside the angular acceptance of the vtxt   //!hard coded for GSI2021_MC
         OldReg == last_vtx_plane_region && NewReg == RegAirAfterVtx // it crosses the last plane of the vertex

     )
      return true;
          }
   return false;

}