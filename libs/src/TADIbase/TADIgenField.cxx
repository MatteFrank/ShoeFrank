
//##############################################################################

/*!
 \class TADIgenField TADIgenField.hxx "TADIgenField.hxx"
 \brief  Wrapper for TEve Mag field **
 */

#include "TADIgenField.hxx"
#include "TAGgeoTrafo.hxx"

ClassImp(TADIgenField);

//______________________________________________________________________________
TADIgenField::TADIgenField(TADIgeoField* field)
: AbsBField(),
  fField(field)
{
}

//______________________________________________________________________________
TADIgenField::~TADIgenField()
{
   // Destructor
}

//______________________________________________________________________________
void TADIgenField::get(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const
{
   TVector3 pos(posX, posY, posZ);
   TVector3 outField(0, 0, 0);
   
   // outField = fField->GetField(pos)*TAGgeoTrafo::GausToTesla();
   outField = fField->GetField(pos)*1e-3; //GENFIT expects kGauss
   
   Bx = outField.X();
   By = outField.Y();
   Bz = outField.Z();
   
}
//______________________________________________________________________________
TVector3 TADIgenField::get(const TVector3& position) const
{
//   return fField->GetField(position)*TAGgeoTrafo::GausToTesla();
  return fField->GetField(position)*1e-3; //GENFIT expects kGauss
}
