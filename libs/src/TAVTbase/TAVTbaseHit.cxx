
#include "TString.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include "TAVTntuHit.hxx"
#include "TAVTbaseHit.hxx"

ClassImp(TAVTbaseHit) // Description of Single Detector TAVTbaseHit 


//______________________________________________________________________________
//  build the hit from the index
TAVTbaseHit::TAVTbaseHit( Int_t aSensorNumber, const Int_t aPixelIndex, Double_t aValue)
: TAGobject(),
  fSensorId(aSensorNumber),
  fPixelIndex(aPixelIndex),
  fPixelLine(0),
  fPixelColumn(0),
  fRawValue(aValue)
{
    fPulseHeight = fRawValue;
}

//______________________________________________________________________________
// Build the pixel from its sensor, line and column// constructor of a Pixel with column and line 
TAVTbaseHit::TAVTbaseHit( Int_t aSensorNumber, Double_t aValue, Int_t aLine, Int_t aColumn )
: TAGobject(),
  fSensorId(aSensorNumber),
  fPixelIndex(0),
  fPixelLine(aLine),
  fPixelColumn(aColumn),
  fRawValue(aValue),
  fValidFrames(true)
{
    fPulseHeight = fRawValue;
}

//______________________________________________________________________________
//
TAVTbaseHit::~TAVTbaseHit()
{
}

//______________________________________________________________________________
//
void TAVTbaseHit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//  
Double_t TAVTbaseHit::Distance(const TVector3& aPosition)
{
   TVector3 result(fPosition);
   result -= aPosition; 
   return result.Perp();
}

//______________________________________________________________________________
//  
Double_t TAVTbaseHit::DistanceU(const TVector3& aPosition)
{
   TVector3 result(fPosition);
   result -= aPosition; 
   return result.x();
}

//______________________________________________________________________________
//  
Double_t TAVTbaseHit::DistanceV(const TVector3& aPosition)
{
   TVector3 result(fPosition);
   result -= aPosition; 
   return result.y();
}

//______________________________________________________________________________
//
void TAVTbaseHit::AddMcTrackIdx(Int_t trackId,Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}

//______________________________________________________________________________
//
Bool_t TAVTbaseHit::IsEqual(const TObject* hit) const
{
   return ((fSensorId    == ((TAVTbaseHit*)hit)->fSensorId)    &&
           (fPixelLine   == ((TAVTbaseHit*)hit)->fPixelLine)   &&
           (fPixelColumn == ((TAVTbaseHit*)hit)->fPixelColumn)
           );
}
