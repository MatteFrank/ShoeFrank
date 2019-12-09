
#include "TString.h"
#include "TClonesArray.h"

#include "TAGgeoTrafo.hxx"
#include "GlobalPar.hxx"

#include "TAMSDntuHit.hxx"

ClassImp(TAMSDntuHit) // Description of Single Detector TAMSDntuHit 


//______________________________________________________________________________
//  build the hit from the index
TAMSDntuHit::TAMSDntuHit()
 : TAGobject(),
   fSensorId(0),
   fPosition(0),
   // fValue(0.),
   fIndex(0),
   fView(0),
   fStrip(0)
{
}

//______________________________________________________________________________
TAMSDntuHit::TAMSDntuHit( Int_t input, Int_t view, Int_t strip)
 : TAGobject(),
   fSensorId(input),
   fPosition(0),
   // fValue(value),
   fIndex(0),
   fView(view),
   fStrip(strip)
{
}

//______________________________________________________________________________
//
void TAMSDntuHit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//
Bool_t TAMSDntuHit::IsEqual(const TObject* hit) const
{
   return ((fSensorId == ((TAMSDntuHit*)hit)->fSensorId) &&
           (fView     == ((TAMSDntuHit*)hit)->fView)     &&
           (fStrip    == ((TAMSDntuHit*)hit)->fStrip)
           );
}

//______________________________________________________________________________
//
void TAMSDntuHit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}

//______________________________________________________________________________
Int_t TAMSDntuHit::Compare(const TObject* obj) const
{
   Int_t view = fView;
   Int_t aView = ((TAMSDntuHit *)obj)->GetView();
   Int_t strip = fStrip;
   Int_t aStrip = ((TAMSDntuHit *)obj)->GetStrip();
   
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
