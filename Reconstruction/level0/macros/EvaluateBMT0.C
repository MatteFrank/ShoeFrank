// Macro to calulate the BM t0s

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
#include "TATWparMap.hxx"
#include "TASTparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TATWparCal.hxx"
#include "TATWparMap.hxx"
#include "TATWparTime.hxx"
#include "TASTdatRaw.hxx"
#include "TATWdatRaw.hxx"
#include "TATWactNtuHit.hxx"
#include "TAGactWDreader.hxx"
#include "TASTactNtuRaw.hxx"

#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparConf.hxx"
#include "TABMdatRaw.hxx"
#include "TABMntuHit.hxx"
#include "TABMactDatRaw.hxx"
#include "TABMactNtuHit.hxx"
#include "TABMactNtuTrack.hxx"

#include "GlobalPar.hxx"

#endif

TAGactDaqReader*  daqActReader = 0x0;
TABMactDatRaw* bmActDatRaw  = 0x0;
TAGactWDreader*      wdActRaw  = 0x0;
//~ TATWactNtuHit*      twActNtu  = 0x0;
TASTactNtuRaw*   stActNtuRaw  = 0x0;

void FillStBm(TString expName) {
  
  //St-Tw parameters
  TAGdataDsc* twDaq    = new TAGdataDsc("twDaq", new TAGdaqEvent());
  daqActReader  = new TAGactDaqReader("daqActReader", twDaq);
  
  TAGparaDsc* parGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
  TASTparGeo* stparGeo = (TASTparGeo*)parGeoSt->Object();
  TString parFileName = "./geomaps/TASTdetector.geo";
  stparGeo->FromFile(parFileName.Data());
  
  TAGparaDsc* parMapSt = new TAGparaDsc("stMap", new TASTparMap()); // need the file
  TASTparMap* stparMap = (TASTparMap*) parMapSt->Object();
  parFileName = Form("./config/%sTASTdetector.cfg", expName.Data());
  stparMap->FromFile(parFileName);
  
  TAGparaDsc* parTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
  TASTparTime* parTime = (TASTparTime*) parTimeSt->Object();
  parTime->FromFile(expName.Data(), 2190);
  
  TAGparaDsc* parMapTw = new TAGparaDsc("twMap", new TATWparMap());
  TATWparMap* parMapt = (TATWparMap*)parMapTw->Object();
  parFileName = Form("./config/%sTATWChannelMap.xml", expName.Data());
  parMapt->FromFile(parFileName.Data());
  
  TAGparaDsc*  parTimeTw = new TAGparaDsc("twTim", new TATWparTime());
  TATWparTime* parTimet = (TATWparTime*) parTimeTw->Object();
  parTimet->FromFile(expName.Data(), 2190);
  
  TAGparaDsc* parGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
  TATWparGeo* parGeoT = (TATWparGeo*)parGeoTw->Object();
  TString parFileNameT = "./geomaps/TATWdetector.geo";
  parGeoT->FromFile(parFileNameT);
  
  TAGparaDsc* parCalTw = new TAGparaDsc("twCal", new TATWparCal());
  TATWparCal* parCalT = (TATWparCal*)parCalTw->Object();
  parFileNameT = Form("./config/%sTATWCalibrationMap.xml", expName.Data());
  parCalT->FromFile(parFileNameT.Data());
  
  //Bm parameters
  TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
  TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
  parGeo->FromFile();
    
  TAGparaDsc* bmGeo    = new TAGparaDsc("bmGeo", new TABMparGeo());
  TABMparGeo* bmgeomap   = (TABMparGeo*) bmGeo->Object();
  parFileName="./geomaps/TABMdetector.geo";
  bmgeomap->FromFile(parFileName);

  TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
  TABMparConf* bmcon = (TABMparConf*)bmConf->Object();
  parFileName = "./config/TABMdetector.cfg";
  bmcon->FromFile(parFileName);
  
  TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
  TABMparMap*  bmparMap = (TABMparMap*)bmMap->Object();
  parFileName = Form("./config/%sTABMdetector.map", expName.Data());
  bmparMap->FromFile(parFileName.Data(), bmgeomap);



  //~ TAGparaDsc* fpParTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
  //~ TASTparTime* parTimeSt = (TASTparTime*) fpParTimeSt->Object();
  //~ parTimeSt->FromFile("GSI/", 2190);
 
 
  //~ TAGparaDsc* stGeo = new TAGparaDsc("stGeo", new TASTparGeo());
  //~ TASTparGeo* parGeo = (TASTparGeo*)stGeo->Object();
  //~ TString parFileName = "./geomaps/TASTdetector.map";
  //~ parGeo->FromFile(parFileName.Data());
 
  //~ TAGparaDsc*  stMap  = new TAGparaDsc("stMap", new TASTparMap());
  //~ TASTparMap*  stparMap = (TASTparMap*)stMap->Object();
  //~ parFileName="./config/";
  //~ parFileName+=expname;
  //~ parFileName+="TASTdetector.cfg";
  //~ stparMap->FromFile(parFileName.Data());
 
  //~ fpParTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
  //~ TASTparTime* parTimeSt = (TASTparTime*) fpParTimeSt->Object();
  //~ parTimeSt->FromFile(expname.Data(), 2190);
 
  //ST-TW TAGdataDsc
  TAGdataDsc* stDat   = new TAGdataDsc("stDat", new TASTdatRaw());
  TAGdataDsc* twDat   = new TAGdataDsc("twdDat", new TATWdatRaw());
  wdActRaw  = new TAGactWDreader("wdActRaw", twDaq, stDat, twDat, parMapSt, parMapTw, parTimeSt, parTimeTw);
  TAGdataDsc* twNtu  = new TAGdataDsc("twNtu", new TATWntuHit());
  //~ twActNtu  = new TATWactNtuHit("twNtuRaw", twDat, twNtu, parGeoTw, parMapTw, parCalTw);
  TAGdataDsc* stNtuRaw   = new TAGdataDsc("stNtu", new TASTntuRaw());
  stActNtuRaw = new TASTactNtuRaw("stActNtuRaw", stDat, stNtuRaw, parMapSt);

 
  //Bm TAGdataDsc
   //~ TAGdataDsc* stDatRaw    = new TAGdataDsc("stDat", new TASTdatRaw());
   //~ stActDatRaw  = new TASTactDatRaw("stActDatRaw", stDatRaw,bmDaq,stMap, fpParTimeSt);
  TAGdataDsc* bmDatRaw    = new TAGdataDsc("bmDat", new TABMdatRaw());
  bmActDatRaw  = new TABMactDatRaw("bmActDatRaw", bmDatRaw, twDaq, bmMap, bmConf, bmGeo,stNtuRaw);  
  
  return;
}


