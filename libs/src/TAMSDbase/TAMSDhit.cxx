
#include "TString.h"
#include "TClonesArray.h"

#include "TAGgeoTrafo.hxx"
#include "GlobalPar.hxx"

#include "TAMSDhit.hxx"

ClassImp(TAMSDhit) // Description of Single Detector TAMSDhit 


//______________________________________________________________________________
//  build the hit from the index
TAMSDhit::TAMSDhit()
 : TAGobject(),
   fSensorId(0),
   fPosition(0),
   fValue(0.),
   fIndex(0),
   fView(0),
   fStrip(0)
{
}

//______________________________________________________________________________
TAMSDhit::TAMSDhit( Int_t input, Float_t value, Int_t view, Int_t strip)
 : TAGobject(),
   fSensorId(input),
   fPosition(0),
   fValue(value),
   fIndex(0),
   fView(view),
   fStrip(strip)
{
}

//______________________________________________________________________________
//
void TAMSDhit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//
Bool_t TAMSDhit::IsEqual(const TObject* hit) const
{
   return ((fSensorId == ((TAMSDhit*)hit)->fSensorId) &&
           (fView     == ((TAMSDhit*)hit)->fView)     &&
           (fStrip    == ((TAMSDhit*)hit)->fStrip)
           );
}

//______________________________________________________________________________
//
void TAMSDhit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}

//______________________________________________________________________________
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
