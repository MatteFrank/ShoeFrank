#ifndef _TAGdataDsc_HXX
#define _TAGdataDsc_HXX
/*!
  \file
  \version $Id: TAGdataDsc.hxx,v 1.6 2003/06/09 18:44:31 mueller Exp $
  \brief   Declaration of TAGdataDsc.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGdata.hxx"
#include "TAGnamed.hxx"

class TList;
class TAGaction;

class TAGdataDsc : public TAGnamed {
  public:
    enum { kValid = BIT(15),
           kEof   = BIT(16)
    };

    explicit        TAGdataDsc(const char* name=0, TAGdata* p_data=0);
    virtual         ~TAGdataDsc();

    void            SetObject(TAGdata* p_data);
    void            SetProducer(TAGaction* p_action);
    void            SetConsumer(TAGaction* p_action);

    TAGdata*        GenerateObject();

    TAGdata**       ObjectPointer()     { return &fpObject;       }

    TAGdata*        Object()      const { return fpObject;        }
    TAGaction*      Producer()    const { return fpProducer;      }

    TClass*         ObjectClass() const { return fpObjectClass;   }

    Bool_t          Valid()       const { return TestBit(kValid); }
    Bool_t          Eof()         const { return TestBit(kEof);   }

    virtual void    Clear(Option_t* opt="");
    Bool_t          Generate();

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    virtual void    RecursiveRemove(TObject* p_obj);

    ClassDef(TAGdataDsc,0)

  private:
    TAGdata*        fpObject;
    TClass*         fpObjectClass;
    TAGaction*      fpProducer;
    TList*          fpConsumerList;
};

#endif
