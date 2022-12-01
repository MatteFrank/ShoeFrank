
#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>
#include <TSystem.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"

int main (int argc, char *argv[])  {

  TString infile;
  TString preout("");
  TString exp("");
  TString calname, mapname, adcpedname;
  TString lis;

  Int_t runNb = -1;
  Int_t nTotEv = 1e7;
  Int_t nIte = 10;
  Bool_t evt0 = kFALSE;
  Bool_t bmstd = kFALSE;

  for (int i = 0; i < argc; i++){
     if(strcmp(argv[i],"-in") == 0)    { infile = TString(argv[++i]);  }   // Root file in input
     if(strcmp(argv[i],"-lis") == 0)   { lis = TString(argv[++i]);  }   // Root file in input
     if(strcmp(argv[i],"-out") == 0)   { preout = TString(argv[++i]); }   // Root file output
     if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
     if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
     if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
     if(strcmp(argv[i],"-ite") == 0)   { nIte = atoi(argv[++i]);  }    // Number of iterations for the BM STREL calibration
     if(strcmp(argv[i],"-evt0") == 0)  { evt0 = kTRUE;  }             // Flag to evaluate t0
     if(strcmp(argv[i],"-bmstd") == 0) { bmstd = kTRUE;  }            // Flag for the stand alone runs
     if(strcmp(argv[i],"-cal") == 0)   { calname = TString(argv[++i]);  }  // path for a specific calibration file
     if(strcmp(argv[i],"-map") == 0)   { mapname = TString(argv[++i]);  }  // path for a specific mapping file
     if(strcmp(argv[i],"-adc") == 0)   { adcpedname = TString(argv[++i]);  }  // path for a specific mapping file

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

   if(infile.IsNull() || gSystem->AccessPathName(infile.Data()))
   {
      Error("main()", "Input file does not exist or is null");
      exit(-1);
   }


  //few checks
  if (preout.IsNull()){
    preout=(evt0==kTRUE) ? "BM_T0Eval" : "BM_STREL" ;
    if(bmstd==kTRUE)
      preout+="_bmstd";
    cout<<"WARNING:: No output name has been set, a default name will be set with a prefix="<<preout.Data()<<endl;
  }
  if(adcpedname.Length()>0 && bmstd==kFALSE){
    cout<<"ERROR: bmstd=FALSE but you want to fit the adc pedestals, did you forget to add a -bmstd flag?"<<endl<<"Please check the input flags and values"<<endl<<"The program will be terminated"<<endl;
    return 1;
  }
  if(adcpedname.Length()>0 && evt0==kTRUE){
    cout<<"WARNING: you are trying to evaluate the T0s and the adc pedestals from the same file!!!!!!!!"<<endl<<"Usually they should be done with different files."<<endl;
  }

  TApplication::CreateApplication();
  TAGrecoManager::Instance(exp);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();
  vector<TF1*> strelvec;
  vector<TF1*> resovec;
  TString plotname;

  //BM T0 and ADC evaluation
  if(evt0==kTRUE || adcpedname.Length()>0){
    TString rootout=preout;
    TString txtout=preout;
    rootout+=".root";
    txtout+=".cal";

    LocalReco* locRec= new LocalReco(exp, runNb, infile, rootout);

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
    if(bmstd){
      TAGrecoManager::GetPar()->IncludeST(false);
      locRec->EnableStdAlone();
    }else{
      TAGrecoManager::GetPar()->IncludeST(true);
    }

    // locRec->EnableTree();
    locRec->EnableHisto();
    locRec->EnableSaveHits();

    TStopwatch watch;
    watch.Start();
    locRec->BeforeEventLoop();
    TABMparGeo* p_bmgeo = (TABMparGeo*) (gTAGroot->FindParaDsc("bmGeo", "TABMparGeo")->Object());
    TABMparConf* p_bmcon = (TABMparConf*) (gTAGroot->FindParaDsc("bmConf", "TABMparConf")->Object());
    TABMparCal*  p_bmcal = (TABMparCal*) (gTAGroot->FindParaDsc("bmCal", "TABMparCal")->Object());
    TABMparMap* p_bmmap = (TABMparMap*) (gTAGroot->FindParaDsc("bmMap", "TABMparMap")->Object());
    TABMactVmeReader *p_actvmereader;
    if(bmstd)
      p_actvmereader=(TABMactVmeReader*)gTAGroot->FindAction("bmActRaw");

    if(calname.Length()>0)
      p_bmcal->FromFile(calname);
    if(mapname.Length()>0)
      p_bmmap->FromFile(mapname,p_bmgeo);

    p_bmcal->ResetT0();

    locRec->LoopEvent(nTotEv);
    if(evt0==kTRUE){
      if(bmstd){
        p_actvmereader->EvaluateT0time();
      }else{
        TABMactNtuRaw *p_actnturaw=(TABMactNtuRaw*)gTAGroot->FindAction("bmActRaw");
        p_actnturaw->EvaluateT0time();
      }
      p_bmcal->PrintT0s(txtout,infile,gTAGroot->CurrentEventNumber());
      p_bmcal->PrintResoStrel(txtout);
    }
    if(adcpedname.Length()>0){
      p_actvmereader->EvaluateAdcPedestals();
      p_bmcal->PrintAdc(adcpedname, infile, gTAGroot->CurrentEventNumber());
    }
    locRec->AfterEventLoop();
    watch.Print();
    if(evt0==kTRUE)
      cout<<"T0 calculation done, calibration file:"<<txtout.Data()<<endl;
    if(adcpedname.Length()>0)
      cout<<"ADC pedestals calculation done, adc pedestal file:"<<adcpedname.Data()<<endl;
    cout<<"root output file="<<rootout.Data()<<endl;
    delete locRec;
  }else{      //BM self calibration loop
    ofstream outfile;
    TString txtout=preout;
    TString end = Form("_%s_%d_ite.txt",exp.Data(),runNb);
    txtout+=end;
    outfile.open(txtout.Data(),ios::out);

    for(Int_t currite=1;currite<=nIte;currite++){
      vector<TString> invecfile;
      if(infile.Length()>0)
        invecfile.push_back(infile);
      else if(lis.Length()>0){
        ifstream filelist;
        filelist.open(lis.Data(),fstream::in);
        char tmp_char[200];
        filelist>>tmp_char;
        TString tmp_filename(tmp_char);
        invecfile.push_back(tmp_filename);
      }
      for(Int_t currfilenum=0;currfilenum<invecfile.size();currfilenum++){
        TString rootout=preout;
        end = Form("_%s_%d_ite_%d.root",exp.Data(),runNb,currite);
        rootout+=end;

        LocalReco* locRec= new LocalReco(exp, runNb, invecfile.at(currfilenum), rootout);

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
        if(bmstd){
          TAGrecoManager::GetPar()->IncludeST(false);
          locRec->EnableStdAlone();
        }else{
          TAGrecoManager::GetPar()->IncludeST(true);
        }
        TAGrecoManager::GetPar()->CalibBM(1);

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
        if(calname.Length()>0)
        p_bmcal->FromFile(calname);
        if(mapname.Length()>0)
        p_bmmap->FromFile(mapname, p_bmgeo);

        if(currite>1){
          p_bmcal->SetResoFunc(resovec.back());
          p_bmcal->SetSTrelFunc(strelvec.back());
        }

        locRec->LoopEvent(nTotEv);
        plotname = Form("BMNewStrelFunc_%d",currite);
        TF1 *newstrel=new TF1(plotname.Data(),"pol4",0.,p_bmcon->GetHitTimeCut());
        plotname = Form("BMNewResoFunc_%d",currite);
        TF1 *resofunc=new TF1("BMNewResoFunc","pol10",0.,0.8);
        Double_t meanTimeReso;
        Double_t meanDistReso;
        TABMactNtuTrack *p_actntutrack=(TABMactNtuTrack*)gTAGroot->FindAction("bmActTrack");

        if(!p_actntutrack->ValidHistogram()){
          cout<<"ERROR!!! p_actntutrack->ValidHistogram() not true!!! the program will be ended"<<endl;
          return 0;
        }

        p_actntutrack->FitWriteCalib(newstrel, resofunc, meanTimeReso, meanDistReso);

        //printout
        outfile<<"BM_strel_calibration: "<<infile.Data()<<"   iteration: "<<currite<<"/"<<nIte<<endl;
        outfile<<"mean_spatialres_time: "<<meanTimeReso<<"  mean_spatialres_dist: "<<meanDistReso<<endl;
        outfile<<"Resolution_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
        outfile<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
        for(Int_t i=0;i<resofunc->GetNpar();i++)
        outfile<<resofunc->GetParameter(i)<<"   ";
        outfile<<endl;
        outfile<<"STrel_function:  "<<newstrel->GetFormula()->GetExpFormula().Data()<<endl;
        outfile<<"STrel_number_of_parameters:  "<<newstrel->GetNpar()<<endl;
        for(Int_t i=0;i<newstrel->GetNpar();i++)
        outfile<<newstrel->GetParameter(i)<<"   ";
        outfile<<endl<<endl;

        locRec->AfterEventLoop();
        resovec.push_back(resofunc);
        strelvec.push_back(newstrel);
        watch.Print();
        cout<<"Bm strel calibration done; iteration:"<<currite<<"/"<<nIte<<endl;
        cout<<"root output file="<<rootout.Data()<<"    txt output file="<<txtout.Data()<<endl;
        cout<<"mean_spatialres_time="<<meanTimeReso<<"  mean_spatialres_dist="<<meanDistReso<<endl<<endl<<endl;
        delete locRec;
      }
    } //end of iteration loop
    outfile.close();
  }
   return 0;
}
