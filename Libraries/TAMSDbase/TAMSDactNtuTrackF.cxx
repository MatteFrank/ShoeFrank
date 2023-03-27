/*!
 \file TAMSDactNtuTrackF.cxx
 \brief   Implementation of TAMSDactNtuTrackF.
 */

#include "TClonesArray.h"
#include "TF1.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVector2.h"

//TAGroot
#include "TAGroot.hxx"

//First
#include "TAGgeoTrafo.hxx"

#include "TAGparGeo.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDparMap.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDactNtuTrackF.hxx"

/*!
 \class TAMSDactNtuTrackF
 \brief NTuplizer for MSD tracks. **
 */

//! Class imp
ClassImp(TAMSDactNtuTrackF);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus cluster container descriptor
//! \param[out] pNtuTrack track container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] p_geo_g target geometry parameter descriptor
TAMSDactNtuTrackF::TAMSDactNtuTrackF(const char* name,
								   TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								   TAGparaDsc* pGeoMap, TAGparaDsc* p_geo_g)
: TAVTactNtuTrackF(name, pNtuClus, pNtuTrack, pConfig, pGeoMap),
  fpGeoMapG(p_geo_g)
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
TAMSDactNtuTrackF::~TAMSDactNtuTrackF()
{
}

//_____________________________________________________________________________
//! IsGoodCandidate
//!
//! \param[in] trk a given track
Bool_t TAMSDactNtuTrackF::IsGoodCandidate(TAGbaseTrack* trk)
{
   
   TAMSDtrack* track = static_cast<TAMSDtrack*>(trk);
   
   for (Int_t i = 0; i < track->GetClustersN(); i++) {
      TAMSDpoint* clus = (TAMSDpoint*)track->GetCluster(i);
      fMapClus[clus] = 1;
   }
   
   if (fpGeoMapG == 0x0)
      return true;
   
   TAGparGeo* pGeoMap = (TAGparGeo*)fpGeoMapG->Object();

   Float_t width  = pGeoMap->GetTargetPar().Size[0];
   Float_t height = pGeoMap->GetTargetPar().Size[1];
   TVector3 vec = track->Intersection(-fpFootGeo->GetMSDCenter()[2]);

   if (TMath::Abs(vec.X()) > width || TMath::Abs(vec.Y()) > height)
	  return false;
   
   return true;
}

//_____________________________________________________________________________
//! Get number of clusters for a given sensor
//!
//! \param[in] iPlane plane index
Int_t TAMSDactNtuTrackF::GetClustersN(Int_t iPlane)
{
   TAMSDntuPoint*  pNtuClus = (TAMSDntuPoint*) fpNtuClus->Object();
   return pNtuClus->GetPointsN(iPlane);
}

//_____________________________________________________________________________
//! Get cluster for a given sensor and a given index
//!
//! \param[in] iPlane plane index
//! \param[in] iClus cluster index
TAGcluster* TAMSDactNtuTrackF::GetCluster(Int_t iPlane, Int_t iClus)
{
   TAMSDntuPoint*  pNtuClus = (TAMSDntuPoint*) fpNtuClus->Object();
   TAMSDpoint* cluster = pNtuClus->GetPoint(iPlane, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//! Get number of tracks
Int_t TAMSDactNtuTrackF::GetTracksN()
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//! Add new track to container using copt cstr
//!
//! \param[in] trk a given track
void TAMSDactNtuTrackF::AddNewTrack(TAGbaseTrack* trk)
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   TAMSDtrack* track = static_cast<TAMSDtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//! New track
TAGbaseTrack* TAMSDactNtuTrackF::NewTrack()
{
   return new TAMSDtrack();
}

//_____________________________________________________________________________
//! Set beam position
//!
//! \param[in] pos position of beam
void TAMSDactNtuTrackF::SetBeamPosition(TVector3 pos)
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   pNtuTrack->SetBeamPosition(pos);
}

//_____________________________________________________________________________
//! Get geometry parameters
TAVTbaseParGeo* TAMSDactNtuTrackF::GetParGeo()
{
   TAVTbaseParGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
   
   return pGeoMap;
}

//_____________________________________________________________________________
//! Get configuration parameters
TAVTbaseParConf* TAMSDactNtuTrackF::GetParConf()
{
   TAMSDparConf* pConfig = (TAMSDparConf*) fpConfig->Object();
   
   return pConfig;
}
