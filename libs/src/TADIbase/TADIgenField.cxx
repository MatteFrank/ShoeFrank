
//##############################################################################

/*!
 \class TADIgenField TADIgenField.hxx "TADIgenField.hxx"
 \brief  Wrapper for TEve Mag field **
 */

#include "TADIgenField.hxx"
#include "TAGgeoTrafo.hxx"

ClassImp(TADIgenField);

//______________________________________________________________________________
//! Constructor
TADIgenField::TADIgenField(TADIgeoField* field)
: AbsBField(),
  fField(field)
{
}

//______________________________________________________________________________
//! Destructor
TADIgenField::~TADIgenField()
{
}

//______________________________________________________________________________
void TADIgenField::get(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const
{
   TVector3 pos(posX, posY, posZ);
   TVector3 outField(0, 0, 0);
   
   outField = fField->GetField(pos)*TAGgeoTrafo::GausToKGaus(); //GENFIT expects kGauss
   
   Bx = outField.X();
   By = outField.Y();
   Bz = outField.Z();
   
}
//______________________________________________________________________________
TVector3 TADIgenField::get(const TVector3& position) const
{
  return fField->GetField(position)*TAGgeoTrafo::GausToKGaus(); //GENFIT expects kGauss
}


//______________________________________________________________________________
void TADIgenField::getGauss(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const
{
   TVector3 pos(posX, posY, posZ);
   TVector3 outField(0, 0, 0);
   
   outField = fField->GetField(pos); 
   
   Bx = outField.X();
   By = outField.Y();
   Bz = outField.Z();
   
}
//______________________________________________________________________________
TVector3 TADIgenField::getGauss(const TVector3& position) const
{
  return fField->GetField(position); 
}
