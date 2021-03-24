#ifndef _TAGactTreeReader_HXX
#define _TAGactTreeReader_HXX
/*!
  \file
  \version $Id: TAGactTreeReader.hxx,v 1.8 2003/07/08 18:54:43 mueller Exp $
  \brief   Declaration of TAGactTreeReader.
*/
/*------------------------------------------+---------------------------------*/

#include "TList.h"
#include "TString.h"

#include "TAGactionFile.hxx"

class TFile;
class TTree;
class TBranch;

class TAGactTreeReader;

class TAGactTreeReaderBranch : public TObject {
  public:
    friend class TAGactTreeReader;

                    TAGactTreeReaderBranch(TAGdataDsc* p_data, TString name);
    ClassDef(TAGactTreeReaderBranch,0)

  private:
    TAGdataDsc*     fpDataDsc;
    TString         fName;
    TBranch*        fpBranch;
    Int_t           fiNByte;
};

//##############################################################################

class TAGactTreeReader : public TAGactionFile {
  public:
    explicit        TAGactTreeReader(const char* name=0);
    virtual         ~TAGactTreeReader();

    void            SetupBranch(TAGdataDsc* p_data, const char* branch);
    Bool_t          CheckBranch(const char* branch, Bool_t verbose = false);

    virtual Int_t   Open(const TString& name, Option_t* option="READ", const TString treeName="tree", Bool_t dscBranch = true);
    virtual void    Close();

    virtual Bool_t  IsOpen() const;

    virtual void    Reset();

    virtual Int_t   NEvents();
  
    virtual Bool_t  Process();

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
   
    virtual TTree*  GetTree() { return fpTree; }

    ClassDef(TAGactTreeReader,0)

  private:
    TList*          fpBranchList;
    TFile*          fpFile;
    TTree*          fpTree;
    Int_t           fiNEntry;
    Int_t           fiCurrentEntry;
    Bool_t          fbDscBranch;
};

#endif
