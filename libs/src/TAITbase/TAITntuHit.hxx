
#ifndef _TAITntuHit_HXX
#define _TAITntuHit_HXX

#include "TAVTbaseHit.hxx"

/** TAITntuHit class interface to base class
*/
/*------------------------------------------+---------------------------------*/

class TAITntuHit : public TAVTbaseHit {
   
public:
    TAITntuHit();
    TAITntuHit( Int_t iSensor, const Int_t aIndex, Double_t aValue);
    TAITntuHit( Int_t iSensor, Double_t aValue, Int_t aLine, Int_t aColumn);
    virtual ~TAITntuHit() {};

    ClassDef(TAITntuHit,3)
};

//##############################################################################



#endif
