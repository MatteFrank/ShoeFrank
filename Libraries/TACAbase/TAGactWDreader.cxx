/*!
  \file TAGactWDreader.cxx
  \brief   Implementation of TAGactWDreader.
*/

#include "TAGrecoManager.hxx"
#include "TASTparMap.hxx"
#include "TATWparMap.hxx"
#include "TANEparMap.hxx"
#include "TAWDparTime.hxx"
#include "TAWDparMap.hxx"

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

Bool_t TAGactWDreader::fgArduinoTempCA = false;

//! Class Imp
ClassImp(TAGactWDreader);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] p_datdaq daq event container descriptor
//! \param[out] p_stwd ST wave form container descriptor
//! \param[out] p_twwd TW wave form container descriptor
//! \param[out] p_cawd CA wave form container descriptor
//! \param[in] p_WDtrigInfo trigger wave form container descriptor
//! \param[in] p_WDmap mapping parameter descriptor
//! \param[in] p_WDtim time parameter descriptor
//! \param[in] p_CAmap  mapping parameter descriptor
//! \param[in] stdAlone standalone DAQ flag
//! \param[in] p_newd  wave form container descriptor
//! \param[in] p_NEmap  mapping parameter descriptor
TAGactWDreader::TAGactWDreader(const char* name,
                               TAGdataDsc* p_datdaq,
                               TAGdataDsc* p_stwd,
                               TAGdataDsc* p_twwd,
                               TAGdataDsc* p_cawd,
                               TAGdataDsc* p_WDtrigInfo,
                               TAGparaDsc* p_WDmap,
                               TAGparaDsc* p_WDtim,
                               TAGparaDsc* p_CAmap,
                               Bool_t stdAlone,
                               TAGdataDsc* p_newd,
                               TAGparaDsc* p_NEmap)
  : TAGaction(name, "TAGactWDreader - Unpack WaveDAQ and Arduino raw data"),
    fpDatDaq(p_datdaq),
    fpStWd(p_stwd),
    fpTwWd(p_twwd),
    fpCaWd(p_cawd),
    fpNeWd(p_newd),
    fpWDtrigInfo(p_WDtrigInfo),
    fpWDMap(p_WDmap),
    fpWDTim(p_WDtim),
    fpCAMap(p_CAmap),
    fpNEMap(p_NEmap)
{   
   fgStdAloneFlag = stdAlone;
   
   if (!fgStdAloneFlag) {
     AddDataIn(p_datdaq, "TAGdaqEvent");
   }
   AddDataOut(p_stwd, "TASTntuRaw");
   AddDataOut(p_twwd, "TATWntuRaw");
   if (p_cawd)
     AddDataOut(p_cawd, "TACAntuRaw");
   if (p_newd)
     AddDataOut(p_newd, "TANEntuRaw");
   AddDataOut(p_WDtrigInfo, "TAWDntuTrigger");
   AddPara(p_WDmap, "TAWDparMap");
   AddPara(p_WDtim, "TAWDparTime");
   if (p_CAmap)
     AddPara(p_CAmap, "TACAparMap");

   fProcFiles=0;  
   fEventsN=0;
   fMaxFiles=1;
 

   nAcqEventsRate=0;
   nSTcounts=0; nSTcounts_prev=0;
   nTWcounts=0; nTWcounts_prev=0;
   nCAcounts=0; nCAcounts_prev=0;
   nSTcountsrate=0; nSTcountsrate_prev=0;
   nTWcountsrate=0; nTWcountsrate_prev=0;
   nCAcountsrate=0; nCAcountsrate_prev=0;


   time=0.;
   time_prev=0.;
   runtime=0.;
   deltatimeev=0.;
   deltatimerate=0.;
   
   if (fpCAMap) {
      int nCry = ((TACAparMap*)fpCAMap->Object())->GetCrystalsN();
      fTempCA = new double [nCry];
      for (int cryID=0; cryID<nCry; ++cryID) {
         fTempCA[cryID] = 0;
      }
   }
}
   
//------------------------------------------+-----------------------------------
//! Destructor.
TAGactWDreader::~TAGactWDreader()
{
   if (fTempCA && fpCAMap) delete [] fTempCA;
}

//------------------------------------------+-----------------------------------
//! Open
//!
//! \param[in] fname file name
Int_t TAGactWDreader::Open(const TString& fname) {

   fWDstream = fopen(fname.Data(),"r");
   if (fWDstream==NULL) {
      cout << "ERROR in TAGactWDreader::cannot open the file=" << fname.Data() << endl;
   } else {
      cout << "TAGactWDReader::file " << fname.Data() << " opened" << endl;
   }

   return kTRUE;

}

