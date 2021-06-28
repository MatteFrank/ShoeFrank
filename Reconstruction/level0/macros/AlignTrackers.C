// Macro to make alignment
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TString.h>
#include <TStopwatch.h>

#include "TAGroot.hxx"
#include "TAIRalignC.hxx"

#endif

//void Align(TString name = "12C_400_vtx_X11_100u_Out.root")
//void Align(TString name = "12C_400_vtx_it_clus_Out.root")
//void Align(TString name = "12C_400_vtx_msd_clus_Out.root")
//void Align(TString name = "12C_400_vtx_Out_100mu.root")
//void Align(TString name = "run_1029_cut35_110kEvt.root")
//void Align(TString name = "run_1028_cut35_5kEvt.root")
//void Align(TString name = "run_1028_cut5_dist500mu_5kEvt.root")
//void Align(TString name = "run_1021_cut35_33kEvt.root")
void AlignTrackers(TString name = "run.2211_cut30-40_61kEvt_DP_N.root", const TString expName = "GSI", Int_t runNumber = 211)
{
   Bool_t flagVtx = true;
   Bool_t flagIt  = false;
   Bool_t flagMsd = false;
   
   
  TAIRalignC* g = TAIRalignC::Instance(name, expName, runNumber, flagVtx, flagIt, flagMsd, -1);
   
   TStopwatch watch;
   watch.Start();
   
   g->LoopEvent(62000);

   watch.Print();
}


