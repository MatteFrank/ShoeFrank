////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAITcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAITcluster.hxx"
#include "TAITtrack.hxx"
#include "TAIThit.hxx"

ClassImp(TAITcluster) // Description of a cluster

//______________________________________________________________________________
//  
TAITcluster::TAITcluster()
:  TAVTbaseCluster()
{
   // TAITcluster constructor
   SetupClones();
}

//______________________________________________________________________________
//
void TAITcluster::SetupClones()
{
   fListOfPixels = new TClonesArray("TAIThit");
   fListOfPixels->SetOwner(true);
}

//______________________________________________________________________________
//  
TAITcluster::TAITcluster(const TAITcluster& cluster)
:  TAVTbaseCluster(cluster)
{
}

//______________________________________________________________________________
//  
TAITcluster::~TAITcluster()
{ 
   // TAITcluster default destructor
}

//______________________________________________________________________________
//
Float_t TAITcluster::Distance(TAITtrack *aTrack) {
   // Return the distance between this cluster and the pointed track impact in the plane
   //
   
   TVector3 impactPosition( aTrack->Intersection( GetPositionG()[2]) );
   impactPosition -= GetPositionG();
   // Insure that z position is 0 for 2D length computation
   impactPosition.SetXYZ(impactPosition(0), impactPosition(1), 0.);
   
   return impactPosition.Mag();
}


//______________________________________________________________________________
//  
void TAITcluster::AddPixel(TAIThit* pixel)
{
   for (Int_t k = 0; k < pixel->GetMcTracksN(); ++k) {
      Int_t idx = pixel->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &pixelArray = *fListOfPixels;
   new(pixelArray[pixelArray.GetEntriesFast()]) TAIThit(*pixel);
   
   fElementsN = fListOfPixels->GetEntries();
}



