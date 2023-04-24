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
      fpHisTrackMultiplicity_frag = new TH1F(Form("%sTrackMultiplicity_frag", fPrefix.Data()), Form("%s - Track multiplicity of clusters when there if fragmentation (MC)", fTitleDev.Data()), 5, -0.5, 4.5);
      AddHistogram(fpHisTrackMultiplicity_frag );
      fpHisTrackMultiplicity_primary  = new TH1F(Form("%sTrackMultiplicity_primary", fPrefix.Data()), Form("%s - Track multiplicity of clusters if no fragmentation (MC)", fTitleDev.Data()), 5, -0.5, 4.5);
      AddHistogram(fpHisTrackMultiplicity_primary);

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

   
   FindTiltedTracks();

   //looking straight
   // FindStraightTracks();

   // //looking inclined line
   //     if (!FindTiltedTracks())
   // { // it is never false with TAVTactNtuTrackF::FindTiltedTracks()
   //   if (ValidHistogram())
	// 	 FillHistogramm();
	//   fpNtuTrack->SetBit(kValid);
	//   return true;
   // }

   // if (FindVertices()){    //it is always false
	//   FindTiltedTracks();
   // }

   // if(FootDebugLevel(1)) {
	//   printf("%s %d tracks found\n", this->GetName(), pNtuTrack->GetTracksN()); //print name of action since it's used for all trackers
	//   for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
	// 	 TAVTtrack* track = pNtuTrack->GetTrack(i);
	// 	 printf("   with # clusters %d\n", track->GetClustersN());
	//   }
   // }

   if (GetFlagMC())
   EvaluateTrack();

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
//! Study of the purity of the track
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
   Bool_t isFragment = false;

   TAMCpart *primary_MC = pNtuEve->GetTrack(0); // mcparticle primary
   if ( primary_MC->GetMotherID() == -1  // if primary of the beam
    && primary_MC->GetInitPos().Z() < -100  // if the particle is born at the beginning of the setup
    && (primary_MC->GetFinalPos().Z() >= -(target_sizeZ/2.) && primary_MC->GetFinalPos().Z() <= (target_sizeZ/2.)) // if the particle goes into fragmentation inside the target
   ) isFragment = true;


   // if the primary particle goes into fragmentation in the target:

   for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i)   //for every track
   {  
      id_vec.clear();
      m.clear();
      //cout << "track n°" << i << endl;
         TAVTtrack *track = pNtuTrack->GetTrack(i);
     for (Int_t j = 0; j < track->GetClustersN(); ++j){     // for every cluster
         TAVTcluster *clus = (TAVTcluster *)track->GetCluster(j);
         good_clus = true;
         if (clus->GetMcTracksN()>1){   // i study only tracks with clusters well reconstructed
            good_clus = false;
            break;
         }
         id_vec.push_back(clus->GetMcTrackIdx(0));
         //cout << "id particle: "<< clus->GetMcTrackIdx(0) << endl;
         
         //cout << "particella charge: " << track->GetCharge() << endl;

     }
     if (good_clus == false) continue;
     //if  (!std::equal(id_vec.begin() + 1, id_vec.end(), id_vec.begin())) //if all the index are not equal
     for (auto &element : id_vec)
     {
         ++m[to_string(element)];
     }

     if (isFragment) fpHisTrackMultiplicity_frag->Fill(m.size());
     else fpHisTrackMultiplicity_primary->Fill(m.size());
     }
}