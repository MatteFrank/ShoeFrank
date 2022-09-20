
/*!
 \file TAMSDhit.cxx
 \brief   Implementation of TAMSDhit.
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

#include "TAMSDhit.hxx"

/*!
 \class TAMSDhit
 \brief Hit for MSD detectors. **
 */

//! Class imp
ClassImp(TAMSDhit) // Description of Single Detector TAMSDhit 

//______________________________________________________________________________
//!  build the hit from the index
TAMSDhit::TAMSDhit()
 : TAGobject(),
   fSensorId(0),
   fPosition(0),
   fEnergyLoss(0.),
   fIndex(0),
   fView(0),
   fStrip(0),
   fIsSeed(false)
{
}

//______________________________________________________________________________
//!  build hit
//!
//! \param[in] input sensor index
//! \param[in] value measured charge
//! \param[in] view plane view id
//! \param[in] strip strip id
TAMSDhit::TAMSDhit( Int_t input, Float_t value, Int_t view, Int_t strip)
 : TAGobject(),
   fSensorId(input),
   fPosition(0),
   fEnergyLoss(value),
   fIndex(0),
   fView(view),
   fStrip(strip),
   fIsSeed(false)
{
}

//______________________________________________________________________________
//!  build hit from copy
//!
//! \param[in] hit hit to copy
TAMSDhit::TAMSDhit(const TAMSDhit& hit)
 : TAGobject(hit),
   fSensorId(hit.fSensorId),
   fPosition(hit.fPosition),
   fEnergyLoss(hit.fEnergyLoss),
   fIndex(hit.fIndex),
   fView(hit.fView),
   fStrip(hit.fStrip),
   fIsSeed(hit.fIsSeed)
{
}

//______________________________________________________________________________
//! Clear
void TAMSDhit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//!  Compare to a hit
//!
//! \param[in] hit hit to compare
Bool_t TAMSDhit::IsEqual(const TObject* hit) const
{
   return ((fSensorId == ((TAMSDhit*)hit)->fSensorId) &&
           (fView     == ((TAMSDhit*)hit)->fView)     &&
           (fStrip    == ((TAMSDhit*)hit)->fStrip)
           );
}

//______________________________________________________________________________
//! Add MC track and hit indexes
//!
//! \param[in] trackId MC track index
//! \param[in] mcId MC hit index
void TAMSDhit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}


//______________________________________________________________________________
//!  Compare to a hit
//!
//! \param[in] obj hit to compare
Int_t TAMSDhit::Compare(const TObject* obj) const
{
   Int_t view = fView;
   Int_t aView = ((TAMSDhit *)obj)->GetView();
   Int_t strip = fStrip;
   Int_t aStrip = ((TAMSDhit *)obj)->GetStrip();
   
   if (view == aView) { //Order ok then order for column
      if(strip<aStrip)
         return -1;
      else
         return 1;
   } else if(view > aView)
      return 1;
   else
      return -1;
}
