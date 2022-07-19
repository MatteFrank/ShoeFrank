/*!
  \file TAGactWDreader.cxx
  \brief   Implementation of TAGactWDreader.
*/

#include "TAGrecoManager.hxx"
#include "TASTparMap.hxx"
#include "TATWparMap.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGbaseWDparMap.hxx"

#include "DAQMarkers.hh"
#include "WDEvent.hh"
#include "ArduinoEvent.hh"

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
  \class TAGactWDreader
  \brief Get ST, TW and CA raw data from WaveDAQ. **
*/

ClassImp(TAGactWDreader);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactWDreader::TAGactWDreader(const char* name,
              TAGdataDsc* p_datdaq,
              TAGdataDsc* p_stwd, 
              TAGdataDsc* p_twwd,
              TAGdataDsc* p_cawd,
              TAGdataDsc* p_WDtrigInfo,
              TAGparaDsc* p_WDmap, 
              TAGparaDsc* p_WDtim,
              TAGparaDsc* p_CAmap,
               Bool_t stdAlone)
  : TAGaction(name, "TAGactWDreader - Unpack WaveDAQ raw data"),
    fpDatDaq(p_datdaq),
    fpStWd(p_stwd),
    fpTwWd(p_twwd),
    fpCaWd(p_cawd),
    fpWDtrigInfo(p_WDtrigInfo),
    fpWDMap(p_WDmap),
    fpWDTim(p_WDtim),
    fpCAMap(p_CAmap)
{

   fgStdAloneFlag = stdAlone;
   
   if (!fgStdAloneFlag) {
      AddDataIn(p_datdaq, "TAGdaqEvent");
   }
   AddDataOut(p_stwd, "TASTntuRaw");
   AddDataOut(p_twwd, "TATWntuRaw");
   AddDataOut(p_cawd, "TACAntuRaw");
   AddDataOut(p_WDtrigInfo, "TAGWDtrigInfo");
   AddPara(p_WDmap, "TAGbaseWDparMap");
   AddPara(p_WDtim, "TAGbaseWDparTime");
   AddPara(p_CAmap, "TACAparMap");

   fProcFiles=0;  
   fEventsN=0;
   fMaxFiles=1;
   
   int nCry = ((TACAparMap*)fpCAMap->Object())->GetCrystalsN();
   fTempCA = new double [nCry];
   for (int cryID=0; cryID<nCry; ++cryID) {
      fTempCA[cryID] = 0;
   }
}


//------------------------------------------+-----------------------------------
//! Destructor.
TAGactWDreader::~TAGactWDreader()
{
   if (fTempCA) delete [] fTempCA;
}

//------------------------------------------+-----------------------------------
//! Action.


Int_t TAGactWDreader::Open(const TString &fname) {

   fWDstream = fopen(fname.Data(),"r");
   if (fWDstream==NULL) {
      cout<<"ERROR in TAGactWDreader::cannot open the file="<<fname.Data()<<endl;
   } else {
      cout<<"TAGactWDReader::file "<<fname.Data()<<" opened"<<endl;
   }

   return kTRUE;

}


//------------------------------------------+-----------------------------------
Int_t TAGactWDreader::UpdateFile() {
  
   do {
      Int_t pos1 = fInitName.Last('_');
      Int_t pos2 = fInitName.Last('.');
      TString baseName = fInitName(0, pos1+1);
      TString ext = fInitName(pos2, fInitName.Length()-pos2);
      TString slocRunNum = fInitName(pos1+1, pos2-pos1-1);
      Int_t locRunNum = atoi(slocRunNum.Data());
      //fCurrName = baseName+to_string(locRunNum+fProcFiles)+ext;
      fCurrName = baseName+TString(Form("%04d",locRunNum+fProcFiles))+ext;
      fProcFiles++;
      if (fProcFiles>=fMaxFiles) {
         return kFALSE;
      }
   } while(access(fCurrName.Data(), F_OK )!=0);

   return kTRUE;
}


//------------------------------------------+-----------------------------------
Int_t TAGactWDreader::Close() {

   if (fgStdAloneFlag && fWDstream!=NULL)
      fclose(fWDstream);
   fWDstream=NULL;

   return kTRUE;
}

