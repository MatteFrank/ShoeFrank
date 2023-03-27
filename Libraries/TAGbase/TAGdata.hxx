#ifndef _TAGdata_HXX
#define _TAGdata_HXX
/*!
  \file TAGdata.hxx
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

    //! Fail
    Bool_t          Fail()           const { return TestBit(kFail); }
    // Need auto delete for array
    virtual Bool_t  NeedAutoDelete() const;
    // virtual Set up clones
    virtual void    SetupClones();
    // virtual Clear
    virtual void    Clear(Option_t* opt="");
    // virtual To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    // virtual Print
    virtual void    Print(Option_t* option="") const;

    ClassDef(TAGdata,2)
};

ostream& operator<<(ostream& os, const TAGdata& obj);

#endif
