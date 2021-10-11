#ifndef _TAMntuEvent_HXX
#define _TAMntuEvent_HXX

#include "TString.h"
#include "TAGWDtrigInfo.hxx"
#include "TAGdata.hxx"

class TAGntuEvent : public TAGdata {
public:
  TAGntuEvent();
  virtual          ~TAGntuEvent();
  
  UInt_t            GetTimeSec()                      const { return fTimeSec;                }
  void              SetTimeSec(UInt_t nb)                   { fTimeSec = nb;                  }
  
  UInt_t            GetTimeUSec()                     const { return fTimeUsec;               }
  void              SetTimeUSec(UInt_t nb)                  { fTimeUsec = nb;                 }
  
  UInt_t            GetEventNumber()                  const { return fEventNumber;            }
  void              SetEventNumber(UInt_t nb)               { fEventNumber = nb;              }
  
  UInt_t            GetLiveTime()                     const { return fLiveTime;               }
  void              SetLiveTime(UInt_t nb)                  { fLiveTime = nb;                 }
  
  UInt_t            GetTimeSinceLastTrigger()         const { return fTimeSinceLastTrigger;   }
  void              SetTimeSinceLastTrigger(UInt_t nb)      { fTimeSinceLastTrigger = nb;     }
  
  UInt_t            GetClockCounter()                 const { return fClockCounter;           }
  void              SetClockCounter(UInt_t nb)              { fClockCounter = nb;             }
  
  UInt_t            GetTriggerCounter()               const { return fTriggerCounter;         }
  void              SetTriggerCounter(UInt_t nb)            { fTriggerCounter = nb;           }
  
  UInt_t            GetBCOofTrigger()                 const { return fBCOofTrigger;           }
  void              SetBCOofTrigger(UInt_t nb)              { fBCOofTrigger = nb;             }
  
  UInt_t            GetSpillNrAndTrgFineDelay()       const { return fSpillNrAndTrgFineDelay; }
  void              SetSpillNrAndTrgFineDelay(UInt_t nb)    { fSpillNrAndTrgFineDelay = nb;   }
  
  UInt_t            GetPMTsAndBusy()                  const { return fPMTsAndBusy;            }
  void              SetPMTsAndBusy(UInt_t nb)               { fPMTsAndBusy = nb;              }

  Int_t             GetTriggerID()                    const { return fTriggerID;              }
  void              SetTriggerID(Int_t id)                  { fTriggerID = id;                }

  UInt_t*           GetTriggersStatus()                     { return fTriggersStatus;         }
  void              SetTriggersStatus(Int_t status[NMAXTRIG]) { for(int i=0;i<NMAXTRIG;i++)fTriggersStatus[i] = status[i];}

  
  Double_t          TimeToDouble()                    const { return (Double_t)fTimeSec + 1.e-6 * (Double_t)fTimeUsec;}
  void              TimeFromDouble(Double_t time);
  void              SetCurrentTime();
  
  TAGntuEvent&      operator+(Double_t deltatime);
  TAGntuEvent&      operator-(Double_t deltatime);
  
  friend Double_t   operator-(const TAGntuEvent& lhs, const TAGntuEvent& rhs);
  
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
  
public:
  static const Char_t* GetBranchName() { return fgkBranchName.Data(); }
  
private:
  UInt_t            fTimeSec;
  UInt_t            fTimeUsec;
  UInt_t            fEventNumber;
  UInt_t            fLiveTime;
  UInt_t            fTimeSinceLastTrigger;
  UInt_t            fClockCounter;
  UInt_t            fTriggerCounter;
  UInt_t            fBCOofTrigger;
  UInt_t            fSpillNrAndTrgFineDelay;
  UInt_t            fPMTsAndBusy;
  Int_t            fTriggerID;
  UInt_t            fTriggersStatus[NMAXTRIG];

private:
  static TString    fgkBranchName;
  
  ClassDef(TAGntuEvent,1)
  
};

#endif
