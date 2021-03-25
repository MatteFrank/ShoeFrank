#ifndef _TAMntuEvent_HXX
#define _TAMntuEvent_HXX

#include "TString.h"

#include "TAGdata.hxx"

class TAMCntuEvent : public TAGdata {
public:
  TAMCntuEvent();
  virtual          ~TAMCntuEvent();
  
  Int_t             GetEventNumber()      const { return fEventNumber;   }
  void              SetEventNumber(Int_t nb)    { fEventNumber = nb;     }

  Int_t             GetTriggerNumber()    const { return fTriggerNumber; }
  void              SetTriggerNumber(Int_t nb)  { fTriggerNumber = nb;   }

  ULong64_t         GetTimeStamp()        const { return fTimeStamp;     }
  void              SetTimeStamp(ULong64_t nb)  { fTimeStamp = nb;       }
  
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
  
public:
  static const Char_t* GetBranchName() { return fgkBranchName.Data(); }
  
private:
  Int_t             fEventNumber;
  Int_t             fTriggerNumber;
  ULong64_t         fTimeStamp;

private:
  static TString    fgkBranchName;
  
  ClassDef(TAMCntuEvent,1)
  
};

#endif
