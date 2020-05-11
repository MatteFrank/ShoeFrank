#ifndef _TAGobject_HXX
#define _TAGobject_HXX
/*!
  \file
  \version $Id: TAGobject.hxx,v 1.5 2003/06/09 18:40:09 mueller Exp $
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

    Bool_t          Fail() const;

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    virtual void    Print(Option_t* option="") const;
   
    Bool_t          Found()         const;
    void            SetFound(Bool_t b = true);

  protected:
   Bool_t  fFound;                    // flag, that pixel/strip/bar is found in hit
   
   ClassDef(TAGobject,4)

};

ostream& operator<<(ostream& os, const TAGobject& obj);

#include "TAGobject.icc" 

#endif
