#ifndef _TAGnamed_HXX
#define _TAGnamed_HXX
/*!
  \file TAGnamed.hxx
  \brief   Declaration of TAGnamed.
*/
/*------------------------------------------+---------------------------------*/

#include <iostream>
#include <iomanip>

using namespace std;

#include "TNamed.h"

class TAGnamed : public TNamed {
  public:
    enum { kFail = BIT(14)
    };

                    TAGnamed();
                    TAGnamed(const char* name, const char* title);
                    TAGnamed(const TString& name, const TString& title);
    virtual         ~TAGnamed();

    //! Fail
    Bool_t          Fail() const { return TestBit(kFail); }

    //! To Stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    //! Print
    virtual void    Print(Option_t* option="") const;

   ClassDef(TAGnamed,3)

};

ostream& operator<<(ostream& os, const TAGnamed& obj);

#endif
