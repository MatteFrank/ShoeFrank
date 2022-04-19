#ifndef _TAGaction_HXX
#define _TAGaction_HXX
/*!
  \file TAGaction.hxx
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
    //!  bit cases
    enum { kValid = BIT(15),
           kEof   = BIT(16)
    };

    explicit        TAGaction(const char* name=0, const char* title=0);
    virtual         ~TAGaction();

    // Clear
    virtual void    Clear(Option_t* opt="");

    // Process
    virtual Bool_t  Process();
    // Action
    virtual Bool_t  Action();

    // Create histogram
    virtual void    CreateHistogram();
    // Delete histogram
    void            DeleteHistogram();
    // Clear histogram
    void            ClearHistogram();
    // Write histogram
    virtual void    WriteHistogram();
    // Set histogram directory
    void            SetHistogramDir(TDirectory* dir);

    //! To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    //! Recursive removing object
    virtual void    RecursiveRemove(TObject* p_obj);

    //! Returns setting of the kValid status bit.
    Bool_t          Valid()             const { return TestBit(kValid); }
  
    //! Returns setting of the kEof status bit.
    Bool_t          Eof()               const { return TestBit(kEof);   }
  
    //! Returns true if histograms are properly booked
    Bool_t          ValidHistogram()    const { return fbHistValid;     }
  
    //! Set histogram booking state
    TList*          GetHistogrammList() const { return fpHistList;      }
  
  protected:
    // Add data constainer out
    void            AddDataOut(TAGdataDsc* p_data, const char* baseclass);
    // Add data constainer in
    void            AddDataIn(TAGdataDsc* p_data, const char* baseclass);
    // Add parameter
    void            AddPara(TAGparaDsc* p_para, const char* baseclass);

    // Add histogram to list
    void            AddHistogram(TH1* p_hist);
    //! Set valid histogram filling
    void            SetValidHistogram(Bool_t b_ok=kTRUE)  { fbHistValid = b_ok; }

    // Chech dependencie
    Bool_t          CheckDependencies();
    // Set bit all data container out
    void            SetBitAllDataOut(UInt_t i_bits);

protected:
    TDirectory*     fDirectory;     ///< Directory

  private:
    TList*          fpDataOutList;   ///< List of data containers out
    TList*          fpDataInList;    ///< List of data containers in
    TList*          fpParaList;      ///< List of parameters
    TList*          fpHistList;      ///< List of histograms
    Bool_t          fbHistValid;     ///< Histogram filling flag
    Bool_t          fbIsOpenFile;    ///< Open file status flag
   
   ClassDef(TAGaction,0)
};

#endif
