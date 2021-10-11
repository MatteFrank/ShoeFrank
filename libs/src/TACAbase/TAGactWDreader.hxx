#ifndef _TAGactWDreader_HXX
#define _TAGactWDreader_HXX
/*!
  \file
  \version $Id: TAGactWDreader.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TAGactWDreader.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TASTntuRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TACAntuRaw.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGbaseWDparMap.hxx"
#include <TH2F.h>

class WDEvent;

#define GLB_EVT_HEADER 0xeadebaba
#define FILE_HEADER 0x30514457
#define TIME_HEADER 0x454d4954
#define BOARD_HEADER 0x00002342
#define CH_HEADER 0x00003043
#define EVT_HEADER 0x52444845
#define EVT_FOOTER 0xfafefafe
#define TRIG_HEADER 0x00002354
#define TRGI_BANK_HEADER 0x49475254
#define TGEN_BANK_HEADER 0x4e454754



class TAGactWDreader : public TAGaction {

public:

  explicit        TAGactWDreader(const char* name,
				 TAGdataDsc* p_datdaq,
				 TAGdataDsc* p_stwd, 
				 TAGdataDsc* p_twwd,
				 TAGdataDsc* p_cawd,
				 TAGparaDsc* p_WDmap,
                                 TAGparaDsc* p_WDtim);

  virtual         ~TAGactWDreader();
  void CreateHistogram();
  virtual Bool_t  Action();
  
  ClassDef(TAGactWDreader,0)
    
  private:
    
  TAGdataDsc*     fpDatDaq;		    // input data dsc
  TAGdataDsc*     fpStWd;		    // output data dsc
  TAGdataDsc*     fpTwWd;		    // output data dsc
  TAGdataDsc*     fpCaWd;		    // output data dsc
  TAGparaDsc*     fpWDTim;		    // parameter dsc
  TAGparaDsc*     fpWDMap;		    // parameter dsc

  int             fEventsN;

  //vector<TH1F *> wv0;
  
  TH1F *hST[20][8];
  TH1F *hTW[20][4];
  TH1F *hCalo[20][9];
  TH1F *hClk[20];
  TH1F *hTrig;
  TH2F *hTrigClk;

  
 private:

  Int_t DecodeWaveforms(const WDEvent* evt,  TAGbaseWDparTime *p_WDTim, TAGbaseWDparMap *p_WDMap);
  Bool_t WaveformsTimeCalibration();
  Bool_t CreateHits(TASTntuRaw *p_straw, TATWntuRaw *p_twraw, TACAntuRaw *p_caraw);
  void Clear();

  vector<double> ADC2Volt(vector<int>, double);
  vector<double> ADC2Volt_CLK(vector<int>);
  double ComputeJitter(TWaveformContainer*);
  void  SavePlot(TWaveformContainer *w, string type);
  void FillHistogram(TH1F *h, TWaveformContainer *w);
  
  vector<TWaveformContainer*> st_waves;
  vector<TWaveformContainer*> tw_waves;
  vector<TWaveformContainer*> ca_waves;
  map<pair<int,int>, TWaveformContainer*> clk_waves;

  
};

#endif
