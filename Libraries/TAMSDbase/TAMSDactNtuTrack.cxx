/*!
 \file TAMSDactNtuTrack.cxx
 \brief   Implementation of TAMSDactNtuTrack.
 */
#include "TClonesArray.h"
#include "TMath.h"


//TAGroot
#include "TAGroot.hxx"

// Foot trafo
#include "TAGgeoTrafo.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDtrack.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDactNtuTrack.hxx"

/*!
 \class TAMSDactNtuTrack
 \brief NTuplizer for MSD tracks.
 */

//! Class imp
ClassImp(TAMSDactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus cluster container descriptor
//! \param[out] pNtuTrack track container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
TAMSDactNtuTrack::TAMSDactNtuTrack(const char* name,
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								 TAGparaDsc* pGeoMap)
: TAVTactNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap)
{
   TString device(name);
   if (device.Contains("ms")) {
      AddDataIn(pNtuClus,   "TAMSDntuPoint");
      AddDataOut(pNtuTrack, "TAMSDntuTrack");
   }
   
   TAMSDparConf* config = (TAMSDparConf*)fpConfig->Object();
   fRequiredClusters   = config->GetAnalysisPar().PlanesForTrackMinimum;
   fSearchClusDistance = config->GetAnalysisPar().SearchHitDistance;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuTrack::~TAMSDactNtuTrack()
{

}

//_____________________________________________________________________________
//! Action
Bool_t TAMSDactNtuTrack::Action()
{
   // BM tracks
   if (fpBMntuTrack) 
	  CheckBM();
   
   
   // VTX
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();
   
   // looking straight
   FindStraightTracks();
   
   // looking inclined line
   if (!FindTiltedTracks()){
	  if (ValidHistogram())
		 FillHistogramm();
	  fpNtuTrack->SetBit(kValid);
	  return true;
   }
   
   if (FindVertices())
	  FindTiltedTracks();
   
   if(FootDebugLevel(1)) {
	  printf("%s %d tracks found\n", this->GetName(), pNtuTrack->GetTracksN()); //print name of action since it's used for all trackers
	  for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
		 TAMSDtrack* track = pNtuTrack->GetTrack(i);
		 printf("   with # clusters %d\n", track->GetClustersN());
	  }
   }
   

   if (ValidHistogram())
	  FillHistogramm();
   
   // Set charge probability
   SetChargeProba();
   
   fpNtuTrack->SetBit(kValid);
   return true;
}


//_____________________________________________________________________________
//! Get number of clusters for a given sensor
//!
//! \param[in] iPlane plane index
Int_t TAMSDactNtuTrack::GetClustersN(Int_t iPlane)
{
   TAMSDntuPoint*  pNtuClus  = (TAMSDntuPoint*)  fpNtuClus->Object();
   return pNtuClus->GetPointsN(iPlane);
}

//_____________________________________________________________________________
//! Get cluster for a given sensor and a given index
//!
//! \param[in] iPlane plane index
//! \param[in] iClus cluster index
TAGcluster* TAMSDactNtuTrack::GetCluster(Int_t iPlane, Int_t iClus)
{
   TAMSDntuPoint*  pNtuClus  = (TAMSDntuPoint*) fpNtuClus->Object();
   TAMSDpoint* cluster = pNtuClus->GetPoint(iPlane, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//! Get number of tracks
Int_t TAMSDactNtuTrack::GetTracksN()
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//! Add new track to container using copt cstr
//!
//! \param[in] trk a given track
void TAMSDactNtuTrack::AddNewTrack(TAGbaseTrack* trk)
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   TAMSDtrack* track = static_cast<TAMSDtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//! New track
TAGbaseTrack* TAMSDactNtuTrack::NewTrack()
{
   return new TAMSDtrack();
}

//_____________________________________________________________________________
//! Set beam position
//!
//! \param[in] pos position of beam
void TAMSDactNtuTrack::SetBeamPosition(TVector3 pos)
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   pNtuTrack->SetBeamPosition(pos);
}

//_____________________________________________________________________________
//! Get geometry parameters
TAVTbaseParGeo* TAMSDactNtuTrack::GetParGeo()
{
   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
   
   return pGeoMap;
}

//_____________________________________________________________________________
//! Get configuration parameters
TAVTbaseParConf* TAMSDactNtuTrack::GetParConf()
{
   TAMSDparConf* pConfig = (TAMSDparConf*) fpConfig->Object();
   
   return pConfig;
}
