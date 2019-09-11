// Macro to make reconstruction from MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

#include "TAGparGeo.hxx"

#include "TASTparMap.hxx"
#include "TASTdatRaw.hxx"
#include "TASTactDatRaw.hxx"

#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparCon.hxx"
#include "TABMdatRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TABMactDatRaw.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactNtuTrack.hxx"

#include "GlobalPar.hxx"

#endif

TAGactDaqReader*  daqActReader = 0x0;
TASTactDatRaw* stActDatRaw  = 0x0;
TABMactDatRaw* bmActDatRaw  = 0x0;

void Booking(TFile *f_out, TABMparMap *bmmap){

  f_out->cd();
  TH1D* h=nullptr;
  TH2D* h2=nullptr;
  char tmp_char[200];
  f_out->mkdir("TDC");
  f_out->cd("TDC");
  gDirectory->mkdir("TDC_raw");
    gDirectory->cd("TDC_raw");
    h=new TH1D("all_tdc_rawcha","Number of all tdc raw signals; TDC channel; counts",bmmap->GetTdcMaxcha(),0.,bmmap->GetTdcMaxcha());
    for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      sprintf(tmp_char,"tdc_rawcha_%d",i);
      h=new TH1D(tmp_char,"Registered time;Time [ns]; counts",3000,-1000.,2000.);
    }
    gDirectory->cd("..");
    
  gDirectory->mkdir("TDC_raw_less_sync");
    gDirectory->cd("TDC_raw_less_sync");
    for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      sprintf(tmp_char,"tdc_cha-sync_%d",i);
      h=new TH1D(tmp_char,"TDC time - synctime;Time [ns]; counts",3000,-1000.,2000.);
    }
    gDirectory->cd("..");
    
  gDirectory->mkdir("TDC_charged");
    gDirectory->cd("TDC_charged");
    h=new TH1D("all_tdc_charged","Number of tdc signals saved as BM hit; TDC channel; counts",bmmap->GetTdcMaxcha(),0.,bmmap->GetTdcMaxcha());
    for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      sprintf(tmp_char,"tdc_cha-t0-sync_%d",i);
      h=new TH1D(tmp_char,"charged tdc time;Time [ns]; counts",3000,-1000.,2000.);
    }
    gDirectory->cd("..");
  
  h=new TH1D("tdc_error_xhit","Number of tdc signals with errors; Event number; counts",10,0.,10);//distinguish the type of error!
  h=new TH1D("tdc_error_xevent","Number of tdc events with errors; Event number; counts",10,0.,10);//distinguish the type of error!
  h=new TH1D("tdc_sync","Registered time;Time [ns]; counts",3000,-1000.,2000.);
  h=new TH1D("st_trigtime","Registered trigger time from st;Time [ns]; counts",3000,-1000.,2000.);
  h=new TH1D("bm_trigtime","Registered trigger time from bm;Time [ns]; counts",3000,-1000.,2000.);
  h=new TH1D("bm-st_trigtime","Trigger time difference between bm and st;Time [ns]; counts",1000,-100.,900.);
  f_out->cd("..");
    
  return;
}


void FillBm(TString name) {
   
  //parameters
  TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
  TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
  parGeo->FromFile();
    
  TAGparaDsc* bmGeo    = new TAGparaDsc("bmGeo", new TABMparGeo());
  TABMparGeo* bmgeomap   = (TABMparGeo*) bmGeo->Object();
  bmgeomap->FromFile();

  TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparCon());
  TABMparCon* parConf = (TABMparCon*)bmConf->Object();
  TString parFileName = "./config/TABMdetector.cfg";
  
  TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
  TABMparMap*  bmparMap = (TABMparMap*)bmMap->Object();
  parFileName = "./config/TABMdetector.map";
  bmparMap->FromFile(parFileName.Data(), bmgeomap);

   TAGparaDsc* fpParTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
   TASTparTime* parTimeSt = (TASTparTime*) fpParTimeSt->Object();
   parTimeSt->FromFile("GSI_", 2190);
  
   TAGparaDsc*  stMap  = new TAGparaDsc("stMap", new TASTparMap());
   TASTparMap*  stparMap = (TASTparMap*)stMap->Object();
   parFileName="./config/TASTdetector.cfg";
   stparMap->FromFile(parFileName.Data());
 
  //TAGdataDsc
  TAGdataDsc* bmDaq    = new TAGdataDsc("bmDaq", new TAGdaqEvent());
  daqActReader  = new TAGactDaqReader("daqActReader", bmDaq); 
  
     
   TAGdataDsc* stDatRaw    = new TAGdataDsc("stDat", new TASTdatRaw());
   stActDatRaw  = new TASTactDatRaw("stActDatRaw", stDatRaw,bmDaq,stMap, fpParTimeSt);
  

  TAGdataDsc* bmDatRaw    = new TAGdataDsc("bmDat", new TABMdatRaw());
  bmActDatRaw  = new TABMactDatRaw("bmActDatRaw", bmDatRaw, bmDaq, bmMap, bmConf, bmGeo,stDatRaw);  
  return;
}


