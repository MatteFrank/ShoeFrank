/*!
  \file
  \version $Id: TAGactWDreader.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TAGactWDreader.
*/

#include "TASTparMap.hxx"
#include "TATWparMap.hxx"
#include "TASTparTime.hxx"
#include "TATWparTime.hxx"

#include "WDEvent.hh"
#include "TWaveformContainer.hxx"
#include "TAGdaqEvent.hxx"
#include "TASTdatRaw.hxx"
#include "TATWdatRaw.hxx"
#include "TAGactWDreader.hxx"
#include <unistd.h>
#include <stdint.h>

/*!
  \class TAGactWDreader TAGactWDreader.hxx "TAGactWDreader.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TAGactWDreader);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactWDreader::TAGactWDreader(const char* name,
			     TAGdataDsc* p_datdaq,
			     TAGdataDsc* p_stwd, 
			     TAGdataDsc* p_twwd, 
			     TAGparaDsc* p_stmap, 
			     TAGparaDsc* p_twmap, 
			     TAGparaDsc* p_sttim, 
			     TAGparaDsc* p_twtim)
  : TAGaction(name, "TAGactWDreader - Unpack ST raw data"),
    fpDatDaq(p_datdaq),
    fpStWd(p_stwd),
    fpTwWd(p_twwd),
    fpStMap(p_stmap),
    fpTwMap(p_twmap),
    fpStTim(p_sttim),
    fpTwTim(p_twtim)
{
  AddDataIn(p_datdaq, "TAGdaqEvent");
  AddDataOut(p_stwd, "TASTdatRaw");
  AddDataOut(p_twwd, "TATWdatRaw");
  AddPara(p_stmap, "TASTparMap");
  AddPara(p_twmap, "TATWparMap");
  AddPara(p_sttim, "TASTparTime");
  AddPara(p_twtim, "TATWparTime");

  m_debug = GetDebugLevel();
  m_nev=0;
}


//------------------------------------------+-----------------------------------
//! Destructor.

TAGactWDreader::~TAGactWDreader()
{}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAGactWDreader::Action() {

    if(GetDebugLevel()) { cout<<" Entering the TAGactWDreader action "<<endl; }

   TAGdaqEvent*   p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();
   TASTdatRaw*    p_stwd = (TASTdatRaw*)   fpStWd->Object();
   TATWdatRaw*    p_twwd = (TATWdatRaw*)   fpTwWd->Object();
   TASTparMap*    p_stmap = (TASTparMap*)   fpStMap->Object();
   TATWparMap*    p_twmap = (TATWparMap*)   fpTwMap->Object();
   TASTparTime*    p_sttim = (TASTparTime*)   fpStTim->Object();
   TATWparTime*    p_twtim = (TATWparTime*)   fpTwTim->Object();
 
   
   Int_t nFragments = p_datdaq->GetFragmentsN();


   //decoding fragment and fillin the datRaw class
   for (Int_t i = 0; i < nFragments; ++i) {
     TString type = p_datdaq->GetClassType(i);
     if (type.Contains("WDEvent")) {
       const WDEvent* evt = static_cast<const WDEvent*> (p_datdaq->GetFragment(i));
       DecodeHits(evt, p_stwd, p_stmap, p_sttim, p_twwd, p_twmap, p_twtim);
     }
   }
   
   m_nev++;
   
   fpStWd->SetBit(kValid);
   fpTwWd->SetBit(kValid);
   
  return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Decoding

Bool_t TAGactWDreader::DecodeHits(const WDEvent* evt, TASTdatRaw *p_straw, TASTparMap *p_stMap, TASTparTime *p_stTime, TATWdatRaw *p_twraw, TATWparMap *p_twMap, TATWparTime *p_twTime)
{
  

  u_int word;
  
  int iW=0;
  int board_id=0, ch_num=0;
  float time_bin=0;
  float temperature=0, range=0, sampling_freq;
  int flags=0;
  int trig_cell=0;
  int fe_settings;
  int adc_sa=0;
  double v_sa=0;
  double tmp_tcal;
  double tmo_signal;
  int bco_counter, trig_type, ser_evt_number;
  vector<double> w_time;
  vector<double> w_amp;
  vector<float> w_tcal;
  
  int nmicro=0;
  TWaveformContainer w;
  int nhitsA = 0;
  iW=0;
  bool foundFooter = false;
  while(iW < evt->evtSize && !foundFooter){

    if(evt->values.at(iW) == GLB_EVT_HEADER){
      if(m_debug)printf("found glb header::%08x %08x\n", evt->values.at(iW), evt->values.at(iW+1));

      iW+=5;
      nmicro = evt->values.at(iW);
      nmicro = 1000;
      p_straw->UpdateRunTime(nmicro);
      
      iW++; //
      
      //found evt_header
      if(evt->values.at(iW) == EVT_HEADER){
	if(m_debug)printf("found evt header::%08x   %08x   %08x\n", evt->values.at(iW),evt->values.at(iW+1),evt->values.at(iW+2));
      
	iW++;
	trig_type = (evt->values.at(iW)>>16) & 0xffff;
	ser_evt_number =  evt->values.at(iW)& 0xffff;
      
	iW++;
	bco_counter = (int)evt->values.at(iW);
      
	iW++;
	while((evt->values.at(iW) & 0xffff)== BOARD_HEADER){
	  board_id = (evt->values.at(iW)>>16)  & 0xffff;
	  if(m_debug)printf("found board header::%08x num%d\n", evt->values.at(iW), board_id);
	  iW++;
	  temperature = *((float*)&evt->values.at(iW));
	  if(m_debug)printf("temperatrue::%08x num%d\n", evt->values.at(iW), board_id);

	
	  iW++;
	  range = *((float*)&evt->values.at(iW));
	
	  if(m_debug)
	    printf("range::%08x num%d\n", evt->values.at(iW), board_id);
		
	  iW++;
	  sampling_freq = (float)(( evt->values.at(iW)>> 16)&0xffff);
	  flags = evt->values.at(iW) & 0xffff;
	  if(m_debug)printf("sampling::%08x    %08x   %08x    num%d\n", evt->values.at(iW),evt->values.at(iW+1),evt->values.at(iW+2), board_id);
	
	  iW++;
	
	  while((evt->values.at(iW) & 0xffff)== CH_HEADER){

	    char tmp_chstr[3]={'0','0','\0'};
	    tmp_chstr[1] = (evt->values.at(iW)>>24)  & 0xff;
	    tmp_chstr[0] = (evt->values.at(iW)>>16)  & 0xff;
	    ch_num = atoi(tmp_chstr);
	    if(m_debug)
	      printf("found channel header::%08x num%d\n", evt->values.at(iW), ch_num);
	  
	    iW++;
	    trig_cell = (evt->values.at(iW)>>16) &0xffff;
	  
	    fe_settings = ((evt->values.at(iW))&0xffff);
	    iW++;
	    
	    int adctmp=0;
	    int delta=0,deltaold=0;
	    bool jump_up=false;
	    vector<double> w_adc;
	    w_amp.clear();
	    
	    for(int iSa=0;iSa<512;iSa++){
	      // if(m_debug)printf("found sample isa::%d    ::%08x\n", iSa, word);
	      adc_sa = evt->values.at(iW);
	      adctmp  = (adc_sa & 0xffff);
	      w_adc.push_back(adctmp);
	      adctmp = ((adc_sa >> 16) & 0xffff);
	      w_adc.push_back(adctmp);
	      iW++;
	    }

	    if(ch_num != 16 && ch_num != 17){
	      int adcold=w_adc.at(5);
	      for(int iSa=5;iSa<w_adc.size();iSa++){
		if(fabs(w_adc.at(iSa)-adcold)>30000){
		  if(w_adc.at(iSa)-adcold<30000)w_adc.at(iSa) += 65536;
		  if(w_adc.at(iSa)-adcold>30000)w_adc.at(iSa) -= 65536;
		}
		adcold = w_adc.at(iSa);
	      }
	    } else {
	      for(int iSa=0;iSa<w_adc.size();iSa++){
		if(abs(w_adc.at(iSa)>35000)){
		  w_adc.at(iSa) -= 65536;
		}
	      }
	    }

	    
	    if(ch_num != 16 && ch_num != 17){
	      for(int iSa=0;iSa<w_adc.size();iSa++){
		v_sa = w_adc.at(iSa)/65536.+range;
		w_amp.push_back(v_sa);
	      }
	    } else {
	      for(int iSa=0;iSa<w_adc.size();iSa++){
		v_sa = w_adc.at(iSa)/65536.;
		w_amp.push_back(v_sa);
	      }

	    }

	    if(p_stMap->IsSTChannel(ch_num) && p_stMap->IsSTBoard(board_id)){
	      p_stTime->GetTimeArray(board_id, ch_num, trig_cell, &w_time);
	      w.SetDel(2); w.SetFrac(0.2); w.SetBinMin(10); w.SetBinMax(2);
	    } else {
	      p_twTime->GetTimeArray(board_id, ch_num, trig_cell, &w_time);
	      w.SetDel(2); w.SetFrac(0.2); w.SetBinMin(40); w.SetBinMax(10);
	    }
	    
	    w.ChannelId = ch_num;
	    w.BoardId = board_id;
	    w.m_vectT = w_time;
	    w.m_vectW = w_amp;
	    for(int iw = 0; iw<w_amp.size(); iw++) {
	      w.T[iw] = w_time.at(iw);
	      w.W[iw] = w_amp.at(iw);
	    }
	    w.TrigType=trig_type;
	    if((p_stMap->IsSTChannel(ch_num) || p_stMap->IsSTClock(ch_num)) && p_stMap->IsSTBoard(board_id)){
	      p_straw->NewHit(w);
	    } else {
	      p_twraw->NewHit(w);
	    }
	    if(ValidHistogram()) {
	      if (nhitsA<wv0.size())
		w.GraphWaveForm(wv0[nhitsA]);
	    }
	    nhitsA++;
	    w.Clear();

	    w_amp.clear();
	    w_time.clear();
	  
	  }
	}
      }
      
      if(evt->values.at(iW) == EVT_FOOTER){
	if(m_debug)printf("found footer\n");
	iW++;
	foundFooter = true;
      }else{
	printf("warining:: footer not found, event corrupted, iW::%d   last word::%08x!!\n",iW, evt->values.at(iW));
	break;
      }  
    }
  }
  
  return true;
}



//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAGactWDreader::CreateHistogram()
{
  // max number of histo
  Int_t MaxHist0=20;
  DeleteHistogram();
  wv0.resize(MaxHist0);
  //
  for (int i=0;i<MaxHist0;++i)
    {
      wv0[i] = new TH1F();
      wv0[i]->SetName("WD"+TString::Format("%d",i));
      wv0[i]->SetTitle("WD graph"+TString::Format("%d",i));
      AddHistogram(wv0[i]);
    }
  SetValidHistogram(kTRUE);
}


