/*!
 \file TAVTtrack.cxx
 \brief   Class for VTX tracks
 */

#include "TMath.h"
#include "TClonesArray.h"

#include "TAGrecoManager.hxx"
#include "TAVTtrack.hxx"


//#################################################################

  //////////////////////////////////////////////////////////////////
  // Class Description of TAVTtrack                               //
  //                                                              //
  // A particle track from e.g. accelerator passing the tracker   //
  // The track is measured by the tracker with its silicon        //
  // reference planes                                             //
  // The track is e.g. a straight line                            //
  // The line is found by a fit to the hits in the silicon planes // 
  //                                                              //
  //////////////////////////////////////////////////////////////////

/*!
 \class TAVTtrack
 \brief Class for VTX tracks
 */

ClassImp(TAVTtrack) // Description of a Track

//______________________________________________________________________________
//  
TAVTtrack::TAVTtrack()
:  TAGbaseTrack(),
   fPosVertex(0.,0.,0.)
{
   SetupClones();
}

//______________________________________________________________________________
//
TAVTtrack::TAVTtrack(const TAVTtrack& aTrack)
:  TAGbaseTrack(aTrack),
   fPosVertex(aTrack.fPosVertex)
{
   
}

//______________________________________________________________________________
//  
TAVTtrack::~TAVTtrack()
{
}

//______________________________________________________________________________
//
void TAVTtrack::SetupClones()
{
   fListOfClusters = new TClonesArray("TAVTcluster");
   fListOfClusters->SetOwner(true);
}

// __________________________________________________________________________
//
void TAVTtrack::AddCluster(TAGcluster* clus)
{
  TAVTcluster* cluster = static_cast<TAVTcluster*>(clus);
   for (Int_t k = 0; k < cluster->GetMcTracksN(); ++k) {
      Int_t idx = cluster->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &clusterArray = *fListOfClusters;
   new(clusterArray[clusterArray.GetEntriesFast()]) TAVTcluster(*cluster);
   fMeanEltsN  += cluster->GetPixelsN();
   fMeanCharge += cluster->GetCharge();
}
