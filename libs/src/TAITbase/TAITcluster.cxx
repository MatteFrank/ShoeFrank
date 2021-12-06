/*!
 \file TAITcluster.cxx
 \brief   Class for ITR cluster
 */

#include "TAITcluster.hxx"
#include "TAITtrack.hxx"
#include "TAIThit.hxx"

/*!
 \class TAITcluster
 \brief  Class for ITR cluster
 */

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



