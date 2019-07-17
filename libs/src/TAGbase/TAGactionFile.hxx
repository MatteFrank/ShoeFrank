#ifndef _TAGactionFile_HXX
#define _TAGactionFile_HXX
/*!
  \file
  \version $Id: TAGactionFile.hxx,v 1.1 2003/07/08 18:51:50 mueller Exp $
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

    virtual Int_t   Open(const TString& name, Option_t* option=0, const TString treeName="tree");
   
    virtual void    SetupChannel(TAGdataDsc* p_data, TAGnamed* p_filt);
   
    virtual void    SetupBranch(TAGdataDsc* p_data, const char* branch);

    virtual void    Close();

    virtual Bool_t  IsOpen() const;

    ClassDef(TAGactionFile,0)

  public:
    static UInt_t GetCurrentTriger()            { return fgCurrentTriggerCnt; }
    static void   SetCurrentTriger(UInt_t trig) { fgCurrentTriggerCnt = trig; }
    static void   IncrementTrigger()            { fgCurrentTriggerCnt++; }
    static Bool_t CheckTriggerCnt(UInt_t trig);

  protected:
    TString         fOpenOpt;		    // default open option
   
  protected:
    static UInt_t    fgCurrentTriggerCnt;

};

#endif
