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

//! Class Imp
ClassImp(TAVTcluster) // Description of a cluster

//______________________________________________________________________________
//!  constructor
TAVTcluster::TAVTcluster()
:  TAVTbaseCluster()
{
 
   SetupClones();
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] cluster cluster to copy
TAVTcluster::TAVTcluster(const TAVTcluster& cluster)
:  TAVTbaseCluster(cluster)
{
}

//______________________________________________________________________________
//! default destructor
TAVTcluster::~TAVTcluster()
{ 
}

//______________________________________________________________________________
//! Set up clones
void TAVTcluster::SetupClones()
{
   fListOfPixels = new TClonesArray("TAVThit");
   fListOfPixels->SetOwner(true);
}

//______________________________________________________________________________
//! Add pixel to list
//!
//! \param[in] pixel pixel to add
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



