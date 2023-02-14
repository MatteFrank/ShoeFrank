/*!
 \file TAVTbaseHit.cxx
  \brief   Base class of VTX hits
 */

#include "TString.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include "TAVTntuHit.hxx"
#include "TAVTbaseHit.hxx"

/*!
 \class TAVTbaseHit
 \brief  Base class of VTX hits
 */

//! Class Imp
ClassImp(TAVTbaseHit)

//______________________________________________________________________________
//! Standard constructor
TAVTbaseHit::TAVTbaseHit()
 : TAGobject(),
   fSensorId(-1),
   fPixelIndex(-1),
   fPixelLine(0),
   fPixelColumn(0),
   fRawValue(0),
   fPulseHeight(0),
   fValidFrames(false),
   fPileUp(false)
{
}

//______________________________________________________________________________
//!  build the hit from the index and value for a given sensor
//!
//! \param[in] sensorId sensor index
//! \param[in] aPixelIndex pixel index
//! \param[in] aValue pixel value
TAVTbaseHit::TAVTbaseHit( Int_t sensorId, const Int_t aPixelIndex, Double_t aValue)
: TAGobject(),
  fSensorId(sensorId),
  fPixelIndex(aPixelIndex),
  fPixelLine(0),
  fPixelColumn(0),
  fRawValue(aValue),
  fValidFrames(true),
  fPileUp(false)
{
    fPulseHeight = fRawValue;
}

//______________________________________________________________________________
//! Build the pixel from its sensor, line and column
//!
//! \param[in] sensorId sensor index
//! \param[in] aValue pixel value
//! \param[in] aLine line number
//! \param[in] aColumn column number
TAVTbaseHit::TAVTbaseHit( Int_t sensorId, Double_t aValue, Int_t aLine, Int_t aColumn )
: TAGobject(),
  fSensorId(sensorId),
  fPixelIndex(0),
  fPixelLine(aLine),
  fPixelColumn(aColumn),
  fRawValue(aValue),
  fPileUp(false),
  fValidFrames(true)
{
    fPulseHeight = fRawValue;
}

//______________________________________________________________________________
//! Destructor
TAVTbaseHit::~TAVTbaseHit()
{
}

//______________________________________________________________________________
//! Clear
void TAVTbaseHit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//! Compute distance to a position
//!
//! \param[in] aPosition a given position
Double_t TAVTbaseHit::Distance(const TVector3& aPosition)
{
   TVector3 result(fPosition);
   result -= aPosition; 
   return result.Perp();
}

//______________________________________________________________________________
//! Compute distance to a position in U direction
//!
//! \param[in] aPosition a given position
Double_t TAVTbaseHit::DistanceU(const TVector3& aPosition)
{
   TVector3 result(fPosition);
   result -= aPosition; 
   return result.x();
}

//______________________________________________________________________________
//! Compute distance to a position in V direction
//!
//! \param[in] aPosition a given position
Double_t TAVTbaseHit::DistanceV(const TVector3& aPosition)
{
   TVector3 result(fPosition);
   result -= aPosition; 
   return result.y();
}

//______________________________________________________________________________
//! Add MC track and hit indexes
//!
//! \param[in] trackId MC track index
//! \param[in] mcId MC hit index
void TAVTbaseHit::AddMcTrackIdx(Int_t trackId,Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}

//______________________________________________________________________________
//! Equal with a given hit
//!
//! \param[in] hit a given hit
Bool_t TAVTbaseHit::IsEqual(const TObject* hit) const
{
   return ((fSensorId    == ((TAVTbaseHit*)hit)->fSensorId)    &&
           (fPixelLine   == ((TAVTbaseHit*)hit)->fPixelLine)   &&
           (fPixelColumn == ((TAVTbaseHit*)hit)->fPixelColumn)
           );
}
