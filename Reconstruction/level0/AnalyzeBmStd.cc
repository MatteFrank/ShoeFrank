
#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "TABMntuRaw.hxx"
#include "TABMhit.hxx"

int Booking(vector<TH1*> &scaplots,vector<TH1*> &adcplots,vector<TH1*> &recoplots){
  TH1D* h;
  TH2D* h2d;
  TABMparMap* p_bmmap = (TABMparMap*) (gTAGroot->FindParaDsc("bmMap", "TABMparMap")->Object());

  //scaler
  if(p_bmmap->GetScaMaxCh()>0){
    h=new TH1D("0_Scaler_mean","Mean value of scaler measurements;Scaler channel; mean measurements",p_bmmap->GetScaMaxCh(),-0.5,p_bmmap->GetScaMaxCh()-0.5);
    scaplots.push_back(h);
    h=new TH1D("1_Scaler_devstd","DevStd of scaler measurements;Scaler channel; devstd",p_bmmap->GetScaMaxCh(),-0.5,p_bmmap->GetScaMaxCh()-0.5);
    scaplots.push_back(h);
    h=new TH1D("2_Scaler_finalcounts","Scaler final counts;Scaler channel; final counts",p_bmmap->GetScaMaxCh(),-0.5,p_bmmap->GetScaMaxCh()-0.5);
    scaplots.push_back(h);
    h=new TH1D("3_rate_busy","rate of busy signal from scaler;rate (Hz); Measurements",2000,0,20000);
    scaplots.push_back(h);
    h=new TH1D("4_rate_nobusy","rate of no busy signal from scaler;rate (Hz); Measurements",2000,0,20000);
    scaplots.push_back(h);
    h=new TH1D("5_timevsrateNoBusy","Beam rate as a function of time;Time [evts]; No busy rate [Hz]",5000,0,50000);
    scaplots.push_back(h);
  }

  //adc
  if(p_bmmap->GetAdcMaxCh()>0){
    Int_t adcnbin=2100;
    Double_t adclimit=4200;
    h=new TH1D("0_Adc_Ped_mean","Loaded ADC pedestal values;Scaler channel; mean measurements",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);
    adcplots.push_back(h);
    h=new TH1D("1_Adc_Ped_devstd","DevStd of adc pedestals;Adc channel; devstd",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);
    adcplots.push_back(h);
    h=new TH1D("2_AdcLessPed_mean","Mean values of adc - ped meas;Adc channel; Mean",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);
    adcplots.push_back(h);
    h=new TH1D("3_AdcLessPed_devstd","dev std of adc - ped meas;Adc channel; Mean",p_bmmap->GetAdcMaxCh(),-0.5,p_bmmap->GetAdcMaxCh()-0.5);  adcplots.push_back(h);
    for(Int_t i=1;i<p_bmmap->GetAdcMaxCh();i++){
      if(p_bmmap->GetAdcCh(i)!=-1){
        if(i==1){
          h=new TH1D("4_AdcWeightedSum","Sum of the Adc weighted measurements;Adc weighted counts; Events",adcnbin,-(Double_t)adclimit/2.,(Double_t)adclimit/2.);
          adcplots.push_back(h);
        }
        Int_t adcch=p_bmmap->GetAdcCh(i);
        TString title=(char)(4+i);
        title+="AdcWeighted_measCh_";
        title+=adcch;
        h=new TH1D(title.Data(),"Adc measurement weighted measurements;Adc weighted counts; Events",adcnbin,0.,adclimit);
        adcplots.push_back(h);
      }else
        break;
    }
  }

  //2d plots
  h2d=new TH2D("0_timevsbeamY","Beam Y position;Time [evts]; Beam position [cm]",5000,0,50000, 600,-3.,3.);
  recoplots.push_back(h2d);
  h2d=new TH2D("1_timevsbeamX","Beam X position;Time [evts]; Beam position [cm]",5000,0,50000, 600,-3.,3.);
  recoplots.push_back(h2d);


  return 0;
}

