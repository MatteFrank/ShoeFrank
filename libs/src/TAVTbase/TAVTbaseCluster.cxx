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

//! Class Imp
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
//! Compute cluster size (lines columns)
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
//! Set position in detector framework
//!
//! \param[in] posGlo cluster position
void TAVTbaseCluster::SetPositionG(TVector3& posGlo)
{
   fPosition2.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   fPosError2.SetXYZ(fPosError1.X(), fPosError1.Y(), 0.01);
}

//______________________________________________________________________________
//! Get pixel for a given index
//!
//! \param[in] idx pixel index
TAVThit* TAVTbaseCluster::GetPixel(Int_t idx)                    
{ 
   if (idx >=0 && idx < fListOfPixels->GetEntries())
	  return (TAVThit*)fListOfPixels->At(idx); 
   else
	  return 0x0;
}

//______________________________________________________________________________
//! Compute distance between a given pixel and the seed one (index = 0) in U direction
//!
//! \param[in] index pixel index
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
//! Compute distance between a given pixel and the seed one (index = 0) in V direction
//!
//! \param[in] index pixel index
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
//! Get Seed pixel position in U direction
//!
//! Old method taken index = 0
Float_t TAVTbaseCluster::GetSeedU() const 
{ 
   TAVTbaseHit* pixelSeed = (TAVTbaseHit*)fListOfPixels->At(0);
   return pixelSeed->GetPosition().Px();
}

//______________________________________________________________________________
//! Get Seed pixel position in V direction
//!
//! Old method taken index = 0
Float_t TAVTbaseCluster::GetSeedV() const 
{ 
   TAVTbaseHit* pixelSeed = (TAVTbaseHit*)fListOfPixels->At(0);
   return pixelSeed->GetPosition().Py();
}

//______________________________________________________________________________
//! Compute distance to a given cluster
//!
//! Return the distance between this clusters and the given cluster
//! regardless of the plane
//! \param[in] aClus a given cluster
Float_t TAVTbaseCluster::Distance(TAGcluster* aClus)
{
   TVector3 clusPosition( aClus->GetPositionG() );
   
   // Now compute the distance beetween the two hits
   clusPosition -= (GetPositionG());
   
   // Insure that z position is 0 for 2D length computation
   clusPosition.SetXYZ( clusPosition[0], clusPosition[1], 0.);
   
   return clusPosition.Mag();
}

//______________________________________________________________________________
//! Clear pixels list
void TAVTbaseCluster::ResetPixels()
{
   fListOfPixels->Delete();
}
