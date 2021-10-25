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


ClassImp(TAITtrack) // Description of a Track

//______________________________________________________________________________
//  
TAITtrack::TAITtrack()
:  TAGbaseTrack()
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
TAITtrack::TAITtrack(const TAGbaseTrack& aTrack)
: TAGbaseTrack::TAGbaseTrack(aTrack)
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
   for (Int_t k = 0; k < cluster->GetMcTracksN(); ++k) {
      Int_t idx = cluster->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &clusterArray = *fListOfClusters;
   new(clusterArray[clusterArray.GetEntriesFast()]) TAITcluster(*cluster);
   fMeanEltsN  += cluster->GetPixelsN();
   fMeanCharge += cluster->GetCharge();
}
