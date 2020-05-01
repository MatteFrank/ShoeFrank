#ifndef _TAGpara_HXX
#define _TAGpara_HXX
/*!
  \file
  \version $Id: TAGpara.hxx,v 1.3 2003/06/09 18:37:39 mueller Exp $
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

    Bool_t          Fail() const;

    virtual void    Clear(Option_t* opt="");

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    virtual void    Print(Option_t* option="") const;
   
   ClassDef(TAGpara,2)
};

ostream& operator<<(ostream& os, const TAGpara& obj);

#include "TAGpara.icc" 

#endif
