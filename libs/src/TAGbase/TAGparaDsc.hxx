#ifndef _TAGparaDsc_HXX
#define _TAGparaDsc_HXX
/*!
  \file
  \version $Id: TAGparaDsc.hxx,v 1.4 2003/06/09 18:45:13 mueller Exp $
  \brief   Declaration of TAGparaDsc.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGpara.hxx"
#include "TAGnamed.hxx"

class TList;
class TAGaction;

class TAGparaDsc : public TAGnamed {
  public:
    enum { kValid = BIT(15)
    };

    explicit        TAGparaDsc(const char* name, TAGpara* p_para=0);
    virtual         ~TAGparaDsc();

    void            SetObject(TAGpara* p_para);
    void            SetConsumer(TAGaction* p_action);

    TAGpara**       ObjectPointer()     { return &fpObject;       }
    TAGpara*        Object()      const { return fpObject;        }
    TClass*         ObjectClass() const { return fpObjectClass;   }
    Bool_t          Valid()       const { return TestBit(kValid); }

    virtual void    Clear(Option_t* opt="");

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    virtual void    RecursiveRemove(TObject* p_obj);

    ClassDef(TAGparaDsc,0)

  private:
    TAGpara*        fpObject;
    TClass*         fpObjectClass;
    TList*          fpConsumerList;
};

#endif
