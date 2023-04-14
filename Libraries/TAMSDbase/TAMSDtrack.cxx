/*!
 \file TAMSDtrack.cxx
 \brief Implementation of TAMSDtrack
 */

#include "TMath.h"
#include "TClonesArray.h"

#include "TAGrecoManager.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDtrack.hxx"

//#################################################################

  //////////////////////////////////////////////////////////////////
  // Class Description of TAMSDtrack                               //
  //                                                              //
  // A particle track from e.g. accelerator passing the tracker   //
  // The track is measured by the tracker with its silicon        //
  // reference planes                                             //
  // The track is e.g. a straight line                            //
  // The line is found by a fit to the hits in the silicon planes // 
  //                                                              //
  //////////////////////////////////////////////////////////////////

/*!
 \class TAMSDtrack
 \brief simple container class for tracks with the associated points
 */

//! Class Imp
ClassImp(TAMSDtrack) // Description of a Track

//______________________________________________________________________________
//! Constructor
TAMSDtrack::TAMSDtrack()
:  TAGbaseTrack()
{
   SetupClones();
}

//______________________________________________________________________________
//! Destructor
TAMSDtrack::~TAMSDtrack()
{
}

//______________________________________________________________________________
//!  build track from copy
//!
//! \param[in] aTrack track to copy
TAMSDtrack::TAMSDtrack(const TAGbaseTrack& aTrack)
: TAGbaseTrack::TAGbaseTrack(aTrack)
{
   
}

//______________________________________________________________________________
//! Setup clones
void TAMSDtrack::SetupClones()
{
   fListOfClusters = new TClonesArray("TAMSDpoint");
   fListOfClusters->SetOwner(true);
}

//______________________________________________________________________________
//!  Add cluster to track
//!
//! \param[in] clus cluster to add
void TAMSDtrack::AddCluster(TAGcluster* clus)
{
   TAMSDpoint* point = static_cast<TAMSDpoint*>(clus);

   for (Int_t k = 0; k < point->GetMcTracksN(); ++k) {
      Int_t idx = point->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &pointArray = *fListOfClusters;
   new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint(*point);
   fMeanEltsN  += point->GetElementsN();
   fMeanCharge += point->GetEnergyLoss();
}
