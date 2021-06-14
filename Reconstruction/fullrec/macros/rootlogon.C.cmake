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

   tmp = "@TAMC_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TADI_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TADAQ_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAAPI_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAST_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAIR_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TABM_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAVT_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAIT_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAMSD_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TATW_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TACA_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAED_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TATOE_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAGF_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TOE_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@TAFO_INCLUDE_DIR@";
   printf("- Add %s to the include path \n",tmp.Data());
   tmp.Prepend(" .include ");
   gROOT->ProcessLine(tmp.Data());

   tmp = "@GENFIT_INCLUDE_ROOT@";
   printf("- Add GenFit to the include path \n");
   gSystem->AddIncludePath(tmp.Data());
}