void PrintT0s(TString output_filename, TString input_filename, Long64_t tot_num_ev, const int t0_switch, const int t0_choice, const vector<vector<double>> &t0values, const int t0pos){
  ofstream outfile;
  outfile.open(output_filename.Data(),ios::out);
  outfile<<"calculated_from: "<<input_filename.Data()<<"    number_of_events= "<<tot_num_ev<<"     t0_switch= "<<t0_switch<<"    t0_choice= "<<t0_choice<<endl;
  for(Int_t i=0;i<36;i++)
    outfile<<"cellid= "<<i<<"  T0_time= "<<t0values.at(i).at(t0pos)<<endl;
  outfile.close();
  return;
}



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
    
  gDirectory->mkdir("TDC_raw_less_bmsync");
    gDirectory->cd("TDC_raw_less_bmsync");
    for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      sprintf(tmp_char,"tdc_cha-bmsync_%d",i);
      h=new TH1D(tmp_char,"TDC time - BM synctime;Time [ns]; counts",3000,-1000.,2000.);
    }
    gDirectory->cd("..");

  gDirectory->mkdir("TDC_raw_less_stsync");
    gDirectory->cd("TDC_raw_less_stsync");
    for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      sprintf(tmp_char,"tdc_cha-stsync_%d",i);
      h=new TH1D(tmp_char,"TDC time - ST synctime;Time [ns]; counts",3000,-1000.,2000.);
    }
    gDirectory->cd("..");
    
  gDirectory->mkdir("TDC_raw_-stsync-bmsync");
    gDirectory->cd("TDC_raw_-stsync-bmsync");
    for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      sprintf(tmp_char,"tdc_cha-stsync-bmsync_%d",i);
      h=new TH1D(tmp_char,"TDC time - ST synctime - BM synctime;Time [ns]; counts",3000,-1000.,2000.);
    }
    gDirectory->cd("..");
    
  gDirectory->mkdir("TDC_raw_+stsync-bmsync");
    gDirectory->cd("TDC_raw_+stsync-bmsync");
    for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      sprintf(tmp_char,"tdc_cha+stsync-bmsync_%d",i);
      h=new TH1D(tmp_char,"TDC time + ST synctime - BM synctime;Time [ns]; counts",3000,-1000.,2000.);
    }
    gDirectory->cd("..");
    
  //~ gDirectory->mkdir("TDC_charged");
    //~ gDirectory->cd("TDC_charged");
    //~ h=new TH1D("all_tdc_charged","Number of tdc signals saved as BM hit; TDC channel; counts",bmmap->GetTdcMaxcha(),0.,bmmap->GetTdcMaxcha());
    //~ for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
      //~ sprintf(tmp_char,"tdc_cha-t0-sync_%d",i);
      //~ h=new TH1D(tmp_char,"charged tdc time;Time [ns]; counts",3000,-1000.,2000.);
    //~ }
    //~ gDirectory->cd("..");
  
  h=new TH1D("tdc_drop_xhit","Number of tdc signals with dropped hits; Event number; counts",10,0.,10);//distinguish the type of error!
  h->SetBinContent(1,1);
  h->SetBinContent(2,1);//in order to use addbincontent later
  h=new TH1D("tdc_drop_xevent","Number of tdc events with dropped hits; Event number; counts",10,0.,10);//distinguish the type of error!
  h=new TH1D("st_trigtime","Registered trigger time from st;Time [ns]; counts",3000,-1000.,2000.);
  h=new TH1D("bm_trigtime","Registered trigger time from bm;Time [ns]; counts",3000,-1000.,2000.);
  h=new TH1D("-bm-st_trigtime","Trigger time difference between bm and st;Time [ns]; counts",500,-800.,-300.);
  h=new TH1D("+bm-st_trigtime","Trigger time bmtrigger - sttrigger;Time [ns]; counts",500,200.,700.);
  h=new TH1D("-bm+st_trigtime","Trigger time -bmtrigger + sttrigger;Time [ns]; counts",500,-800.,-300.);
  f_out->cd("..");
    
  return;
}


