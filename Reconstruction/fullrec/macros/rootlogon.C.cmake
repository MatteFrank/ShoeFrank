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
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TABMbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data()); 

   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAMSDbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());
   
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAGfoot/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data()); 
   
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAIRbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data());

   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TASTbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TADIbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAEDbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TATWbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data()); 
   
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TACAbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data()); 
   
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAGbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data());
   
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAMCbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data());
   
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAITbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data());
   
   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAVTbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data());

   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TATOEbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@CMAKE_LIBRARY_OUTPUT_DIRECTORY@";
   tmp += "/TAGFbase/"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@GENFIT_INCLUDE_ROOT@";
   printf("- Add GenFit to the include path \n");
   gSystem->AddIncludePath(tmp.Data());
   
   gSystem->Load("libEve.so");
   gSystem->Load("libgenfit2.so");

   gSystem->Load("libTAGbase.so");
   gSystem->Load("libTAMCbase.so");
   gSystem->Load("libTASTase.so");
   gSystem->Load("libTABMbase.so");
   gSystem->Load("libTAVTbase.so");
   gSystem->Load("libTAITbase.so");
   gSystem->Load("libTAMSDbase.so");
   gSystem->Load("libTATWbase.so");
   gSystem->Load("libTACAbase.so");
   gSystem->Load("libTATOEbase.so");
   gSystem->Load("libTAGFbase.so");
   gSystem->Load("libTAGfoot.so");
}
