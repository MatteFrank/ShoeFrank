#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "GlobalToeReco.hxx"

#include "TATOEoptimizer.hxx"
#include "TATOEutilities.hxx"

#include "flag_set.hpp"

namespace procedure{
struct grfs{ static constexpr uint8_t shift = 0; };
struct gso{ static constexpr uint8_t shift = 1; };
}
namespace scoring{
    struct baseline { static constexpr uint8_t shift = 2;};
    struct mass { static constexpr uint8_t shift = 3;};
    struct momentum { static constexpr uint8_t shift = 4; };
struct new_baseline { static constexpr uint8_t shift = 5;};
}

int main (int argc, char *argv[])  {

   TString in("");
   TString exp("");

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
    std::size_t event_to_skip{0};
   
    uint8_t opcode{0};
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
       if(strcmp(argv[i],"-nskip") == 0)   { event_to_skip = atoi(argv[++i]); }
       
       if( std::string{argv[i]} == "-score" ){
           auto scoring_function = std::string{argv[++i]};
           if( scoring_function == "baseline"){ opcode |= flag_set<scoring::baseline>{}; }
           else if( scoring_function == "mass"){ opcode |= flag_set<scoring::mass>{}; }
           else if( scoring_function == "momentum"){ opcode |= flag_set<scoring::momentum>{}; }
           else if( scoring_function == "new_baseline"){ opcode |= flag_set<scoring::new_baseline>{}; }
       }
       if( std::string{argv[i]} == "-method" ){
           auto specified_procedure = std::string{argv[++i]};
           if( specified_procedure == "grfs"){ opcode |= flag_set<procedure::grfs>{}; }
           if( specified_procedure == "gso"){ opcode |= flag_set<procedure::gso>{}; }
       }
      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Optimize help:"<<endl;
         cout<<" Ex: Optimize [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
          cout<<"      -score scoring_function      : [def=baseline]"<<endl;
          cout<<"      -method name      : [def=grfs]"<<endl;
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
//    using config_t = details::configuration< details::vertex_tag, details:: msd_tag, details::tof_tag >;
//    using config_t = details::configuration< details::vertex_tag, details:: it_tag, details::tof_tag >;
    
    TATOEbaseOptimizer * o_h{nullptr};
    
    switch(opcode){
        case flag_set< procedure::grfs, scoring::baseline >{} : {
            std::cout << "grfs+baseline\n";
            o_h = new_optimizer<
                                    config_t,
                                    baseline_scorer,
                                    global_scan_procedure,
                                    rough_scan_procedure,
                                    fine_scan_procedure
                                                   >( "toeActOptimizer" );
            break;
        }
        case flag_set< procedure::grfs, scoring::new_baseline >{} : {
            std::cout << "grfs+new_baseline\n";
            o_h = new_optimizer<
                                    config_t,
                                    new_baseline_scorer,
                                    global_scan_procedure,
                                    rough_scan_procedure,
                                    fine_scan_procedure
                                                   >( "toeActOptimizer" );
            break;
        }
        case flag_set< procedure::grfs, scoring::mass >{} : {
            std::cout << "grfs+mass\n";
            o_h = new_optimizer<
                                    config_t,
                                   mass_scorer,
                                    global_scan_procedure,
                                    rough_scan_procedure,
                                    fine_scan_procedure
                                                   >( "toeActOptimizer" );
            break;
        }
        case flag_set< procedure::grfs, scoring::momentum >{} : {
            std::cout << "grfs+momentum\n";
            o_h = new_optimizer<
                                    config_t,
                                   momentum_scorer,
                                    global_scan_procedure,
                                    rough_scan_procedure,
                                    fine_scan_procedure
                                                   >( "toeActOptimizer" );
            break;
        }
        case flag_set< procedure::gso, scoring::baseline >{} : {
            std::cout << "gso+baseline\n";
            o_h = new_optimizer<
                                config_t,
                                baseline_scorer,
                                global_scan_procedure_only
                                               >( "toeActOptimizer" );
            break;
        }
        case flag_set< procedure::gso, scoring::new_baseline >{} : {
            std::cout << "gso+new_baseline\n";
            o_h = new_optimizer<
                                config_t,
                                new_baseline_scorer,
                                global_scan_procedure_only
                                               >( "toeActOptimizer" );
            break;
        }
        case flag_set< procedure::gso, scoring::mass >{} : {
            std::cout << "gso+mass\n";
            o_h = new_optimizer<
                                config_t,
                                mass_scorer,
                                global_scan_procedure_only
                                               >( "toeActOptimizer" );
            break;
        }
        case flag_set< procedure::gso, scoring::momentum >{} : {
            std::cout << "gso+momentum\n";
            o_h = new_optimizer<
                                config_t,
                                momentum_scorer,
                                global_scan_procedure_only
                                               >( "toeActOptimizer" );
            break;
        }
        default:{
            std::cout << "default\n";
            o_h = new_optimizer<
                            config_t,
                            baseline_scorer,
                            global_scan_procedure,
                            rough_scan_procedure,
                            fine_scan_procedure
                                           >( "toeActOptimizer" );
        }
    }

    gTAGroot->AddRequiredItem("toeActOptimizer");
    
    while( !o_h->is_optimization_done() ){
        std::cout << "new iteration: \n";
        std::cout <<"is_optimization_done: " << std::boolalpha << o_h->is_optimization_done() << '\n';
        std::cout <<"is_procedure_done: " << std::boolalpha << o_h->is_procedure_done() << '\n';
        
        if( o_h->is_procedure_done() ){
            o_h->finalise_procedure();
            o_h->switch_procedure();
            o_h->setup_procedure();
            continue; //continue is needed because gloabl_scan::finalize can decide the optimization is done
        }
        
        o_h->setup_cuts();
        o_h->output_cuts();
        
        gTAGroot->ClearAllData();
        static_cast<TAGactTreeReader*>(gTAGroot->FindAction("evtReader"))->Reset(event_to_skip);
        gTAGroot->SetEventNumber(event_to_skip);
        glbRec->LoopEvent(nTotEv);
        o_h->call();
        
        o_h->setup_next_iteration();
    }
    
    o_h->output_cuts();
    
   glbRec->AfterEventLoop();
   
   watch.Print();

   delete glbRec;
   
   return 0;
} 















