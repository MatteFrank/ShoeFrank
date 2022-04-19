/*!
 \file TADIgenField.cxx
 \brief Implementation of TADIgenField.cxx
 */

#include "TADIgenField.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TADIgenField
 \brief  Wrapper for TEve Mag field **
 */

//! Class Imp
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
//! Get field in kGauss vector for a given position
//!
//! \param[in] posX position in X-direction
//! \param[in] posY position in Y-direction
//! \param[in] posZ position in Z-direction
//! \param[out] Bx field in X-direction
//! \param[out] By field in Y-direction
//! \param[out] Bz field in Z-direction
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
//! Return field vector in kGauss for a given position
//!
//! \param[in] position position
//! \return field in kGauss
TVector3 TADIgenField::get(const TVector3& position) const
{
  return fField->GetField(position)*TAGgeoTrafo::GausToKGaus(); //GENFIT expects kGauss
}

//______________________________________________________________________________
//! Get field vector in Gauss for a given position
//!
//! \param[in] posX position in X-direction
//! \param[in] posY position in Y-direction
//! \param[in] posZ position in Z-direction
//! \param[out] Bx field in X-direction
//! \param[out] By field in Y-direction
//! \param[out] Bz field in Z-direction
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
//! Return field vector in kGauss for a given position
//!
//! \param[in] position position
//! \return field in Gauss
TVector3 TADIgenField::getGauss(const TVector3& position) const
{
  return fField->GetField(position); 
}
