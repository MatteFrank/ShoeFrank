#ifndef _TAGactWDreader_HXX
#define _TAGactWDreader_HXX
/*!
  \file TAGactWDreader.hxx
  \brief   Declaration of TAGactWDreader.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TASTntuRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TACAntuRaw.hxx"
#include "TANEntuRaw.hxx"
#include "TACAparMap.hxx"
#include "TAWDntuTrigger.hxx"
#include "TAWDparTime.hxx"
#include "TAWDparMap.hxx"
#include <TH2F.h>
#include <TH1F.h>

#define NSAMPLING 1024               ///< define sample
#define GLB_EVT_HEADER 0xeadebaba    ///< global event header
#define FILE_HEADER 0x30514457       ///< file header
#define TIME_HEADER 0x454d4954       ///< time header
#define BOARD_HEADER 0x00002342      ///< board header
#define CH_HEADER 0x00003043         ///< channel header
#define EVT_HEADER 0x52444845        ///< event header
#define EVT_FOOTER 0xfafefafe        ///< event footer
#define TRIG_HEADER 0x00002354       ///< trigger header
#define TRGI_BANK_HEADER 0x49475254  ///< trigger bank header
#define TGEN_BANK_HEADER 0x4e454754  ///< trigger number bank header
#define TRGC_BANK_HEADER 0x43475254  ///< trigger counter bank header

#define ARDUINO_HEADER 0x00463730    ///< Arduino event header

class WDEvent;
class ArduinoEvent;
class TAGactWDreader : public TAGaction {

public:
  explicit           TAGactWDreader(const char* name = 0,
                                    TAGdataDsc* p_datdaq = 0,
                                    TAGdataDsc* p_stwd = 0,
                                    TAGdataDsc* p_twwd = 0,
                                    TAGdataDsc* p_cawd = 0,
                                    TAGdataDsc* p_WDtrigInfo = 0,
                                    TAGparaDsc* p_WDmap = 0,
                                    TAGparaDsc* p_WDtim = 0,
                                    TAGparaDsc* p_CAmap = 0,
                                    Bool_t standAlone = false,
                                    TAGdataDsc* p_newd = 0,
                                    TAGparaDsc* p_NEmap = 0);

   virtual          ~TAGactWDreader();
   virtual void      CreateHistogram();
   virtual Bool_t    Action();
   Int_t             Open(const TString &name);
   Int_t             Close();
   Int_t             UpdateFile();
   
   //! Set maximum number of files
   void              SetMaxFiles(Int_t value)  { fMaxFiles = value;}
   //! Set initial names
   void              SetInitName(TString name) { fInitName = name; fCurrName = name; }
   
public:
   static void EnableArduinoTempCA()  { fgArduinoTempCA = true;}
   static void DisableArduinoTempCA() { fgArduinoTempCA = false;}

private:
   TAGdataDsc*     fpDatDaq;          ///< input data dsc
   TAGdataDsc*     fpStWd;            ///< output data dsc
   TAGdataDsc*     fpTwWd;            ///< output data dsc
   TAGdataDsc*     fpCaWd;            ///< output data dsc
   TAGdataDsc*     fpNeWd;            ///< output data dsc
   TAGparaDsc*     fpWDTim;           ///< parameter dsc
   TAGparaDsc*     fpWDMap;           ///< parameter dsc
   TAGparaDsc*     fpCAMap;           ///< parameter dsc
   TAGparaDsc*     fpNEMap;           ///< parameter dsc
   TAGdataDsc*     fpWDtrigInfo;      ///< output data dsc

   TString         fInitName;         ///< init file name
   TString         fCurrName;         ///< current file name
   FILE*           fWDstream;         ///< FILE descriptor
   Int_t           fEventsN;          ///< number of events
   Bool_t          fgStdAloneFlag;    ///< standalone DAQ flag
   Int_t           fProcFiles;        ///< Process file
   Int_t           fMaxFiles;         ///< maximum number of files

  Double_t time,runtime,time_prev;
  Double_t deltatimeev,deltatimerate;
  Int_t nAcqEventsRate;
  Int_t nSTcounts, nSTcounts_prev;
  Int_t nTWcounts, nTWcounts_prev;
  Int_t nCAcounts, nCAcounts_prev;
  Int_t nSTcountsrate, nSTcountsrate_prev;
  Int_t nTWcountsrate, nTWcountsrate_prev;
  Int_t nCAcountsrate, nCAcountsrate_prev;

  

  TH1F *hTriggerID;
  TH1F *hTriggerIDstatus;
  TH1F *hDAQRate;
  TH1F *hSTRate;
  TH1F *hSTRate100;
  TH1F *hTWRate;
  TH1F *hTWRate100;
  TH1F *hCARate;
  TH1F *hCARate100;

  TH1F *hDAQRateVsTime;
  TH1F *hSTRateVsTime;
  TH1F *hTWRateVsTime;
  TH1F *hCARateVsTime;
  TH2F *hDAQVsST;
  TH1F *hRatioDAQ_ST;
  map<pair<int,int>, TH1F*>hBoardTimeCalib;
  
private:
  vector<TWaveformContainer*>             fSTwaves;  ///< wave form container vector for ST
   vector<TWaveformContainer*>             fTWwaves;  ///< wave form container vector for TW
   vector<TWaveformContainer*>             fCAwaves;  ///< wave form container vector for CA
   vector<TWaveformContainer*>             fNEwaves;  ///< wave form container vector for Neutrons
   map<pair<int,int>, TWaveformContainer*> fCLKwaves; ///< wave form container map for trigger
   double*                                 fTempCA;   ///< temp container for CA
  
private:
   static Bool_t fgArduinoTempCA; ///< flag for reading back Arduino Tempetrature for CA
   ///<
private:
   Int_t          DecodeArduinoTempCA(const ArduinoEvent* evt);
   Int_t          DecodeWaveforms(const WDEvent* evt,  TAWDntuTrigger* p_WDtrigInfo, TAWDparTime *p_WDTim, TAWDparMap *p_WDMap);
   Int_t          ReadStdAloneEvent(bool &endoffile, TAWDntuTrigger* p_WDtrigInfo, TAWDparTime *p_WDTim, TAWDparMap *p_WDMap);
   Bool_t         WaveformsTimeCalibration();
   Bool_t         CreateHits(TASTntuRaw *p_straw, TATWntuRaw *p_twraw, TACAntuRaw *p_caraw, TANEntuRaw *p_neraw);
   void           Clear();

   vector<double> ADC2Volt(vector<int>, double);
   vector<double> ADC2Volt_CLK(vector<int>);
   double         ComputeJitter(TWaveformContainer*);
   void           SavePlot(TWaveformContainer *w, string type);
 
   ClassDef(TAGactWDreader,0)
};

#endif
