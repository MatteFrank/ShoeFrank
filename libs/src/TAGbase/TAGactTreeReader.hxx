#ifndef _TAGactTreeReader_HXX
#define _TAGactTreeReader_HXX
/*!
  \file TAGactTreeReader.hxx
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

  private:
    TAGdataDsc*     fpDataDsc; ///< data dsc
    TString         fName;     ///< branch name
    TBranch*        fpBranch;  ///< branch
    Int_t           fiNByte;   ///< number of bytes
   
   ClassDef(TAGactTreeReaderBranch,0)
};

//##############################################################################

class TAGactTreeReader : public TAGactionFile {
  public:
    explicit        TAGactTreeReader(const char* name=0);
    virtual         ~TAGactTreeReader();

    //! Set up branch
    void            SetupBranch(TAGdataDsc* p_data, const char* branch);
    //! Check branch
    Bool_t          CheckBranch(const char* branch, Bool_t verbose = false);

    //! Open file
    virtual Int_t   Open(const TString& name, Option_t* option="READ", const TString treeName="tree", Bool_t dscBranch = true);
    //! Adding a friend tree
    virtual void    AddFriendTree(TString fileName = "", TString treeName = "EventTree");
    //! Close file
    virtual void    Close();
    //! Open file flag
    virtual Bool_t  IsOpen() const;
    //! Reset event
    virtual void    Reset(Int_t iEvent = 0);
    //! Number of event
    virtual Int_t   NEvents();
    //! Process
    virtual Bool_t  Process();
    //! To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    //! Get tree
    virtual TTree*  GetTree() { return fpTree; }
   
  private:
    TList*          fpBranchList;       ///< List of branch
    TFile*          fpFile;             ///< Root file
    TTree*          fpTree;             ///< tree
    Int_t           fiNEntry;           ///< Number of entries in tree
    Int_t           fiCurrentEntry;     ///< Current entry in tree
    Bool_t          fbDscBranch;        ///< Flag for data dsc branches
    TString         fpFriendFileName;   ///< Friend file name
    TFile*          fpFriendFile;       ///< Friend file
    TString         fpFriendTreeName;   ///< Friend tree name
   
   ClassDef(TAGactTreeReader,0)
};

#endif
