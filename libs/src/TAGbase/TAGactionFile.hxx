#ifndef _TAGactionFile_HXX
#define _TAGactionFile_HXX
/*!
  \file TAGactionFile.hxx
  \brief   Declaration of TAGactionFile.
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"

#include "TAGnamed.hxx"
#include "TAGaction.hxx"

class TAGactionFile : public TAGaction {
  public:
    explicit        TAGactionFile(const char* name=0, const char* title=0,
				  const char* openopt=0);
    virtual         ~TAGactionFile();

    // Virtual Open file
    virtual Int_t   Open(const TString& name, Option_t* option=0, const TString treeName="tree", Bool_t dscBranch = true);
    // virtual set up channel
    virtual void    SetupChannel(TAGdataDsc* p_data, TAGnamed* p_filt);
    // virtual set up branch
    virtual void    SetupBranch(TAGdataDsc* p_data, const char* branch);
    // virtual adding a friend tree
    virtual void    AddFriendTree(TString fileName = "", TString treeName = "EventTree");

    // virtual close file
    virtual void    Close();
    // virtual open file flag
    virtual Bool_t  IsOpen() const;
   
    // Reset event
    virtual void    Reset(Int_t iEvent = 0);

  public:
    //! Get current trigger
    static UInt_t GetCurrentTriger()            { return fgCurrentTriggerCnt; }
    //! Set current trigger
    static void   SetCurrentTriger(UInt_t trig) { fgCurrentTriggerCnt = trig; }
    //! Increment trigger
    static void   IncrementTrigger()            { fgCurrentTriggerCnt++; }
    // Check trigger
    static Bool_t CheckTriggerCnt(UInt_t trig);

  protected:
    TString          fOpenOpt;		      ///<  default open option
   
  protected:
    static UInt_t    fgCurrentTriggerCnt; ///< Current trigger counter
   
   ClassDef(TAGactionFile,0)
};

#endif
