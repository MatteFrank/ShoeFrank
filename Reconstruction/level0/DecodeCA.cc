// To be use on CA testbeams.
// read daq from WD boards and write file with simple ntuple quantities
// to used by standalone analysis/calibration software developed by Lorenzo
// 2022, E. Lopez


#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TStopwatch.h>
#include <TGeoVolume.h>
#include <TRandom3.h>
#include <TCanvas.h>
#include <TRootCanvas.h>
#include <TAttCanvas.h>
#include <TApplication.h>
#include <TGApplication.h>
#include <TH1.h>

#include "TAGcampaignManager.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGWDtrigInfo.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGbaseWDparMap.hxx"
#include "TWaveformContainer.hxx"
#include "TASTparGeo.hxx"

#include "TAGparGeo.hxx"
#include "TACAparGeo.hxx"
#include "TACAparMap.hxx"

#include "TAGrecoManager.hxx"



////////////////////////////////////////////////////////////////////////////////
//------------------------------------------+-----------------------------------
//! CAactRaw2Ntu Action class
// Main parts taken from TAGactWDreader

#define NSAMPLING 1024
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
#define TRGC_BANK_HEADER 0x43475254


class CAactRaw2Ntu : public TAGaction {

public:

  explicit        CAactRaw2Ntu( TAGparaDsc* pCAmap=0,
                                TAGparaDsc* pWDmap=0,
                                TAGparaDsc* pWDtim=0);
  virtual        ~CAactRaw2Ntu();

         Bool_t   Action();
         void     SetTreeBranches();
         Int_t    Open(const TString &name);
         Int_t    OpenOut(const TString &nameOut);
         Int_t    Close();
         Int_t    CloseOut();
         Int_t    UpdateFile();
         void     Clear();
  inline void     SetInitName(TString name) {
                        fInitName = name;
                        fCurrName = name;
                     } 
         void     DrawChAmp(int ch, Option_t* opt = NULL);


private:
         Int_t    ReadStdAloneEvent(bool &endoffile, 
                                   // TAGWDtrigInfo* p_WDtrigInfo, 
                                    TAGbaseWDparTime *p_WDTim, 
                                    TAGbaseWDparMap *p_WDMap);
         Bool_t   WaveformsTimeCalibration();
  vector<double>  ADC2Volt(vector<int>, double);
 

private:
   TAGparaDsc*     fpCAParMap;  // parameter dsc
   TAGparaDsc*     fpWDMap;     // parameter dsc
   TAGparaDsc*     fpWDTim;     // parameter dsc

   FILE            *fWDstream;
   TFile           *fCAnutOut;
   TString         fInitName;
   TString         fCurrName;
   TTree           *fTree;
   Int_t           fEventsN;
   Int_t           fnCry;
   Int_t           fProcFiles;
   Int_t           fMaxFiles;  

   TACAparGeo*    fGeometry;

   //vector<TWaveformContainer*> fCAwaves; //waveform

   double  *       fTempCh;
   UShort_t **     fAmpCh;     // waveform for each channel/crystal

   double          fRange;
};


//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in]  pWDMap  daq map descriptor
CAactRaw2Ntu::CAactRaw2Ntu(TAGparaDsc* pCAmap, TAGparaDsc* pWDmap, TAGparaDsc* pWDtim)
  : TAGaction("CAactRaw2Ntu", "CAactRaw2Ntu - Unpack CA raw data"),
  //fpNtuEvt(pNtuEvt),
  fpCAParMap(pCAmap),
  fpWDMap(pWDmap),
  fpWDTim(pWDtim)
{
   AddPara(pCAmap, "TACAparMap");
   AddPara(pWDmap, "TAGbaseWDparMap");
   AddPara(pWDtim, "TAGbaseWDparTime");

   fGeometry = (TACAparGeo*) gTAGroot->FindParaDsc(TACAparGeo::GetDefParaName(), "TACAparGeo")->Object();
 
   fProcFiles=0;  
   fMaxFiles=1;

   int nCry = fGeometry->GetCrystalsN();

   fTempCh = new double [nCry];
   fAmpCh = new UShort_t* [nCry];

   for (int cryID=0; cryID<nCry; ++cryID) {
      fAmpCh[cryID] = new UShort_t [NSAMPLING];
   }
   fRange = 0.1;

}

