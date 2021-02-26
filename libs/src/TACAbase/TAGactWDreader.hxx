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
#include "TASTdatRaw.hxx"
#include "TATWdatRaw.hxx"
#include "TACAdatRaw.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGbaseWDparMap.hxx"


class WDEvent;

#define GLB_EVT_HEADER 0xeadebaba
#define FILE_HEADER 0x30514457
#define TIME_HEADER 0x454d4954
#define BOARD_HEADER 0x00002342
#define CH_HEADER 0x00003043
#define EVT_HEADER 0x52444845
#define EVT_FOOTER 0xfafefafe


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

  //vector<TH1F *> wv0;

  int m_nev;
  
 private:

  Int_t DecodeWaveforms(const WDEvent* evt,  TAGbaseWDparTime *p_WDTim, TAGbaseWDparMap *p_WDMap);
  Bool_t WaveformsTimeCalibration();
  Bool_t CreateHits(TASTdatRaw *p_straw, TATWdatRaw *p_twraw, TACAdatRaw *p_caraw);
  void Clear();

  vector<double> ADC2Volt(vector<int>, double);
  vector<double> ADC2Volt_CLK(vector<int>);
  double ComputeJitter(TWaveformContainer*);
  
  vector<TWaveformContainer*> st_waves;
  vector<TWaveformContainer*> tw_waves;
  vector<TWaveformContainer*> ca_waves;
  map<pair<int,int>, TWaveformContainer*> clk_waves;

  
};

#endif
