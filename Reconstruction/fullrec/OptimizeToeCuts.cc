#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "GlobalToeReco.hxx"

#include "TATOEoptimizer.hxx"
#include "TATOEutilities.hxx"


int main (int argc, char *argv[])  {

   TString in("");
   TString exp("");

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Optimize help:"<<endl;
         cout<<" Ex: Optimize [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         return 1;
      }
   }
   
   TApplication::CreateApplication();
   
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   TAGrecoManager::GetPar()->DisableTree();
   TAGrecoManager::GetPar()->DisableHisto();
   
   TAGrecoManager::GetPar()->IncludeTOE(true);
   TAGrecoManager::GetPar()->IncludeKalman(false);
   
    BaseReco* glbRec = new GlobalToeReco(exp, runNb, in, "", true);
    glbRec->EnableRecCutter();
    glbRec->EnableTracking();

    
   TStopwatch watch;
   watch.Start();
   
   glbRec->BeforeEventLoop();
    using config_t = details::configuration< details::vertex_tag, details::it_tag, details:: msd_tag, details::tof_tag >;
//    auto * o_h = new_optimizer<
//                    config_t,
//                    global_scan_procedure,
//                    rough_scan_procedure<details::local_scan_parameters<6,3>>,
//                    fine_scan_procedure<details::local_scan_parameters<3,1>>
//                                   >( "toeActOptimizer" );
    auto * o_h = new_optimizer<
                    config_t,
                    global_scan_procedure_only
                                   >( "toeActOptimizer" );
    gTAGroot->AddRequiredItem("toeActOptimizer");
    
    while( !o_h->is_optimization_done() ){
        
        if( o_h->is_procedure_done() ){
            o_h->finalise_procedure();
            o_h->switch_procedure();
            o_h->setup_procedure();
            continue; //continue is needed because gloabl_scan::finalize can decide the optimization is done
        }
        
        o_h->setup_cuts();
        o_h->output_cuts();
        
        static_cast<TAGactTreeReader*>(gTAGroot->FindAction("evtReader"))->Reset();
        gTAGroot->SetEventNumber(0);
        glbRec->LoopEvent(nTotEv);
        o_h->call();
        
        o_h->setup_next_iteration();
    }
    
   glbRec->AfterEventLoop();
   
   watch.Print();

   delete glbRec;
   
   return 0;
} 