//------------------------------------------+-----------------------------------
//! Destructor.
CAactRaw2Ntu::~CAactRaw2Ntu() {   

   delete [] fTempCh;

   int nCry = fGeometry->GetCrystalsN();

   for (int cryID=0; cryID<nCry; ++cryID) {
      delete [] fAmpCh[cryID];
   }
   delete [] fAmpCh;

}


//------------------------------------------+-----------------------------------
Int_t CAactRaw2Ntu::Open(const TString &fname) {

   fWDstream = fopen(fname.Data(),"r");
   if (fWDstream == NULL) {
      cout << "ERROR in CAactRaw2Ntu::cannot open the file = " << fname.Data() << endl;
      return kFALSE;
   } else {
      cout <<"CAactRaw2Ntu::file " << fname.Data() << " opened" << endl;
   }

   return kTRUE;
}

//------------------------------------------+-----------------------------------
Int_t CAactRaw2Ntu::OpenOut(const TString &nameOut) {

   fCAnutOut = TFile::Open( nameOut.Data(), "RECREATE");
   if (fCAnutOut->IsZombie()) {
      Fatal("OpenOut", "Can't open output file %s", nameOut.Data());
      return 0;
   }

   return kTRUE;
}

//------------------------------------------+-----------------------------------
Int_t CAactRaw2Ntu::UpdateFile() {
  
   do {
      Int_t pos1 = fInitName.Last('_');
      Int_t pos2 = fInitName.Last('.');
      TString baseName = fInitName(0, pos1+1);
      TString ext = fInitName(pos2, fInitName.Length()-pos2);
      TString slocRunNum = fInitName(pos1+1, pos2-pos1-1);
      Int_t locRunNum = atoi(slocRunNum.Data());
      fCurrName = baseName+to_string(locRunNum+fProcFiles)+ext;
      fProcFiles++;
      if (fProcFiles>=fMaxFiles) {
         return kFALSE;
      }
   } while(access(fCurrName.Data(), F_OK )!=0);

   return kTRUE;
}


//------------------------------------------+-----------------------------------
Int_t CAactRaw2Ntu::Close() {

   if (fWDstream != NULL)
      fclose(fWDstream);
   fWDstream = NULL;

   return kTRUE;
}

//------------------------------------------+-----------------------------------
Int_t CAactRaw2Ntu::CloseOut() {
   // write tree and close output file
   fTree->Write();

   if (fCAnutOut != NULL) fCAnutOut->Close();

   return kTRUE;
}


//------------------------------------------+-----------------------------------
void CAactRaw2Ntu::Clear() {

   int nCry = fGeometry->GetCrystalsN();

   fill_n(fTempCh, nCry, 0);
   for (int cryID=0; cryID<nCry; ++cryID) {
      fill_n(fAmpCh[cryID],  NSAMPLING, 0);
   }
 
   return;
}

//------------------------------------------+-----------------------------------
void CAactRaw2Ntu::SetTreeBranches() {

   // Creating the TTree with a branch for each channel
   fTree = new TTree("tree", "Waveforms");
   
   // Branch setting

   // Get number of channels (crystals)
   int nCry = fGeometry->GetCrystalsN();

   for (int cryID=0; cryID<nCry; ++cryID) {
      fTree->Branch(Form("ch_%d", cryID), fAmpCh[cryID], Form("waveform[%d]/s", NSAMPLING));
      fTree->Branch(Form("temp_adc_%d", cryID), &fTempCh[cryID]);
   }

   // cross check WD map vs geo maps
   TAGbaseWDparMap* pWDmap = (TAGbaseWDparMap*)fpWDMap->Object();
   TACAparMap* pCAmap = (TACAparMap*)fpCAParMap->Object();
   vector<int> boards = pWDmap->GetBoards("CALO");

   for (auto & board : boards) {
      //cout << "Checking board " << board << endl;
      for(int iCh=0; iCh<18; iCh++) {
         string ch_type;
         ch_type = pWDmap->GetChannelType(board, iCh);
   
         if (ch_type == "CALO") {
            int cryID = pCAmap->GetCrystalId(board, iCh);
            if (cryID >= nCry) {
               Error("SetTreeBranches", "CryID (%d) from parmap bigger than number of crystal (%d) in geomap for board %d, channel %d", cryID, nCry, board, iCh);
               continue;
            } 
         }
      }
   }

}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t CAactRaw2Ntu::Action() {

   Clear();

   bool eof = false;
   int nmicro = ReadStdAloneEvent(eof, (TAGbaseWDparTime*)fpWDTim->Object(), (TAGbaseWDparMap*)fpWDMap->Object());

   // WaveformsTimeCalibration();
   fTree->Fill();
  
   fEventsN++;

   if (eof) {
      Close();
      Bool_t status = UpdateFile();
      if (status) {
         if ( Open(fCurrName) )
            return kTRUE;
         else
            return kFALSE;
      } else {
         return kFALSE;
      }
   }
  
   return kTRUE;
}

