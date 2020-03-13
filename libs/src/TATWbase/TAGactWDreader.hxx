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
#include "TASTparMap.hxx"
#include "TATWparMap.hxx"
#include "TASTparTime.hxx"
#include "TATWparTime.hxx"

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
				 TAGparaDsc* p_stmap, 
				 TAGparaDsc* p_twmap, 
				 TAGparaDsc* p_sttim, 
				 TAGparaDsc* p_twtim);

  virtual         ~TAGactWDreader();
  void CreateHistogram();
  virtual Bool_t  Action();
      
  private:
    
  TAGdataDsc*     fpDatDaq;		    // input data dsc
  TAGdataDsc*     fpStWd;		    // output data dsc
  TAGdataDsc*     fpTwWd;		    // output data dsc
  TAGparaDsc*     fpStMap;		    // parameter dsc
  TAGparaDsc*     fpTwMap;		    // parameter dsc
  TAGparaDsc*     fpStTim;		    // parameter dsc
  TAGparaDsc*     fpTwTim;		    // parameter dsc

  //vector<TH1F *> wv0;

  bool m_debug;
  int m_nev;
  
 private:
  Bool_t DecodeHits(const WDEvent* evt, TASTdatRaw *p_straw, TASTparMap *p_stMap, TASTparTime *p_stTim, TATWdatRaw *p_twraw, TATWparMap *p_twMap, TATWparTime *p_twTim);
};

#endif
