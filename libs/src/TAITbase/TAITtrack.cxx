#include "TMath.h"
#include "TClonesArray.h"

#include "GlobalPar.hxx"
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


ClassImp(TAITtrack) // Description of a Track

//______________________________________________________________________________
//  
TAITtrack::TAITtrack()
:  TAVTbaseTrack()
{
   SetupClones();
}

//______________________________________________________________________________
//  
TAITtrack::~TAITtrack()
{
}

//______________________________________________________________________________
//
TAITtrack::TAITtrack(const TAVTbaseTrack& aTrack)
: TAVTbaseTrack::TAVTbaseTrack(aTrack)
{
   
}

//______________________________________________________________________________
//
void TAITtrack::SetupClones()
{
   fListOfClusters = new TClonesArray("TAITcluster");
   fListOfClusters->SetOwner(true);
}


// __________________________________________________________________________
//
void TAITtrack::AddCluster(TAITcluster* cluster)
{
   for (Int_t k = 0; k < cluster->GetMcTrackCount(); ++k) {
      Int_t idx = cluster->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &clusterArray = *fListOfClusters;
   new(clusterArray[clusterArray.GetEntriesFast()]) TAITcluster(*cluster);
   fMeanPixelsN += cluster->GetPixelsN()/float(fListOfClusters->GetEntries());
}
