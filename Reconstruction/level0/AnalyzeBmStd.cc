
#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "TABMntuRaw.hxx"
#include "TABMhit.hxx"

int Booking(vector<TH1*> &scaplots,vector<TH1*> &adcplots,vector<TH1*> &randplots,  vector<TH2*> &my2dplots){
  TH1D* h;
  TH2D* h2d;
  TABMparMap* p_bmmap = (TABMparMap*) (gTAGroot->FindParaDsc("bmMap", "TABMparMap")->Object());

  //scaler
  h=new TH1D("0_Scaler_mean","Mean value of scaler measurements;Scaler channel; mean measurements",p_bmmap->GetScaMaxCh(),-0.5,p_bmmap->GetScaMaxCh()-0.5);
  scaplots.push_back(h);
  h=new TH1D("1_Scaler_devstd","DevStd of scaler measurements;Scaler channel; devstd",p_bmmap->GetScaMaxCh(),-0.5,p_bmmap->GetScaMaxCh()-0.5);
  scaplots.push_back(h);
  h=new TH1D("2_Scaler_finalcounts","Scaler final counts;Scaler channel; final counts",p_bmmap->GetScaMaxCh(),-0.5,p_bmmap->GetScaMaxCh()-0.5);
  scaplots.push_back(h);

  //adc
  h=new TH1D("0_Adc_Ped_mean","Loaded ADC pedestal values;Scaler channel; mean measurements",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);
  adcplots.push_back(h);
  h=new TH1D("1_Adc_Ped_devstd","DevStd of scaler measurements;Scaler channel; devstd",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);
  adcplots.push_back(h);
  h=new TH1D("2_AdcLessPed_mean","Mean values of adc - ped meas;Adc channel; Mean",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);
  adcplots.push_back(h);
  h=new TH1D("3_AdcLessPed_devstd","dev std of adc - ped meas;Adc channel; Mean",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);  adcplots.push_back(h);

  //others
  h=new TH1D("0_rate_busy","rate of busy signal from scaler;rate (Hz); Measurements",2000,0,20000);
  randplots.push_back(h);
  h=new TH1D("1_rate_nobusy","rate of no busy signal from scaler;rate (Hz); Measurements",2000,0,20000);
  randplots.push_back(h);

  //2d plots
  h2d=new TH2D("0_timevsbeamY","Beam Y position;Time [evts]; Beam position [cm]",5000,0,50000, 600,-3.,3.);
  my2dplots.push_back(h2d);
  h2d=new TH2D("1_timevsbeamX","Beam X position;Time [evts]; Beam position [cm]",5000,0,50000, 600,-3.,3.);
  my2dplots.push_back(h2d);
  h2d=new TH2D("2_timevsrateNoBusy","Beam rate as a function of time;Time [evts]; No busy rate [Hz]",5000,0,50000, 2000,0.,20000.);
  my2dplots.push_back(h2d);

  return 0;
}

