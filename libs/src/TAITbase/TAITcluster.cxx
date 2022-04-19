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

//! Class Imp
ClassImp(TAITcluster) // Description of a cluster

//______________________________________________________________________________
//! Constructor
TAITcluster::TAITcluster()
:  TAVTbaseCluster()
{
   // TAITcluster constructor
   SetupClones();
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] cluster cluster to copy
TAITcluster::TAITcluster(const TAITcluster& cluster)
:  TAVTbaseCluster(cluster)
{
}

//______________________________________________________________________________
//! Default destructor
TAITcluster::~TAITcluster()
{ 
   
}

//______________________________________________________________________________
//! Set up clones
void TAITcluster::SetupClones()
{
   fListOfPixels = new TClonesArray("TAIThit");
   fListOfPixels->SetOwner(true);
}

//______________________________________________________________________________
//! Add pixel to list
//!
//! \param[in] pixel pixel to add
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



