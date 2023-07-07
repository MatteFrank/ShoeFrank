
/*!
 \file  TAMPhit.cxx
 \brief Class for ITR hit
 */

#include "TAMPhit.hxx"

/*!
 \class TAMPhit
 \brief  Class for ITR hit
 */

//! Class Imp
ClassImp(TAMPhit) // Description of Single Detector TAMPhit

//______________________________________________________________________________
//!  build the hit from the index
TAMPhit::TAMPhit()
: TAVTbaseHit()
{
}

//______________________________________________________________________________
//!  build the hit from the index and value for a given sensor
//!
//! \param[in] aSensorNumber sensor index
//! \param[in] aPixelIndex pixel index
//! \param[in] aValue pixel value
TAMPhit::TAMPhit( Int_t aSensorNumber, const Int_t aPixelIndex, Double_t aValue)
: TAVTbaseHit(aSensorNumber, aPixelIndex, aValue)
{
}

//______________________________________________________________________________
//! Build the pixel from its sensor, line and column
//!
//! \param[in] aSensorNumber sensor index
//! \param[in] aValue pixel value
//! \param[in] aLine line number
//! \param[in] aColumn column number
TAMPhit::TAMPhit( Int_t aSensorNumber, Double_t aValue, Int_t aLine, Int_t aColumn )
: TAVTbaseHit(aSensorNumber, aValue, aLine, aColumn)
{
}
