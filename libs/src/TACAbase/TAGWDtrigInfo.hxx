#ifndef _TAGWDtrigInfo_HXX
#define _TAGWDtrigInfo_HXX

#include "TString.h"
#include "TAGdata.hxx"


#define TRIG_HEADER 0x00002354
#define TRGI_BANK_HEADER 0x49475254
#define TGEN_BANK_HEADER 0x4e454754
#define TRGC_BANK_HEADER 0x43475254

#define NMAXTRIG 64
#define NMONTRIG 14
#define NCLK 32


class TAGWDtrigInfo : public TAGdata {
public:
  TAGWDtrigInfo();
  virtual          ~TAGWDtrigInfo();

  Int_t GetTriggerID()const {return fTriggerID;}
  inline Int_t* GetTriggersStatus(){return fTriggersStatus;}
  inline Int_t* GetTriggersCounter(){return fTriggersCounter;}
  void GetTriggersTiming(Int_t timing[NMONTRIG][NCLK]){timing=fTriggersTiming;}

  inline Double_t GetWDruntime(){return fWDruntime;}
  inline Double_t GetWDtrigNum(){return fWDtrigNum;}

  
  void AddInfo(int tbo, int triggerID, int nbanks, vector<uint32_t> words);
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
  
public:
  static const Char_t* GetBranchName() { return fgkBranchName.Data(); }
  
private:
  
  Int_t            fTriggerID;
  Int_t            fTriggersStatus[NMAXTRIG];
  Int_t            fTriggersTiming[NMONTRIG][NCLK];
  Int_t            fTriggersCounter[NMAXTRIG];
  Int_t            fTriggersPrevCounter[NMAXTRIG];
  Double_t         fWDruntime;
  Int_t            fWDtrigNum;
  Double_t         fMajRate;
  Double_t         fWDprevruntime;

  
private:
  static TString    fgkBranchName;
  
  ClassDef(TAGWDtrigInfo,1)
  
};

#endif
