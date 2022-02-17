#ifndef _TADIgenField_HXX
#define _TADIgenField_HXX
/*!
  \file TADIgenField.hxx
  \brief   Declaration of TADIgenField.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "AbsBField.h"
#include "TADIgeoField.hxx"


//##############################################################################
using namespace genfit;

class TADIgenField : public AbsBField {
   
private:
   TADIgeoField* fField; ///< Mag field from geometry
   
public:
   TADIgenField();
   TADIgenField(TADIgeoField* field);
   virtual ~TADIgenField();
   
   TVector3 get(const TVector3& position) const;
   void get(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const ;
   TVector3 getGauss(const TVector3& position) const;
   void getGauss(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const ;

   
   ClassDef(TADIgenField,1)
};

#endif
