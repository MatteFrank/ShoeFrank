#ifndef _TADIeveField_HXX
#define _TADIeveField_HXX
/*!
  \file
  \brief   Declaration of TADIeveField.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "FootField.hxx"
#include "TEveTrackPropagator.h"
#include "TEveVector.h"


//##############################################################################

class TADIeveField : public TEveMagField {
   
private:
   FootField* fField;
   
public:
   TADIeveField(FootField* field);
   virtual ~TADIeveField();
   
   virtual TEveVectorD GetFieldD(Double_t x, Double_t y, Double_t z) const;
   
private:
   static const Float_t Tesla;
   
   ClassDef(TADIeveField,1)
};

#endif
