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

  private:
    TAGdataDsc*     fpDataDsc;     ///< data dsc
    TString         fName;         ///< branch name
    Int_t           fiBasketSize;  ///< basket size
    Int_t           fiCompress;    ///< compress factor
    TBranch*        fpBranch;      ///< branch
    Bool_t          fbObject;      ///< object flag
   
   ClassDef(TAGactTreeWriterBranch,0)
};

//##############################################################################

class TAGactTreeWriter : public TAGactionFile {
  public:
    explicit        TAGactTreeWriter(const char* name=0);
    virtual         ~TAGactTreeWriter();

   //! Set up branch with object
    void            SetupObjectBranch(TAGdataDsc* p_data, const char* branch,
                                      Int_t i_size=32000, Int_t i_compress=-1);
   //! Set up branch with data dsc
    void            SetupElementBranch(TAGdataDsc* p_data, const char* branch,
                                       Int_t i_size=32000, Int_t i_compress=-1);
   //! Set compression level of tree
    void            SetCompressionLevel(Int_t i_compress=1);

    //! Open file
    virtual Int_t   Open(const TString& name, Option_t* option="NEW", const TString treeName="tree", Bool_t dscBranch = true);
    //! Close file
    virtual void    Close();
    //! Is open flag
    virtual Bool_t  IsOpen() const;
    //! Get root file
    TFile*          File() { return fpFile; }
    //! Get tree
    TTree*          Tree() { return fpTree; }
    //! Process
    virtual Bool_t  Process();
    //! To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

  public:
    //! Get type branch
    static Int_t get_be_type(TBranch* p_branch);
    //! Get id branch
    static Int_t get_be_id(TBranch* p_branch);
    //! Sum branch
    static void sum_branch(Int_t& i_nsub, Double_t& d_usize, Double_t& d_csize, TBranch* p_branch);
    //! Print branch info
    static void print_branch(ostream& os, Double_t d_bsize, TBranch* p_branch, Bool_t b_ptop);
  
  private:
    TList*          fpBranchList; ///< List of branches
    TFile*          fpFile;       ///< Root file
    TTree*          fpTree;       ///< Tree
    Int_t           fiNEntry;     ///< Number of entries in tree
    Int_t           fiCompress;   ///< Compress factor
   
   ClassDef(TAGactTreeWriter,0)
};

#endif
