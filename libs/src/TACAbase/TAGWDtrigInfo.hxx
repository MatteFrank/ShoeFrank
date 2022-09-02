#ifndef _TAGWDtrigInfo_HXX
#define _TAGWDtrigInfo_HXX

/*!
 \file TAGWDtrigInfo.hxx
 \brief   Declaration of TAGWDtrigInfo.
 */

/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TAGdata.hxx"


#define TRIG_HEADER 0x00002354       ///< trigger header
#define TRGI_BANK_HEADER 0x49475254  ///< trigger bank header
#define TGEN_BANK_HEADER 0x4e454754  ///< trigger number bank header
#define TRGC_BANK_HEADER 0x43475254  ///< trigger counter bank header

#define NMAXTRIG 64                  ///< maximum number of triggers
#define NMONTRIG 14                  ///< number of mon trigger
#define NCLK 32                      ///< number of clocks


class TAGWDtrigInfo : public TAGdata {
public:
  TAGWDtrigInfo();
  virtual          ~TAGWDtrigInfo();

  //! Get Trigger id
  Int_t    GetTriggerID()         const { return fTriggerID;           }
  //! Get Trigger status
  Int_t*   GetTriggersStatus()          { return fTriggersStatus;      }
  //! Get Trigger counter
  Int_t*   GetTriggersCounter()         { return fTriggersCounter;     }
  //! Get Trigger previous counter
  Int_t*   GetTriggersPrevCounter()     { return fTriggersPrevCounter; }
  //! Get Wave Dream run time
  Double_t GetWDruntime()               { return fWDruntime;           }
  //! Get Wave Dream previous run time
  Double_t GetWDprevruntime()           { return fWDprevruntime;       }
  //! Get Wave Dream trigger number
  Int_t    GetWDtrigNum()               { return fWDtrigNum;           }
  //! Get Maj rate
  Double_t GetMajRate()                 { return fMajRate;             }
  //! Get triggers timing
  void     GetTriggersTiming(Int_t timing[NMONTRIG][NCLK]) { timing=fTriggersTiming; }

  void     AddInfo(int tbo, int triggerID, int nbanks, vector<uint32_t> words);
  void     Clear(Option_t* opt="");
  void     ToStream(ostream& os=cout, Option_t* option="") const;

public:
   //! Get branch name
  static const Char_t* GetBranchName() { return fgkBranchName.Data(); }

private:

  Int_t            fTriggerID;                      ///< trigger id
  Int_t            fTriggersStatus[NMAXTRIG];       ///< trigger status
  Int_t            fTriggersTiming[NMONTRIG][NCLK]; ///< triggers timing
  Int_t            fTriggersCounter[NMAXTRIG];      ///< trigger counter
  Int_t            fTriggersPrevCounter[NMAXTRIG];  ///< trigger previous counter
  Double_t         fWDruntime;                      ///< WD run time
  Int_t            fWDtrigNum;                      ///< WD trigger number
  Double_t         fMajRate;                        ///< Maj rate
  Double_t         fWDprevruntime;                  ///< WD previous run time


private:
  static TString    fgkBranchName; ///< branch name

  ClassDef(TAGWDtrigInfo,1)

};

#endif
