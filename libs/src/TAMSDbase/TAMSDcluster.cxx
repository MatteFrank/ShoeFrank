////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAMSDcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAMSDcluster.hxx"
#include "TAMSDntuHit.hxx"

ClassImp(TAMSDcluster) // Description of a cluster

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster()
:  TAGobject(),
   fPosition(0.),
   fPosError(0),
   fPositionG(new TVector3(0., 0., 0.)),
   fPlaneNumber(10)
{
   // TAMSDcluster constructor
   SetupClones();
   fMcTrackIdx.Reset();
   fMcTrackMap.clear();
}

//______________________________________________________________________________
//
void TAMSDcluster::SetupClones()
{
   fListOfStrips = new TClonesArray("TAMSDntuHit");
   fListOfStrips->SetOwner(true);
}

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster(const TAMSDcluster& cluster)
:  TAGobject(),
   fPosition(cluster.fPosition),
   fPosError(cluster.fPosError),
   fPositionG(new TVector3(*cluster.fPositionG)),
   fPlaneNumber(cluster.fPlaneNumber),
   fMcTrackIdx(cluster.fMcTrackIdx)
{
   fListOfStrips = (TClonesArray*)cluster.fListOfStrips->Clone();
}

//______________________________________________________________________________
//  
TAMSDcluster::~TAMSDcluster()
{ 
   // TAMSDcluster default destructor
   delete fPositionG;
   delete fListOfStrips;
}


//______________________________________________________________________________
//  
void TAMSDcluster::AddPixel(TAMSDntuHit* pixel)
{
   for (Int_t k = 0; k < pixel->GetMcTrackCount(); ++k) {
      Int_t idx = pixel->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &pixelArray = *fListOfStrips;
   new(pixelArray[pixelArray.GetEntriesFast()]) TAMSDntuHit(*pixel);
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPositionG(TVector3* posGlo)
{
   fPositionG->SetXYZ(posGlo->Px(), posGlo->Py(), posGlo->Pz());
}

//______________________________________________________________________________
//
TAMSDntuHit* TAMSDcluster::GetStrip(Int_t idx)
{
   if (idx >=0 && idx < fListOfStrips->GetEntries())
      return (TAMSDntuHit*)fListOfStrips->At(idx);
   else
      return 0x0;
}
//______________________________________________________________________________
//
Float_t TAMSDcluster::Distance(TAMSDcluster *aClus) {
   // Return the distance between this clusters and the pointed cluster
   // regardless of the plane
   
   TVector3 clusPosition( aClus->GetPositionG() );
   
   // Now compute the distance beetween the two hits
   clusPosition -= (GetPositionG());
   
   // Insure that z position is 0 for 2D length computation
   clusPosition.SetXYZ( clusPosition[0], clusPosition[1], 0.);
   
   return clusPosition.Mag();
}

//______________________________________________________________________________
//
void TAMSDcluster::AddMcTrackIdx(Int_t trackIdx)
{
   if (fMcTrackMap[trackIdx] == 0) {
      fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
      fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackIdx;
      fMcTrackMap[trackIdx] = 1;
   }
}
