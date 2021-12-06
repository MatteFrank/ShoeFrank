/*!
 \file TAVTcluster.cxx
 \brief  Class VTX cluster
 */

#include "TAVTcluster.hxx"
#include "TAVTtrack.hxx"

/*!
 \class TAVTcluster
 \brief Class for cluster in VTX
 */

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
   fListOfPixels = new TClonesArray("TAVThit");
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
void TAVTcluster::AddPixel(TAVThit* pixel)
{
   for (Int_t k = 0; k < pixel->GetMcTracksN(); ++k) {
      Int_t idx = pixel->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }

   TClonesArray &pixelArray = *fListOfPixels;
   new(pixelArray[pixelArray.GetEntriesFast()]) TAVThit(*pixel);
   
   fElementsN = fListOfPixels->GetEntries();
}



