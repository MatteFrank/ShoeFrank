// To be use on CA test-beams.
// read daq from WD boards and Arduino 
// write file with simple ntuple quantities
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

#include "DAQMarkers.hh"



////////////////////////////////////////////////////////////////////////////////
//------------------------------------------+-----------------------------------
//! CAactRaw2Ntu Action class
//! WD read parts taken from TAGactWDreader

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

#define ARDUINO_HEADER 0x00463730


class CAactRaw2Ntu : public TAGaction {

public:

  explicit        CAactRaw2Ntu( TAGparaDsc* pCAmap=0,
                                TAGparaDsc* pWDmap=0);
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
        void      SetMaxFiles(Int_t value) {fMaxFiles = value;}

private:
         Int_t    ReadStdAloneEvent(bool &endoffile, 
                                    TAGbaseWDparMap *p_WDMap);
         Bool_t   WaveformsTimeCalibration();
  vector<double>  ADC2Volt(vector<int>, double);
  double          ADC2Temp(double adc);
 

private:
   TAGparaDsc*     fpCAParMap;  // parameter dsc
   TAGparaDsc*     fpWDMap;     // parameter dsc

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

   // output 
   double  *       fTempCh;
   UShort_t **     fAmpCh;     // waveform for each channel/crystal

   double          fRange;
};