//------------------------------------------+-----------------------------------
Bool_t TAGactWDreader::Action() {

   if (FootDebugLevel(1)) { cout << " Entering the TAGactWDreader action " << endl; }

   TAGdaqEvent*         p_datdaq;
   if (!fgStdAloneFlag) p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();

   TAGbaseWDparTime*    p_WDtim = (TAGbaseWDparTime*)   fpWDTim->Object();
   TAGbaseWDparMap*     p_WDmap = (TAGbaseWDparMap*)   fpWDMap->Object();
   TACAparMap*          p_CAmap = (TACAparMap*)  fpCAMap->Object();
   TASTntuRaw*          p_stwd = (TASTntuRaw*)   fpStWd->Object();
   TATWntuRaw*          p_twwd = (TATWntuRaw*)   fpTwWd->Object();
   TACAntuRaw*          p_cawd = (TACAntuRaw*)   fpCaWd->Object();
   TAGWDtrigInfo*       p_WDtrigInfo = (TAGWDtrigInfo*)   fpWDtrigInfo->Object();
    
   Int_t nmicro;

   Clear();

   bool eof = false;

   if (!fgStdAloneFlag) {
      Int_t nFragments = p_datdaq->GetFragmentsN();
      //decoding fragment and filling the datRaw class
      for (Int_t i = 0; i < nFragments; ++i) {
         TString type = p_datdaq->GetClassType(i);
         if (type.Contains("WDEvent")) {
            const WDEvent* evt = static_cast<const WDEvent*> (p_datdaq->GetFragment(i));
            nmicro = DecodeWaveforms(evt,  p_WDtrigInfo, p_WDtim, p_WDmap);
         }

         if (type.Contains("ArduinoEvent")) {
            const ArduinoEvent* evt = static_cast<const ArduinoEvent*> (p_datdaq->GetFragment(i));
            DecodeArduinoTempCA(evt, p_CAmap);
         }

      }
   } else {
      nmicro = ReadStdAloneEvent(eof, p_WDtrigInfo, p_WDtim, p_WDmap, p_CAmap);
   }

   WaveformsTimeCalibration();
   CreateHits(p_stwd, p_twwd, p_cawd, p_CAmap);

   p_stwd->UpdateRunTime(nmicro);
   p_twwd->UpdateRunTime(nmicro);
   //   p_cawd->UpdateRunTime(nmicro);

   fEventsN++;

   fpStWd->SetBit(kValid);
   fpTwWd->SetBit(kValid);
   fpCaWd->SetBit(kValid);
   fpWDtrigInfo->SetBit(kValid);

   if (fgStdAloneFlag) {
      if (eof) {
         Close();
         Bool_t status = UpdateFile();
         if (status) {
            Open(fCurrName);
            return kTRUE;
         } else {
            return kFALSE;
         }
      }
   }

   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Decoding Arduino
Int_t TAGactWDreader::DecodeArduinoTempCA(const ArduinoEvent* evt, TACAparMap *p_CAmap)
{
   // empty event
   if ( evt->evtSize == 0 ) return 0;

   int nCry = p_CAmap->GetCrystalsN();

   // only reset values if there a not empty event
   for (int cryID=0; cryID<nCry; ++cryID) {
      fTempCA[cryID] = 0;
   }

   if (FootDebugLevel(1)) 
      printf(" -- Found Arduino event, size:%d \n", evt->evtSize);
   
   int nRead = 0;
   while ( nRead < evt->evtSize-1 ) {
      u_int boardID = evt->values[nRead++];
      if (FootDebugLevel(1)) cout << "   boardID:" << boardID << endl;
      // loop over 5 multiplexer   
      for (int k=0; k<5; ++k) {
         u_int muxnum = evt->values[nRead++]; 
         if (FootDebugLevel(1)) cout << "    mux:" << muxnum << endl;
         for (int ch=0; ch<16; ++ch) {
            // Convert to float from u_int
            double tempADC = *((float*)(&(evt->values[nRead++]))); // average over 8 measurements
            // not connected channels will read 1023
            if (tempADC < 1023) {
               int iCry = p_CAmap->GetArduinoCrystalId(boardID, muxnum, ch);
               if (iCry < 0 || iCry >= nCry) { 
                  Error("TAGactWDreader", " --- Not well mapped Arduino vs crystal ID. board: %d mux: %d  ch: %d -> criID %d", boardID, muxnum, ch, iCry);
                  continue;
               }
               fTempCA[iCry] = tempADC; 
               if (FootDebugLevel(1))  
                  cout << "      cryID:" << iCry << " Temp  ADC:" << tempADC  << endl;
            }
         }
      }
   }

   return 1;
}

//------------------------------------------+-----------------------------------
//! Decoding

Int_t TAGactWDreader::DecodeWaveforms(const WDEvent* evt,  TAGWDtrigInfo *p_WDtrigInfo, TAGbaseWDparTime *p_WDTim, TAGbaseWDparMap *p_WDMap) {
  
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

   int TGEN[65];
   
   int nmicro=0;
   TWaveformContainer *w;
   int nhitsA = 0;
   iW=0;
   bool foundFooter = false;
   while (iW < evt->evtSize && !foundFooter) {
      
      if (evt->values.at(iW) == GLB_EVT_HEADER) {
         if (FootDebugLevel(1)) printf("found glb header::%08x %08x\n", evt->values.at(iW), evt->values.at(iW+1));
            
         iW+=5;
         nmicro = evt->values.at(iW);
         nmicro = 1000;
            
         iW++; //
         if (FootDebugLevel(1)) printf("word:%08x\n", evt->values.at(iW));

         //found evt_header
         if (evt->values.at(iW) == EVT_HEADER) {
            if (FootDebugLevel(1)) printf("found evt header::%08x   %08x   %08x\n", evt->values.at(iW),evt->values.at(iW+1),evt->values.at(iW+2));
                  
            iW++;
            trig_type = (evt->values.at(iW)>>16) & 0xffff;
            ser_evt_number =  evt->values.at(iW)& 0xffff;
                  
            iW++;
            bco_counter = (int)evt->values.at(iW);
                  
            iW++;
            while ((evt->values.at(iW) & 0xffff)== BOARD_HEADER) {
               board_id = (evt->values.at(iW)>>16)  & 0xffff;
               if (FootDebugLevel(1)) printf("found board header::%08x num%d\n", evt->values.at(iW), board_id);
               iW++;
               temperature = *((float*)&evt->values.at(iW));
               if (FootDebugLevel(1)) printf("temperature::%08x num%d\n", evt->values.at(iW), board_id);
                                        
               iW++;
               range = *((float*)&evt->values.at(iW));
                           
               if (FootDebugLevel(1))
                  printf("range::%08x num%d\n", evt->values.at(iW), board_id);
               
               iW++;
               
               sampling_freq =  (evt->values.at(iW) >>16)& 0xffff;
               flags = evt->values.at(iW) & 0xffff;
               if (FootDebugLevel(1)) printf("sampling::%08x    %08x   %08x    num%d\n", evt->values.at(iW),evt->values.at(iW+1),evt->values.at(iW+2), board_id);
                     
               iW++;
                           
               while((evt->values.at(iW) & 0xffff)== CH_HEADER) {
            
                  char tmp_chstr[3]={'0','0','\0'};
                  tmp_chstr[1] = (evt->values.at(iW)>>24)  & 0xff;
                  tmp_chstr[0] = (evt->values.at(iW)>>16)  & 0xff;
                  ch_num = atoi(tmp_chstr);
                  if (FootDebugLevel(1))
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
                              
                  for(int iSa=0;iSa<512;iSa++) {
                     adc_sa = evt->values.at(iW);
                     adctmp  = (adc_sa & 0xffff);
                     w_adc.push_back(adctmp);
                     adctmp = ((adc_sa >> 16) & 0xffff);
                     w_adc.push_back(adctmp);
                     iW++;
                  }
                              
                  if (ch_num != 16 && ch_num != 17) {
                     w_amp = ADC2Volt(w_adc, range);
                  } else {
                     w_amp = ADC2Volt_CLK(w_adc);
                  }
                              
                              
                  w = new TWaveformContainer();
                  w->SetChannelId(ch_num);
                  w->SetBoardId(board_id);
                  w->GetVectA() = w_amp;
                  w->GetVectRawT() = p_WDTim->GetRawTimeArray(board_id, ch_num, trig_cell);
                  w->GetVectT() = w->GetVectRawT();
                  w->SetNEvent(fEventsN);
                  w->SetEmptyFlag(false);
                  w->SetTrigType(trig_type);
                  w->SetTriggerCellId(trig_cell);
                  if (FootDebugLevel(1)) printf("found waveform board:%d  channel:%d\n", board_id,ch_num);
                              
                  string ch_type;
                  ch_type = p_WDMap->GetChannelType(board_id, ch_num);
                              
                  if (FootDebugLevel(1)) printf("type::%s\n", ch_type.data());
                              
                  if (ch_type =="ST") {
                     fSTwaves.push_back(w);
                  } else if (ch_type == "TW") {
                     fTWwaves.push_back(w);
                  } else if (ch_type == "CALO") {
                     fCAwaves.push_back(w);
                  } else if (ch_type == "CLK") {
                     fCLKwaves.insert(std::pair<std::pair<int,int>, TWaveformContainer*>(make_pair(board_id, ch_num),w));
                  } else {
                     if (FootDebugLevel(1)) {
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
         } else if (evt->values.at(iW) == EVT_FOOTER) {
            if (FootDebugLevel(1)) printf("found footer\n");
            return nmicro;
         }
              
         vector<uint32_t> trigInfoWords;
         int tbo,nbanks;
         if ((evt->values.at(iW) &0xffff)== TRIG_HEADER) {
            tbo =  (evt->values.at(iW) >> 16)& 0xffff;
            iW++;
            nbanks =  evt->values.at(iW) & 0xffff;
            iW++;
            if (FootDebugLevel(1)) printf("found trigger board %d header, nbanks::%d\n",tbo,nbanks);
            for(int ibank=0;ibank<nbanks;ibank++) {
               if (evt->values.at(iW) == TRGI_BANK_HEADER) {
                  if (FootDebugLevel(1)) printf("TRGI header::%08x\n",evt->values.at(iW));
                  trigInfoWords.push_back(evt->values.at(iW));
                  iW++;
                  int size =  evt->values.at(iW);
                  trigInfoWords.push_back(evt->values.at(iW));
                  if (FootDebugLevel(1)) printf("size::%08x\n",evt->values.at(iW));
                  iW++;
                  for(int i=0;i<size;i++) {
                     int word= evt->values.at(iW);
                     if (FootDebugLevel(1)) printf("data::%08x\n",evt->values.at(iW));
                     trigInfoWords.push_back(evt->values.at(iW));
                     iW++;
                  }
               }else if (evt->values.at(iW) == TGEN_BANK_HEADER) {
                  if (FootDebugLevel(1)) printf("TGEN header::%08x\n",evt->values.at(iW));
                  trigInfoWords.push_back(evt->values.at(iW));
                  iW++;
                  int size = evt->values.at(iW);
                  trigInfoWords.push_back(evt->values.at(iW));
                  if (FootDebugLevel(1)) printf("size::%08x\n",evt->values.at(iW));
                  iW++;
                  for(int i=0;i<size;i++) {
                     if (FootDebugLevel(1)) printf("data::%08x\n",evt->values.at(iW));
                     trigInfoWords.push_back(evt->values.at(iW));
                     iW++;
                  }
               }else if (evt->values.at(iW) == TRGC_BANK_HEADER) {
                  trigInfoWords.push_back(evt->values.at(iW));
                  if (FootDebugLevel(1)) printf("TRCG header::%08x\n",evt->values.at(iW));
                  iW++;
                  int size = evt->values.at(iW);
                  if (FootDebugLevel(1)) printf("size::%08x\n",evt->values.at(iW));
                  trigInfoWords.push_back(evt->values.at(iW));
                  iW++;
                  for(int i=0;i<size;i++) {
                     if (FootDebugLevel(1)) printf("data::%08x\n",evt->values.at(iW));
                     trigInfoWords.push_back(evt->values.at(iW));
                     iW++;
                  }
               }
            }
            p_WDtrigInfo->AddInfo(tbo, trig_type, nbanks, trigInfoWords);
         }
         
         
         if (evt->values.at(iW) == EVT_FOOTER) {
            if (FootDebugLevel(1))printf("found footer\n");
            iW++;
            foundFooter = true;
         } else {
            printf("warning:: footer not found, event corrupted, iW::%d   last word::%08x!!\n",iW, evt->values.at(iW));
            break;
         }
      }
   }
  
   return nmicro;
}
    
//------------------------------------------+-----------------------------------
//! Setup all histograms.
    
void TAGactWDreader::CreateHistogram() {
  DeleteHistogram();
  SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
vector<double> TAGactWDreader::ADC2Volt(vector<int> v_amp, double dynamic_range) {
   vector<double> v_volt;
   double v_sa;
   
   int adcold=v_amp.at(5);
   for(int iSa=0;iSa<v_amp.size();iSa++) {
      if (iSa>5 && iSa < 1020) {
         if (fabs(v_amp.at(iSa)-adcold)>32000) {
            if (v_amp.at(iSa)-adcold<32000)v_amp.at(iSa) += 65535;
            if (v_amp.at(iSa)-adcold>32000)v_amp.at(iSa) -= 65535;
         }
         v_sa = v_amp.at(iSa)/65535.+dynamic_range-0.5;   
         v_volt.push_back(v_sa);
      } else {
         v_volt.push_back(0);
      }
      adcold = v_amp.at(iSa);
   }

   return v_volt;
}

//------------------------------------------+-----------------------------------
vector<double> TAGactWDreader::ADC2Volt_CLK(vector<int> v_amp) {

   vector<double> v_volt;
   double v_sa;
   
   for(int iSa=0;iSa<v_amp.size();iSa++) {
      if (fabs(v_amp.at(iSa)>35000)) {
         v_amp.at(iSa) -= 65536;
      }
      v_sa = v_amp.at(iSa)/65536.;
      v_volt.push_back(v_sa);
   }
   
   return v_volt;
}

//------------------------------------------+-----------------------------------
Bool_t TAGactWDreader::WaveformsTimeCalibration() {

   double dt, t_trig_ref, t_trig;
   TWaveformContainer *wclk_ref;
   
   TAGbaseWDparMap*     p_WDmap = (TAGbaseWDparMap*)   fpWDMap->Object();
   vector<int> vSTbo = p_WDmap->GetBoards("ST");
   int STbo=27;
   
   if (!vSTbo.size()) {
      printf("ST board not defined, I can not apply time calibration!!\n");
      return false;
   } else {
      STbo = vSTbo.at(0);
   }

   if (!fCLKwaves.count(make_pair(STbo,16))) {
      printf("reference clock not found!!\n");
      return false;
   }
   

   wclk_ref = fCLKwaves.find(make_pair(STbo,16))->second;
   //  t_trig_ref = wclk_ref->GetVectRawT().at((1024-wclk_ref->GetTriggerCellId())%1024);
   t_trig_ref = wclk_ref->GetVectRawT().at(1023);

   
   //clocks alignment and jitter calculation
   map<pair<int,int>, double> clk_jitter;
   map<pair<int,int>, TWaveformContainer*>::iterator it;
   for(it=fCLKwaves.begin(); it != fCLKwaves.end(); it++) {
      TWaveformContainer *wclk = it->second;
      //t_trig = wclk->GetVectRawT().at((1024-wclk->GetTriggerCellId())%1024);
      t_trig = wclk->GetVectRawT().at(1023);
      dt = t_trig_ref - t_trig;
      vector<double> calib_time;
      for(int i=0;i<wclk->GetVectRawT().size();i++) {
         calib_time.push_back(wclk->GetVectRawT().at(i)+dt);
      }
      wclk->GetVectT() = calib_time;
      double jitter = ComputeJitter(wclk);
      clk_jitter[make_pair(wclk->GetBoardId(), wclk->GetChannelId())] = jitter;

   }


   //st time calibration
   for(int i=0; i<(int)fSTwaves.size();i++) {
      TWaveformContainer *wst = fSTwaves.at(i);
      int ch_clk = (wst->GetChannelId()<8) ? 16 : 17;
      double phaseST = clk_jitter.find(make_pair(STbo,16))->second;
      double phase = clk_jitter.find(make_pair(wst->GetBoardId(),ch_clk))->second;
      //t_trig = wst->GetVectRawT().at((1024-wst->GetTriggerCellId())%1024);
      t_trig = wst->GetVectRawT().at(1023);
      dt = t_trig_ref - t_trig +phaseST - phase;
      //dt = t_trig_ref - t_trig;
      //dt = - phaseST+phase;
      //cout << "jitterST::" << jitter << endl;
      vector<double> calib_time;
      for(int i=0;i<wst->GetVectRawT().size();i++) {
         calib_time.push_back(wst->GetVectRawT().at(i)+dt);
         if (wst->GetChannelId()==0) {
      //	cout << "ST isa::" << i << "   time::" << wst->m_vectRawT.at(i)+ t_trig_ref - t_trig << endl;
         }
      }
      fSTwaves.at(i)->GetVectT() = calib_time;
   }
   

   //tw time calibration
   for(int i=0; i<(int)fTWwaves.size();i++) {
      TWaveformContainer *wtw = fTWwaves.at(i);
      int ch_clk = (wtw->GetChannelId()<8) ? 16 : 17;
      double phaseST = clk_jitter.find(make_pair(STbo,16))->second;
      double phase = clk_jitter.find(make_pair(wtw->GetBoardId(),ch_clk))->second;
      // t_trig = wtw->GetVectRawT().at((1024-wtw->GetTriggerCellId())%1024);
      t_trig = wtw->GetVectRawT().at(1023);
      dt = t_trig_ref - t_trig + phaseST-phase;
      //dt = t_trig_ref - t_trig;
      // cout << "board::" << wtw->GetBoardId() << "  jitter:" << -phaseST+phase << endl;
      // cout << "phaseST::" << phaseST << endl;
      // cout << "phase::" << phase << endl;
      vector<double> calib_time;
      for(int i=0;i<wtw->GetVectRawT().size();i++) {
         calib_time.push_back(wtw->GetVectRawT().at(i)+dt);
         if (wtw->GetChannelId()==0) {
      //	cout << "TW isa::" << i << "   time::" << wtw->m_vectRawT.at(i)+ t_trig_ref - t_trig << endl;
         }
      }
      fTWwaves.at(i)->GetVectT() = calib_time;
   }


   //calo time calibration
   for(int i=0; i<(int)fCAwaves.size();i++) {
      TWaveformContainer *wca = fCAwaves.at(i);
      int ch_clk = (wca->GetChannelId()<8) ? 16 : 17;
      double jitter = clk_jitter.find(make_pair(wca->GetBoardId(),ch_clk))->second;
      t_trig = wca->GetVectRawT().at((1024-wca->GetTriggerCellId())%1024);
      dt = t_trig_ref - t_trig - jitter;
      vector<double> calib_time;
      for(int i=0;i<wca->GetVectRawT().size();i++) {
         calib_time.push_back(wca->GetVectRawT().at(i)+dt);
         if (wca->GetChannelId()==0) {
         }
      }
      fCAwaves.at(i)->GetVectT() = calib_time;
   }

   if (FootDebugLevel(1)) printf("WD calibration performed\n");

   return true;
}

//------------------------------------------+-----------------------------------
double TAGactWDreader::ComputeJitter(TWaveformContainer *wclk)
{
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
   
   // for(int i=5;i<vtime.size()-5;i++) {

   //     t1 = vtime.at(i-1);
   //     t2 = vtime.at(i);
   //     a1 = vamp.at(i-1);
   //     a2 = vamp.at(i);
         
   //     if (a1<wave_0 && a2>=wave_0) {
   // 	m = (a2-a1)/(t2-t1);
   // 	q = a1 - m*t1;
   // 	tzerocrossing = (wave_0-q/m);
   // 	tzeros.push_back(tzerocrossing);
   // 	ncycles++;
   // 	nclock.push_back((double)ncycles);
   //     }
   // }

   for(int i=vtime.size()-1; i>=1; i--) {

      t1 = vtime.at(i-1);
      t2 = vtime.at(i);
      a1 = vamp.at(i-1);
      a2 = vamp.at(i);
      
      if (a1<wave_0 && a2>=wave_0) {
         m = (a2-a1)/(t2-t1);
         q = a1 - m*t1;
         tzerocrossing = (wave_0-q)/m;
         tzeros.push_back(tzerocrossing);
         ncycles++;
         nclock.push_back((double)ncycles);
         //cout << "tzero::" << tzerocrossing << endl;
      }
      if (ncycles==10) break;
   }
   

   
   double phase, period=0;
   int status;

   reverse(tzeros.begin(), tzeros.end());
   //for(int i=0;i<nclock.size();i++)cout << "nclock::" << nclock.at(i) << endl;
   TGraph tmp_gr(nclock.size(),&nclock[0], &tzeros[0]);
   tmp_gr.LeastSquareLinearFit(nclock.size(), phase, period, status);
   // TCanvas c("c","",600,600);
   // c.cd();
   // tmp_gr.Draw("AP*");
   // //  tmp_gr.GetXaxis()->SetLimits(-3,3);
   // //tmp_gr.SetMaximum(25);
   // //tmp_gr.SetMinimum(-2);
   // TF1 fun("fun","[0]+[1]*x",-1,100);
   // fun.SetParameter(0, phase);
   // fun.SetParameter(1, period);
   // fun.Draw("same");
   // c.SaveAs(Form("clock_board%d.pdf",wclk->GetBoardId()));
   // cout << "board::" << BoardId << "  phase::" << phase << endl;
   
   return phase;
}


Bool_t TAGactWDreader::CreateHits(TASTntuRaw *p_straw, TATWntuRaw *p_twraw, TACAntuRaw *p_caraw, TACAparMap *p_CAmap) {

   TAGbaseWDparTime*    p_WDtim = (TAGbaseWDparTime*)fpWDTim->Object();

   string  algoST = p_WDtim->GetCFDalgo("ST");
   string  algoTW = p_WDtim->GetCFDalgo("TW");
   string  algoCA = p_WDtim->GetCFDalgo("CA");

   double fracST = p_WDtim->GetCFDfrac("ST");
   double fracTW = p_WDtim->GetCFDfrac("TW");
   double fracCA = p_WDtim->GetCFDfrac("CA");

   double delST = p_WDtim->GetCFDdel("ST");
   double delTW = p_WDtim->GetCFDdel("TW");
   double delCA = p_WDtim->GetCFDdel("CA");

   
   for(int i=0; i<(int)fSTwaves.size(); i++) {
      p_straw->NewHit(fSTwaves.at(i), algoST, fracST, delST);
      if (FootDebugLevel(1)) printf("ST hit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoST.data(), fracST, delST);
   }

   for(int i=0; i<(int)fTWwaves.size(); i++) {
      p_twraw->NewHit(fTWwaves.at(i), algoTW, fracTW, delTW);
      if (FootDebugLevel(1)) printf("TW hit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoTW.data(), fracTW, delTW);
   }
   
   int nCry = p_CAmap->GetCrystalsN();
   for(int i=0; i<(int)fCAwaves.size(); i++) {
      int board_id = fCAwaves.at(i)->GetBoardId();
      int ch_num = fCAwaves.at(i)->GetChannelId();
      int criID = p_CAmap->GetCrystalId(board_id, ch_num);
      if (criID < 0 || criID >= nCry) { 
         Error("CreateHits", " CALO Hit skipped --- Not well mapped WD vs crystal ID. board: %d ch: %d -> iCry %d", board_id, ch_num, criID);
         continue;
      }
      p_caraw->NewHit(fCAwaves.at(i), fTempCA[criID]);
      if (FootDebugLevel(1)) printf("CA hit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoCA.data(), fracCA, delCA);
   }

   p_straw->NewSuperHit(fSTwaves, algoST, fracST, delST);
   if (FootDebugLevel(1)) printf("ST superhit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoST.data(), fracST, delST);

   if (FootDebugLevel(1)) printf("WD hits performed\n");

   return true;
}

//------------------------------------------+-----------------------------------
void TAGactWDreader::Clear()
{
   for(int i=0; i<fSTwaves.size(); i++) {
      delete fSTwaves.at(i);
   }
   for(int i=0; i<fTWwaves.size(); i++) {
      delete fTWwaves.at(i);
   }
   for(int i=0; i<fCAwaves.size(); i++) {
      delete fCAwaves.at(i);
   }
   map<pair<int,int>, TWaveformContainer*>::iterator it;
   for(it=fCLKwaves.begin(); it != fCLKwaves.end(); it++) {
      delete it->second;
   }
      
   fSTwaves.clear();
   fTWwaves.clear();
   fCAwaves.clear();
   fCLKwaves.clear();

   TAGWDtrigInfo* p_WDtrigInfo = (TAGWDtrigInfo*)   fpWDtrigInfo->Object();
   p_WDtrigInfo->Clear();

   return;
}



//------------------------------------------+-----------------------------------
Int_t TAGactWDreader::ReadStdAloneEvent(bool &endoffile, TAGWDtrigInfo *p_WDtrigInfo, TAGbaseWDparTime *p_WDTim, TAGbaseWDparMap *p_WDMap, TACAparMap *p_CAmap) {

   u_int word;
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

   int TGEN[65];
   int nmicro=0;
   TWaveformContainer *w;
   bool endEvent = false;
   bool startEvent = false;
   bool foundTimeCalib = false;
   bool eof=false;
   int ret=0;

   int nCry = p_CAmap->GetCrystalsN();

   while(!endEvent) {

      ret = fread(&word, 4, 1, fWDstream); 
      if (FootDebugLevel(1)) printf("word:%08x\n", word);
      if (ret==0) {
         endoffile=true;
         return 0;
      }

      if ( (word == EventMarker) && !startEvent ) {
         startEvent = true;
         if (FootDebugLevel(1)) printf("\n ================= Found new event =================\n");
      } else if ((word == EventMarker) && startEvent) {
         fseek(fWDstream, -4, SEEK_CUR);
         endEvent = true;
         return 0;
      } 
      
      if (word == FILE_HEADER) {
         fseek(fWDstream, -4, SEEK_CUR);
         p_WDTim->GetTimeInfo(fWDstream);
         foundTimeCalib = true;
      } else if (word == EVT_HEADER) {
         if (FootDebugLevel(1)) printf("found evt header::%08x", word);
         startEvent = true;
         ret = fread(&word, 4, 1, fWDstream);
         ser_evt_number =  word& 0xffff;
         
         ret = fread(&word, 4,1, fWDstream);
         //
   
         while(fread(&word, 4, 1, fWDstream) !=0 && (word & 0xffff)== BOARD_HEADER) {
            board_id = (word>>16)  & 0xffff;
            if (FootDebugLevel(1)) printf("found board header::%08x num%d\n", word, board_id);

            ret = fread(&word, 4, 1, fWDstream);
            temperature = *((float*)&word);
            if (FootDebugLevel(1)) printf("temperatrue::%08x num%d\n", word, board_id);
            
            ret = fread(&word, 4, 1, fWDstream);
            range = *((float*)&word);
            
            if (FootDebugLevel(1)) printf("range::%08x num%d\n", word, board_id);
               
            ret = fread(&word, 4, 1, fWDstream);
            sampling_freq =  (word >>16)& 0xffff;
            flags = word & 0xffff;
            if (FootDebugLevel(1)) printf("sampling::%08x  num%d\n", word, board_id);

            while(fread(&word, 4, 1, fWDstream) !=0 && (word & 0xffff)== CH_HEADER) {	

               char tmp_chstr[3]={'0','0','\0'};
               tmp_chstr[1] = (word>>24)  & 0xff;
               tmp_chstr[0] = (word>>16)  & 0xff;
               ch_num = atoi(tmp_chstr);
               if (FootDebugLevel(1)) printf("found channel header::%08x num%d\n", word, ch_num);
                  
               ret = fread(&word, 4, 1, fWDstream);
               trig_cell = (word>>16) &0xffff;
               fe_settings = ((word)&0xffff);

               int adctmp=0;
               int w_adc[NSAMPLING];
               uint32_t usa[NSAMPLING/2];
               ret = fread(usa, 4,NSAMPLING/2,fWDstream);
               for (int iSa=0; iSa<NSAMPLING/2; iSa++) {
                  adc_sa = usa[iSa];
                  adctmp  = (adc_sa & 0xffff);	  
                  w_adc[2*iSa]=adctmp;
                  adctmp = ((adc_sa >> 16) & 0xffff);
                  w_adc[2*iSa+1]=adctmp;
               }

               vector<int> wd_adc(w_adc,w_adc+NSAMPLING);
               vector<double> w_amp;
               if (ch_num != 16 && ch_num != 17) {
                  w_amp = ADC2Volt(wd_adc, range);
               } else {
                  w_amp = ADC2Volt_CLK(wd_adc);
               }

               w = new TWaveformContainer();
               w->SetChannelId(ch_num);
               w->SetBoardId(board_id);
               w->GetVectA() = w_amp;
               w->GetVectRawT() = p_WDTim->GetRawTimeArray(board_id, ch_num, trig_cell);
               w->GetVectT() = w->GetVectRawT();
               w->SetNEvent(fEventsN);
               w->SetEmptyFlag(false);
               w->SetTrigType(trig_type);
               w->SetTriggerCellId(trig_cell);
               if (FootDebugLevel(1)) printf("found waveform board:%d  channel:%d\n", board_id, ch_num);
                  
               string ch_type;
               ch_type = p_WDMap->GetChannelType(board_id, ch_num);

               if (FootDebugLevel(1)) printf("type::%s\n", ch_type.data());
                  
               if (ch_type =="ST") {
                  fSTwaves.push_back(w);
               } else if (ch_type == "TW") {
                  fTWwaves.push_back(w);
               } else if (ch_type == "CALO") {                
                  fCAwaves.push_back(w);
               } else if (ch_type == "CLK") {
                  fCLKwaves.insert(std::pair<std::pair<int,int>, TWaveformContainer*>(make_pair(board_id, ch_num),w));
               } else {
                  delete w;
                  if (FootDebugLevel(1)) {
                     cout<<"******************************************* "<<endl;
                     cout<<"*******    CORRUPTED EVENT      *********** "<<endl;
                     cout<<"*******    Bo:: "<<board_id<<" Cha:: "<<ch_num<<"      *********** "<<endl;
                     cout<<"******************************************* "<<endl;
                  }
               }
            }
            fseek(fWDstream, -4, SEEK_CUR);
         }
         fseek(fWDstream, -4, SEEK_CUR);

      } else if ((word &0xffff)== TRIG_HEADER) {

         if (foundTimeCalib) continue;
         vector<uint32_t> trigInfoWords;
         int tbo,nbanks;
         tbo =  (word >> 16)& 0xffff;

         ret = fread(&word, 4, 1, fWDstream);
         nbanks =  word & 0xffff;
         
         if (FootDebugLevel(1)) printf("found trigger board %d header, nbanks::%d, word::%08x\n",tbo,nbanks,word);
         for (int ibank=0; ibank<nbanks; ibank++) {
            ret = fread(&word, 4, 1, fWDstream);
            if (word == TRGI_BANK_HEADER) {
            if (FootDebugLevel(1)) printf("TRGI header::%08x\n",word);
            trigInfoWords.push_back(word);
            ret = fread(&word, 4, 1, fWDstream);
            int size =  word;
            trigInfoWords.push_back(word);
            if (FootDebugLevel(1)) printf("size::%08x\n",word);
            for(int i=0;i<size;i++) {
               ret = fread(&word, 4, 1, fWDstream);
               if (FootDebugLevel(1)) printf("data::%08x\n",word);
               trigInfoWords.push_back(word);
            }
            }else if (word == TGEN_BANK_HEADER) {
            if (FootDebugLevel(1)) printf("TGEN header::%08x\n",word);
            trigInfoWords.push_back(word);
            ret = fread(&word, 4, 1, fWDstream);
            int size = word;
            trigInfoWords.push_back(word);
            if (FootDebugLevel(1)) printf("size::%08x\n",word);
            for (int i=0;i<size;i++) {
               ret = fread(&word, 4, 1, fWDstream);
               if (FootDebugLevel(1)) printf("data::%08x\n",word);
               trigInfoWords.push_back(word);
            }
            } else if ((word)==  TRGC_BANK_HEADER) {
               trigInfoWords.push_back(word);
               ret = fread(&word, 4, 1, fWDstream);
               int size = word;
               if (FootDebugLevel(1)) printf("TRCG bank size::%08x\n", word);
               trigInfoWords.push_back(word);
               for(int i=0;i<size;i++) {
                  ret = fread(&word, 4, 1, fWDstream);
                  if (FootDebugLevel(1)) printf("word::%08x\n",word);
                  trigInfoWords.push_back(word);
               }
            }
         }
         p_WDtrigInfo->AddInfo(tbo, trig_type, nbanks, trigInfoWords);

      } 

      /////////////////////////////////////////////////
      // Read CALO temp sensors
      if( word == ARDUINO_HEADER) {

         ret = fread(&word, 4, 1, fWDstream);
         double time = *((float*)&word);
         //if (FootDebugLevel(1)) cout << "    time:" << time << endl;

         ret = fread(&word, 4, 1, fWDstream);
         double time_ms  = *((float*)&word);
         //if (FootDebugLevel(1)) cout << "    time ms:" << time_ms << endl;

         ret = fread(&word, 4, 1, fWDstream);
         int numEvent  = word & 0xffff;  
         if (FootDebugLevel(1)) 
            cout << " === Arduino   Event num:" << numEvent << endl;

         ret = fread(&word, 4, 1, fWDstream);// skip word

         ret = fread(&word, 4, 1, fWDstream); 
         int eventSize = word & 0xffff;
         if (FootDebugLevel(1)) cout << "    event Size:" << eventSize << endl;

         int nWordRead=0;
         // if event no empty, read the boards (80 channel each),  5 mux x 16 channel
         if (eventSize > 0) {
            //int nChRead = 0;
            while ( nWordRead < eventSize-1 ) {
               ret = fread(&word, 4, 1, fWDstream); ++nWordRead;
               u_int boardID =  word & 0xffff;
               if (FootDebugLevel(1)) 
                  cout << "   boardID:" << boardID << endl;

               // loop over 5 multiplexer   
               for (int k=0; k<5; ++k) {
                  ret = fread(&word, 4, 1, fWDstream); ++nWordRead;
                  u_int muxnum =  word & 0xffff;
                  if (FootDebugLevel(1)) 
                     cout << "    mux:" << muxnum << endl;

                  for (int ch=0; ch<16; ++ch) {
                     ret = fread(&word, 4, 1, fWDstream); ++nWordRead;
                     double tempADC =  *((float*)&word); // average over 8 measurements

                     // not connected channel will read 1023
                     if (tempADC < 1023) {
                        int iCry = p_CAmap->GetArduinoCrystalId(boardID, muxnum, ch);
                        if (iCry < 0 || iCry >= nCry) { 
                           Error("ReadStdAloneEvent", " --- Not well mapped Arduino vs crystal ID. board: %d mux: %d  ch: %d -> iCry %d", boardID, muxnum, ch, iCry);
                           continue;
                        }
                        //double temp = ADC2Temp(tempADC);
                        if (FootDebugLevel(1)) 
                           cout << "      cryID:" << iCry << "  ADC:" << tempADC << endl;

                        fTempCA[iCry] = tempADC; 
                     }
                  } // for ch
               }
            }
         }
      }       
      //else if (word == EVT_HEADER && startEvent) {
      //   if (FootDebugLevel(1)) printf("found new event\n");
      //   fseek(fWDstream, -4, SEEK_CUR);
      //   endEvent = true;
      //}
   }
  
  return nmicro;
}



//------------------------------------------+-----------------------------------
void  TAGactWDreader::SavePlot(TWaveformContainer *w, string type) {

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

   c.Print(Form("waveform%s_board%d_ch%d_nev%d.png", type.data(), w->GetBoardId(), w->GetChannelId(), fEventsN));
}