// 0<=cellindex<=35;  triggerindex:0=bmtrigger, 3=sttrigger, 6=bm-st trigger
void EvaluateT0time(char *graphname, vector<vector<double>> &t0values, const int cellindex, const int triggerindex, const int tdcchannel){
  int tdc_peak, start_bin, peak_bin;
  if(((TH1D*)gDirectory->Get(graphname))->GetEntries()>20){//to evaluate the T0 I need at least 20 data... number to be optimized                
    start_bin=-1000;
    peak_bin=((TH1D*)gDirectory->Get(graphname))->GetMaximumBin();
    tdc_peak=peak_bin;
    for(Int_t j=((TH1D*)gDirectory->Get(graphname))->GetMaximumBin();j>0;j--)
      if(((TH1D*)gDirectory->Get(graphname))->GetBinContent(j)>((TH1D*)gDirectory->Get(graphname))->GetBinContent(((TH1D*)gDirectory->Get(graphname))->GetMaximumBin())/10.)
        start_bin=j;
    t0values.at(cellindex).at(triggerindex)=(Double_t)((TH1D*)gDirectory->Get(graphname))->GetBinCenter(start_bin);
    for(Int_t j=start_bin;j<=((TH1D*)gDirectory->Get(graphname))->GetMaximumBin()+1;j++)
      if((((TH1D*)gDirectory->Get(graphname))->GetBinContent(j) < ((TH1D*)gDirectory->Get(graphname))->GetBinContent(j-1)) && (((TH1D*)gDirectory->Get(graphname))->GetBinContent(j-1) > ((TH1D*)gDirectory->Get(graphname))->GetBinContent(((TH1D*)gDirectory->Get(graphname))->GetMaximumBin())*3./5.)){
        peak_bin=j-1;      
        break;      
      }
    t0values.at(cellindex).at(triggerindex+1)=(Double_t)((TH1D*)gDirectory->Get(graphname))->GetBinCenter((int)((peak_bin+start_bin+0.5)/2));
    t0values.at(cellindex).at(triggerindex+2)=(Double_t)((TH1D*)gDirectory->Get(graphname))->GetBinCenter(peak_bin);
    if(start_bin==0 || peak_bin==0)
      cout<<"WARNING: check if your tdc bin is ok!!! "<<"graphname="<<graphname<<"   cell: "<<cellindex<<"  tdcchannel="<<tdcchannel<<"  triggerindex="<<triggerindex<<"   peak_bin="<<peak_bin<<"   start_bin="<<start_bin<<"  tdc_peak="<<tdc_peak<<"   MaximumBin="<<((TH1D*)gDirectory->Get(graphname))->GetMaximumBin()<<endl;
  }
  else{
    cout<<"WARNING  too few events to evaluate T0 in tdcchannel="<<tdcchannel<<"  cellid="<<cellindex<<"  Number of events="<<((TH1D*)gDirectory->Get(graphname))->GetEntries()<<"  T0 for this channel will wrongly set to -20000"<<endl;
    t0values.at(cellindex).at(triggerindex)=-20000;
    t0values.at(cellindex).at(triggerindex+1)=-20000;
    t0values.at(cellindex).at(triggerindex+2)=-20000;
  }
  return;
}

