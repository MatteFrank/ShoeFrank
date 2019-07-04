
//##############################################################################

/*!
 \class TADIeveField TADIeveField.hxx "TADIeveField.hxx"
 \brief  Wrapper for TEve Mag field **
 */

#include "TADIeveField.hxx"

ClassImp(TADIeveField);

const Float_t TADIeveField::Tesla = 1e-4;

//______________________________________________________________________________
TADIeveField::TADIeveField(FootField* field)
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
   
   fieldB = fField->get(pos);
   
   return TEveVectorD(fieldB[0]*Tesla, fieldB[1]*Tesla, fieldB[2]*Tesla);
}
