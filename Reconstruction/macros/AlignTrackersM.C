// Macro to make alignment
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TString.h>
#include <TStopwatch.h>

#include "TAIRalignM.hxx"

#endif

void AlignTrackersM(TString name = "ionEventoC_IR_100k_s10_s11_s27_Out.root", const TString expName = "12C_200new", Int_t runNumber = 1)
{
  TAIRalignM* g = TAIRalignM::Instance(name, expName, runNumber);
   
   TStopwatch watch;
   watch.Start();
   
   g->LoopEvent(50000);

   watch.Print();
}


