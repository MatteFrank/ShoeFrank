
#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "GlobalPar.hxx"
#include "LocalReco.hxx"
#include "LocalRecoNtuMC.hxx"

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
  GlobalPar::Instance(exp);
  GlobalPar::GetPar()->Print();

  //Whatever it is we're ONLY going to calibrate BM!
  for(Int_t currite=1;currite<=nIte;currite++){
  
    TString rootout=preout;
    TString txtout=preout;
    TString end = Form("_%s_%d_ite_%d.root",exp.Data(),runNb,currite);
    rootout+=end;
    end = Form("_%s_%d_ite_%d.txt",exp.Data(),runNb,currite);
    txtout+=end;

    BaseReco* locRec;
     
    
    if(isMC==kTRUE){
      locRec = new LocalRecoNtuMC(exp, runNb, in, rootout);
    }else{
      locRec= new LocalReco(exp, runNb, in, rootout);
    }
    
    // global setting
    GlobalPar::GetPar()->IncludeST(true);
    GlobalPar::GetPar()->IncludeBM(true);
    GlobalPar::GetPar()->CalibBM(true);
    GlobalPar::GetPar()->IncludeTW(false);
    GlobalPar::GetPar()->CalibTW(false);
    GlobalPar::GetPar()->IncludeDI(false);
    GlobalPar::GetPar()->IncludeMSD(false);
    GlobalPar::GetPar()->IncludeCA(false);
    GlobalPar::GetPar()->IncludeTG(false);
    GlobalPar::GetPar()->IncludeVT(false);
    GlobalPar::GetPar()->IncludeIT(false);
    GlobalPar::GetPar()->IncludeTOE(false);
    GlobalPar::GetPar()->DisableLocalReco();
    
    locRec->EnableTree();
    locRec->EnableHisto();
    locRec->EnableSaveHits();
    locRec->EnableTracking();
    
    TStopwatch watch;
    watch.Start();    
    locRec->BeforeEventLoop();
    locRec->LoopEvent(nTotEv);
    TABMactNtuTrack *p_actntutrack=(TABMactNtuTrack*)gTAGroot->FindAction("bmActTrack");
    p_actntutrack->FitWriteCalib(txtout);
    locRec->AfterEventLoop();
    watch.Print();
    delete locRec;
    
    cout<<"Bm strel calibration done; iteration:"<<currite<<"/"<<nIte<<endl;
    cout<<"root output file="<<rootout.Data()<<"    txt output file="<<txtout.Data()<<endl;
    
  } //end of iteration loop

   return 0;
}
