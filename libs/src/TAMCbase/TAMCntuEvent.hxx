#ifndef _TAMntuEvent_HXX
#define _TAMntuEvent_HXX

/*!
 \file TAMCntuEvent.hxx
 \brief   Declaration of TAMCntuEvent.
 */
/*------------------------------------------+---------------------------------*/

#include "TString.h"

#include "TAGdata.hxx"

class TAMCntuEvent : public TAGdata {
public:
  TAMCntuEvent();
  virtual          ~TAMCntuEvent();
  
  //! Get event number
  Int_t             GetEventNumber()      const { return fEventNumber;   }
  //! Set event number
  void              SetEventNumber(Int_t nb)    { fEventNumber = nb;     }

  //! Get trigger number
  Int_t             GetTriggerNumber()    const { return fTriggerNumber; }
  //! Set trigger number
  void              SetTriggerNumber(Int_t nb)  { fTriggerNumber = nb;   }

  //! Get time stamp
  ULong64_t         GetTimeStamp()        const { return fTimeStamp;     }
  //! Set time stamp
  void              SetTimeStamp(ULong64_t nb)  { fTimeStamp = nb;       }
  
  // Clear
  virtual void      Clear(Option_t* opt="");
  // To stream
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
  
public:
  //! Get branch name
  static const Char_t* GetBranchName() { return fgkBranchName.Data(); }
  
private:
  Int_t             fEventNumber;    ///< event number
  Int_t             fTriggerNumber;  ///< trigger number
  ULong64_t         fTimeStamp;      ///< time stamp

private:
  static TString    fgkBranchName;   ///< branch name
  
  ClassDef(TAMCntuEvent,2)
  
};

#endif
