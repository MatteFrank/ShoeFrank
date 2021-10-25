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


ClassImp(TAMSDtrack) // Description of a Track

//______________________________________________________________________________
//  
TAMSDtrack::TAMSDtrack()
:  TAGbaseTrack()
{
   SetupClones();
}

//______________________________________________________________________________
//  
TAMSDtrack::~TAMSDtrack()
{
}

//______________________________________________________________________________
//
TAMSDtrack::TAMSDtrack(const TAGbaseTrack& aTrack)
: TAGbaseTrack::TAGbaseTrack(aTrack)
{
   
}

//______________________________________________________________________________
//
void TAMSDtrack::SetupClones()
{
   fListOfClusters = new TClonesArray("TAMSDpoint");
   fListOfClusters->SetOwner(true);
}

// __________________________________________________________________________
//
void TAMSDtrack::AddCluster(TAMSDpoint* point)
{
   for (Int_t k = 0; k < point->GetMcTracksN(); ++k) {
      Int_t idx = point->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &pointArray = *fListOfClusters;
   new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint(*point);
   fMeanEltsN  += point->GetElementsN();
   fMeanCharge += point->GetEnergyLoss();
}
