
#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"

int main (int argc, char *argv[])  {

  TString in("12C_C_200_1.root");
  TString preout("");
  TString exp("");

  Int_t runNb = -1;
  Int_t nTotEv = 1e7;
  Int_t nIte = 10;
  Bool_t isMC = kFALSE;

  for (int i = 0; i < argc; i++){
     if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
     if(strcmp(argv[i],"-out") == 0)   { preout = TString(argv[++i]); }   // Root file output
     if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
     if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
     if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
     if(strcmp(argv[i],"-ite") == 0)   { nIte = atoi(argv[++i]);  }    // Number of iterations for the BM STREL calibration
     if(strcmp(argv[i],"-isMC") == 0)   { isMC = kTRUE;  }             // Flag for MC studies 

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

  if (preout.IsNull()) 
    preout="STrel_Bm_selfcalib";

  TApplication::CreateApplication();
  TAGrecoManager::Instance(exp);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();
  vector<TF1*> strelvec; 
  vector<TF1*> resovec; 
  TString plotname;

  for(Int_t currite=1;currite<=nIte;currite++){
  
    TString rootout=preout;
    TString txtout=preout;
    TString end = Form("_%s_%d_ite_%d.root",exp.Data(),runNb,currite);
    rootout+=end;
    end = Form("_%s_%d_ite.txt",exp.Data(),runNb);
    txtout+=end;

    BaseReco* locRec;
    
    if(isMC==kTRUE){
      locRec = new LocalRecoMC(exp, runNb, in, rootout);
    }else{
      locRec= new LocalReco(exp, runNb, in, rootout);
    }
    
    // global setting
    TAGrecoManager::GetPar()->IncludeST(true);
    TAGrecoManager::GetPar()->IncludeBM(true);
    TAGrecoManager::GetPar()->CalibBM(true);
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
    
    locRec->EnableTree();
    locRec->EnableHisto();
    locRec->EnableSaveHits();
    locRec->EnableTracking();
    
    TStopwatch watch;
    watch.Start();    
    locRec->BeforeEventLoop();
    TABMparConf* p_bmcon = (TABMparConf*) (gTAGroot->FindParaDsc("bmConf", "TABMparConf")->Object()); 
    TABMparCal*  p_bmcal = (TABMparCal*) (gTAGroot->FindParaDsc("bmCal", "TABMparCal")->Object()); 
    
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
    ofstream outfile;
    outfile.open(txtout.Data(),ios::app);
    outfile<<"BM_strel_calibration: "<<in.Data()<<"   iteration: "<<currite<<"/"<<nIte<<endl;
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
    outfile.close(); 
    
    locRec->AfterEventLoop();
    resovec.push_back(resofunc);
    strelvec.push_back(newstrel);
    watch.Print();
    cout<<"Bm strel calibration done; iteration:"<<currite<<"/"<<nIte<<endl;
    cout<<"root output file="<<rootout.Data()<<"    txt output file="<<txtout.Data()<<endl;
    cout<<"mean_spatialres_time="<<meanTimeReso<<"  mean_spatialres_dist="<<meanDistReso<<endl<<endl<<endl;
    delete locRec;
    
  } //end of iteration loop

   return 0;
}
