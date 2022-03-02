#ifndef _TAGparaDsc_HXX
#define _TAGparaDsc_HXX
/*!
  \file TAGparaDsc.hxx
  \brief   Declaration of TAGparaDsc.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGpara.hxx"
#include "TAGnamed.hxx"

class TList;
class TAGaction;

class TAGparaDsc : public TAGnamed {
  public:
    //! fail bit
    enum { kValid = BIT(15)
    };

    explicit        TAGparaDsc(const char* name, TAGpara* p_para=0);
    virtual         ~TAGparaDsc();

    // Set Object
    void            SetObject(TAGpara* p_para);
    // Set consumer
    void            SetConsumer(TAGaction* p_action);

    //! Get object
    TAGpara**       ObjectPointer()     { return &fpObject;       }
    //! Get object
    TAGpara*        Object()      const { return fpObject;        }
    //! Get object class
    TClass*         ObjectClass() const { return fpObjectClass;   }
    //! Valid
    Bool_t          Valid()       const { return TestBit(kValid); }

    // Clear
    virtual void    Clear(Option_t* opt="");
    // To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    // Recursive removing
    virtual void    RecursiveRemove(TObject* p_obj);

  private:
    TAGpara*        fpObject;       ///< Object
    TClass*         fpObjectClass;  ///< Object class
    TList*          fpConsumerList; ///< Consumer list
   
   ClassDef(TAGparaDsc,0)
};

#endif
