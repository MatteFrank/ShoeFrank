#ifndef _TAGnamed_HXX
#define _TAGnamed_HXX
/*!
  \file
  \version $Id: TAGnamed.hxx,v 1.4 2003/06/09 18:39:32 mueller Exp $
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

    Bool_t          Fail() const;

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    virtual void    Print(Option_t* option="") const;

   ClassDef(TAGnamed,3)

};

ostream& operator<<(ostream& os, const TAGnamed& obj);

#include "TAGnamed.icc"

#endif
