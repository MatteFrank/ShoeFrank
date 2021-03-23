#ifndef _TAGdata_HXX
#define _TAGdata_HXX
/*!
  \file
  \version $Id: TAGdata.hxx,v 1.6 2003/06/09 18:37:01 mueller Exp $
  \brief   Declaration of TAGdata.
*/
/*------------------------------------------+---------------------------------*/

#include <iostream>
#include <iomanip>

#include "TObject.h"

using namespace std;

class TAGdata : public TObject {
  public:
    enum { kFail = BIT(14)
    };

                    TAGdata();
   virtual         ~TAGdata();

    Bool_t          Fail()           const { return TestBit(kFail); }
    virtual Bool_t  NeedAutoDelete() const;

    virtual void    SetupClones();

    virtual void    Clear(Option_t* opt="");

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    virtual void    Print(Option_t* option="") const;

    ClassDef(TAGdata,2)

  private:
};

ostream& operator<<(ostream& os, const TAGdata& obj);

#endif
