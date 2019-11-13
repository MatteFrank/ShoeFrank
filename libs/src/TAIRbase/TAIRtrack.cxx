#include "TMath.h"
#include "TClonesArray.h"

#include "GlobalPar.hxx"
#include "TAIRtrack.hxx"


//#################################################################

  //////////////////////////////////////////////////////////////////
  // Class Description of TAIRtrack                               //
  //                                                              //
  // A particle track from e.g. accelerator passing the tracker   //
  // The track is measured by the tracker with its silicon        //
  // reference planes                                             //
  // The track is e.g. a straight line                            //
  // The line is found by a fit to the hits in the silicon planes // 
  //                                                              //
  //////////////////////////////////////////////////////////////////


ClassImp(TAIRtrack) // Description of a Track

//______________________________________________________________________________
//  
TAIRtrack::TAIRtrack()
:  TAVTbaseTrack()
{
   SetupClones();
}

//______________________________________________________________________________
//  
TAIRtrack::~TAIRtrack()
{
}

//______________________________________________________________________________
//
TAIRtrack::TAIRtrack(const TAIRtrack& aTrack)
: TAVTbaseTrack::TAVTbaseTrack(aTrack)
{
   
}

//______________________________________________________________________________
//
void TAIRtrack::SetupClones()
{
   fListOfClusters = new TClonesArray("TAVTbaseCluster");
   fListOfClusters->SetOwner(true);
}


// __________________________________________________________________________
//
void TAIRtrack::AddCluster(TAVTbaseCluster* cluster)
{
   TClonesArray &clusterArray = *fListOfClusters;
   new(clusterArray[clusterArray.GetEntriesFast()]) TAVTbaseCluster(*cluster);
   fMeanPixelsN += cluster->GetPixelsN()/float(fListOfClusters->GetEntries());
}
