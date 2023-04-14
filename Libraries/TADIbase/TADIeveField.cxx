
/*!
 \file TADIeveField.cxx
 \brief Implementation of TADIeveField.cxx
 */

#include "TADIeveField.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TADIeveField
 \brief  Wrapper for TEve Mag field **
 */

//! Class Imp
ClassImp(TADIeveField);

//______________________________________________________________________________
//! Default constructor
//!
//! \param[in] field geomtry field
TADIeveField::TADIeveField(TADIgeoField* field)
: TEveMagField(),
  fField(field)
{
}

//______________________________________________________________________________
//! Destructor
TADIeveField::~TADIeveField()
{
}

//______________________________________________________________________________
//! Return Eve vector for a given position
//!
//! \param[in] x position in X-direction
//! \param[in] y position in Y-direction
//! \param[in] z position in Z-direction
TEveVectorD TADIeveField::GetFieldD(Double_t x, Double_t y, Double_t z) const
{
   TVector3 pos(x, y, z);
   TVector3 fieldB(0, 0, 0);
   
   fieldB = fField->GetField(pos)*TAGgeoTrafo::GausToTesla();
   
   return TEveVectorD(-fieldB[0], -fieldB[1], fieldB[2]);
}