//------------------------------------------+-----------------------------------
void CAactRaw2Ntu::DrawChAmp(int ch, Option_t *opt) {

   // Draw 

   TCanvas *c1 = (TCanvas *)gROOT->FindObject(Form("wd_ch%d", ch));
   if( !c1 ) { 
      c1 = new TCanvas( Form("wd_ch%d", ch), Form("Waveforms ch #%d",ch),  10, 10, 600, 600); 
      TRootCanvas *rc = (TRootCanvas *)c1->GetCanvasImp();
      rc->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
      gSystem->ProcessEvents();
   }
   c1->cd();
   
   TH1F * hChADC = (TH1F *)gROOT->FindObject(Form("ca_ch%d", ch));
   if(!hChADC) {
      hChADC = new TH1F(Form("ca_ch%d", ch), Form("ch #%d waveform (ADC counts)", ch), NSAMPLING, 0, NSAMPLING);
      //hChADC->GetYaxis()->SetRangeUser(fRange+fRange/2, TMath::Abs(fRange/5.)); // ?Volt dynamic range?
      hChADC->GetYaxis()->SetRangeUser(0, 65535); // ADC range 16 bits
      gStyle->SetOptStat(0);
   }

   for (int i=0; i<NSAMPLING; ++i) {
      hChADC->SetBinContent(i, fAmpCh[ch][i]);
   }

   TString option = opt;
 
   if (option.Contains("SAME")) 
      hChADC->DrawCopy(opt);
   else
      hChADC->Draw(opt); // clear, start new draw

   c1->Modified();
   c1->Update();
   gSystem->ProcessEvents();

}


