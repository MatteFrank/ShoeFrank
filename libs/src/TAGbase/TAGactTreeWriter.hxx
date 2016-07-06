#ifndef _TAGactTreeWriter_HXX
#define _TAGactTreeWriter_HXX
/*!
  \file
  \version $Id: TAGactTreeWriter.hxx,v 1.11 2003/07/08 18:54:43 mueller Exp $
  \brief   Declaration of TAGactTreeWriter.
*/
/*------------------------------------------+---------------------------------*/

#include "TList.h"
#include "TString.h"

#include "TAGactionFile.hxx"

class TFile;
class TTree;
class TBranch;

class TAGactTreeWriter;

class TAGactTreeWriterBranch : public TObject {
  public:
    friend class TAGactTreeWriter;

                    TAGactTreeWriterBranch(TAGdataDsc* p_data, TString name,
					   Int_t i_size, Int_t i_compress,
					   Bool_t b_object);
    ClassDef(TAGactTreeWriterBranch,0)

  private:
    TAGdataDsc*     fpDataDsc;
    TString         fName;
    Int_t           fiBasketSize;
    Int_t           fiCompress;
    TBranch*        fpBranch;
    Bool_t          fbObject;
};

//##############################################################################

class TAGactTreeWriter : public TAGactionFile {
  public:
    explicit        TAGactTreeWriter(const char* name=0);
    virtual         ~TAGactTreeWriter();

    void            SetupObjectBranch(TAGdataDsc* p_data, const char* branch,
				      Int_t i_size=32000, Int_t i_compress=-1);

    void            SetupElementBranch(TAGdataDsc* p_data, const char* branch,
				       Int_t i_size=32000, Int_t i_compress=-1);

    void            SetCompressionLevel(Int_t i_compress=1);

    virtual Int_t   Open(const TString& name, Option_t* option="NEW");
    virtual void    Close();

    virtual Bool_t  IsOpen() const;

    TFile*          File();
    TTree*          Tree();

    virtual Bool_t  Process();

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    ClassDef(TAGactTreeWriter,0)

  private:
    TList*          fpBranchList;
    TFile*          fpFile;
    TTree*          fpTree;
    Int_t           fiNEntry;
    Int_t           fiCompress;
};

#include "TAGactTreeWriter.icc"

#endif
