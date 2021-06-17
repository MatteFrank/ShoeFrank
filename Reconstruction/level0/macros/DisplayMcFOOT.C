// Macro to display FOOT MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAGeventDisplayMC.hxx"
#endif

void DisplayMcFOOT(TString file = "16O_C_200_trig_shoereg.root", TString expName = "GSI2021_MC", Int_t runNumber = 200)
//void DisplayMcFOOT(TString file = "12C_C_200new_trig_shoereg.root", TString expName = "12C_200new", Int_t runNumber = 1)
//void DisplayMcFOOT(TString file = "12C_C_200shoe.root", TString expName = "12C_200", Int_t runNumber = 1)
//void DisplayMcFOOT(TString file = "12C_C_200.root", TString expName = "12C_200", Int_t runNumber = 1)
{
   TAGbaseEventDisplay::SetVtxTrackingAlgo('F');
//   TAGbaseEventDisplay::EnableItrTracking();
//   TAGbaseEventDisplay::DisableTracking();
   
   TAGeventDisplayMC::Instance(expName, runNumber)->ShowDisplay(file);
}


