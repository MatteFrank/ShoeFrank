/*!
 \file TAMPcluster.cxx
 \brief   Class for ITR cluster
 */

#include "TAMPcluster.hxx"
#include "TAMPhit.hxx"

/*!
 \class TAMPcluster
 \brief  Class for ITR cluster
 */

//! Class Imp
ClassImp(TAMPcluster) // Description of a cluster

//______________________________________________________________________________
//! Constructor
TAMPcluster::TAMPcluster()
:  TAVTbaseCluster()
{
   // TAMPcluster constructor
   SetupClones();
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] cluster cluster to copy
TAMPcluster::TAMPcluster(const TAMPcluster& cluster)
:  TAVTbaseCluster(cluster)
{
}

//______________________________________________________________________________
//! Default destructor
TAMPcluster::~TAMPcluster()
{ 
   
}

//______________________________________________________________________________
//! Set up clones
void TAMPcluster::SetupClones()
{
   fListOfPixels = new TClonesArray("TAMPhit");
   fListOfPixels->SetOwner(true);
}

//______________________________________________________________________________
//! Add pixel to list
//!
//! \param[in] pixel pixel to add
void TAMPcluster::AddPixel(TAMPhit* pixel)
{
   for (Int_t k = 0; k < pixel->GetMcTracksN(); ++k) {
      Int_t idx = pixel->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &pixelArray = *fListOfPixels;
   new(pixelArray[pixelArray.GetEntriesFast()]) TAMPhit(*pixel);
   
   fElementsN = fListOfPixels->GetEntriesFast();
}



