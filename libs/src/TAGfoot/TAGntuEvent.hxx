#ifndef _TAMntuEvent_HXX
#define _TAMntuEvent_HXX

/*!
 \file TAGntuEvent.hxx
 \brief Simple container event informations from DAQ
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TAGWDtrigInfo.hxx"
#include "TAGdata.hxx"

class TAGntuEvent : public TAGdata {
public:
  TAGntuEvent();
  virtual          ~TAGntuEvent();
  
   //! Get time in second
  UInt_t            GetTimeSec()                      const { return fTimeSec;                }
   //! Set time in second
  void              SetTimeSec(UInt_t nb)                   { fTimeSec = nb;                  }
  
   //! Get time in microsecond
  UInt_t            GetTimeUSec()                     const { return fTimeUsec;               }
   //! Set time in microsecond
  void              SetTimeUSec(UInt_t nb)                  { fTimeUsec = nb;                 }
  
   //! Get event number
  UInt_t            GetEventNumber()                  const { return fEventNumber;            }
   //! Set event number
  void              SetEventNumber(UInt_t nb)               { fEventNumber = nb;              }
  
   //! Get live time
  UInt_t            GetLiveTime()                     const { return fLiveTime;               }
   //! Set live time
  void              SetLiveTime(UInt_t nb)                  { fLiveTime = nb;                 }
  
   //! Get time since last trigger
  UInt_t            GetTimeSinceLastTrigger()         const { return fTimeSinceLastTrigger;   }
   //! Set time since last trigger
  void              SetTimeSinceLastTrigger(UInt_t nb)      { fTimeSinceLastTrigger = nb;     }
  
   //! Get clock counter
  UInt_t            GetClockCounter()                 const { return fClockCounter;           }
   //! Set clock counter
  void              SetClockCounter(UInt_t nb)              { fClockCounter = nb;             }
  
   //! Get trigger counter
  UInt_t            GetTriggerCounter()               const { return fTriggerCounter;         }
   //! Set trigger counter
  void              SetTriggerCounter(UInt_t nb)            { fTriggerCounter = nb;           }
  
   //! Get BCO of trigger
  UInt_t            GetBCOofTrigger()                 const { return fBCOofTrigger;           }
   //! Set BCO of trigger
  void              SetBCOofTrigger(UInt_t nb)              { fBCOofTrigger = nb;             }
  
   //! Get spill number and trigger fine delay
  UInt_t            GetSpillNrAndTrgFineDelay()       const { return fSpillNrAndTrgFineDelay; }
   //! Set spill number and trigger fine delay
  void              SetSpillNrAndTrgFineDelay(UInt_t nb)    { fSpillNrAndTrgFineDelay = nb;   }
  
   //! Get PMT's and busy
  UInt_t            GetPMTsAndBusy()                  const { return fPMTsAndBusy;            }
   //! Set PMT's and busy
  void              SetPMTsAndBusy(UInt_t nb)               { fPMTsAndBusy = nb;              }

   //! Get trigger ID
  Int_t             GetTriggerID()                    const { return fTriggerID;              }
   //! Set trigger ID
  void              SetTriggerID(Int_t id)                  { fTriggerID = id;                }

   //! Get trigger status
  UInt_t*           GetTriggersStatus()                     { return fTriggersStatus;         }
   //! Set trigger status
  void              SetTriggersStatus(Int_t status[NMAXTRIG]) { for(int i=0;i<NMAXTRIG;i++)fTriggersStatus[i] = status[i];}

  //! Set time to double
  Double_t          TimeToDouble()                    const { return (Double_t)fTimeSec + 1.e-6 * (Double_t)fTimeUsec;}
   // Set time from double
  void              TimeFromDouble(Double_t time);
   // Set current time
  void              SetCurrentTime();
  
   // operator+
  TAGntuEvent&      operator+(Double_t deltatime);
   // operator-
  TAGntuEvent&      operator-(Double_t deltatime);
  // operator-
  friend Double_t   operator-(const TAGntuEvent& lhs, const TAGntuEvent& rhs);
  
   // Clear
  virtual void      Clear(Option_t* opt="");
   // To stream
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
  
public:
   //! Get branch name
  static const Char_t* GetBranchName() { return fgkBranchName.Data(); }
  
private:
  UInt_t            fTimeSec;                  ///< Time in second
  UInt_t            fTimeUsec;                 ///< Time in microsecond
  UInt_t            fEventNumber;              ///< Event number
  UInt_t            fLiveTime;                 ///< Live time
  UInt_t            fTimeSinceLastTrigger;     ///< Time since last trigger
  UInt_t            fClockCounter;             ///< Clock counter
  UInt_t            fTriggerCounter;           ///< Trigger counter
  UInt_t            fBCOofTrigger;             ///< BCO of trigger
  UInt_t            fSpillNrAndTrgFineDelay;   ///< Spill number and trigger fine delay
  UInt_t            fPMTsAndBusy;              ///< PMT's and busy
  Int_t             fTriggerID;                ///< Trigger ID
  UInt_t            fTriggersStatus[NMAXTRIG]; ///< Trigger status

private:
  static TString    fgkBranchName;             ///< Branch name
  
  ClassDef(TAGntuEvent,1)
  
};

#endif
