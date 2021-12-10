#ifndef _TAGpara_HXX
#define _TAGpara_HXX
/*!
  \file TAGpara.hxx
  \brief   Declaration of TAGpara.
*/
/*------------------------------------------+---------------------------------*/

#include <iostream>
#include <iomanip>

using namespace std;

#include "TObject.h"

class TAGpara : public TObject {
  public:
    enum { kFail = BIT(14)
    };

                    TAGpara();
    virtual         ~TAGpara();

    //! Fail
    Bool_t          Fail() const { return TestBit(kFail); }

    //! Clear
    virtual void    Clear(Option_t* opt="");

    //! To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    //! Print
    virtual void    Print(Option_t* option="") const;
   
   ClassDef(TAGpara,3)
};

ostream& operator<<(ostream& os, const TAGpara& obj);

#endif
