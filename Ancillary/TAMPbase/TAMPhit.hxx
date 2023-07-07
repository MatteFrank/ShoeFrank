
#ifndef _TAMPhit_HXX
#define _TAMPhit_HXX

#include "TAVTbaseHit.hxx"

/*!
 \file TAMPhit.hxx
 \brief   Declaration of TAMPhit
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

class TAMPhit : public TAVTbaseHit {
   
public:
    TAMPhit();
    TAMPhit( Int_t iSensor, const Int_t aIndex, Double_t aValue);
    TAMPhit( Int_t iSensor, Double_t aValue, Int_t aLine, Int_t aColumn);
   //! Default destructor
    virtual ~TAMPhit() {};

    ClassDef(TAMPhit,3)
};

//##############################################################################



#endif
