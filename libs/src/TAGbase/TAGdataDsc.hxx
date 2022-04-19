#ifndef _TAGdataDsc_HXX
#define _TAGdataDsc_HXX
/*!
  \file TAGdataDsc.hxx
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

    // Set object
    void            SetObject(TAGdata* p_data);
    // Set producer
    void            SetProducer(TAGaction* p_action);
    // Set consumer
    void            SetConsumer(TAGaction* p_action);

    // Generate object
    TAGdata*        GenerateObject();
    //! Object pointer
    TAGdata**       ObjectPointer()     { return &fpObject;       }

    //! Get object
    TAGdata*        Object()      const { return fpObject;        }
    //! Get action
    TAGaction*      Producer()    const { return fpProducer;      }
    //! Get Class
    TClass*         ObjectClass() const { return fpObjectClass;   }
    //! Valid container
    Bool_t          Valid()       const { return TestBit(kValid); }
    //! EOF
    Bool_t          Eof()         const { return TestBit(kEof);   }

    // Clear
    virtual void    Clear(Option_t* opt="");
    // Generate
    Bool_t          Generate();
    // To stream§
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    // Recursive Remove
    virtual void    RecursiveRemove(TObject* p_obj);

  private:
    TAGdata*        fpObject;        ///< Object
    TClass*         fpObjectClass;   ///< Class
    TAGaction*      fpProducer;      ///< Producer
    TList*          fpConsumerList;  ///< Consumer list
   
   ClassDef(TAGdataDsc,0)
};

#endif
