/*!
 \file TAITactNtuTrackF.cxx
 \brief   NTuplizer for ITR tracks using combinatory algorithm
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

#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"
#include "TAITntuTrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAITactNtuTrackF.hxx"

/*!
 \class TAITactNtuTrackF 
 \brief NTuplizer for ITR tracks using combinatory algorithm
 */

//! Class Imp
ClassImp(TAITactNtuTrackF);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus cluster container descriptor
//! \param[out] pNtuTrack track container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pCalib calibration parameter descriptor
//! \param[in] p_geo_g target geometry descriptor
TAITactNtuTrackF::TAITactNtuTrackF(const char* name, 
								   TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								   TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGparaDsc* p_geo_g)
: TAVTactNtuTrackF(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib),
  fpGeoMapG(p_geo_g)
{
   TString device(name);
   if (device.Contains("it")) {
      AddDataIn(pNtuClus,   "TAITntuCluster");
      AddDataOut(pNtuTrack, "TAITntuTrack");
   }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuTrackF::~TAITactNtuTrackF()
{
 
}

//_____________________________________________________________________________
//! Check if projected track is out of target area
//!
//! \param[in] trk a given track
Bool_t TAITactNtuTrackF::IsGoodCandidate(TAGbaseTrack* trk)
{
   TAITtrack* track = static_cast<TAITtrack*>(trk);
   
   for (Int_t i = 0; i < track->GetClustersN(); i++) {
      TAITcluster* clus = (TAITcluster*)track->GetCluster(i);
      fMapClus[clus] = 1;
   }
   
   if (fpGeoMapG == 0x0)
      return true;
   
   TAGparGeo* pGeoMap = (TAGparGeo*)fpGeoMapG->Object();

   Float_t width  = pGeoMap->GetTargetPar().Size[0];
   Float_t height = pGeoMap->GetTargetPar().Size[1];
   TVector3 vec = track->Intersection(-fpFootGeo->GetITCenter()[2]);

   if (TMath::Abs(vec.X()) > width || TMath::Abs(vec.Y()) > height)
	  return false;
   
   return true;
}

//_____________________________________________________________________________
//! Get number of clusters for a given sensor
//!
//! \param[in] iSensor sensor index
Int_t TAITactNtuTrackF::GetClustersN(Int_t iSensor)
{
   TAITntuCluster* pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   return pNtuClus->GetClustersN(iSensor);
}

//_____________________________________________________________________________
//! Get cluster for a given sensor and a given index
//!
//! \param[in] iSensor sensor index
//! \param[in] iClus cluster index
TAGcluster* TAITactNtuTrackF::GetCluster(Int_t iSensor, Int_t iClus)
{
   TAITntuCluster* pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   TAITcluster* cluster = pNtuClus->GetCluster(iSensor, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//! Get number of tracks
Int_t TAITactNtuTrackF::GetTracksN()
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//! Add new track to container using copt cstr
//!
//! \param[in] trk a given track
void TAITactNtuTrackF::AddNewTrack(TAGbaseTrack* trk)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   TAITtrack* track = static_cast<TAITtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//! Get new track
TAGbaseTrack* TAITactNtuTrackF::NewTrack()
{
   return new TAITtrack();
}

//_____________________________________________________________________________
//! Set beam position
//!
//! \param[in] pos position of beam
void TAITactNtuTrackF::SetBeamPosition(TVector3 pos)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   pNtuTrack->SetBeamPosition(pos);
}

//_____________________________________________________________________________
//! Get geometry parameters
TAVTbaseParGeo* TAITactNtuTrackF::GetParGeo()
{
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   
   return pGeoMap;
}

//_____________________________________________________________________________
//! Get configuration parameters
TAVTbaseParConf* TAITactNtuTrackF::GetParConf()
{
   TAITparConf* pConfig = (TAITparConf*) fpConfig->Object();
   
   return pConfig;
}