//------------------------------------------+-----------------------------------
vector<double> CAactRaw2Ntu::ADC2Volt(vector<int> v_amp, double dynamic_range) {
   vector<double> v_volt;
   double v_sa;
   
   int adcold = v_amp.at(5);
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
Int_t CAactRaw2Ntu::ReadStdAloneEvent(bool &endoffile, 
                                         //TAGWDtrigInfo *p_WDtrigInfo, 
                                         TAGbaseWDparTime *p_WDTim, 
                                         TAGbaseWDparMap *p_WDMap) {

   u_int word;
   int board_id=0, ch_num=0;;
   float temperature=0, range=0;
   float sampling_freq;
   int trig_cell=0;
   //int adc_sa=0;
   int ser_evt_number;

   //int TGEN[65];
   int nmicro=0;
   //TWaveformContainer *w;
   bool endEvent = false;
   bool startEvent = false;
   //bool foundTimeCalib = false;
   bool eof=false;
   int ret=0;

   int nCry = fGeometry->GetCrystalsN();

   
   while(!endEvent) {

      ret = fread(&word, 4, 1, fWDstream); 
      if (FootDebugLevel(1)) printf("word:%08x\n", word);
      if (ret == 0) {
         endoffile = true;
         return 0;
      }

      if (word == FILE_HEADER) {
         fseek(fWDstream, -4, SEEK_CUR);
         p_WDTim->GetTimeInfo(fWDstream);
         //foundTimeCalib = true;

      } else if (word == EVT_HEADER && !startEvent) {

         if (FootDebugLevel(1)) printf("found evt header::%08x", word);
         startEvent = true;
         ret = fread(&word, 4, 1, fWDstream);
         //ser_evt_number =  word& 0xffff;
         
         ret = fread(&word, 4,1, fWDstream);
         
         // read waveDAQ boards
         while(fread(&word, 4,1,fWDstream) !=0 && (word & 0xffff)== BOARD_HEADER){
            board_id = (word>>16)  & 0xffff;
            if (FootDebugLevel(1)) printf("found board header::%08x num%d\n", word, board_id);

            ret = fread(&word, 4, 1, fWDstream);
            temperature = *((float*)&word);
            if (FootDebugLevel(1)) printf("temperature::%08x num%d\n", word, board_id);
            
            ret = fread(&word, 4, 1, fWDstream);
            range = *((float*)&word);
            fRange = range;
            
            if (FootDebugLevel(1)) printf("range::%08x num%d\n", word, board_id);
               
            ret = fread(&word, 4, 1, fWDstream);
            //sampling_freq =  (word >>16)& 0xffff;
            //flags = word & 0xffff;
            //if (FootDebugLevel(1)) printf("sampling::%08x  num%d\n", word, board_id);

            while(fread(&word, 4,1,fWDstream) !=0 && (word & 0xffff)== CH_HEADER){	
               char tmp_chstr[3]={'0','0','\0'};
               tmp_chstr[1] = (word>>24)  & 0xff;
               tmp_chstr[0] = (word>>16)  & 0xff;
               ch_num = atoi(tmp_chstr);
               if (FootDebugLevel(1)) printf("found channel header::%08x num%d\n", word, ch_num);
                  
               ret = fread(&word, 4, 1, fWDstream);
               trig_cell = (word>>16) &0xffff;
               //fe_settings = ((word)&0xffff);

               // read a block of 32 bit NSAMPLING/2
               // split each 32 bit into two 16 bit values
               int w_adc[NSAMPLING];
               uint32_t usa[NSAMPLING/2];
               ret = fread(usa, 4, NSAMPLING/2, fWDstream);
               for(int iSa=0; iSa<NSAMPLING/2; iSa++){
                  uint adc_sa = usa[iSa];
                  int adctmp = ((adc_sa >> 16) & 0xffff);
                  //uint adctmp = (adc_sa & 0xffff);	  
                  w_adc[2*iSa] = adctmp;
                  //adctmp = ((adc_sa >> 16) & 0xffff);
                  adctmp = (adc_sa & 0xffff);
                  w_adc[2*iSa+1] = adctmp;
               }

               /*
               w = new TWaveformContainer();
               w->SetChannelId(ch_num);
               w->SetBoardId(board_id);
               w->GetVectA() = w_amp;
               //w->GetVectRawT() = p_WDTim->GetRawTimeArray(board_id, ch_num, trig_cell);
               //w->GetVectT() = w->GetVectRawT();
               w->SetNEvent(fEventsN);
               w->SetEmptyFlag(false);
               //w->SetTrigType(trig_type);
               w->SetTriggerCellId(trig_cell);
               */

               if (FootDebugLevel(1)) printf("found waveform board:%d  channel:%d\n", board_id,ch_num);
                  
               string ch_type;
               ch_type = p_WDMap->GetChannelType(board_id, ch_num);

               if (FootDebugLevel(1)) printf("type::%s\n", ch_type.data());
            
               // Only process CALO channels
               if (ch_type == "CALO") {

                  // convert ADC count to Volt
                  //vector<int> wd_adc(w_adc, w_adc+NSAMPLING); // convert array to vector
                  //vector<double> w_amp;
                  //w_amp = ADC2Volt(wd_adc, range);

                  int criID = ((TACAparMap*)fpCAParMap->Object())->GetCrystalId(board_id, ch_num);
                  if (criID >=0 && criID < nCry) {
                     //vector<double> v = w->GetVectA();
                     uint adc5 = w_adc[5];
                     for(int i=0; i<NSAMPLING; ++i) {

                       // if (i>5 && i < NSAMPLING-4) {
                       //    if (fabs(w_adc[i]-adc5)>32000) { // 32000 ??? some setting on the boards???
                       //       if (w_adc[i]-adc5<32000)w_adc[i] += 65535;
                       //       if (w_adc[i]-adc5>32000)w_adc[i] -= 65535;
                       //    }
                       // }
                        fAmpCh[criID][i] = (UShort_t)w_adc[i];    // UShort_t
                     //   fAmpCh[criID][i] = w_amp[i];            // Float_t
                     }
                  }

               } 
               //else if (ch_type == "CLK"){
                  //fCLKwaves.insert(std::pair<std::pair<int,int>, TWaveformContainer*>(make_pair(board_id, ch_num),w));
               //} 
            }

            fseek(fWDstream, -4, SEEK_CUR);
         }
         fseek(fWDstream, -4, SEEK_CUR);

         // Read CALO temp sensors
         //while(fread(&word, 4,1,fWDstream) !=0 && (word & 0xffff)== TEMP_HEADER){

            // dummy fake temp for each event
            TRandom3 t(0);
            for (int cryID=0; cryID<nCry; ++cryID) {   //
               fTempCh[cryID] = t.Gaus(25, .5);
            }

         //}
        
 

      /*
      } else if((word &0xffff)== TRIG_HEADER){
         if(foundTimeCalib)continue;
         vector<uint32_t> trigInfoWords;
         int tbo,nbanks;
         tbo =  (word >> 16)& 0xffff;

         ret = fread(&word, 4, 1, fWDstream);
         nbanks =  word & 0xffff;
         
         if (FootDebugLevel(1)) printf("found trigger board %d header, nbanks::%d, word::%08x\n",tbo,nbanks,word);
         for(int ibank=0;ibank<nbanks;ibank++){
            ret = fread(&word, 4, 1, fWDstream);
            if(word == TRGI_BANK_HEADER){
            if (FootDebugLevel(1)) printf("trig header::%08x\n",word);
            trigInfoWords.push_back(word);
            ret = fread(&word, 4, 1, fWDstream);
            int size =  word;
            trigInfoWords.push_back(word);
            if (FootDebugLevel(1)) printf("size::%08x\n",word);
            for(int i=0;i<size;i++){
               ret = fread(&word, 4, 1, fWDstream);
               if (FootDebugLevel(1)) printf("data::%08x\n",word);
               trigInfoWords.push_back(word);
            }
            }else if(word == TGEN_BANK_HEADER){
            if (FootDebugLevel(1)) printf("trig header::%08x\n",word);
            trigInfoWords.push_back(word);
            ret = fread(&word, 4, 1, fWDstream);
            int size = word;
            trigInfoWords.push_back(word);
            if (FootDebugLevel(1)) printf("size::%08x\n",word);
            for(int i=0;i<size;i++){
               ret = fread(&word, 4, 1, fWDstream);
               if (FootDebugLevel(1)) printf("data::%08x\n",word);
               trigInfoWords.push_back(word);
            }
            } else if((word)==  TRGC_BANK_HEADER) {
               trigInfoWords.push_back(word);
               ret = fread(&word, 4, 1, fWDstream);
               int size = word;
               if(FootDebugLevel(1)) printf("TRCG bank size::%08x\n", word);
               trigInfoWords.push_back(word);
               for(int i=0;i<size;i++){
                  ret = fread(&word, 4, 1, fWDstream);
                  if(FootDebugLevel(1)) printf("word::%08x\n",word);
                  trigInfoWords.push_back(word);
               }
            }
         }
         p_WDtrigInfo->AddInfo(tbo, trig_type, nbanks, trigInfoWords);
      */

      } else if(word == EVT_HEADER && startEvent) {
         if (FootDebugLevel(1)) printf("found new event\n");
         fseek(fWDstream, -4, SEEK_CUR);
         endEvent = true;
      }
   }

   return nmicro;
}


int main (int argc, char *argv[])  {

   TString in("");
   TString out("");
   TString exp("");
   
   //Bool_t mth = false;
   Bool_t draw = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t nSkipEv = 0;
   Int_t nFiles=1;

   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
   //   if(strcmp(argv[i],"-nsk") == 0)   { nSkipEv = atoi(argv[++i]); }  // Number of events to be skip
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
   //   if(strcmp(argv[i],"-nf") == 0)    { nFiles = atoi(argv[++i]);  }   //  Number of files
   //   if(strcmp(argv[i],"-mth") == 0)   { mth = true;   } // enable multi threading (for clustering)
      if(strcmp(argv[i],"-draw") == 0)   { draw = true;   } // enable draw waveform sampling

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" DecoderCA help:"<<endl;
         cout<<" Ex: DecoderCA [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] data input file"<<endl;
         cout<<"      -out path/file : [def=*_standalone.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
   //      cout<<"      -nsk value     : [def=0] Skip number of events"<<endl;
   //      cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experiment name for config/geomap extention"<<endl;
   //      cout<<"      -mth           : enable multi threading (for clustering)"<<endl;
         return 1;
      }
   }
   
   //TGApplication::CreateApplication();   
   TApplication* theApp = new TApplication("App", 0, 0);   

   // global par
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   TAGroot tagr;

   // campaign manager
   TAGcampaignManager* campManager = new TAGcampaignManager(exp);
   campManager->FromFile();

   // geo file for CA
   TAGparaDsc* parGeoCA = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
   TACAparGeo* pGeoMap = (TACAparGeo*)parGeoCA->Object();
   TString parFileName = campManager->GetCurGeoFile(TACAparGeo::GetBaseName(), runNb);
   pGeoMap->FromFile(parFileName);
   
   // par map for CA
   TAGparaDsc*  pParMapCa = new TAGparaDsc("caMap", new TACAparMap());
   TACAparMap* parMapCA = (TACAparMap*)pParMapCa->Object();
   parFileName = campManager->GetCurMapFile(TACAparGeo::GetBaseName(), runNb);
   parMapCA->FromFile(parFileName.Data());

   // WD map
   TAGparaDsc *pParMapWD = new TAGparaDsc("WDMap", new TAGbaseWDparMap());
   TAGbaseWDparMap* parMapWD = (TAGbaseWDparMap*)pParMapWD->Object();
   parFileName = campManager->GetCurMapFile(TASTparGeo::GetBaseName(), runNb);
   parMapWD->FromFile(parFileName.Data());

   // WD time map
   TAGparaDsc *pParTimeWD = new TAGparaDsc("WDTim", new TAGbaseWDparTime());
   TAGbaseWDparTime* parTimeWD = (TAGbaseWDparTime*) pParTimeWD->Object();
   parFileName = campManager->GetCurCalFile(TASTparGeo::GetBaseName(), runNb);
   parTimeWD->FromFileCFD(parFileName.Data());

   CAactRaw2Ntu * caDatReader = new CAactRaw2Ntu(pParMapCa, pParMapWD, pParTimeWD);
   if (!caDatReader->Open(in)) return -1;
   caDatReader->SetInitName(in);
   caDatReader->SetTreeBranches();
   
   // Set output
   TString nameOut;
   if (out.IsNull()) {
      Int_t pos = in.Last('.');
      nameOut = in(0, pos);
      nameOut.Append("_standalone.root");
   } else 
      nameOut = out;

   caDatReader->OpenOut(nameOut);

   tagr.AddRequiredItem(caDatReader);

   int frequency = 50;     // frequency to update info/draw
   int frequency2 = 1000;  // frequency to clean waveform canvas


   TStopwatch watch;
   watch.Start();

   tagr.BeginEventLoop();
   

   Int_t ev = 0;
   while (tagr.NextEvent() ) {
      gSystem->ProcessEvents();
      ev++;
      if (ev % frequency == 0) {
         cout << "Event: " << ev << endl;

         if (draw) {
            if (ev % frequency2 == 0 || ev == 0)
               caDatReader->DrawChAmp(0, ""); // clean pad
            else
               caDatReader->DrawChAmp(0, "SAME");
         }
      }
      if (ev > nTotEv) break;
   }

   tagr.EndEventLoop();
   watch.Print();


   caDatReader->CloseOut();

   //theApp->Run(); // keep canvas open



   /*
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   
   geoTrafo->FromFile(parFileName);
   
   TFile* f = new TFile(name.Data());
   f->ls();
   
   TTree* tree = (TTree*)gDirectory->Get("EventTree");
   
   Evento *ev  = new Evento();
   
   EVENT_STRUCT evStr;
   
   ev->FindBranches(tree,&evStr);
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillMCCa(&evStr, runNumber);
   
   tagr.AddRequiredItem("caActRaw");
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_OutCa.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   caActRaw->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Long64_t nentries = tree->GetEntries();
   
   cout<<"Running against ntuple with:: "<<nentries<<" entries!"<<endl;
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t ientry = 0; ientry < nentries; ientry++) {
      
      nb = tree->GetEntry(ientry);
      nbytes += nb;
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
      //if (ientry == 500)
      if (ientry == 50000)
         break;
      
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
   */
}




