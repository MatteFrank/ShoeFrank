{
   printf("******************************************************************** \n") ;
   printf("                    Messages from rootlogon.C \n") ;
   printf("******************************************************************** \n") ; 
   printf("                     Load libraries for @PROJECT_NAME@ \n") ;
   printf("******************************************************************** \n") ; 
   
   TString tmp, prefix = gSystem->Getenv("PWD");
   
   if ( prefix.EndsWith("/") == false )
	  prefix += "/";
	  
   // expand path to get specific macros 
   TString macroPATH = gROOT->GetMacroPath();
   tmp = prefix; 
   if ( macroPATH.Contains(tmp.Data()) == kFALSE ) { 
	  printf("- Add %s to the macros path \n",tmp.Data());
	  tmp += ":";
	  macroPATH.Prepend(tmp.Data()); gROOT->SetMacroPath(macroPATH.Data());
   }    

    // comes from configure
   tmp = "@TAG_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAG@.so");

   tmp = "@TAMC_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAMC@.so");

   tmp = "@TADI_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TADI@.so");

   tmp = "@TADAQ_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TADAQ@.so");

   tmp = "@TAAPI_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAAPI@.so");

   tmp = "@TAST_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAST@.so");

   tmp = "@TAIR_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAIR@.so");

   tmp = "@TABM_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TABM@.so");

   tmp = "@TAVT_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAVT@.so");

   tmp = "@TAIT_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAIT@.so");

   tmp = "@TAMSD_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAMSD@.so");

   tmp = "@TATW_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TATW@.so");

   tmp = "@TACA_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TACA@.so");

   tmp = "@TAED_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAED@.so");


  TString opt("@GENFIT_DIR@");
   if (opt == "ON") {
      tmp = "@GENFIT_INCLUDE_ROOT@";
      printf("- Add GenFit to the include path \n");
      gSystem->AddIncludePath(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@GENFIT_LIBRARIES@.so");

      tmp = "@TAGF_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAGF@.so");
   }

   opt = "@TOE_DIR@";
   if (opt == "ON") {
      tmp = "@TOE_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());

      tmp = "@TATOE_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TATOE@.so");
   }
   
   tmp = "@TAFO_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAFO@.so");
   
   opt = "@ANC_DIR@";
   if (opt == "ON") {
      tmp = "@TAPL_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAPL@.so");

      tmp = "@TACE_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TACE@.so");
   
      tmp = "@TANE_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TANE@.so");
      
      tmp = "@TAPXI_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAPXI@.so");
      
      tmp = "@TAMP_INCLUDE_DIR@";
      printf("- Add %s to the include path \n",tmp.Data());
      tmp.Prepend(" .include ");
      gROOT->ProcessLine(tmp.Data());
      gSystem->Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAMP@.so");
   }
}
