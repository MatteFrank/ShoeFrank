#ifndef _TAGobject_HXX
#define _TAGobject_HXX
/*!
  \file TAGobject.hxx
  \brief   Declaration of TAGobject.
*/
/*------------------------------------------+---------------------------------*/

#include <iostream>
#include <iomanip>

using namespace std;

#include "TObject.h"
#include "TString.h"

class TAGobject : public TObject {
  public:
    enum { kFail = BIT(14)
    };

                    TAGobject();
    virtual         ~TAGobject();

    //! To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    //! Print
    virtual void    Print(Option_t* option="") const;
    //! Fail
    Bool_t          Fail()              const { return TestBit(kFail); }
    //! Found
    Bool_t          Found()             const { return  fFound;        }
    //! Set Found
    void            SetFound(Bool_t b = true) { fFound = b;            }

  protected:
   Bool_t  fFound;                    ///< flag, that pixel/strip/bar is found in hit
   
   ClassDef(TAGobject,4)

};

ostream& operator<<(ostream& os, const TAGobject& obj);

#endif
