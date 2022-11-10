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
   tmp = "@CMAKE_CURRENT_SOURCE_DIR@";
   tmp += "/TAGbase"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include "); 
   gROOT->ProcessLine(tmp.Data()); 

   tmp = "@CMAKE_CURRENT_SOURCE_DIR@";
   tmp += "/TAPLbase"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@CMAKE_CURRENT_SOURCE_DIR@";
   tmp += "/TACEbase"; printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   gSystem->Load("libEve.so");
   gSystem->Load("libTAGbase.so");
   gSystem->Load("libTAPLbase.so");
   gSystem->Load("libTACEbase.so");
}
