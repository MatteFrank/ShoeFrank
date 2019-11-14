////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAVTbaseCluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAVTparGeo.hxx"
#include "TAVTbaseCluster.hxx"
#include "TAVTntuTrack.hxx"

ClassImp(TAVTbaseCluster) // Description of a cluster

//______________________________________________________________________________
//  
TAVTbaseCluster::TAVTbaseCluster()
:  TAGobject(),
   fPosition(new TVector3(0., 0., 0.)),
   fPosError(new TVector3(0., 0., 0.)),
   fPositionG(new TVector3(0., 0., 0.)),
   fListOfPixels(0x0),
   fNumber(0),
   fPlaneNumber(10),
	fFoundXZ(kFALSE),
	fFoundYZ(kFALSE)
{
}

//______________________________________________________________________________
//  
TAVTbaseCluster::TAVTbaseCluster(const TAVTbaseCluster& cluster)
:  TAGobject(),
   fPosition(new TVector3(*cluster.fPosition)),
   fPosError(new TVector3(*cluster.fPosError)),
   fPositionG(new TVector3(*cluster.fPositionG)),
   fNumber(cluster.fNumber),
   fPlaneNumber(cluster.fPlaneNumber),
   fFoundXZ(cluster.fFoundXZ),
   fFoundYZ(cluster.fFoundYZ)
{
   // TAVTbaseCluster constructor
   if (cluster.fListOfPixels)
      fListOfPixels = (TClonesArray*)cluster.fListOfPixels->Clone();
}

//______________________________________________________________________________
//  
TAVTbaseCluster::~TAVTbaseCluster()
{ 
   // TAVTbaseCluster default destructor 
   
   delete fPosition;
   delete fPositionG;
   delete fPosError;
   delete fListOfPixels;
}

//______________________________________________________________________________
//
TVector2 TAVTbaseCluster::ComputeSize()
{
   Int_t minLine = 99999;
   Int_t maxLine = 0;
   
   Int_t minCol = 99999;
   Int_t maxCol = 0;
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntries(); ++iPix) {
      TAVTntuHit* pixel = (TAVTntuHit*)fListOfPixels->At(iPix);
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      
      maxLine = (line > maxLine) ? line : maxLine;
      maxCol  = (col > maxCol)   ? col  : maxCol;
      
      minLine = (line < minLine) ? line : minLine;
      minCol  = (col < minCol)   ? col  : minCol;
   }
   
   Int_t length = (maxLine - minLine) + 1;
   Int_t width  = (maxCol - minCol)   + 1;
   
   return TVector2((float)length, (float)width);
}

//______________________________________________________________________________
//  
void TAVTbaseCluster::SetPosition(TVector3* pos)
{
   fPosition->SetXYZ(pos->Px(), pos->Py(), pos->Pz());
}

//______________________________________________________________________________
//  
void TAVTbaseCluster::SetPosError(TVector3* pos)
{
   fPosError->SetXYZ(pos->Px(), pos->Py(), pos->Pz());
}

//______________________________________________________________________________
//  
void TAVTbaseCluster::SetPositionG(TVector3* posGlo)
{
   fPositionG->SetXYZ(posGlo->Px(), posGlo->Py(), posGlo->Pz());
}

//______________________________________________________________________________
// 
TAVTntuHit* TAVTbaseCluster::GetPixel(Int_t idx)                    
{ 
   if (idx >=0 && idx < fListOfPixels->GetEntries())
	  return (TAVTntuHit*)fListOfPixels->At(idx); 
   else
	  return 0x0;
}


//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetPixelDistanceU(Int_t index) const
{
   TAVTbaseNtuHit* pixelSeed = (TAVTbaseNtuHit*)fListOfPixels->At(0);
   if (index >= 0 && index < fListOfPixels->GetEntries()) {
	  TAVTbaseNtuHit* aNeighbour = (TAVTbaseNtuHit*)fListOfPixels->At(index);
	  return pixelSeed->DistanceU(aNeighbour->GetPosition());
   } else {
	  return -1;
   }   
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetPixelDistanceV(Int_t index) const
{
   TAVTbaseNtuHit* pixelSeed = (TAVTbaseNtuHit*)fListOfPixels->At(0);
   if (index >= 0 && index < fListOfPixels->GetEntries()) {
	  TAVTbaseNtuHit* aNeighbour = (TAVTbaseNtuHit*)fListOfPixels->At(index);
	  return pixelSeed->DistanceV(aNeighbour->GetPosition());
   } else {
	  return -1;
   }   
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetSeedU() const 
{ 
   TAVTbaseNtuHit* pixelSeed = (TAVTbaseNtuHit*)fListOfPixels->At(0);
   return pixelSeed->GetPosition().Px();
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetSeedV() const 
{ 
   TAVTbaseNtuHit* pixelSeed = (TAVTbaseNtuHit*)fListOfPixels->At(0);
   return pixelSeed->GetPosition().Py();
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::Distance(TAVTbaseCluster *aClus) {
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
void TAVTbaseCluster::ResetPixels()
{
   fListOfPixels->Delete();
}

