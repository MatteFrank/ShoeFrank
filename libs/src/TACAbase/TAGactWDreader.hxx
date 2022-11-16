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
#include "TACAparMap.hxx"
#include "TAGWDtrigInfo.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGbaseWDparMap.hxx"
#include <TH2F.h>

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
  explicit           TAGactWDreader(const char* name,
                                 TAGdataDsc* p_datdaq,
                                 TAGdataDsc* p_stwd, 
                                 TAGdataDsc* p_twwd,
                                 TAGdataDsc* p_cawd,
                                 TAGdataDsc* p_WDtrigInfo,
                                 TAGparaDsc* p_WDmap,
                                 TAGparaDsc* p_WDtim,
                                 TAGparaDsc* p_CAmap,
                                 Bool_t standAlone);

   virtual          ~TAGactWDreader();
   void              CreateHistogram();
   virtual Bool_t    Action();
   Int_t             Open(const TString &name);
   Int_t             Close();
   Int_t             UpdateFile();
   
   //! Set maximum number of files
   void              SetMaxFiles(Int_t value)  { fMaxFiles = value;}
   //! Set initial names
   void              SetInitName(TString name) { fInitName = name; fCurrName = name; }
  
private:
   TAGdataDsc*     fpDatDaq;          ///< input data dsc
   TAGdataDsc*     fpStWd;            ///< output data dsc
   TAGdataDsc*     fpTwWd;            ///< output data dsc
   TAGdataDsc*     fpCaWd;            ///< output data dsc
   TAGparaDsc*     fpWDTim;           ///< parameter dsc
   TAGparaDsc*     fpWDMap;           ///< parameter dsc
   TAGparaDsc*     fpCAMap;           ///< parameter dsc
   TAGdataDsc*     fpWDtrigInfo;      ///< output data dsc

   TString         fInitName;         ///< init file name
   TString         fCurrName;         ///< current file name
   FILE*           fWDstream;         ///< FILE descriptor
   Int_t           fEventsN;          ///< number of events
   Bool_t          fgStdAloneFlag;    ///< standalone DAQ flag
   Int_t           fProcFiles;        ///< Process file
   Int_t           fMaxFiles;         ///< maximum number of files

private:
   vector<TWaveformContainer*>             fSTwaves;  ///< wave form container vector for ST
   vector<TWaveformContainer*>             fTWwaves;  ///< wave form container vector for TW
   vector<TWaveformContainer*>             fCAwaves;  ///< wave form container vector for CA
   map<pair<int,int>, TWaveformContainer*> fCLKwaves; ///< wave form container map for trigger
   double*                                 fTempCA;   ///< temp container for CA
  
private:
   Int_t          DecodeArduinoTempCA(const ArduinoEvent* evt, TACAparMap *p_CAmap);
   Int_t          DecodeWaveforms(const WDEvent* evt,  TAGWDtrigInfo* p_WDtrigInfo, TAGbaseWDparTime *p_WDTim, TAGbaseWDparMap *p_WDMap);
   Int_t          ReadStdAloneEvent(bool &endoffile, TAGWDtrigInfo* p_WDtrigInfo, TAGbaseWDparTime *p_WDTim, TAGbaseWDparMap *p_WDMap, TACAparMap *p_CAmap);
   Bool_t         WaveformsTimeCalibration();
   Bool_t         CreateHits(TASTntuRaw *p_straw, TATWntuRaw *p_twraw, TACAntuRaw *p_caraw, TACAparMap *p_CAmap);
   void           Clear();

   vector<double> ADC2Volt(vector<int>, double);
   vector<double> ADC2Volt_CLK(vector<int>);
   double         ComputeJitter(TWaveformContainer*);
   void           SavePlot(TWaveformContainer *w, string type);
 
   ClassDef(TAGactWDreader,0)
};

#endif
