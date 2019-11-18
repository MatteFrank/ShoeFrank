////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAVTcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAVTcluster.hxx"
#include "TAVTtrack.hxx"

ClassImp(TAVTcluster) // Description of a cluster

//______________________________________________________________________________
//  
TAVTcluster::TAVTcluster()
:  TAVTbaseCluster()
{
   // TAVTcluster constructor
   SetupClones();
}

//______________________________________________________________________________
//
void TAVTcluster::SetupClones()
{
   fListOfPixels = new TClonesArray("TAVTntuHit");
   fListOfPixels->SetOwner(true);
}

//______________________________________________________________________________
//  
TAVTcluster::TAVTcluster(const TAVTcluster& cluster)
:  TAVTbaseCluster(cluster)
{
}

//______________________________________________________________________________
//  
TAVTcluster::~TAVTcluster()
{ 
   // TAVTcluster default destructor
}



//______________________________________________________________________________
//  
Float_t TAVTcluster::Distance( TAVTtrack *aTrack) {
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
void TAVTcluster::AddPixel(TAVTntuHit* pixel)
{
   for (Int_t k = 0; k < pixel->GetMcTracksN(); ++k) {
      Int_t idx = pixel->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }

   TClonesArray &pixelArray = *fListOfPixels;
   new(pixelArray[pixelArray.GetEntriesFast()]) TAVTntuHit(*pixel);
}



