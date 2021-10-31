/*!
 \file
 \version $Id: TAITactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAITactNtuTrack.
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
 \brief NTuplizer for Inner tracker tracks. **
 */

ClassImp(TAITactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
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
//
Int_t TAITactNtuTrack::GetClustersN(Int_t iPlane)
{
   TAITntuCluster*  pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   return pNtuClus->GetClustersN(iPlane);
}

//_____________________________________________________________________________
//
TAGcluster* TAITactNtuTrack::GetCluster(Int_t iPlane, Int_t iClus)
{
   TAITntuCluster*  pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   TAITcluster* cluster = pNtuClus->GetCluster(iPlane, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//
Int_t TAITactNtuTrack::GetTracksN()
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//
void TAITactNtuTrack::AddNewTrack(TAGbaseTrack* trk)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   TAITtrack* track = static_cast<TAITtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//
TAGbaseTrack* TAITactNtuTrack::NewTrack()
{
   return new TAITtrack();
}

//_____________________________________________________________________________
//
void TAITactNtuTrack::SetBeamPosition(TVector3 pos)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   pNtuTrack->SetBeamPosition(pos);
}

//_____________________________________________________________________________
//
TAVTbaseParGeo* TAITactNtuTrack::GetParGeo()
{
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   
   return pGeoMap;
}

//_____________________________________________________________________________
//
TAVTbaseParConf* TAITactNtuTrack::GetParConf()
{
   TAITparConf* pConfig = (TAITparConf*) fpConfig->Object();
   
   return pConfig;
}
