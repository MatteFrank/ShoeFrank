/*!
 \file TAVTbaseCluster.cxx
 \brief Base class for VTX cluster containers
 */

#include "TAVTparGeo.hxx"
#include "TAVTbaseCluster.hxx"
#include "TAVTntuTrack.hxx"


/*!
 \class TAVTbaseCluster
 \brief Base class for VTX cluster containers
 */

ClassImp(TAVTbaseCluster) // Description of a cluster

//______________________________________________________________________________
//! Default constructor
TAVTbaseCluster::TAVTbaseCluster()
:  TAGcluster(),
   fListOfPixels(0x0),
   fCharge(0.)
{
}

//______________________________________________________________________________
//! Copy constructor
TAVTbaseCluster::TAVTbaseCluster(const TAVTbaseCluster& cluster)
:  TAGcluster(cluster),
   fCharge(cluster.fCharge)
{
   // TAVTbaseCluster constructor
   if (cluster.fListOfPixels)
      fListOfPixels = (TClonesArray*)cluster.fListOfPixels->Clone();
}

//______________________________________________________________________________
//! Default destructor 
TAVTbaseCluster::~TAVTbaseCluster()
{
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
      TAVThit* pixel = (TAVThit*)fListOfPixels->At(iPix);
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
void TAVTbaseCluster::SetPositionG(TVector3& posGlo)
{
   fPosition2.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   fPosError2.SetXYZ(fPosError1.X(), fPosError1.Y(), 0.01);
}

//______________________________________________________________________________
// 
TAVThit* TAVTbaseCluster::GetPixel(Int_t idx)                    
{ 
   if (idx >=0 && idx < fListOfPixels->GetEntries())
	  return (TAVThit*)fListOfPixels->At(idx); 
   else
	  return 0x0;
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetPixelDistanceU(Int_t index) const
{
   TAVTbaseHit* pixelSeed = (TAVTbaseHit*)fListOfPixels->At(0);
   if (index >= 0 && index < fListOfPixels->GetEntries()) {
	  TAVTbaseHit* aNeighbour = (TAVTbaseHit*)fListOfPixels->At(index);
	  return pixelSeed->DistanceU(aNeighbour->GetPosition());
   } else {
	  return -1;
   }   
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetPixelDistanceV(Int_t index) const
{
   TAVTbaseHit* pixelSeed = (TAVTbaseHit*)fListOfPixels->At(0);
   if (index >= 0 && index < fListOfPixels->GetEntries()) {
	  TAVTbaseHit* aNeighbour = (TAVTbaseHit*)fListOfPixels->At(index);
	  return pixelSeed->DistanceV(aNeighbour->GetPosition());
   } else {
	  return -1;
   }   
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetSeedU() const 
{ 
   TAVTbaseHit* pixelSeed = (TAVTbaseHit*)fListOfPixels->At(0);
   return pixelSeed->GetPosition().Px();
}

//______________________________________________________________________________
//  
Float_t TAVTbaseCluster::GetSeedV() const 
{ 
   TAVTbaseHit* pixelSeed = (TAVTbaseHit*)fListOfPixels->At(0);
   return pixelSeed->GetPosition().Py();
}

//______________________________________________________________________________
//  
//Float_t TAVTbaseCluster::Distance(TAGcluster *aClus) {
//   // Return the distance between this clusters and the pointed cluster
//   // regardless of the plane
//   
//   TVector3 clusPosition( aClus->GetPositionG() );
//   
//   // Now compute the distance beetween the two hits
//   clusPosition -= (GetPositionG());
//   
//   // Insure that z position is 0 for 2D length computation
//   clusPosition.SetXYZ( clusPosition[0], clusPosition[1], 0.);
//   
//   return clusPosition.Mag();
//}


//______________________________________________________________________________
//  
void TAVTbaseCluster::ResetPixels()
{
   fListOfPixels->Delete();
}