//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in]  pCAMap  CA map descriptor
//! \param[in]  pWDMap  WD daq map descriptor
CAactRaw2Ntu::CAactRaw2Ntu(TAGparaDsc* pCAmap, TAGparaDsc* pWDmap)
  : TAGaction("CAactRaw2Ntu", "CAactRaw2Ntu - Unpack CA raw data"),
  //fpNtuEvt(pNtuEvt),
  fpCAParMap(pCAmap),
  fpWDMap(pWDmap)
{
   AddPara(pCAmap, "TACAparMap");
   AddPara(pWDmap, "TAGbaseWDparMap");

   fGeometry = (TACAparGeo*) gTAGroot->FindParaDsc(TACAparGeo::GetDefParaName(), "TACAparGeo")->Object();
 
   fProcFiles=1;  
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
//! Update filename to read next file   xxxx_0001.data, xxxx_0002.data, ...
Int_t CAactRaw2Ntu::UpdateFile() {
  
   do {
      Int_t pos1 = fInitName.Last('_');
      Int_t pos2 = fInitName.Last('.');
      TString baseName = fInitName(0, pos1+1);
      TString ext = fInitName(pos2, fInitName.Length()-pos2);
      TString slocRunNum = fInitName(pos1+1, pos2-pos1-1);
      Int_t locRunNum = atoi(slocRunNum.Data());
      fCurrName = baseName+TString(Form("%04d",locRunNum+fProcFiles))+ext;
      cout << fCurrName << endl;
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

   for (int cryID=0; cryID<nCry; ++cryID) {
      fTempCh[cryID] = 0;
      for(int i=0; i<NSAMPLING; ++i) {
         fAmpCh[cryID][i] = 0;
      }
   }
 
   return;
}

//------------------------------------------+-----------------------------------
void CAactRaw2Ntu::SetTreeBranches() {

   // Creating the TTree with a branch for each channel
   fTree = new TTree("tree", "Waveforms");

   // Get number of channels (crystals)
   int nCry = fGeometry->GetCrystalsN();
   // Set Branch 
   for (int cryID=0; cryID<nCry; ++cryID) {
      fTree->Branch(Form("ch_%d", cryID), fAmpCh[cryID], Form("waveform[%d]/s", NSAMPLING));
      fTree->Branch(Form("temp_adc_%d", cryID), &fTempCh[cryID]);
   }

   //  DEBUG
   //  cross check WD map vs geo maps
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

   ReadStdAloneEvent(eof, (TAGbaseWDparMap*)fpWDMap->Object());

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
// Draw a canvas with the waveform for channel ch
//!
//! \param[in]  ch  channel
//! \param[in]  opt histogram option 
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
//! \param[in]  v_amp  vector with the ADC counts
//! \param[in]  dynamic_range  ADC dynamic range
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
double CAactRaw2Ntu::ADC2Temp(double adc) {

   // the NTC (negative temperature coefficient) sensor

   const double VCC = 5.04; // voltage divider supply voltage (V) measured at VME crate 
   const double R0 = 10000.0; // series resistance in the voltage divider (Ohm)
   const double Ron = 50.;// value of the multiplexer Ron (Ohm) for ADG406B (dual supply)
   
   double Vadc = (VCC/1023.0) * adc; // 10-bit ADC: max. value is 1023
   double Rt = (Vadc/(VCC-Vadc))*R0 - Ron; // voltage divider equation with Ron correction

   // The Steinhart-Hart formula is given below with the nominal coefficients a, b and c, 
   // which after calibration will be replaced by three constants for each crystal:
   double a = 0.00138867, b = 0.000204491, c = 1.05E-07;
   double temp = 1./ (a + b * log(Rt) + c * pow(log(Rt), 3)) - 273.15;
   
   return temp;
}

//------------------------------------------+-----------------------------------
//! Read block of CA WD waveform and Arduino temp from daq file
Int_t CAactRaw2Ntu::ReadStdAloneEvent(bool &endoffile,  TAGbaseWDparMap *p_WDMap) {

   u_int word;

   bool endEvent = false;
   bool startEvent = false;
   bool eof=false;
   int ret=0;

   int nCry = fGeometry->GetCrystalsN();

   while (!endEvent) {

      ret = fread(&word, 4, 1, fWDstream); 
      if (FootDebugLevel(2)) printf("word:%08x\n", word);

      if (ret == 0) {
         cout << "End file ..." << endl;
         endoffile = true;
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

      /////////////////////////////////////////////////
      // Read waveforms
      if (word == EVT_HEADER) {

         ret = fread(&word, 4, 1, fWDstream);
         int ser_evt_number =  word& 0xffff;
         if (FootDebugLevel(1)) 
            printf("====== WD ========== Event num::%d\n", ser_evt_number);

         ret = fread(&word, 4, 1, fWDstream);
         
         // read waveDAQ boards
         while (fread(&word, 4, 1, fWDstream) !=0 && (word & 0xffff) == BOARD_HEADER) {
            
            int board_id = (word>>16)  & 0xffff;
            if (FootDebugLevel(2)) printf("found board header::%08x num %d\n", word, board_id);

            ret = fread(&word, 4, 1, fWDstream);
            float temperature = *((float*)&word);
            if (FootDebugLevel(2)) printf("temperature::%08x num %d\n", word, board_id);
            
            ret = fread(&word, 4, 1, fWDstream);
            float range = *((float*)&word);
            fRange = range;
            
            if (FootDebugLevel(2)) printf("range::%08x num %d\n", word, board_id);
               
            ret = fread(&word, 4, 1, fWDstream);
            int sampling_freq =  (word >>16)& 0xffff;
            //flags = word & 0xffff;
            if (FootDebugLevel(2)) printf("sampling::%08x  num%d\n", word, board_id);
            
            // read channels
            while(fread(&word, 4,1,fWDstream) !=0 && (word & 0xffff) == CH_HEADER) {	
               char tmp_chstr[3]={'0','0','\0'};
               tmp_chstr[1] = (word>>24)  & 0xff;
               tmp_chstr[0] = (word>>16)  & 0xff;
               int ch_num = atoi(tmp_chstr);
               if (FootDebugLevel(2)) printf("   found channel header::%08x num %d\n", word, ch_num);
                  
               ret = fread(&word, 4, 1, fWDstream);
               int trig_cell = (word>>16) &0xffff;
               //fe_settings = ((word)&0xffff);

               // read a block of 32 bit x NSAMPLING/2
               uint32_t usa[NSAMPLING/2];
               ret = fread(usa, 4, NSAMPLING/2, fWDstream);

               if (FootDebugLevel(2)) printf("   found waveform board:%d  channel:%d\n", board_id,ch_num);
                  
               string ch_type;
               ch_type = p_WDMap->GetChannelType(board_id, ch_num);
               if (FootDebugLevel(2)) printf("   type::%s\n", ch_type.data());
 
               // Only process CALO channels
               if (ch_type == "CALO") {
                  if (FootDebugLevel(1)) printf("   found CALO waveform board:%d  channel:%d\n", board_id, ch_num);

                  // split each 32 bit into two 16 bit values
                  int w_adc[NSAMPLING];
                  for (int iSa=0; iSa<NSAMPLING/2; iSa++) {
                     uint adc_sa = usa[iSa];
                     int adctmp = ((adc_sa >> 16) & 0xffff);
                     //uint adctmp = (adc_sa & 0xffff);	  
                     w_adc[2*iSa] = adctmp;
                     //adctmp = ((adc_sa >> 16) & 0xffff);
                     adctmp = (adc_sa & 0xffff);
                     w_adc[2*iSa+1] = adctmp;
                  }

                  // convert ADC count to Volt
                  //vector<int> wd_adc(w_adc, w_adc+NSAMPLING); // convert array to vector
                  //vector<double> w_amp;
                  //w_amp = ADC2Volt(wd_adc, range);

                  // Get the crystal ID for the corresponding board - channel
                  int criID = ((TACAparMap*)fpCAParMap->Object())->GetCrystalId(board_id, ch_num);
                  if (criID >=0 && criID < nCry) {
                     uint adc5 = w_adc[5];
                     for (int i=0; i<NSAMPLING; ++i) {
                        fAmpCh[criID][i] = (UShort_t)w_adc[i];    // UShort_t
                     }
                  }
               } 
            }
            // go back the last read
            fseek(fWDstream, -4, SEEK_CUR);
         }
         // go back the last read   
         fseek(fWDstream, -4, SEEK_CUR);
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
                        int iCry = ((TACAparMap*)fpCAParMap->Object())->GetArduinoCrystalId(boardID, muxnum, ch);
                        if (iCry < 0 || iCry >= nCry) { 
                           Error("CAactRaw2Ntu", " --- Not well mapped Arduino vs crystal ID. board: %d mux: %d  ch: %d", boardID, muxnum, ch);
                           continue;
                        }
                        double temp = ADC2Temp(tempADC);
                        //if (FootDebugLevel(1)) 
                           cout << "      cryID:" << iCry << "  ADC:" << tempADC  << " T:" << temp  << endl;

                        fTempCh[iCry] = temp; 
                        //fTempCh[nChRead]   = temp;               
                        //nChRead++;
                     }
                  } // for ch
               }
            }
         }
      } 
   }

   return 0;
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
      if(strcmp(argv[i],"-nf") == 0)    { nFiles = atoi(argv[++i]);  }   //  Number of files
   //   if(strcmp(argv[i],"-mth") == 0)   { mth = true;   } // enable multi threading (for clustering)
      if(strcmp(argv[i],"-draw") == 0)   { draw = true;   } // enable draw waveform sampling

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" DecoderCA help:"<<endl;
         cout<<" Ex: DecoderCA [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] data input file"<<endl;
         cout<<"      -out path/file : [def=*_standalone.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -nf value      : [def=1] Number of files"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
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
   //TAGparaDsc *pParTimeWD = new TAGparaDsc("WDTim", new TAGbaseWDparTime());
   //TAGbaseWDparTime* parTimeWD = (TAGbaseWDparTime*) pParTimeWD->Object();
   //parFileName = campManager->GetCurCalFile(TASTparGeo::GetBaseName(), runNb);
   //parTimeWD->FromFileCFD(parFileName.Data());

   CAactRaw2Ntu * caDatReader = new CAactRaw2Ntu(pParMapCa, pParMapWD);
   if (!caDatReader->Open(in)) return -1;
   caDatReader->SetInitName(in);
   caDatReader->SetTreeBranches();
   if (nFiles > 1) caDatReader->SetMaxFiles(nFiles);
   
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
   int frequency2 = 2000;  // frequency to clean waveform canvas


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

   delete campManager;

   return 0;
}




