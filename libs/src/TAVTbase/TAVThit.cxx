/*!
 \file  TAVThit.cxx
 \version $Id: TAVThit
 \brief Class for VTX hit
 */

#include "TAVThit.hxx"

/*!
 \class TAVThit
 \brief  Class for VTX hit
 */

ClassImp(TAVThit) // Description of Single Detector TAVThit 


//______________________________________________________________________________
//  build the hit from the index
TAVThit::TAVThit()
: TAVTbaseHit()
{
}

//______________________________________________________________________________
//  build the hit from the index
TAVThit::TAVThit( Int_t aSensorNumber, const Int_t aPixelIndex, Double_t aValue)
: TAVTbaseHit(aSensorNumber, aPixelIndex, aValue)
{
}

//______________________________________________________________________________
// Build the pixel from its sensor, line and column// constructor of a Pixel with column and line 
TAVThit::TAVThit( Int_t aSensorNumber, Double_t aValue, Int_t aLine, Int_t aColumn )
: TAVTbaseHit(aSensorNumber, aValue, aLine, aColumn)
{
}
