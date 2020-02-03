
//##############################################################################

/*!
 \class TADIeveField TADIeveField.hxx "TADIeveField.hxx"
 \brief  Wrapper for TEve Mag field **
 */

#include "TADIeveField.hxx"
#include "TAGgeoTrafo.hxx"

ClassImp(TADIeveField);

//______________________________________________________________________________
TADIeveField::TADIeveField(TADIgeoField* field)
: TEveMagField(),
  fField(field)
{
}

//______________________________________________________________________________
TADIeveField::~TADIeveField()
{
   // Destructor
}

//______________________________________________________________________________
TEveVectorD TADIeveField::GetFieldD(Double_t x, Double_t y, Double_t z) const
{
   TVector3 pos(x, y, z);
   TVector3 fieldB(0, 0, 0);
   
   fieldB = fField->GetField(pos)*TAGgeoTrafo::GausToTesla();
   
   return TEveVectorD(-fieldB[0], -fieldB[1], fieldB[2]);
}
