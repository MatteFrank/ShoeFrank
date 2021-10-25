/*!
 \file
 \version $Id: TAMSDactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
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
 \brief NTuplizer for Inner tracker tracks. **
 */

ClassImp(TAMSDactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
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
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuTrack::~TAMSDactNtuTrack()
{

}

//_____________________________________________________________________________
//
Int_t TAMSDactNtuTrack::GetClustersN(Int_t iPlane)
{
   TAMSDntuPoint*  pNtuClus  = (TAMSDntuPoint*)  fpNtuClus->Object();
   return pNtuClus->GetPointsN(iPlane);
}

//_____________________________________________________________________________
//
TAGcluster* TAMSDactNtuTrack::GetCluster(Int_t iPlane, Int_t iClus)
{
   TAMSDntuPoint*  pNtuClus  = (TAMSDntuPoint*) fpNtuClus->Object();
   TAMSDpoint* cluster = pNtuClus->GetPoint(iPlane, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//
Int_t TAMSDactNtuTrack::GetTracksN()
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//
void TAMSDactNtuTrack::AddNewTrack(TAGbaseTrack* trk)
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   TAMSDtrack* track = static_cast<TAMSDtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//
TAGbaseTrack* TAMSDactNtuTrack::NewTrack()
{
   return new TAMSDtrack();
}