int main (int argc, char *argv[])  {

  TString in("12C_C_200_1.root");
  TString rootout("");
  TString exp("");
  TString calname, mapname, adcname;
  Bool_t print=kFALSE;

  Int_t runNb = -1;
  Int_t nTotEv = 1e7;

  for (int i = 0; i < argc; i++){
     if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
     if(strcmp(argv[i],"-out") == 0)   { rootout = TString(argv[++i]); }   // Root file output
     if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
     if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
     if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
     if(strcmp(argv[i],"-cal") == 0)   { calname = TString(argv[++i]);  }  // path for a specific calibration file
     if(strcmp(argv[i],"-map") == 0)   { mapname = TString(argv[++i]);  }  // path for a specific mapping file
     if(strcmp(argv[i],"-adc") == 0)   { adcname = TString(argv[++i]);  }  // path to load the adc pedestals
     if(strcmp(argv[i],"-print") == 0) { print = kTRUE;  }  // to printout infos, useful during data takings

     if(strcmp(argv[i],"-help") == 0)  {
        cout<<" Decoder help:"<<endl;
        cout<<" Ex: Decoder [opts] "<<endl;
        cout<<" possible opts are:"<<endl;
        cout<<"      -in path/file  : [def= 12C_C_200_1.root] ROOT input file"<<endl;
        cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
        cout<<"      -run value     : [def=-1] Run number"<<endl;
        cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
        return 1;
     }
  }

  //few checks
  if (rootout.IsNull()){
    rootout+="analyzedbmstd.root";
    cout<<"WARNING:: No output name has been set, a default name will be set with a name="<<rootout.Data()<<endl;
  }

  TApplication::CreateApplication();
  TAGrecoManager::Instance(exp);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();

  LocalReco* locRec= new LocalReco(exp, runNb, in, rootout);

  // global setting
  TAGrecoManager::GetPar()->IncludeBM(true);
  TAGrecoManager::GetPar()->IncludeTW(false);
  TAGrecoManager::GetPar()->CalibTW(false);
  TAGrecoManager::GetPar()->IncludeDI(false);
  TAGrecoManager::GetPar()->IncludeMSD(false);
  TAGrecoManager::GetPar()->IncludeCA(false);
  TAGrecoManager::GetPar()->IncludeTG(false);
  TAGrecoManager::GetPar()->IncludeVT(false);
  TAGrecoManager::GetPar()->IncludeIT(false);
  TAGrecoManager::GetPar()->IncludeTOE(false);
  TAGrecoManager::GetPar()->DisableLocalReco();
  TAGrecoManager::GetPar()->IncludeST(false);
  locRec->EnableStdAlone();

  // locRec->EnableTree();
  locRec->EnableHisto();
  locRec->EnableTracking();

  TStopwatch watch;
  watch.Start();
  locRec->BeforeEventLoop();
  TABMparGeo* p_bmgeo = (TABMparGeo*) (gTAGroot->FindParaDsc("bmGeo", "TABMparGeo")->Object());
  TABMparConf* p_bmcon = (TABMparConf*) (gTAGroot->FindParaDsc("bmConf", "TABMparConf")->Object());
  TABMparCal*  p_bmcal = (TABMparCal*) (gTAGroot->FindParaDsc("bmCal", "TABMparCal")->Object());
  TABMparMap* p_bmmap = (TABMparMap*) (gTAGroot->FindParaDsc("bmMap", "TABMparMap")->Object());
  TABMactVmeReader *p_actvme=(TABMactVmeReader*)gTAGroot->FindAction("bmActRaw");
  BM_struct* bmstruct=p_actvme->GetEvtStruct();
  TABMntuTrack *p_ntutrack = (TABMntuTrack*) (gTAGroot->FindDataDsc("bmTrack", "TABMntuTrack")->Object());

  if(calname.Length()>0)
    p_bmcal->FromFile(calname);
  if(mapname.Length()>0){
    p_bmmap->FromFile(mapname, p_bmgeo);
    p_actvme->SetAdcSize(p_bmmap->GetAdcMaxCh());
    p_actvme->SetScaSize(p_bmmap->GetScaMaxCh());
  }
  if(adcname.Length()>0)
    p_bmcal->LoadAdc(adcname,p_bmmap);
  else{
    cout<<"WARNING::no adc pedestal file has been charged, all the adc pedestals will be set to 0"<<endl;
    p_bmcal->ResetAdc(p_bmmap->GetAdcMaxCh());
  }

  vector<TH1*> scaplots, adcplots, randplots;
  vector<TH2*> my2dplots;
  Booking(scaplots, adcplots, randplots, my2dplots);
  Long64_t ientry;

  //set some useful variables
  Int_t scanobusy=p_bmmap->GetScaCh(1); //not busy coincidence channel adopted to count the incident particles
  Int_t scabusy=p_bmmap->GetScaCh(2); //not busy coincidence channel adopted to count the incident particles

  //few checks
  if(scanobusy<0 || scanobusy>p_bmmap->GetScaMaxCh()){
    cout<<"ERROR: scanobusy channel="<<scanobusy<<"   check your map!!"<<endl;
    return 1;
  }
  if(scanobusy<0 || scanobusy>p_bmmap->GetScaMaxCh()){
    cout<<"ERROR: scanobusy channel="<<scanobusy<<"   check your map!!"<<endl;
    return 1;
  }

  //event loop
  for ( ientry= 0; ientry < nTotEv; ientry++) {
    if(ientry%1000==0 || ientry%(nTotEv/10)==0 )
      cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
    if(!gTAGroot->NextEvent())
      break;

    // p_actvme->PrintBMstruct();

    //fill some plots
    if(bmstruct->time_evtoev!=0){
      randplots.at(0)->Fill(p_actvme->GetScaMeas(scabusy).first*1000000./bmstruct->time_evtoev);
      randplots.at(1)->Fill(p_actvme->GetScaMeas(scanobusy).first*1000000./bmstruct->time_evtoev);
      my2dplots.at(2)->Fill(ientry,p_actvme->GetScaMeas(scanobusy).first*1000000./bmstruct->time_evtoev);
    }
    for(Int_t i=0;i<p_ntutrack->GetTracksN();i++){
      TABMtrack *track=p_ntutrack->GetTrack(i);
      my2dplots.at(0)->Fill(ientry,track->GetOrigin().Y());
      my2dplots.at(1)->Fill(ientry,track->GetOrigin().X());
    }


  }//end of event loop

  //further post event loop analysis:
  //Scaler meas. loop
  for(Int_t i=0;i<p_actvme->GetRawSca().size();i++){
    scaplots.at(0)->SetBinContent(i+1,p_actvme->GetRawSca().at(i)->GetMean());
    scaplots.at(1)->SetBinContent(i+1,p_actvme->GetRawSca().at(i)->GetStdDev());
    scaplots.at(2)->SetBinContent(i+1,p_actvme->GetScaMeas(i).second);
  }
  //Adc meas. Loop
  for(Int_t i=0;i<p_actvme->GetAdcLessPed().size();i++){
    adcplots.at(0)->SetBinContent(i+1,p_bmcal->GetAdcPed(i));
    adcplots.at(1)->SetBinContent(i+1,p_bmcal->GetAdcDevStd(i));
    adcplots.at(2)->SetBinContent(i+1,p_actvme->GetAdcLessPed().at(i)->GetMean());
    adcplots.at(3)->SetBinContent(i+1,p_actvme->GetAdcLessPed().at(i)->GetStdDev());
  }

  //printout useful stuffs
  if(print==kTRUE){
    cout<<"Data analysis completed for input file: "<<in.Data()<<endl;
    cout<<"mean rate=";
  }

  //save my histos
  locRec->AfterEventLoop();
  TFile fileout(rootout.Data(),"UPDATE");
  for(Int_t i=0;i<scaplots.size();i++)
    fileout.Add(scaplots.at(i));
  for(Int_t i=0;i<adcplots.size();i++)
    fileout.Add(adcplots.at(i));
  for(Int_t i=0;i<randplots.size();i++)
    fileout.Add(randplots.at(i));
  for(Int_t i=0;i<my2dplots.size();i++)
    fileout.Add(my2dplots.at(i));
  fileout.Write();
  fileout.Close();

  watch.Print();
  delete locRec;
  cout<<"AnalyzeBmStd done, output file="<<rootout.Data()<<endl;
 return 0;
}