//~ void ReadBmRaw(TString name = "data_test.00001462.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data")
//~ void EvaluateBMT0(TString in_filename = "data_built.2205.physics_foot.daq.WD.full.dat")//not working: it crashes after 1200 events and prob in datas
//~ void EvaluateBMT0(TString in_filename = "data/GSI_electronic/DataGSI_match/data_built.2212.physics_foot.daq.VTX.1.dat")
void EvaluateBMT0(const TString in_filename = "data/GSI_electronic/DataGSI_match/data_built.2212.physics_foot.daq.WD.1.dat")
{  
   int maxevnum=1e10;
   //~ int maxevnum=1000;
   TString expname="GSI/";
   int tmp_int; 
   TString out_filename=in_filename(in_filename.Last('/')+1,in_filename.Last('.'));
   out_filename.Prepend("bmraw_");
   out_filename.Append(".root");
      
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();

   TAGroot tagr;
   TAGgeoTrafo geoTrafo;
   geoTrafo.FromFile();

   tagr.SetCampaignNumber(1);
   tagr.SetRunNumber(1);

   FillStBm(expname);
   if(daqActReader->Open(in_filename)==-1) 
     return ;
   cout<<"input filename="<<in_filename.Data()<<"  output root file name:"<<out_filename.Data()<<endl;  
   TFile *f_out = new TFile(out_filename.Data(),"RECREATE");  
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(wdActRaw);   
   //~ tagr.AddRequiredItem(twActNtu);
   tagr.AddRequiredItem(stActNtuRaw);
   tagr.AddRequiredItem(bmActDatRaw);  

   tagr.Print();
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   TABMparGeo* bmgeo = (TABMparGeo*)(tagr.FindParaDsc("bmGeo","TABMparGeo")->Object());
   TABMparConf* bmcon = (TABMparConf*)(tagr.FindParaDsc("bmConf","TABMparConf")->Object());
   TABMparMap* bmmap = (TABMparMap*)(tagr.FindParaDsc("bmMap","TABMparMap")->Object());
   TASTntuRaw* stnturaw;
   TABMdatRaw* pbmdatraw;
   TAGdaqEvent*   p_datdaq;
   int nFragments, channel, measurement;
   char tmp_char[200];
   TString type;
   Long64_t ientry;
   Double_t bmtrigger;
   Double_t  sttrigger;
   
   Booking(f_out, bmmap);
   f_out->cd();
   //event loop
   for (ientry = 0; ientry<maxevnum; ientry++) {
    //~ if(ientry % 100 == 0)
       cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
      if(tagr.NextEvent()) {
        cout<<"nextevent sucess"<<endl;
        bmtrigger=-1000;
        sttrigger=-1000;
        p_datdaq = (TAGdaqEvent*)(tagr.FindDataDsc("twDaq","TAGdaqEvent")->Object());
        pbmdatraw = (TABMdatRaw*)(tagr.FindDataDsc("bmDat", "TABMdatRaw")->Object());
        stnturaw = (TASTntuRaw*)(tagr.FindDataDsc("stNtu", "TASTntuRaw")->Object());
        nFragments = p_datdaq->GetFragmentsN();
        for (Int_t i = 0; i < nFragments; i++) {       
          type=p_datdaq->GetClassType(i);
          if (type.Contains("TDCEvent")){ 
            const TDCEvent* evt = static_cast<const TDCEvent*> (p_datdaq->GetFragment(i));
            for(int k = 0; k < evt->measurement.size();k++){//loop to find bmtrigger
              channel=(evt->measurement.at(k)>>19) & 0x7f;
              measurement=evt->measurement.at(k) & 0x7ffff;
              if(channel==bmmap->GetTrefCh()){
                bmtrigger=measurement;
                break;
              }
            }
            sttrigger=stnturaw->GetTriggerTime();
            if(bmtrigger!=-1000)
              bmtrigger=bmtrigger/10.;
            for(int k = 0; k < evt->measurement.size();k++){
              measurement=evt->measurement.at(k) & 0x7ffff;
              channel=(evt->measurement.at(k)>>19) & 0x7f;
              if(channel<bmmap->GetTdcMaxcha()){
              //~ cout<<"meas="<<std::dec<<measurement/10.<<"  ch="<<std::dec<<channel<<"  time="<<measurement/10.-stnturaw->GetTriggerTime()<<endl;
                //~ if(channel==47)
                  //~ cout<<"trigtime="<<stnturaw->GetTriggerTime()<<"   ch47="<<measurement/10.<<"  diff="<<stnturaw->GetTriggerTime()-measurement/10.<<endl;
                sprintf(tmp_char,"TDC/TDC_raw/tdc_rawcha_%d",channel);
                ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.);    
                ((TH1D*)gDirectory->Get("TDC/TDC_raw/all_tdc_rawcha"))->Fill(channel);
                if(bmtrigger!=-1000){            
                  sprintf(tmp_char,"TDC/TDC_raw_less_bmsync/tdc_cha-bmsync_%d",channel);
                  ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.-bmtrigger);    
                }
                if(sttrigger!=-1000 && sttrigger>40){
                  sprintf(tmp_char,"TDC/TDC_raw_less_stsync/tdc_cha-stsync_%d",channel);
                  ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.-sttrigger);    
                }
                if(bmtrigger!=-1000 && sttrigger>40){
                  sprintf(tmp_char,"TDC/TDC_raw_-stsync-bmsync/tdc_cha-stsync-bmsync_%d",channel);
                  ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.-sttrigger-bmtrigger);    
                  sprintf(tmp_char,"TDC/TDC_raw_+stsync-bmsync/tdc_cha+stsync-bmsync_%d",channel);
                  ((TH1D*)gDirectory->Get(tmp_char))->Fill(measurement/10.+sttrigger-bmtrigger);    
                }
              }else{
                cout<<"ERROR IN TDCEVENT: CHANNEL="<<channel<<"  bmmap->GetTdcMaxcha()="<<bmmap->GetTdcMaxcha()<<"  event number="<<ientry<<"  the program will be finished"<<endl;
                tagr.EndEventLoop();
                return;
              }
            }
          }
        }//end of loop on p_datdaq, now loop on bmdatraw
        //~ for(int i=0;i<pbmdatraw->NHit();i++){
          //~ TABMrawHit pbmrawhit=pbmdatraw->Hit(i);
          //~ sprintf(tmp_char,"TDC/TDC_charged/tdc_cha-t0-sync_%d",bmmap->cell2tdc(pbmrawhit.Idcell()));
          //~ ((TH1D*)gDirectory->Get(tmp_char))->Fill(pbmrawhit.Time());    
          //~ ((TH1D*)gDirectory->Get("TDC/tdc_error_xhit"))->Fill(0);            
          //~ ((TH1D*)gDirectory->Get("TDC/TDC_charged/all_tdc_charged"))->Fill(bmmap->cell2tdc(pbmrawhit.Idcell()));            
        //~ }
        if(pbmdatraw->NDrop()>0){
          ((TH1D*)gDirectory->Get("TDC/tdc_drop_xevent"))->Fill(1);
          ((TH1D*)gDirectory->Get("TDC/tdc_drop_xhit"))->AddBinContent(2,pbmdatraw->NDrop());
        }else{            
          ((TH1D*)gDirectory->Get("TDC/tdc_drop_xevent"))->Fill(0);
          ((TH1D*)gDirectory->Get("TDC/tdc_drop_xhit"))->AddBinContent(1,pbmdatraw->NHit());
          }
      }else
        break;// no more events
      ((TH1D*)gDirectory->Get("TDC/st_trigtime"))->Fill(sttrigger);
      if(bmtrigger!=-1000)            
        ((TH1D*)gDirectory->Get("TDC/bm_trigtime"))->Fill(bmtrigger);            
      if(sttrigger!=-1000 && sttrigger>40)
        ((TH1D*)gDirectory->Get("TDC/st_trigtime"))->Fill(sttrigger);     
      if(sttrigger!=-1000 && sttrigger>40 &&  bmtrigger!=-1000){
        ((TH1D*)gDirectory->Get("TDC/-bm-st_trigtime"))->Fill(-bmtrigger-sttrigger);
        ((TH1D*)gDirectory->Get("TDC/+bm-st_trigtime"))->Fill(+bmtrigger-sttrigger);
        ((TH1D*)gDirectory->Get("TDC/-bm+st_trigtime"))->Fill(-bmtrigger+sttrigger);
      }
             
    }//end of the event loop
   
    tagr.EndEventLoop();
  
  
  //*****************************************************************EVALUATE T0****************************************************
  vector<vector<double>> t0values(36,vector<double>(12)); // for each cell (36): 0=meas-bmsync start; 1= meas-bmsync middle; 2=meas-bmsync peak; 3=meas-stsync start; 4=meas-stsync middle; 5= meas-stsync peak;6=meas-st-bmsync start; 7=meas-st-bmsync middle; 8=meas-st-bmsync peak, 9=meas-bmsync+st start, 10=meas-bmsync+st middle, 11=meas-bmsync+st peak;

  cout<<"TH1D done, now I will EVALUATE T0"<<endl;
  for(Int_t i=0;i<bmmap->GetTdcMaxcha();i++){
    if(bmmap->tdc2cell(i)>=0){
      sprintf(tmp_char,"TDC/TDC_raw_less_bmsync/tdc_cha-bmsync_%d",i);
      EvaluateT0time(tmp_char, t0values, bmmap->tdc2cell(i), 0, i);    
      sprintf(tmp_char,"TDC/TDC_raw_less_stsync/tdc_cha-stsync_%d",i);
      EvaluateT0time(tmp_char, t0values, bmmap->tdc2cell(i), 3, i);    
      sprintf(tmp_char,"TDC/TDC_raw_-stsync-bmsync/tdc_cha-stsync-bmsync_%d",i);
      EvaluateT0time(tmp_char, t0values, bmmap->tdc2cell(i), 6, i);    
      sprintf(tmp_char,"TDC/TDC_raw_+stsync-bmsync/tdc_cha+stsync-bmsync_%d",i);
      EvaluateT0time(tmp_char, t0values, bmmap->tdc2cell(i), 9, i);    
    }  
  }
  
  //~ //final check
  //~ for(Int_t i=0;i<36;i++)
    //~ if(bmcon->GetT0(i)==-10000)
      //~ cout<<"WARNING EvaluateT0! channel not considered in tdc map tdc_cha=i="<<i<<"  cellid="<<bmmap->tdc2cell(i)<<" T0 for this channel will set to -10000"<<endl;
  
  //~ bmcon->CoutT0();
  //~ bmcon->PrintT0s(T0filename, in_filename,ientry);

//dacanc:
//~ void PrintT0s(TString output_filename, TString input_filename, Long64_t tot_num_ev, const int t0_switch, const int to_choice, const double &vector<vector<double>>t0values, cons
  
  //print the t0s
  int t0switch;
  TString T0filename;
  cout<<"T0 evaluation finished, now I'll print the T0s on *.cfg files"<<endl;
  for(int t0choice=0;t0choice<4;t0choice++){  //t0choice: 0=meas-internal, 1=meas-st, 2=meas-internal-st, 3=meas-internal+st
    for(int k=0;k<3;k++){//k=t0_switch: 0=t0 from the beginning of the tdc signal, 1=peak/2, 2=peak, 3=negative T0 enabled
      T0filename=in_filename(in_filename.Last('/')+1,in_filename.Last('.'));
      T0filename.Prepend("T0evaluation_t0switch_");
      T0filename+=k;
      T0filename+="_t0choice_";
      T0filename+=t0choice;
      T0filename.Append(".cfg");
      PrintT0s(T0filename, in_filename, ientry, k, t0choice, t0values,t0choice*3+k);
    }
  }
  
  
  f_out->Write();
  f_out->Close(); 
  watch.Print();
  return;
}


