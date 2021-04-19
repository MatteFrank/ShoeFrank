// Macro to display FOOT MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAGeventDisplayMC.hxx"
#endif

//void DisplayMcFOOT(TString file = "12C_C_200shoe.root", TString expName = "12C_200", Int_t runNumber = 1)
void DisplayMcFOOT(TString file = "12C_C_200.root", TString expName = "12C_200", Int_t runNumber = 1)
{
  // type = 1: Fluka MC structure
  // type = 2: Shoe MC structure
   TAGbaseEventDisplay::SetVtxTrackingAlgo('F');
//   TAGbaseEventDisplay::EnableItrTracking();
//   TAGbaseEventDisplay::DisableTracking();
   
   TAGeventDisplayMC::Instance(expName, runNumber)->ShowDisplay(file);
}