int main (int argc, char *argv[])  {

  TString in("12C_C_200_1.root");
  TString rootout("");
  TString exp("");
  TString calname, mapname, adcname;
  Bool_t print=kFALSE, recobool=kTRUE, tree=kFALSE;
  TH2F* h2f;

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
  locRec->EnableHisto();

  if(tree)
    locRec->EnableTree();
  if(recobool)
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
  TABMntuTrack *p_ntutrack;
  TABMactNtuTrack *p_acttrack;
  if(recobool){
    p_ntutrack = (TABMntuTrack*) (gTAGroot->FindDataDsc("bmTrack", "TABMntuTrack")->Object());
    p_acttrack=(TABMactNtuTrack*)gTAGroot->FindAction("bmActTrack");
  }

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

  vector<TH1*> scaplots, adcplots, recoplots;
  Booking(scaplots, adcplots, recoplots);
  Long64_t ientry;

  //set some useful variables
  Int_t scanobusy=-1;
  Int_t scabusy=-1;
  Int_t nobusycounts[4]; // sometimes the scaler wrongly read a negative counts. For this reason is better to shoe the four final counts of the nobusy channel to estimate the total noumber of particles
  Int_t nobusyweightcounts[4]; // scaler final counts weighted with the ADC measurements
  Int_t nobusylastindex=0;
  Double_t adcsumweighted;
  vector<Double_t>  adcmean;
  //default mean set to 0 and limit set to 2000 (2000 is a random high number)
  for(Int_t i=1;i<p_bmmap->GetAdcMaxCh();i++){ //set the adc integral value for each petal channel
    if(p_bmmap->GetAdcCh(i)!=-1){
      adcmean.push_back(0.);
    }else
      break;
  }
  //Set manually the means and the limits if desired

  if(p_bmmap->GetScaMaxCh()>0){
    scanobusy=p_bmmap->GetScaCh(1); //not busy coincidence channel adopted to count the incident particles
    scabusy=p_bmmap->GetScaCh(2); //not busy coincidence channel adopted to count the incident particles
    //few checks
    if(scanobusy<0 || scanobusy>p_bmmap->GetScaMaxCh()){
      cout<<"ERROR: scanobusy channel="<<scanobusy<<"   check your map!!"<<endl;
      return 1;
    }
    if(scanobusy<0 || scanobusy>p_bmmap->GetScaMaxCh()){
      cout<<"ERROR: scanobusy channel="<<scanobusy<<"   check your map!!"<<endl;
      return 1;
    }
  }


  //event loop
  for ( ientry= 0; ientry < nTotEv; ientry++) {
    if(ientry%1000==0 || ientry%(nTotEv/10)==0 )
      cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
    if(!gTAGroot->NextEvent())
      break;

    // p_actvme->PrintBMstruct();

    //fill some plots
    if(p_bmmap->GetScaMaxCh()>0){
      nobusycounts[nobusylastindex]=p_actvme->GetScaMeas(scanobusy).second;
      nobusylastindex=(nobusylastindex==3) ? 0 : nobusylastindex+1;
      if(bmstruct->time_evtoev!=0){
        scaplots.at(3)->Fill(p_actvme->GetScaMeas(scabusy).first*1000000./bmstruct->time_evtoev);
        scaplots.at(4)->Fill(p_actvme->GetScaMeas(scanobusy).first*1000000./bmstruct->time_evtoev);
        if((ientry+2)<scaplots.at(5)->GetNbinsX())
          scaplots.at(5)->SetBinContent(ientry+1,p_actvme->GetScaMeas(scanobusy).first*1000000./bmstruct->time_evtoev);
      }
    }
    if(recobool){
      for(Int_t i=0;i<p_ntutrack->GetTracksN();i++){
        TABMtrack *track=p_ntutrack->GetTrack(i);
        recoplots.at(0)->Fill(ientry,track->GetOrigin().Y());
        recoplots.at(1)->Fill(ientry,track->GetOrigin().X());
      }
    }
    if(p_bmmap->GetAdcCh(1)!=-1){
      adcsumweighted=0;
      for(Int_t i=0;i<adcmean.size();i++){
        Int_t adccha=p_bmmap->GetAdcCh(i+1);
        if(adccha!=-1){
          Double_t meas=p_actvme->GetAdcMeas(adccha).first-adcmean.at(i)-(Int_t)(p_bmcal->GetAdcPed(adccha)+.3*p_bmcal->GetAdcDevStd(adccha));
          adcplots.at(i+5)->Fill(meas);
          adcsumweighted+=meas;
        }else{
          cout<<"ERROR in the adcmean vector!!! check your parameters"<<endl;
        }
      }
      adcplots.at(4)->Fill(adcsumweighted);
    }


  }//end of event loop

  //further analysis:
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

  //adc and scaler
  Double_t adcratio=1;
  if(p_bmmap->GetAdcCh(1)!=-1 && p_bmmap->GetScaMaxCh()>0){
    TF1 *adcsumfit = new TF1("adcsumfit","gaus", -1000.,1000);
    adcsumfit->SetParameter(0,adcplots.at(4)->GetEntries());
    adcsumfit->SetParameter(1,adcplots.at(4)->GetMean());
    adcsumfit->SetParameter(2,adcplots.at(4)->GetStdDev());
    adcplots.at(4)->Fit("adcsumfit", "QB+");
    adcratio=(Double_t)adcplots.at(4)->GetEntries()/adcplots.at(4)->Integral(1,adcplots.at(4)->FindBin(adcsumfit->GetParameter(1)+adcsumfit->GetParameter(2)*5.));
    for(Int_t i=0;i<4;i++)
      nobusyweightcounts[i]=nobusycounts[i]*(adcratio);
  }

  //printout useful stuffs
  if(print==kTRUE){
    cout<<endl<<"Total number of events:"<<ientry<<endl;
    if(recobool){
      h2f=(TH2F*)p_acttrack->GetHistogrammList()->FindObject("bmTrackTargetMap");
      cout<<"number of reconstructed tracks="<<h2f->GetEntries()<<endl;
      cout<<"Beam mean Xpos="<<h2f->GetMean(1)<<" +- "<<h2f->GetStdDev(1)<<"  Ypos="<<h2f->GetMean(2)<<"  +- "<<h2f->GetStdDev(2)<<endl;;
    }
    if(p_bmmap->GetScaMaxCh()>0)
      cout<<"scaler nobusy channel="<<scanobusy<<";   nobusy final four counts="<<nobusycounts[0]<<" , "<<nobusycounts[1]<<" ,  "<<nobusycounts[2]<<" , "<<nobusycounts[3]<<endl;
    if(p_bmmap->GetAdcCh(1)!=-1){
      cout<<"Adc ratio="<<adcratio<<"  Final counts with adcratio="<<nobusyweightcounts[0]<<" , "<<nobusyweightcounts[1]<<" , "<<nobusyweightcounts[2]<<" , "<<nobusyweightcounts[3]<<endl<<endl;
    }
  }


  //save my histos
  locRec->AfterEventLoop();
  TFile fileout(rootout.Data(),"UPDATE");
  for(Int_t i=0;i<scaplots.size();i++)
    fileout.Add(scaplots.at(i));
  for(Int_t i=0;i<adcplots.size();i++)
    fileout.Add(adcplots.at(i));
  for(Int_t i=0;i<recoplots.size();i++)
    fileout.Add(recoplots.at(i));
  fileout.Write();
  fileout.Close();

  watch.Print();
  delete locRec;
  cout<<"AnalyzeBmStd done, output file="<<rootout.Data()<<endl;
 return 0;
}
