/*!
  \file
  \version $Id: TAGactWDreader.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TAGactWDreader.
*/

#include "TAGrecoManager.hxx"
#include "TASTparMap.hxx"
#include "TATWparMap.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGbaseWDparMap.hxx"


#include "WDEvent.hh"
#include "TWaveformContainer.hxx"
#include "TAGdaqEvent.hxx"
#include "TASTntuRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TACAntuRaw.hxx"
#include "TAGactWDreader.hxx"
#include <TCanvas.h>
#include <unistd.h>
#include <stdint.h>

/*!
  \class TAGactWDreader TAGactWDreader.hxx "TAGactWDreader.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TAGactWDreader);

//------------------------------------------+-----------------------------------
//! Default constructor.


const int MaxEvents=20;
const int MaxST=8;
const int MaxTW=4;
const int MaxCalo=9;


TAGactWDreader::TAGactWDreader(const char* name,
			     TAGdataDsc* p_datdaq,
			     TAGdataDsc* p_stwd, 
			     TAGdataDsc* p_twwd,
			     TAGdataDsc* p_cawd,
			     TAGparaDsc* p_WDmap, 
			     TAGparaDsc* p_WDtim)
  : TAGaction(name, "TAGactWDreader - Unpack WD raw data"),
    fpDatDaq(p_datdaq),
    fpStWd(p_stwd),
    fpTwWd(p_twwd),
    fpCaWd(p_cawd),
    fpWDMap(p_WDmap),
    fpWDTim(p_WDtim)
{
  AddDataIn(p_datdaq, "TAGdaqEvent");
  AddDataOut(p_stwd, "TASTntuRaw");
  AddDataOut(p_twwd, "TATWntuRaw");
  AddDataOut(p_cawd, "TACAntuRaw");
  AddPara(p_WDmap, "TAGbaseWDparMap");
  AddPara(p_WDtim, "TAGbaseWDparTime");

  m_nev=0;
}


//------------------------------------------+-----------------------------------
//! Destructor.

TAGactWDreader::~TAGactWDreader()
{}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAGactWDreader::Action() {

    if(FootDebugLevel(1)) { cout<<" Entering the TAGactWDreader action "<<endl; }

   TAGdaqEvent*         p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();
   TAGbaseWDparTime*    p_WDtim = (TAGbaseWDparTime*)   fpWDTim->Object();
   TAGbaseWDparMap*     p_WDmap = (TAGbaseWDparMap*)   fpWDMap->Object();
   TASTntuRaw*          p_stwd = (TASTntuRaw*)   fpStWd->Object();
   TATWntuRaw*          p_twwd = (TATWntuRaw*)   fpTwWd->Object();
   TACAntuRaw*          p_cawd = (TACAntuRaw*)   fpCaWd->Object();

   
   Int_t nFragments = p_datdaq->GetFragmentsN();
   Int_t nmicro;



   //decoding fragment and fillin the datRaw class
   for (Int_t i = 0; i < nFragments; ++i) {
     TString type = p_datdaq->GetClassType(i);
     if (type.Contains("WDEvent")) {
       const WDEvent* evt = static_cast<const WDEvent*> (p_datdaq->GetFragment(i));
       nmicro = DecodeWaveforms(evt, p_WDtim, p_WDmap);
       WaveformsTimeCalibration();
       CreateHits(p_stwd, p_twwd, p_cawd);
     }
   }
   
   
   p_stwd->NewSuperHit(st_waves);
   
   p_stwd->UpdateRunTime(nmicro);
   p_twwd->UpdateRunTime(nmicro);
   //   p_cawd->UpdateRunTime(nmicro);
   
   Clear();

   m_nev++;
   
   fpStWd->SetBit(kValid);
   fpTwWd->SetBit(kValid);
   fpCaWd->SetBit(kValid);
   
  return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Decoding

Int_t TAGactWDreader::DecodeWaveforms(const WDEvent* evt,  TAGbaseWDparTime *p_WDTim, TAGbaseWDparMap *p_WDMap){

  
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
  TWaveformContainer *w;
  int nhitsA = 0;
  iW=0;
  bool foundFooter = false;
  while(iW < evt->evtSize && !foundFooter){

    if(evt->values.at(iW) == GLB_EVT_HEADER){
      if(FootDebugLevel(1))printf("found glb header::%08x %08x\n", evt->values.at(iW), evt->values.at(iW+1));

      iW+=5;
      nmicro = evt->values.at(iW);
      nmicro = 1000;
           
      iW++; //
      if(FootDebugLevel(1))printf("word:%08x\n", evt->values.at(iW));
      //found evt_header
      if(evt->values.at(iW) == EVT_HEADER){
	if(FootDebugLevel(1))printf("found evt header::%08x   %08x   %08x\n", evt->values.at(iW),evt->values.at(iW+1),evt->values.at(iW+2));
      
	iW++;
	trig_type = (evt->values.at(iW)>>16) & 0xffff;
	ser_evt_number =  evt->values.at(iW)& 0xffff;
	
	iW++;
	bco_counter = (int)evt->values.at(iW);
      
	iW++;
	while((evt->values.at(iW) & 0xffff)== BOARD_HEADER){
	  board_id = (evt->values.at(iW)>>16)  & 0xffff;
	  if(FootDebugLevel(1))printf("found board header::%08x num%d\n", evt->values.at(iW), board_id);
	  iW++;
	  temperature = *((float*)&evt->values.at(iW));
	  if(FootDebugLevel(1))printf("temperatrue::%08x num%d\n", evt->values.at(iW), board_id);

	
	  iW++;
	  range = *((float*)&evt->values.at(iW));
	
	  if(FootDebugLevel(1))
	    printf("range::%08x num%d\n", evt->values.at(iW), board_id);
		
	  iW++;
	  

	  sampling_freq =  (evt->values.at(iW) >>16)& 0xffff;
	  flags = evt->values.at(iW) & 0xffff;
	  if(FootDebugLevel(1))printf("sampling::%08x    %08x   %08x    num%d\n", evt->values.at(iW),evt->values.at(iW+1),evt->values.at(iW+2), board_id);
	 	
	  iW++;
	
	  while((evt->values.at(iW) & 0xffff)== CH_HEADER){

	    char tmp_chstr[3]={'0','0','\0'};
	    tmp_chstr[1] = (evt->values.at(iW)>>24)  & 0xff;
	    tmp_chstr[0] = (evt->values.at(iW)>>16)  & 0xff;
	    ch_num = atoi(tmp_chstr);
	    if(FootDebugLevel(1))
	      printf("found channel header::%08x num%d\n", evt->values.at(iW), ch_num);
	  
	    iW++;
	    trig_cell = (evt->values.at(iW)>>16) &0xffff;
	  
	    fe_settings = ((evt->values.at(iW))&0xffff);
	    iW++;
	    
	    int adctmp=0;
	    int delta=0,deltaold=0;
	    bool jump_up=false;
	    vector<int> w_adc;
	    w_amp.clear();
	    
	    for(int iSa=0;iSa<512;iSa++){
	      adc_sa = evt->values.at(iW);
	      adctmp  = (adc_sa & 0xffff);
	      w_adc.push_back(adctmp);
	      adctmp = ((adc_sa >> 16) & 0xffff);
	      w_adc.push_back(adctmp);
	      iW++;
	    }

	    if(ch_num != 16 && ch_num != 17){
	      w_amp = ADC2Volt(w_adc, range);
	    }else{
	      w_amp = ADC2Volt_CLK(w_adc);
	    }

	    
	    w = new TWaveformContainer();
	    w->SetChannelId(ch_num);
	    w->SetBoardId(board_id);
	    w->GetVectA() = w_amp;
	    w->GetVectRawT() = p_WDTim->GetRawTimeArray(board_id, ch_num, trig_cell);
	    w->GetVectT() = w->GetVectRawT();
	    w->SetNEvent(m_nev);
	    w->SetEmptyFlag(false);
	    w->SetTrigType(trig_type);
	    w->SetTriggerCellId(trig_cell);
	    if(FootDebugLevel(1))printf("found waveform board:%d  channel:%d\n", board_id,ch_num);
	    
	    string ch_type;
	    ch_type = p_WDMap->GetChannelType(board_id, ch_num);

	    if(FootDebugLevel(1))printf("type::%s\n", ch_type.data());
	    
	    if(ch_type =="ST"){
	      st_waves.push_back(w);
	    }else if(ch_type == "TW"){
	      tw_waves.push_back(w);
	    }else if(ch_type == "CALO"){
	      ca_waves.push_back(w);
	    }else if(ch_type == "CLK"){
	      clk_waves.insert(std::pair<std::pair<int,int>, TWaveformContainer*>(make_pair(board_id, ch_num),w));
	    } else {
	      if(FootDebugLevel(1)) {
		cout<<"******************************************* "<<endl;
		cout<<"*******    CORRUPTED EVENT      *********** "<<endl;
		cout<<"*******    Bo:: "<<board_id<<" Cha:: "<<ch_num<<"      *********** "<<endl;
		cout<<"******************************************* "<<endl;
	      }
	    }

	    nhitsA++;
	    w_amp.clear();
	  }
	}
      }else if(evt->values.at(iW) == EVT_FOOTER){
	if(FootDebugLevel(1))printf("found footer\n");
	return nmicro;
      }
      
      
      if((evt->values.at(iW) &0xffff)== TRIG_HEADER){
	int tbo =  (evt->values.at(iW) >> 16)& 0xffff;
	iW++;
	int nbanks =  evt->values.at(iW) & 0xffff;
	iW++;	    
	if(FootDebugLevel(1))printf("found trigger board %d header, nbanks::%d\n",tbo,nbanks);
	for(int ibank=0;ibank<nbanks;ibank++){
	  if(evt->values.at(iW) == TRGI_BANK_HEADER){
	    if(FootDebugLevel(1))printf("trig header::%08x\n",evt->values.at(iW));
	    iW++;
	    int size =  evt->values.at(iW);
	    if(FootDebugLevel(1))printf("size::%08x\n",evt->values.at(iW));
	    for(int i=0;i<size;i++){
	      iW++;
	      if(FootDebugLevel(1))printf("data::%08x\n",evt->values.at(iW));
	      //fill trigger class
	    }
	  }else if(evt->values.at(iW) == TGEN_BANK_HEADER){
	    if(FootDebugLevel(1))printf("trig header::%08x\n",evt->values.at(iW));
	    iW++;
	    int size = evt->values.at(iW);;
	    if(FootDebugLevel(1))printf("size::%08x\n",evt->values.at(iW));
	    iW++;
	    for(int i=0;i<size;i++){
	      if(FootDebugLevel(1))printf("data::%08x\n",evt->values.at(iW));
	      iW++;
	      //fill trigger class
	    }
	  }
	}
      }

      
      iW++;
      if(evt->values.at(iW) == EVT_FOOTER){
	if(FootDebugLevel(1))printf("found footer\n");
	iW++;
	foundFooter = true;
      }else{
	printf("warining:: footer not found, event corrupted, iW::%d   last word::%08x!!\n",iW, evt->values.at(iW));
	break;
      }  
    }
  }
  

  
  return nmicro;
}



//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAGactWDreader::CreateHistogram()
{

  DeleteHistogram();

  TAGbaseWDparTime*    p_WDtim = (TAGbaseWDparTime*)   fpWDTim->Object();

  for(int iEv=0;iEv<MaxEvents;iEv++){
    for(int iCh=0;iCh<8;iCh++){
      Double_t xmin =  p_WDtim->GetRawTimeArray(27, iCh, 0).at(0);
      Double_t xmax =  p_WDtim->GetRawTimeArray(27, iCh, 0).at(1023);
      hST[iEv][iCh] = new TH1F(Form("hST_board27_ch%d_nev%d",iCh, iEv),"",1024,xmin,xmax);
      AddHistogram(hST[iEv][iCh]);
    }
    for(int iCh=0;iCh<4;iCh++){
      Double_t xmin =  p_WDtim->GetRawTimeArray(166, iCh, 0).at(0);
      Double_t xmax =  p_WDtim->GetRawTimeArray(166, iCh, 0).at(1023);
      hTW[iEv][iCh] = new TH1F(Form("hTW_board166_ch%d_nev%d",iCh, iEv),"",1024,xmin,xmax);
      AddHistogram(hTW[iEv][iCh]);
    }
    for(int iCh=0;iCh<9;iCh++){
      Double_t xmin =  p_WDtim->GetRawTimeArray(161, iCh, 0).at(0);
      Double_t xmax =  p_WDtim->GetRawTimeArray(161, iCh, 0).at(1023);
      hCalo[iEv][iCh] = new TH1F(Form("hCA_board161_ch%d_nev%d",iCh, iEv),"",1024,xmin,xmax);
      AddHistogram(hCalo[iEv][iCh]);
    }
    
    Double_t xmin =  p_WDtim->GetRawTimeArray(27, 16, 0).at(0);
    Double_t xmax =  p_WDtim->GetRawTimeArray(27, 16, 0).at(1023);
    hClk[iEv] = new TH1F(Form("hCLK_board27_ch16_nev%d", iEv),"",1024,xmin,xmax);
    AddHistogram(hClk[iEv]);
  }



  
  SetValidHistogram(kTRUE);
}


void TAGactWDreader::FillHistogram(TH1F *h, TWaveformContainer *w){


  vector<Double_t> vtime = w->GetVectT();
  vector<Double_t> vamp = w->GetVectA();
  Int_t nsample = vtime.size();
  for(int i=0;i<nsample;i++) {
    h->SetBinContent(i+1,vamp.at(i));
  }
  
}


vector<double> TAGactWDreader::ADC2Volt(vector<int> v_amp, double dynamic_range){

  vector<double> v_volt;
  double v_sa;
  
  int adcold=v_amp.at(5);
  for(int iSa=0;iSa<v_amp.size();iSa++){
    if(iSa>5 && iSa < 1020){
      if(fabs(v_amp.at(iSa)-adcold)>32000){
	if(v_amp.at(iSa)-adcold<32000)v_amp.at(iSa) += 65535;
	if(v_amp.at(iSa)-adcold>32000)v_amp.at(iSa) -= 65535;
      }
      v_sa = v_amp.at(iSa)/65535.+dynamic_range-0.5;   
      v_volt.push_back(v_sa);
    }else{
      v_volt.push_back(0);
    }
    adcold = v_amp.at(iSa);
  }

  return v_volt;

}



vector<double> TAGactWDreader::ADC2Volt_CLK(vector<int> v_amp){

  vector<double> v_volt;
  double v_sa;
  
  for(int iSa=0;iSa<v_amp.size();iSa++){
    if(fabs(v_amp.at(iSa)>35000)){
      v_amp.at(iSa) -= 65536;
    }
    v_sa = v_amp.at(iSa)/65536.;
    v_volt.push_back(v_sa);
  }
  
  return v_volt;

}





Bool_t TAGactWDreader::WaveformsTimeCalibration(){

  double dt, t_trig_ref, t_trig;
  TWaveformContainer *wclk_ref;

  if(!clk_waves.count(make_pair(27,16))){
    printf("reference clock not found!!\n");
    return false;
  }
  
  wclk_ref = clk_waves.find(make_pair(27,16))->second;
  t_trig_ref = wclk_ref->GetVectRawT().at((1024-wclk_ref->GetTriggerCellId())%1024);

  //clocks alignment and jitter calculation
  map<pair<int,int>, double> clk_jitter;
  map<pair<int,int>, TWaveformContainer*>::iterator it;
  for(it=clk_waves.begin(); it != clk_waves.end(); it++){
    TWaveformContainer *wclk = it->second;
    t_trig = wclk->GetVectRawT().at((1024-wclk->GetTriggerCellId())%1024);
    dt = t_trig_ref - t_trig;
    vector<double> calib_time;
    for(int i=0;i<wclk->GetVectRawT().size();i++){
      calib_time.push_back(wclk->GetVectRawT().at(i)+dt);
    }
    wclk->GetVectT() = calib_time;
    double jitter = ComputeJitter(wclk);
    clk_jitter[make_pair(wclk->GetBoardId(), wclk->GetChannelId())] = jitter;

  }


  //st time calibration
  for(int i=0; i<(int)st_waves.size();i++){
    TWaveformContainer *wst = st_waves.at(i);
    int ch_clk = (wst->GetChannelId()<8) ? 16 : 17;
    double jitter = clk_jitter.find(make_pair(wst->GetBoardId(),ch_clk))->second;
    t_trig = wst->GetVectRawT().at((1024-wst->GetTriggerCellId())%1024);
    dt = t_trig_ref - t_trig - jitter;
    vector<double> calib_time;
    for(int i=0;i<wst->GetVectRawT().size();i++){
      calib_time.push_back(wst->GetVectRawT().at(i)+dt);
      if(wst->GetChannelId()==0){
	//	cout << "ST isa::" << i << "   time::" << wst->m_vectRawT.at(i)+ t_trig_ref - t_trig << endl;
      }
    }
    st_waves.at(i)->GetVectT() = calib_time;
  }
  

  
  
  //tw time calibration
  for(int i=0; i<(int)tw_waves.size();i++){
    TWaveformContainer *wtw = tw_waves.at(i);
    int ch_clk = (wtw->GetChannelId()<8) ? 16 : 17;
    double jitter = clk_jitter.find(make_pair(wtw->GetBoardId(),ch_clk))->second;
    t_trig = wtw->GetVectRawT().at((1024-wtw->GetTriggerCellId())%1024);
    dt = t_trig_ref - t_trig - jitter;
    vector<double> calib_time;
    for(int i=0;i<wtw->GetVectRawT().size();i++){
      calib_time.push_back(wtw->GetVectRawT().at(i)+dt);
      if(wtw->GetChannelId()==0){
	//	cout << "TW isa::" << i << "   time::" << wtw->m_vectRawT.at(i)+ t_trig_ref - t_trig << endl;
      }
    }
    tw_waves.at(i)->GetVectT() = calib_time;
  }


  //calo time calibration
  for(int i=0; i<(int)ca_waves.size();i++){
    TWaveformContainer *wca = ca_waves.at(i);
    int ch_clk = (wca->GetChannelId()<8) ? 16 : 17;
    double jitter = clk_jitter.find(make_pair(wca->GetBoardId(),ch_clk))->second;
    t_trig = wca->GetVectRawT().at((1024-wca->GetTriggerCellId())%1024);
    dt = t_trig_ref - t_trig - jitter;
    vector<double> calib_time;
    for(int i=0;i<wca->GetVectRawT().size();i++){
      calib_time.push_back(wca->GetVectRawT().at(i)+dt);
      if(wca->GetChannelId()==0){
      }
    }
    ca_waves.at(i)->GetVectT() = calib_time;
  }

  
  
  return true;

}


double TAGactWDreader::ComputeJitter(TWaveformContainer *wclk){

  vector<double> tzeros, nclock;
  double prod;
  double t1,t2,a1,a2;
  double m,q;
  int ncycles=0;
  double tzerocrossing;

  vector<double> vtime, vamp;
  vtime = wclk->GetVectT();
  vamp = wclk->GetVectA();


  //Get the max and min of the CLK to find its "zero"
  auto minmax = std::minmax_element(vamp.begin(), vamp.end());
  
  //Find the zero of the CLK waveform
  double wave_0 = (*(minmax.second) + *(minmax.first))/2;
  
  for(int i=5;i<vtime.size()-5;i++){

      t1 = vtime.at(i-1);
      t2 = vtime.at(i);
      a1 = vamp.at(i-1);
      a2 = vamp.at(i);
      
      if(a1<wave_0 && a2>=wave_0){
	m = (a2-a1)/(t2-t1);
	q = a1 - m*t1;
	tzerocrossing = -q/m;
	tzeros.push_back(tzerocrossing);
	nclock.push_back((double)ncycles);
	ncycles++;
      }
  }
  
  double phase, period=0;
  int status;
  TGraph tmp_gr(nclock.size(),&nclock[0], &tzeros[0]);
  tmp_gr.LeastSquareLinearFit(nclock.size(), phase, period, status);
  // TCanvas c("c","",600,600);
  // c.cd();
  tmp_gr.Draw("AP*");
  tmp_gr.GetXaxis()->SetLimits(-3,3);
  tmp_gr.SetMaximum(25);
  // tmp_gr.SetMinimum(-2);
  // TF1 fun("fun","[0]+[1]*x",-1,100);
  // fun.SetParameter(0, phase);
  // fun.SetParameter(1, period);
  // fun.Draw("same");
  // c.SaveAs(Form("clock_board%d.pdf", BoardId));
  // cout << "board::" << BoardId << "  phase::" << phase << endl;
  
  return phase;

  
}



Bool_t TAGactWDreader::CreateHits(TASTntuRaw *p_straw, TATWntuRaw *p_twraw, TACAntuRaw *p_caraw){

  for(int i=0; i<(int)st_waves.size();i++){
    p_straw->NewHit(st_waves.at(i));
    int ch = st_waves.at(i)->GetChannelId();
    if(ValidHistogram() && m_nev<MaxEvents && ch<MaxST)FillHistogram(hST[m_nev][ch], st_waves.at(i));
  }

  for(int i=0; i<(int)tw_waves.size();i++){
    p_twraw->NewHit(tw_waves.at(i));
    int ch = tw_waves.at(i)->GetChannelId();
    if(ValidHistogram() && m_nev<MaxEvents && ch<MaxTW)FillHistogram(hTW[m_nev][ch], tw_waves.at(i));
  }

  for(int i=0; i<(int)ca_waves.size();i++){
    p_caraw->NewHit(ca_waves.at(i));
    int ch = ca_waves.at(i)->GetChannelId();
    if(ValidHistogram() && m_nev<MaxEvents && ch<MaxCalo)FillHistogram(hCalo[m_nev][ch], ca_waves.at(i));
  }

   map<pair<int,int>, TWaveformContainer*>::iterator it;
   for(it=clk_waves.begin(); it != clk_waves.end(); it++){
     int bo = it->second->GetBoardId();
     int ch = it->second->GetChannelId();
     if(bo==27 && ch == 16){
       if(ValidHistogram() && m_nev<MaxEvents)FillHistogram(hClk[m_nev], it->second);
     }
   }

  return true;
  
}


void TAGactWDreader::Clear(){

  for(int i=0;i<st_waves.size();i++){
    delete st_waves.at(i);
  }
  for(int i=0;i<tw_waves.size();i++){
    delete tw_waves.at(i);
  }
  for(int i=0;i<ca_waves.size();i++){
    delete ca_waves.at(i);
  }
  map<pair<int,int>, TWaveformContainer*>::iterator it;
  for(it=clk_waves.begin(); it != clk_waves.end(); it++){
    delete it->second;
  }
   
  st_waves.clear();
  tw_waves.clear();
  ca_waves.clear();
  clk_waves.clear();
   

  return;


}



void  TAGactWDreader::SavePlot(TWaveformContainer *w, string type){

  TCanvas c("c","",600,600);
  c.cd();

  vector<Double_t> vtime = w->GetVectT();
  vector<Double_t> vamp = w->GetVectA();
  Int_t nsample = vtime.size();
  TGraph WaveGraph(nsample,&vtime[0], &vamp[0]);
  
  WaveGraph.Draw("APL");
  WaveGraph.SetMarkerSize(0.5);
  WaveGraph.SetMarkerStyle(22);
  WaveGraph.SetMarkerColor(kBlue);
  WaveGraph.GetXaxis()->SetRangeUser(0,100);

  c.Print(Form("waveform%s_board%d_ch%d_nev%d.png", type.data(), w->GetBoardId(), w->GetChannelId(), m_nev));
  
}
