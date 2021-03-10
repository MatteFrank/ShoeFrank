
#ifndef _TAVThit_HXX
#define _TAVThit_HXX

#include "TAVTbaseHit.hxx"

/** TAVThit class interface to base class

*/
/*------------------------------------------+---------------------------------*/

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