//------------------------------------------+-----------------------------------
//! Update file
Int_t TAGactWDreader::UpdateFile()
{
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
//! Close file
Int_t TAGactWDreader::Close()
{
   if (fgStdAloneFlag && fWDstream!=NULL)
      fclose(fWDstream);
   fWDstream=NULL;

   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Action
Bool_t TAGactWDreader::Action()
{
   if (FootDebugLevel(1)) { cout << " Entering the TAGactWDreader action " << endl; }

   TAGdaqEvent*         p_datdaq;
   if (!fgStdAloneFlag) p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();

   TAWDparTime*     p_WDtim = (TAWDparTime*) fpWDTim->Object();
   TAWDparMap*      p_WDmap = (TAWDparMap*)  fpWDMap->Object();
   TASTntuRaw*      p_stwd  = (TASTntuRaw*)  fpStWd->Object();
   TATWntuRaw*      p_twwd  = (TATWntuRaw*)  fpTwWd->Object();
   TAWDntuTrigger*  p_WDtrigInfo = (TAWDntuTrigger*) fpWDtrigInfo->Object();

   TACAntuRaw*  p_cawd  = 0x0;
   if (fpCaWd)
      p_cawd = (TACAntuRaw*) fpCaWd->Object();
   TACAparMap*  p_CAmap = 0x0;
   if (fpCAMap)
      p_CAmap =  (TACAparMap*) fpCAMap->Object();

   TANEntuRaw*  p_newd  = 0x0;
   if (fpNeWd)
      p_newd = (TANEntuRaw*) fpNeWd->Object();

  
   Int_t nmicro;
  
   Clear();

   bool eof = false;

   if (!fgStdAloneFlag) {
      //decoding fragment and filling the datRaw class
      const WDEvent* evt = static_cast<const WDEvent*> (p_datdaq->GetFragment("WDEvent"));
      if (evt) 
      nmicro = DecodeWaveforms(evt,  p_WDtrigInfo, p_WDtim, p_WDmap);
      
      TrgEvent*  trgEvent  = p_datdaq->GetTrgEvent();
      if (trgEvent) {

         nAcqEventsRate++;
         Double_t trigID = p_WDtrigInfo->GetTriggerID();
         nSTcounts = (p_WDtrigInfo->GetTriggersCounter())[42];
         nTWcounts = (p_WDtrigInfo->GetTriggersCounter())[20];
         nCAcounts = (p_WDtrigInfo->GetTriggersCounter())[30];
         nSTcountsrate += nSTcounts-nSTcounts_prev;
         nTWcountsrate += nTWcounts-nTWcounts_prev;
         nCAcountsrate += nCAcounts-nCAcounts_prev;
         
         time = trgEvent->time_sec+trgEvent->time_usec*1E-6;
         deltatimeev = (trgEvent->eventNumber==0) ? 0 : time-time_prev;
         runtime+=deltatimeev;
         deltatimerate+=deltatimeev;

         if (ValidHistogram()) {

	   
	   hTriggerID->Fill(trigID);
	   for(int iTrig=0;iTrig<NMAXTRIG;iTrig++){
	     if((p_WDtrigInfo->GetTriggersStatus())[iTrig]){
	       hTriggerIDstatus->Fill(iTrig);
	     }
	   }
	   hDAQRateVsTime->Fill(runtime);
            hSTRateVsTime->Fill(runtime,nSTcounts-nSTcounts_prev);
            hTWRateVsTime->Fill(runtime,nTWcounts-nTWcounts_prev);
            hCARateVsTime->Fill(runtime,nCAcounts-nCAcounts_prev);
            
            if (deltatimeev>0){
               hSTRate->Fill((nSTcounts-nSTcounts_prev)/deltatimeev);
               hTWRate->Fill((nTWcounts-nTWcounts_prev)/deltatimeev);
               hCARate->Fill((nCAcounts-nCAcounts_prev)/deltatimeev);
            }

            if (deltatimerate>0.1){
               hDAQRate->Fill(nAcqEventsRate/deltatimerate);
               hDAQVsST->Fill(nSTcountsrate/deltatimerate, nAcqEventsRate/deltatimerate);
               hRatioDAQ_ST->Fill(nAcqEventsRate/(Double_t)nSTcountsrate);
               hSTRate100->Fill(nSTcountsrate/deltatimerate);
               hTWRate100->Fill(nTWcountsrate/deltatimerate);
               hCARate100->Fill(nCAcountsrate/deltatimerate);
               deltatimerate=0;
               nAcqEventsRate=0;
               nSTcountsrate=0;
               nTWcountsrate=0;
               nCAcountsrate=0;
            }
            
            time_prev = time;
            nSTcounts_prev = nSTcounts;
            nTWcounts_prev = nTWcounts;
            nCAcounts_prev = nTWcounts;
         }
      }

      if (fgArduinoTempCA && fpCaWd) {
         const ArduinoEvent* evtA = static_cast<const ArduinoEvent*> (p_datdaq->GetFragment("ArduinoEvent"));
         if (evtA)
            DecodeArduinoTempCA(evtA);
      }
      
   } else {
     nmicro = ReadStdAloneEvent(eof, p_WDtrigInfo, p_WDtim, p_WDmap);
   }


   WaveformsTimeCalibration();
   CreateHits(p_stwd, p_twwd, p_cawd, p_newd);


   
   p_stwd->UpdateRunTime(nmicro);
   p_twwd->UpdateRunTime(nmicro);
   //   p_cawd->UpdateRunTime(nmicro);

   fEventsN++;

   fpStWd->SetBit(kValid);
   fpTwWd->SetBit(kValid);
   if (fpCaWd)
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
//! Decoding Arduino events (Calorimeter crystal temperature sensors)
//!
//! \param[in] evt     arduino event descriptor
//! \param[in] p_CAmap  channel map parameter descriptor
Int_t TAGactWDreader::DecodeArduinoTempCA(const ArduinoEvent* evt)
{
   // Arduino events (Temp) are read only every few seconds
   // so, the are a lot of empty events 
   
   TACAparMap* p_CAmap = (TACAparMap*)  fpCAMap->Object();

   if ( evt->evtSize == 0 ) return 0; // empty event

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
         for (int ch=0; ch<16; ++ch) { // each multiplexer has 16 channels
            // Convert to float from u_int
            double tempADC = *((float*)(&(evt->values[nRead++]))); // average over 8 measurements
            // not connected channels will read 1023
            if (tempADC < 1023) {
               int iCry = p_CAmap->GetArduinoCrystalId(boardID, muxnum, ch);
               if (iCry >= 0 && !(p_CAmap->IsActive(iCry))) continue; // skip not active crystals
               if (iCry < 0 || iCry >= nCry) { 
                  Error("TAGactWDreader", " --- Not well mapped Arduino vs crystal ID. Board: %d mux: %d  ch: %d -> criID %d", boardID, muxnum, ch, iCry);
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
//!
//! \param[in] evt wave dream event descriptor
//! \param[in] p_WDtrigInfo trigger wave form container descriptor
//! \param[in] p_WDmap channel map parameter descriptor
//! \param[in] p_WDtim time parameter descriptor
Int_t TAGactWDreader::DecodeWaveforms(const WDEvent* evt,  TAWDntuTrigger* p_WDtrigInfo, TAWDparTime* p_WDTim, TAWDparMap* p_WDMap)
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

   int TGEN[65];
   
   int nmicro=0;
   TWaveformContainer *w;
   int nhitsA = 0;
   iW=0;
   bool foundFooter = false;
   while (iW < evt->evtSize && !foundFooter) {
      
      if (evt->values[iW] == GLB_EVT_HEADER) {
         if (FootDebugLevel(1)) printf("found glb header::%08x %08x\n", evt->values[iW], evt->values[iW+1]);
            
         iW+=5;
         nmicro = evt->values[iW];
         nmicro = 1000;
            
         iW++; //
         if (FootDebugLevel(1)) printf("word:%08x\n", evt->values[iW]);

         //found evt_header
         if (evt->values[iW] == EVT_HEADER) {
            if (FootDebugLevel(1)) printf("found evt header::%08x   %08x   %08x\n", evt->values[iW],evt->values[iW+1],evt->values[iW+2]);
                     
            iW++;
            trig_type = (evt->values[iW]>>16) & 0xffff;
            ser_evt_number =  evt->values[iW]& 0xffff;
                           
            iW++;
            bco_counter = (int)evt->values[iW];
                  
            iW++;
            while ((evt->values[iW] & 0xffff)== BOARD_HEADER) {
               board_id = (evt->values[iW]>>16)  & 0xffff;
               if (FootDebugLevel(1)) printf("found board header::%08x num%d\n", evt->values[iW], board_id);
               iW++;
               temperature = *((float*)&evt->values[iW]);
               if (FootDebugLevel(1)) printf("temperatrue::%08x num%d\n", evt->values[iW], board_id);
                                                
               iW++;
               range = *((float*)&evt->values[iW]);
                                    
               if (FootDebugLevel(1))
               printf("range::%08x num%d\n", evt->values[iW], board_id);
                        
               iW++;
                        
               sampling_freq =  (evt->values[iW] >>16)& 0xffff;
               flags = evt->values[iW] & 0xffff;
               if (FootDebugLevel(1)) printf("sampling::%08x    %08x   %08x    num%d\n", evt->values[iW],evt->values[iW+1],evt->values[iW+2], board_id);

               iW++;

               while((evt->values[iW] & 0xffff)== CH_HEADER) {
                     
                  char tmp_chstr[3]={'0','0','\0'};
                  tmp_chstr[1] = (evt->values[iW]>>24)  & 0xff;
                  tmp_chstr[0] = (evt->values[iW]>>16)  & 0xff;
                  ch_num = atoi(tmp_chstr);
                  if (FootDebugLevel(1))
                     printf("found channel header::%08x num%d\n", evt->values[iW], ch_num);

                  iW++;
                  trig_cell = (evt->values[iW]>>16) &0xffff;
                                          
                  fe_settings = ((evt->values[iW])&0xffff);
                  iW++;

                  int adctmp=0;
                  int delta=0,deltaold=0;
                  bool jump_up=false;
                  vector<int> w_adc;
                  w_amp.clear();
                                          
                  for(int iSa=0;iSa<512;iSa++) {
                     adc_sa = evt->values[iW];
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
		  //		  SavePlot(w,ch_type);
			   
                  if (FootDebugLevel(1)) printf("type::%s\n", ch_type.data());

                  if (ch_type =="ST") {
                     fSTwaves.push_back(w);
                  } else if (ch_type == "TW") {
                     fTWwaves.push_back(w);
                  } else if (ch_type == "CALO") {
                     fCAwaves.push_back(w);
                  } else if (ch_type == "NEU") {
                     fNEwaves.push_back(w);
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
      } else if (evt->values[iW] == EVT_FOOTER) {
         if (FootDebugLevel(1)) printf("found footer\n");
         return nmicro;
      }

      vector<uint32_t> trigInfoWords;
      int tbo,nbanks;
      if ((evt->values[iW] &0xffff)== TRIG_HEADER) {
         tbo =  (evt->values[iW] >> 16)& 0xffff;
         iW++;
         nbanks =  evt->values[iW] & 0xffff;
         iW++;
         if (FootDebugLevel(1)) printf("found trigger board %d header, nbanks::%d\n",tbo,nbanks);
         for(int ibank=0;ibank<nbanks;ibank++) {
            if (evt->values[iW] == TRGI_BANK_HEADER) {
               if (FootDebugLevel(1)) printf("TRGI header::%08x\n",evt->values[iW]);
               trigInfoWords.push_back(evt->values[iW]);
               iW++;
               int size =  evt->values[iW];
               trigInfoWords.push_back(evt->values[iW]);
               if (FootDebugLevel(1)) printf("size::%08x\n",evt->values[iW]);
               iW++;
               for(int i=0;i<size;i++) {
                  int word= evt->values[iW];
                  if (FootDebugLevel(1)) printf("data::%08x\n",evt->values[iW]);
                  trigInfoWords.push_back(evt->values[iW]);
                  iW++;
               }
            } else if (evt->values[iW] == TGEN_BANK_HEADER) {
               if (FootDebugLevel(1)) printf("TGEN header::%08x\n",evt->values[iW]);
               trigInfoWords.push_back(evt->values[iW]);
               iW++;
               int size = evt->values[iW];
               trigInfoWords.push_back(evt->values[iW]);
               if (FootDebugLevel(1)) printf("size::%08x\n",evt->values[iW]);
               iW++;
               for(int i=0;i<size;i++) {
                  if (FootDebugLevel(1)) printf("data::%08x\n",evt->values[iW]);
                  trigInfoWords.push_back(evt->values[iW]);
                  iW++;
               }
            } else if (evt->values[iW] == TRGC_BANK_HEADER) {
               trigInfoWords.push_back(evt->values[iW]);
               if (FootDebugLevel(1)) printf("TRCG header::%08x\n",evt->values[iW]);
               iW++;
               int size = evt->values[iW];
               if (FootDebugLevel(1)) printf("size::%08x\n",evt->values[iW]);
               trigInfoWords.push_back(evt->values[iW]);
               iW++;
               for(int i=0;i<size;i++) {
                  if (FootDebugLevel(1)) printf("data::%08x\n",evt->values[iW]);
                  trigInfoWords.push_back(evt->values[iW]);
                  iW++;
               }
            }
         }
         p_WDtrigInfo->AddInfo(tbo, trig_type, nbanks, trigInfoWords);
      }
         
         
         if (evt->values[iW] == EVT_FOOTER) {
            if (FootDebugLevel(1))printf("found footer\n");
            iW++;
            foundFooter = true;
         } else {
            printf("warning:: footer not found, event corrupted, iW::%d   last word::%08x!!\n",iW, evt->values[iW]);
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

  TAWDparMap*     p_WDmap = (TAWDparMap*)   fpWDMap->Object();
  
   DeleteHistogram();

   char histoname[100]="";
   if(FootDebugLevel(1))
     cout<<"I have created the WD histo. "<<endl;
      
   strcpy(histoname,"wdTriggerID");
   hTriggerID = new TH1F(histoname, histoname, 64, -0.5, 63.5);
   AddHistogram(hTriggerID);

   strcpy(histoname,"wdTriggerIDstatus");
   hTriggerIDstatus = new TH1F(histoname, histoname, 64, -0.5, 63.5);
   AddHistogram(hTriggerIDstatus);
   
   strcpy(histoname,"wdDAQRate");
   hDAQRate = new TH1F(histoname, histoname, 1000,0,2000);
   AddHistogram(hDAQRate);
   
   strcpy(histoname,"wdSTRate");
   hSTRate = new TH1F(histoname, histoname, 10000,0,100000);
   AddHistogram(hSTRate);

   strcpy(histoname,"wdSTRate100");
   hSTRate100 = new TH1F(histoname, histoname, 10000,0,100000);
   AddHistogram(hSTRate100);

   strcpy(histoname,"wdTWRate");
   hTWRate = new TH1F(histoname, histoname, 10000,0,100000);
   AddHistogram(hTWRate);

   strcpy(histoname,"wdTWRate100");
   hTWRate100 = new TH1F(histoname, histoname, 10000,0,100000);
   AddHistogram(hTWRate100);

   strcpy(histoname,"wdCARate");
   hCARate = new TH1F(histoname, histoname, 10000,0,100000);
   AddHistogram(hCARate);

   strcpy(histoname,"wdCARate100");
   hCARate100 = new TH1F(histoname, histoname, 10000,0,100000);
   AddHistogram(hCARate100);

   strcpy(histoname,"wdDAQRateVsTime");
   hDAQRateVsTime = new TH1F(histoname, histoname, 12000,0,1200);
   AddHistogram(hDAQRateVsTime);
   
   strcpy(histoname,"wdSTRateVsTime");
   hSTRateVsTime = new TH1F(histoname, histoname, 12000,0,1200);
   AddHistogram(hSTRateVsTime);
   
   strcpy(histoname,"wdTWRateVsTime");
   hTWRateVsTime = new TH1F(histoname, histoname, 12000,0,1200);
   AddHistogram(hTWRateVsTime);
   
   strcpy(histoname,"wdCARateVsTime");
   hCARateVsTime = new TH1F(histoname, histoname, 12000,0,1200);
   AddHistogram(hCARateVsTime);

   strcpy(histoname,"wdDAQVsST");
   hDAQVsST = new TH2F(histoname, histoname, 5000,0,50000,100,0,1000);
   AddHistogram(hDAQVsST);

   strcpy(histoname,"wdRatioDAQ_ST");
   hRatioDAQ_ST = new TH1F(histoname, histoname, 1000,0,1);
   AddHistogram(hRatioDAQ_ST);

   vector<int> boardlist;   
   boardlist.insert(boardlist.end(), (p_WDmap->GetBoards("ST")).begin(), (p_WDmap->GetBoards("ST")).end());
   boardlist.insert(boardlist.end(), (p_WDmap->GetBoards("TW")).begin(), (p_WDmap->GetBoards("TW")).end());
   boardlist.insert(boardlist.end(), (p_WDmap->GetBoards("CA")).begin(), (p_WDmap->GetBoards("CA")).end());
   int nBoards = boardlist.size();
   
   for(int iBo=0;iBo<nBoards;iBo++){
     
     snprintf(histoname,100,"deltaClk16_board%d",boardlist.at(iBo));
     hBoardTimeCalib[make_pair(boardlist.at(iBo),16)] = new TH1F(histoname, histoname, 1000,-10,10);
     AddHistogram(hBoardTimeCalib.find(make_pair(boardlist.at(iBo),16))->second);
     
     snprintf(histoname,100,"deltaClk17_board%d",boardlist.at(iBo));
     hBoardTimeCalib[make_pair(boardlist.at(iBo),17)] = new TH1F(histoname, histoname, 1000,-10,10);
     AddHistogram(hBoardTimeCalib.find(make_pair(boardlist.at(iBo),17))->second);
   }

   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Get volt vector
//!
//! \param[in] v_amp amplitude vector
//! \param[in] dynamic_range dynamic range
vector<double> TAGactWDreader::ADC2Volt(vector<int> v_amp, double dynamic_range)
{
   vector<double> v_volt;
   double v_sa;
   
   int adcold=v_amp[5];
   for(int iSa=0;iSa<v_amp.size();iSa++) {
      if (iSa>=5 && iSa < 1020) {
         if (fabs(v_amp[iSa]-adcold)>32000) {
            if (v_amp[iSa]-adcold<32000)v_amp[iSa] += 65535;
            if (v_amp[iSa]-adcold>32000)v_amp[iSa] -= 65535;
         }
         v_sa = v_amp[iSa]/65535.+dynamic_range-0.5;   
         v_volt.push_back(v_sa);
      } else {
         v_volt.push_back(0);
      }
      adcold = v_amp[iSa];
   }

   return v_volt;
}

//------------------------------------------+-----------------------------------
//! Get volt vector clock
//!
//! \param[in] v_amp amplitude vector
vector<double> TAGactWDreader::ADC2Volt_CLK(vector<int> v_amp) {

   vector<double> v_volt;
   double v_sa;
   
   for(int iSa=0;iSa<v_amp.size();iSa++) {
      if (fabs(v_amp[iSa]>35000)) {
         v_amp[iSa] -= 65536;
      }
      v_sa = v_amp[iSa]/65536.;
      v_volt.push_back(v_sa);
   }
   
   return v_volt;
}

//------------------------------------------+-----------------------------------
//! Wave form time calibration
Bool_t TAGactWDreader::WaveformsTimeCalibration()
{

   double dt, t_trig_ref, t_trig;
   TWaveformContainer *wclk_ref;
   
   TAWDparMap*     p_WDmap = (TAWDparMap*)   fpWDMap->Object();
   vector<int> vSTbo = p_WDmap->GetBoards("ST");
   int STbo=27;
   
   if (!vSTbo.size()) {
      if (FootDebugLevel(1)) printf("ST board not defined, I can not apply time calibration!!\n");
      return false;
   } else {
      STbo = vSTbo[0];
   }

   if (!fCLKwaves.count(make_pair(STbo,16))) {
      if (FootDebugLevel(1)) printf("reference clock not found!!\n");
      return false;
   }
   

   wclk_ref = fCLKwaves.find(make_pair(STbo,16))->second;
   t_trig_ref = wclk_ref->GetVectRawT()[(1024-wclk_ref->GetTriggerCellId())%1024];
   //t_trig_ref = wclk_ref->GetVectRawT()[1023];

   
   //clocks alignment and jitter calculation
   map<pair<int,int>, double> clk_jitter;
   map<pair<int,int>, TWaveformContainer*>::iterator it;
   for(it=fCLKwaves.begin(); it != fCLKwaves.end(); it++) {
      TWaveformContainer *wclk = it->second;
      t_trig = wclk->GetVectRawT()[(1024-wclk->GetTriggerCellId())%1024];
      //t_trig = wclk->GetVectRawT()[1023];
      dt = t_trig_ref - t_trig;
      vector<double> calib_time;
      for(int i=0;i<wclk->GetVectRawT().size();i++) {
         calib_time.push_back(wclk->GetVectRawT()[i]+dt);
      }
      wclk->GetVectT() = calib_time;
      double jitter = ComputeJitter(wclk);
      clk_jitter[make_pair(wclk->GetBoardId(), wclk->GetChannelId())] = jitter;
      wclk->SetDeltaClk(jitter);
   }


    

   //st time calibration
   for(int i=0; i<(int)fSTwaves.size();i++) {
      TWaveformContainer *wst = fSTwaves[i];
      int ch_clk = (wst->GetChannelId()<8) ? 16 : 17;
      double phaseST = clk_jitter.find(make_pair(STbo,16))->second;
      double phase = clk_jitter.find(make_pair(wst->GetBoardId(),ch_clk))->second;
      t_trig = wst->GetVectRawT()[(1024-wst->GetTriggerCellId())%1024];
      //t_trig = wst->GetVectRawT()[1023];
      dt = t_trig_ref - t_trig +phaseST - phase;
      //dt = t_trig_ref - t_trig;
      //dt = - phaseST+phase;
      //cout << "jitterST::" << jitter << endl;
      vector<double> calib_time;
      for(int i=0;i<wst->GetVectRawT().size();i++) {
         calib_time.push_back(wst->GetVectRawT()[i]+dt);
         if (wst->GetChannelId()==0) {
         // cout << "ST isa::" << i << "   time::" << wst->m_vectRawT[i]+ t_trig_ref - t_trig << endl;
         }
      }
      fSTwaves[i]->GetVectT() = calib_time;
      fSTwaves[i]->SetDeltaClk(phaseST - phase);
      
      if(hBoardTimeCalib.count(make_pair(wst->GetBoardId(), ch_clk))){
	hBoardTimeCalib.find(make_pair(wst->GetBoardId(), ch_clk))->second->Fill(phaseST - phase);
      }
      


   }
   

   
   
   //tw time calibration
   for(int i=0; i<(int)fTWwaves.size();i++) {
      TWaveformContainer *wtw = fTWwaves[i];
      int ch_clk = (wtw->GetChannelId()<8) ? 16 : 17;
      double phaseST = clk_jitter.find(make_pair(STbo,16))->second;
      double phase = clk_jitter.find(make_pair(wtw->GetBoardId(),ch_clk))->second;
       t_trig = wtw->GetVectRawT()[(1024-wtw->GetTriggerCellId())%1024];
       //t_trig = wtw->GetVectRawT()[1023];
      dt = t_trig_ref - t_trig + phaseST-phase;
      //dt = t_trig_ref - t_trig;
      // cout << "board::" << wtw->GetBoardId() << "  jitter:" << -phaseST+phase << endl;
      // cout << "phaseST::" << phaseST << endl;
      // cout << "phase::" << phase << endl;
      vector<double> calib_time;
      for(int i=0;i<wtw->GetVectRawT().size();i++) {
         calib_time.push_back(wtw->GetVectRawT()[i]+dt);
         if (wtw->GetChannelId()==0) {
         // cout << "TW isa::" << i << "   time::" << wtw->m_vectRawT[i]+ t_trig_ref - t_trig << endl;
         }
      }
      fTWwaves[i]->GetVectT() = calib_time;
      fTWwaves[i]->SetDeltaClk(phaseST - phase);

      if(hBoardTimeCalib.count(make_pair(wtw->GetBoardId(), ch_clk))){
	hBoardTimeCalib.find(make_pair(wtw->GetBoardId(), ch_clk))->second->Fill(phaseST - phase);
      }
   }


   //calo time calibration
   for(int i=0; i<(int)fCAwaves.size();i++) {
      TWaveformContainer *wca = fCAwaves[i];
      int ch_clk = (wca->GetChannelId()<8) ? 16 : 17;
      double jitter = clk_jitter.find(make_pair(wca->GetBoardId(),ch_clk))->second;
      t_trig = wca->GetVectRawT()[(1024-wca->GetTriggerCellId())%1024];
      dt = t_trig_ref - t_trig - jitter;
      vector<double> calib_time;
      for(int i=0;i<wca->GetVectRawT().size();i++) {
         calib_time.push_back(wca->GetVectRawT()[i]+dt);
         if (wca->GetChannelId()==0) {
         }
      }
      fCAwaves[i]->GetVectT() = calib_time;
   }
   
   //Neutrons time calibration
   for(int i=0; i<(int)fNEwaves.size();i++) {
      TWaveformContainer *wnu = fNEwaves[i];
      int ch_clk = (wnu->GetChannelId()<8) ? 16 : 17;
      double jitter = clk_jitter.find(make_pair(wnu->GetBoardId(),ch_clk))->second;
      t_trig = wnu->GetVectRawT()[(1024-wnu->GetTriggerCellId())%1024];
      dt = t_trig_ref - t_trig - jitter;
      vector<double> calib_time;
      for(int i=0;i<wnu->GetVectRawT().size();i++) {
         calib_time.push_back(wnu->GetVectRawT()[i]+dt);
         if (wnu->GetChannelId()==0) {
         }
      }
      fNEwaves[i]->GetVectT() = calib_time;
   }

   if (FootDebugLevel(1)) printf("WD calibration performed\n");

   return true;
}

//------------------------------------------+-----------------------------------
//! Compute time jitter
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

   //     t1 = vtime[i-1];
   //     t2 = vtime[i];
   //     a1 = vamp[i-1];
   //     a2 = vamp[i];
         
   //     if (a1<wave_0 && a2>=wave_0) {
   //        m = (a2-a1)/(t2-t1);
   //        q = a1 - m*t1;
   //        tzerocrossing = (wave_0-q/m);
   //        tzeros.push_back(tzerocrossing);
   //        ncycles++;
   //        nclock.push_back((double)ncycles);
   //     }
   // }

   for(int i=vtime.size()-1; i>=1; i--) {

      t1 = vtime[i-1];
      t2 = vtime[i];
      a1 = vamp[i-1];
      a2 = vamp[i];
      
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
   //for(int i=0;i<nclock.size();i++)cout << "nclock::" << nclock[i] << endl;
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
   // cout << "board::" << wclk->GetBoardId() << "  phase::" << phase << "   period::" << period << endl;
   
   return phase;
}

//------------------------------------------+-----------------------------------
//! Create hits
//!
//! \param[in] p_straw ST raw data container
//! \param[in] p_twraw TW raw data container
//! \param[in] p_caraw CA raw data container
//! \param[in] p_CAmap CA map descriptor
Bool_t TAGactWDreader::CreateHits(TASTntuRaw* p_straw, TATWntuRaw* p_twraw, TACAntuRaw* p_caraw, TANEntuRaw* p_neraw)
{
   TAWDparTime*    p_WDtim = (TAWDparTime*)fpWDTim->Object();
  
   TACAparMap*          p_CAmap = 0x0;
   if (fpCAMap)
      p_CAmap = (TACAparMap*)  fpCAMap->Object();
 
   TANEparMap*          p_NEmap = 0x0;
   if (fpNEMap)
      p_NEmap = (TANEparMap*)  fpNEMap->Object();

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
      p_straw->NewHit(fSTwaves[i], algoST, fracST, delST);
      if (FootDebugLevel(1)) printf("ST hit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoST.data(), fracST, delST);
   }

   
   for(int i=0; i<(int)fTWwaves.size(); i++) {
      p_twraw->NewHit(fTWwaves[i], algoTW, fracTW, delTW);
      if (FootDebugLevel(1)) printf("TW hit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoTW.data(), fracTW, delTW);
   }
   if (p_CAmap) {
      int nCry = p_CAmap->GetCrystalsN();
      for(int i=0; i<(int)fCAwaves.size(); i++) {
         int board_id = fCAwaves[i]->GetBoardId();
         int ch_num = fCAwaves[i]->GetChannelId();
         int criID = p_CAmap->GetCrystalId(board_id, ch_num);
         if (criID >= 0 && !(p_CAmap->IsActive(criID))) continue; // skip not active crystals
         if (criID < 0 || criID >= nCry) {
            Error("CreateHits", " CALO Hit skipped --- Not well mapped WD vs crystal ID. board: %d ch: %d -> iCry %d", board_id, ch_num, criID);
            continue;
         }
         p_caraw->NewHit(fCAwaves[i], fTempCA[criID]);
         if (FootDebugLevel(1)) printf("CA hit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoCA.data(), fracCA, delCA);
      }
   }
   if (fpNEMap) {
      int nMod = p_NEmap->GetModulesN();
      for(int i=0; i<(int)fNEwaves.size(); i++) {
         int board_id = fNEwaves[i]->GetBoardId();
         int ch_num = fNEwaves[i]->GetChannelId();
         int modID = p_NEmap->GetModuleId(board_id, ch_num);
         if (modID < 0 || modID >= nMod) {
            Error("CreateHits", " Neutron Hit skipped --- Not well mapped WD vs module ID. board: %d ch: %d -> iMod %d", board_id, ch_num, modID);
            continue;
         }
         p_neraw->NewHit(fNEwaves[i]);
         if (FootDebugLevel(1)) printf("NE hit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoCA.data(), fracCA, delCA);
      }
   }
   
   p_straw->NewSuperHit(fSTwaves, algoST, fracST, delST);
   if (FootDebugLevel(1)) printf("ST superhit created, time calculated with algo::%s, frac::%lf  del::%lf\n", algoST.data(), fracST, delST);

   if (FootDebugLevel(1)) printf("WD hits performed\n");

   return true;
}

//------------------------------------------+-----------------------------------
//! Clear
void TAGactWDreader::Clear()
{
   for(int i=0; i<fSTwaves.size(); i++) {
      delete fSTwaves[i];
   }
   for(int i=0; i<fTWwaves.size(); i++) {
      delete fTWwaves[i];
   }
   for(int i=0; i<fCAwaves.size(); i++) {
      delete fCAwaves[i];
   }
   for(int i=0; i<fNEwaves.size(); i++) {
      delete fNEwaves[i];
   }
   map<pair<int,int>, TWaveformContainer*>::iterator it;
   for(it=fCLKwaves.begin(); it != fCLKwaves.end(); it++) {
      delete it->second;
   }
      
   fSTwaves.clear();
   fTWwaves.clear();
   fCAwaves.clear();
   fNEwaves.clear();
   fCLKwaves.clear();

   TAWDntuTrigger* p_WDtrigInfo = (TAWDntuTrigger*)   fpWDtrigInfo->Object();
   p_WDtrigInfo->Clear();

   return;
}

//------------------------------------------+-----------------------------------
//! Read stand alone
//!
//! \param[in] endoffile end of file flag
//! \param[in] p_WDtrigInfo trigger wave form container descriptor
//! \param[in] p_WDmap mapping parameter descriptor
//! \param[in] p_WDtim time parameter descriptor
//! \param[in] p_CAmap CA map descriptor
Int_t TAGactWDreader::ReadStdAloneEvent(bool &endoffile, TAWDntuTrigger *p_WDtrigInfo, TAWDparTime *p_WDTim, TAWDparMap *p_WDMap) 
{
   TACAparMap* p_CAmap = (TACAparMap*)   fpCAMap->Object();

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

      // if ( (word == EventMarker) && !startEvent ) {
      if ( !startEvent ) {
         startEvent = true;
         if (FootDebugLevel(1)) printf("\n ================= Found new event =================\n");
      } else if (word==EVT_HEADER && startEvent) {
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
      else if( word == ARDUINO_HEADER) {

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
                        if (iCry >= 0 && !(p_CAmap->IsActive(iCry))) continue; // skip not active crystals
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

   }

  
  return nmicro;
}

//------------------------------------------+-----------------------------------
//! Save plots
//!
//! \param[in] w wave form container
//! \param[in] type type of data
void  TAGactWDreader::SavePlot(TWaveformContainer *w, string type)
{
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
   WaveGraph.GetXaxis()->SetRangeUser(0,300);

   c.Print(Form("waveform%s_board%d_ch%d_nev%d.png", type.data(), w->GetBoardId(), w->GetChannelId(), fEventsN));
}
