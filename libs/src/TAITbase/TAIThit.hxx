
#ifndef _TAIThit_HXX
#define _TAIThit_HXX

#include "TAVTbaseHit.hxx"

/** TAIThit class interface to base class
*/
/*------------------------------------------+---------------------------------*/

class TAIThit : public TAVTbaseHit {
   
public:
    TAIThit();
    TAIThit( Int_t iSensor, const Int_t aIndex, Double_t aValue);
    TAIThit( Int_t iSensor, Double_t aValue, Int_t aLine, Int_t aColumn);
    virtual ~TAIThit() {};

    ClassDef(TAIThit,3)
};

//##############################################################################



#endif
