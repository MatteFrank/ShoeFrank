#ifndef _TAGaction_HXX
#define _TAGaction_HXX
/*!
  \file
  \version $Id: TAGaction.hxx,v 1.10 2003/06/19 17:41:00 mueller Exp $
  \brief   Declaration of TAGaction.
*/
/*------------------------------------------+---------------------------------*/

#include "TList.h"

#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TAGnamed.hxx"

class TH1;
class TDirectory;

class TAGaction : public TAGnamed {
  public:
    enum { kValid = BIT(15),
           kEof   = BIT(16)
    };

    explicit        TAGaction(const char* name=0, const char* title=0);
    virtual         ~TAGaction();

    virtual void    Clear(Option_t* opt="");

    virtual Bool_t  Process();
    virtual Bool_t  Action();

    virtual void    CreateHistogram();
    void            DeleteHistogram();
    void            ClearHistogram();
    virtual void    WriteHistogram();
    void            SetHistogramDir(TDirectory* dir);

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    virtual void    RecursiveRemove(TObject* p_obj);

    //! Returns setting of the kValid status bit.
    Bool_t          Valid()             const { return TestBit(kValid); }
  
    //! Returns setting of the kEof status bit.
    Bool_t          Eof()               const { return TestBit(kEof);   }
  
    //! Returns true if histograms are properly booked
    Bool_t          ValidHistogram()    const { return fbHistValid;     }
  
    //! Set histogram booking state
    TList*          GetHistogrammList() const { return fpHistList;      }
  
    ClassDef(TAGaction,0)

  protected:
    void            AddDataOut(TAGdataDsc* p_data, const char* baseclass);
    void            AddDataIn(TAGdataDsc* p_data, const char* baseclass);
    void            AddPara(TAGparaDsc* p_para, const char* baseclass);

    void            AddHistogram(TH1* p_hist);
    void            SetValidHistogram(Bool_t b_ok=kTRUE)  { fbHistValid = b_ok; }

    Bool_t          CheckDependencies();
    void            SetBitAllDataOut(UInt_t i_bits);

  private:
    TList*          fpDataOutList;
    TList*          fpDataInList;
    TList*          fpParaList;
    TList*          fpHistList;
    Bool_t          fbHistValid;
    Bool_t          fbIsOpenFile;
};

#endif
