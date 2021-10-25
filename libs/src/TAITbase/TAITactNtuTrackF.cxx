/*!
 \file
 \version $Id: TAITactNtuTrackF.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAITactNtuTrackF.
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
 \brief NTuplizer for vertex tracks. **
 */

ClassImp(TAITactNtuTrackF);

//------------------------------------------+-----------------------------------
//! Default constructor.
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
//
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
//
Int_t TAITactNtuTrackF::GetClustersN(Int_t iPlane)
{
   TAITntuCluster* pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   return pNtuClus->GetClustersN(iPlane);
}

//_____________________________________________________________________________
//
TAGcluster* TAITactNtuTrackF::GetCluster(Int_t iPlane, Int_t iClus)
{
   TAITntuCluster* pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   TAITcluster* cluster = pNtuClus->GetCluster(iPlane, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//
Int_t TAITactNtuTrackF::GetTracksN()
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//
void TAITactNtuTrackF::AddNewTrack(TAGbaseTrack* trk)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   TAITtrack* track = static_cast<TAITtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//
TAGbaseTrack* TAITactNtuTrackF::NewTrack()
{
   return new TAITtrack();
}

