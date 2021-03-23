#ifndef _TAMntuEvent_HXX
#define _TAMntuEvent_HXX

#include "TString.h"

#include "TAGdata.hxx"

class TAMCntuEvent : public TAGdata {
public:
  TAMCntuEvent();
  virtual          ~TAMCntuEvent();
  
  Int_t             GetEventNumber()    const { return fEventNumber; }
  void              SetEventNumber(Int_t nb)  { fEventNumber = nb;   }

  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
  
public:
  static const Char_t* GetBranchName() { return fgkBranchName.Data(); }
  
private:
  Int_t             fEventNumber;
  
private:
  static TString    fgkBranchName;
  
  ClassDef(TAMCntuEvent,1)
  
};

#endif
