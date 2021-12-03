
/*!
 \file  TAIThit.cxx
 \version $Id: TAVThit
 \brief Class for ITR hit
 */

#include "TAIThit.hxx"

/*!
 \class TAIThit
 \brief  Class for ITR hit
 */

ClassImp(TAIThit) // Description of Single Detector TAIThit 

//______________________________________________________________________________
//  build the hit from the index
TAIThit::TAIThit()
: TAVTbaseHit()
{
}

//______________________________________________________________________________
//  build the hit from the index
TAIThit::TAIThit( Int_t aSensorNumber, const Int_t aPixelIndex, Double_t aValue)
: TAVTbaseHit(aSensorNumber, aPixelIndex, aValue)
{
}

//______________________________________________________________________________
// Build the pixel from its sensor, line and column// constructor of a Pixel with column and line 
TAIThit::TAIThit( Int_t aSensorNumber, Double_t aValue, Int_t aLine, Int_t aColumn )
: TAVTbaseHit(aSensorNumber, aValue, aLine, aColumn)
{
}
