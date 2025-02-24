/*!
 \file TAITtrack.cxx
 \brief   Class for ITR tracks
 */

#include "TMath.h"
#include "TClonesArray.h"

#include "TAGrecoManager.hxx"
#include "TAITcluster.hxx"
#include "TAITtrack.hxx"


//#################################################################

  //////////////////////////////////////////////////////////////////
  // Class Description of TAITtrack                               //
  //                                                              //
  // A particle track from e.g. accelerator passing the tracker   //
  // The track is measured by the tracker with its silicon        //
  // reference planes                                             //
  // The track is e.g. a straight line                            //
  // The line is found by a fit to the hits in the silicon planes // 
  //                                                              //
  //////////////////////////////////////////////////////////////////


/*!
 \class TAITtrack
 \brief Class for ITR tracks
 */

//! Class Imp
ClassImp(TAITtrack) // Description of a Track

//______________________________________________________________________________
//! Constructor
TAITtrack::TAITtrack()
:  TAGbaseTrack()
{
   SetupClones();
}

//______________________________________________________________________________
//! Destructor
TAITtrack::~TAITtrack()
{
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] aTrack ytack to copy
TAITtrack::TAITtrack(const TAGbaseTrack& aTrack)
: TAGbaseTrack::TAGbaseTrack(aTrack)
{
   
}

//______________________________________________________________________________
//! Set up clones
void TAITtrack::SetupClones()
{
   fListOfClusters = new TClonesArray("TAITcluster");
   fListOfClusters->SetOwner(true);
}

// __________________________________________________________________________
//! Add cluster to track
//!
//! \param[in] clus cluster to add
void TAITtrack::AddCluster(TAGcluster* clus)
{
   TAITcluster* cluster = static_cast<TAITcluster*>(clus);

   for (Int_t k = 0; k < cluster->GetMcTracksN(); ++k) {
      Int_t idx = cluster->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &clusterArray = *fListOfClusters;
   new(clusterArray[clusterArray.GetEntriesFast()]) TAITcluster(*cluster);
   fMeanEltsN  += cluster->GetPixelsN();
   fMeanCharge += cluster->GetCharge();
}