//~ void ReadBmRaw(TString name = "data_test.00001462.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data")
//~ void EvaluateBMT0(TString in_filename = "data_built.2205.physics_foot.daq.WD.full.dat")//not working: it crashes after 1200 events and prob in datas
//~ void EvaluateBMT0(TString in_filename = "data/GSI_electronic/DataGSI_match/data_built.2212.physics_foot.daq.VTX.1.dat")
void EvaluateBMT0(TString in_filename = "data/GSI_electronic/DataGSI_match/data_built.2242.physics_foot.daq.VTX.1.dat")
{  
   int maxevnum=1e8;
   int t0choice=1;//0=wd, 1=internal
   int t0switch=0;//0=t0 from the beginning of the tdc signal, 1=from the peak, 2=negative T0 enabled, 3=peak/2
   int tmp_int; 
   TString tmp_tstring;
   TString out_filename=in_filename(in_filename.Last('/')+1,in_filename.Last('.'));
   tmp_tstring="bmraw_t0choice_";
   tmp_tstring+=t0choice;
   tmp_tstring+="_";
   out_filename.Prepend(tmp_tstring.Data());
   out_filename.Append(".root");
   TString T0filename=in_filename(in_filename.Last('/')+1,in_filename.Last('.'));
   T0filename.Prepend("./config/T0evaluation_");
   T0filename.Append(".cfg");
      
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();

   TAGroot tagr;
   TAGgeoTrafo geoTrafo;
   geoTrafo.FromFile();

   tagr.SetCampaignNumber(1);
   tagr.SetRunNumber(1);

   FillBm(in_filename);
   if(daqActReader->Open(in_filename)==-1) 
     return ;
   cout<<"input filename="<<in_filename.Data()<<"  output root file name:"<<out_filename.Data()<<endl;  
   TFile *f_out = new TFile(out_filename.Data(),"RECREATE");  
   
   tagr.AddRequiredItem("daqActReader");
   tagr.AddRequiredItem("stActDatRaw");
   tagr.AddRequiredItem("bmActDatRaw");  
   tagr.Print();
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   TABMparGeo* bmgeo = (TABMparGeo*)(tagr.FindParaDsc("bmGeo","TABMparGeo")->Object());
   TABMparCon* bmcon = (TABMparCon*)(tagr.FindParaDsc("bmConf","TABMparCon")->Object());
   TABMparMap* bmmap = (TABMparMap*)(tagr.FindParaDsc("bmMap","TABMparMap")->Object());
   TASTdatRaw* pstdatraw;
   TABMdatRaw* pbmdatraw;
   TAGdaqEvent*   p_datdaq;
   int nFragments, channel, measurement;
   char tmp_char[200];
   TString type;
   Long64_t ientry;
   Int_t mytrigger;
   Double_t  sttrigger;
   Double_t  usedtrigger;
   
   bmcon->SetT0choice(t0choice);
   Booking(f_out, bmmap);
   bmcon->SetT0switch(t0switch);
   f_out->cd();
   //event loop
   for (ientry = 0; ientry<maxevnum; ientry++) {
    if(ientry % 100 == 0)
       cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
      if(tagr.NextEvent()) {
        mytrigger=-10000;
        sttrigger=-10000;
        usedtrigger=-10000;
        p_datdaq = (TAGdaqEvent*)(tagr.FindDataDsc("bmDaq","TAGdaqEvent")->Object());
        pbmdatraw = (TABMdatRaw*)(tagr.FindDataDsc("bmDat", "TABMdatRaw")->Object());
        pstdatraw = (TASTdatRaw*)(tagr.FindDataDsc("stDat", "TASTdatRaw")->Object());
        nFragments = p_datdaq->GetFragmentsN();
        for (Int_t i = 0; i < nFragments; i++) {       
          type=p_datdaq->GetClassType(i);
          if (type.Contains("TDCEvent")){ 
            const TDCEvent* evt = static_cast<const TDCEvent*> (p_datdaq->GetFragment(i));
            for(Int_t k = 0; k < evt->measurement.size();k++){//first loop to find mytrigger
              channel=(evt->measurement.at(k)>>19) & 0x7f;
              measurement=evt->measurement.at(k) & 0x7ffff;
              if(channel==bmmap->GetTrefCh()){
                mytrigger=measurement;
                break;
              }
            }
            sttrigger=pstdatraw->TrigTime();
            if(sttrigger!=-10000 && bmcon->GetT0choice()==0)
              usedtrigger=sttrigger;
            if(mytrigger!=-10000 && bmcon->GetT0choice()==1)
              usedtrigger=mytrigger/10.;
            for(Int_t k = 0; k < evt->measurement.size();k++){
              measurement=evt->measurement.at(k) & 0x7ffff;
              channel=(evt->measurement.at(k)>>19) & 0x7f;
              if(channel<bmmap->GetTdcMaxcha()){
              //~ cout<<"meas="<<std::dec<<measurement/10.<<"  ch="<<std::dec<<channel<<"  time="<<measurement/10.-pstdatraw->TrigTime()<<endl;
                //~ if(channel==47)
                  //~ cout<<"trigtime="<<pstdatraw->TrigTime()<<"   ch47="<<measurement/10.<<"  diff="<<pstdatraw->TrigTime()-measurement/10.<<endl;
                sprintf(tmp_char,"TDC/TDC_raw/tdc_rawcha_%d",channel);
                ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.);    
                ((TH1D*)gDirectory->Get("TDC/TDC_raw/all_tdc_rawcha"))->Fill(channel);            
                sprintf(tmp_char,"TDC/TDC_raw_less_sync/tdc_cha-sync_%d",channel);
                //~ ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.-pstdatraw->TrigTime());    //use the ST triggertime
                ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.-usedtrigger);    
              }else{
                cout<<"ERROR IN TDCEVENT: CHANNEL="<<channel<<"  bmmap->GetTdcMaxcha()="<<bmmap->GetTdcMaxcha()<<"  event number="<<ientry<<"  the program will be finished"<<endl;
                tagr.EndEventLoop();
                return;
              }
            }
          }
        }//end of loop on p_datdaq, now loop on bmdatraw
        for(int i=0;i<pbmdatraw->NHit();i++){
          TABMrawHit pbmrawhit=pbmdatraw->Hit(i);
          sprintf(tmp_char,"TDC/TDC_charged/tdc_cha-t0-sync_%d",bmmap->cell2tdc(pbmrawhit.Idcell()));
          ((TH1D*)gDirectory->Get(tmp_char))->Fill(pbmrawhit.Time());    
          ((TH1D*)gDirectory->Get("TDC/tdc_error_xhit"))->Fill(0);            
          ((TH1D*)gDirectory->Get("TDC/TDC_charged/all_tdc_charged"))->Fill(bmmap->cell2tdc(pbmrawhit.Idcell()));            
        }
        if(pbmdatraw->NDrop()>0){
          ((TH1D*)gDirectory->Get("TDC/tdc_error_xevent"))->Fill(1);
          for(int i=0;i<pbmdatraw->NDrop();i++)
            ((TH1D*)gDirectory->Get("TDC/tdc_error_xhit"))->Fill(1);
        }else            
          ((TH1D*)gDirectory->Get("TDC/tdc_error_xevent"))->Fill(0);
        ((TH1D*)gDirectory->Get("TDC/tdc_sync"))->Fill(usedtrigger);            
      }else
        break;// no more events
      ((TH1D*)gDirectory->Get("TDC/st_trigtime"))->Fill(sttrigger);            
      ((TH1D*)gDirectory->Get("TDC/bm_trigtime"))->Fill(mytrigger/10.);            
      ((TH1D*)gDirectory->Get("TDC/bm-st_trigtime"))->Fill(mytrigger/10.-sttrigger);            
    }//end of the event loop
   
    tagr.EndEventLoop();
  
  //*****************************************************************EVALUATE T0****************************************************
  int tdc_peak, start_bin, peak_bin;
  if(bmcon->GetBMdebug()>0)
    cout<<"TH1D done, now I will EVALUATE T0"<<endl;
  for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
    if(bmmap->tdc2cell(i)>=0){
      sprintf(tmp_char,"TDC/TDC_raw_less_sync/tdc_cha-sync_%d",i);
      if(((TH1D*)gDirectory->Get(tmp_char))->GetEntries()>20){//to evaluate the T0 I need at least 10 data... number to be optimized                
        start_bin=0;
        peak_bin=((TH1D*)gDirectory->Get(tmp_char))->GetMaximumBin();
        tdc_peak=peak_bin;
        for(Int_t j=((TH1D*)gDirectory->Get(tmp_char))->GetMaximumBin();j>0;j--)
          if(((TH1D*)gDirectory->Get(tmp_char))->GetBinContent(j)>((TH1D*)gDirectory->Get(tmp_char))->GetBinContent(((TH1D*)gDirectory->Get(tmp_char))->GetMaximumBin())/10.)
            start_bin=j;
        if(bmcon->GetT0switch()==0)
          tdc_peak=start_bin;
        for(Int_t j=start_bin;j<=((TH1D*)gDirectory->Get(tmp_char))->GetMaximumBin()+1;j++)
          if((((TH1D*)gDirectory->Get(tmp_char))->GetBinContent(j) < ((TH1D*)gDirectory->Get(tmp_char))->GetBinContent(j-1)) && (((TH1D*)gDirectory->Get(tmp_char))->GetBinContent(j-1) > ((TH1D*)gDirectory->Get(tmp_char))->GetBinContent(((TH1D*)gDirectory->Get(tmp_char))->GetMaximumBin())*3./5.)){
            peak_bin=j-1;      
            break;      
          }              
        if(bmcon->GetT0switch()==1 || bmcon->GetT0switch()==2)
          tdc_peak=peak_bin;
        if(bmcon->GetT0switch()==3)//T0=middle point between first signal and peak   
          tdc_peak=(peak_bin+start_bin+0.5)/2;
        if(start_bin==0 || peak_bin==0)
          cout<<"WARNING: check if your tdc_peak is ok:   cell: "<<bmmap->tdc2cell(i)<<"  i="<<i<<"   peak_bin="<<peak_bin<<"   start_bin="<<start_bin<<"  tdc_peak="<<tdc_peak<<"   MaximumBin="<<((TH1D*)gDirectory->Get(tmp_char))->GetMaximumBin()<<endl;
       bmcon->SetT0(bmmap->tdc2cell(i),(Double_t)((TH1D*)gDirectory->Get(tmp_char))->GetBinCenter(tdc_peak)); 
      }
      else{
        cout<<"WARNING  too few events to evaluate T0 in tdc_cha=i="<<i<<"  cellid="<<bmmap->tdc2cell(i)<<"  Number of events="<<((TH1D*)gDirectory->Get(tmp_char))->GetEntries()<<"  T0 for this channel will wrongly set to -20000"<<endl;
        bmcon->SetT0(bmmap->tdc2cell(i),-20000.);
      }
    }  
  }
  //final check
  for(Int_t i=0;i<36;i++)
    if(bmcon->GetT0(i)==-10000)
      cout<<"WARNING EvaluateT0! channel not considered in tdc map tdc_cha=i="<<i<<"  cellid="<<bmmap->tdc2cell(i)<<" T0 for this channel will set to -10000"<<endl;
  
  //  if(bmcon->GetBMdebug()>3)
  bmcon->CoutT0();
  bmcon->PrintT0s(T0filename, in_filename,ientry);
  
  f_out->Write();
  f_out->Close(); 
  watch.Print();
  return;
}


