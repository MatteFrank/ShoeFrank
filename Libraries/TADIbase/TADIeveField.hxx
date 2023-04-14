#ifndef _TADIeveField_HXX
#define _TADIeveField_HXX
/*!
  \file TADIeveField.hxx
  \brief   Declaration of TADIeveField.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TADIgeoField.hxx"
#include "TEveTrackPropagator.h"
#include "TEveVector.h"


//##############################################################################

class TADIeveField : public TEveMagField {
   
private:
   TADIgeoField* fField;    ///< Mag field from geometry
   
public:
   TADIeveField(TADIgeoField* field);
   virtual ~TADIeveField();
   
   virtual TEveVectorD GetFieldD(Double_t x, Double_t y, Double_t z) const;
   
   ClassDef(TADIeveField,1)
};

#endif
