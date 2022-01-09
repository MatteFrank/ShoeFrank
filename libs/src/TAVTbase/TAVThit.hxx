
#ifndef _TAVThit_HXX
#define _TAVThit_HXX

/*!
 \file TAVThit.hxx
 \brief   class interface to base class
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTbaseHit.hxx"

class TAVThit : public TAVTbaseHit {
   
public:
    TAVThit();
    TAVThit( Int_t iSensor, const Int_t aIndex, Double_t aValue);
    TAVThit( Int_t iSensor, Double_t aValue, Int_t aLine, Int_t aColumn);
    virtual ~TAVThit() {};
   
    ClassDef(TAVThit,3)
};

//##############################################################################



#endif
