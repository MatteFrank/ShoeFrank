/*!
 \file
 \version $Id: TAMSDactNtuTrackF.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
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
 \brief NTuplizer for vertex tracks. **
 */

ClassImp(TAMSDactNtuTrackF);

//------------------------------------------+-----------------------------------
//! Default constructor.
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
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuTrackF::~TAMSDactNtuTrackF()
{
 
}

//_____________________________________________________________________________
//  
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
//
Int_t TAMSDactNtuTrackF::GetClustersN(Int_t iPlane)
{
   TAMSDntuPoint*  pNtuClus = (TAMSDntuPoint*) fpNtuClus->Object();
   return pNtuClus->GetPointsN(iPlane);
}

//_____________________________________________________________________________
//
TAGcluster* TAMSDactNtuTrackF::GetCluster(Int_t iPlane, Int_t iClus)
{
   TAMSDntuPoint*  pNtuClus = (TAMSDntuPoint*) fpNtuClus->Object();
   TAMSDpoint* cluster = pNtuClus->GetPoint(iPlane, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//
Int_t TAMSDactNtuTrackF::GetTracksN()
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//
void TAMSDactNtuTrackF::AddNewTrack(TAGbaseTrack* trk)
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   TAMSDtrack* track = static_cast<TAMSDtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//
TAGbaseTrack* TAMSDactNtuTrackF::NewTrack()
{
   return new TAMSDtrack();
}

