/*!
 \file TAITactNtuTrack.cxx
 \brief   NTuplizer for ITR tracks
 */
#include "TClonesArray.h"
#include "TMath.h"


//TAGroot
#include "TAGroot.hxx"

// Foot trafo
#include "TAGgeoTrafo.hxx"

//BM
#include "TABMntuTrack.hxx"

#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITtrack.hxx"
#include "TAITntuTrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAITactNtuTrack.hxx"

/*!
 \class TAITactNtuTrack
 \brief NTuplizer for ITR tracks
 */

//! Class Imp
ClassImp(TAITactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus cluster container descriptor
//! \param[out] pNtuTrack track container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pCalib calibration parameter descriptor
//! \param[in] pBMntuTrack input BM track container descriptor
TAITactNtuTrack::TAITactNtuTrack(const char* name,
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAVTactNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib, pBMntuTrack)
{
   TString device(name);
   if (device.Contains("it")) {
      AddDataIn(pNtuClus,   "TAITntuCluster");
      AddDataOut(pNtuTrack, "TAITntuTrack");
   }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuTrack::~TAITactNtuTrack()
{
}

//_____________________________________________________________________________
//! Get number of clusters for a given sensor
//!
//! \param[in] iSensor sensor index
Int_t TAITactNtuTrack::GetClustersN(Int_t iSensor)
{
   TAITntuCluster*  pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   return pNtuClus->GetClustersN(iSensor);
}

//_____________________________________________________________________________
//! Get cluster for a given sensor and a given index
//!
//! \param[in] iSensor sensor index
//! \param[in] iClus cluster index
TAGcluster* TAITactNtuTrack::GetCluster(Int_t iSensor, Int_t iClus)
{
   TAITntuCluster*  pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   TAITcluster* cluster = pNtuClus->GetCluster(iSensor, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//! Get number of tracks
Int_t TAITactNtuTrack::GetTracksN()
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//! Add new track to container using copt cstr
//!
//! \param[in] trk a given track
void TAITactNtuTrack::AddNewTrack(TAGbaseTrack* trk)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   TAITtrack* track = static_cast<TAITtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//! Get new track
TAGbaseTrack* TAITactNtuTrack::NewTrack()
{
   return new TAITtrack();
}

//_____________________________________________________________________________
//! Set beam position
//!
//! \param[in] pos position of beam
void TAITactNtuTrack::SetBeamPosition(TVector3 pos)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   pNtuTrack->SetBeamPosition(pos);
}

//_____________________________________________________________________________
//! Get geometry parameters
TAVTbaseParGeo* TAITactNtuTrack::GetParGeo()
{
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   
   return pGeoMap;
}

//_____________________________________________________________________________
//! Get configuration parameters
TAVTbaseParConf* TAITactNtuTrack::GetParConf()
{
   TAITparConf* pConfig = (TAITparConf*) fpConfig->Object();
   
   return pConfig;
}
