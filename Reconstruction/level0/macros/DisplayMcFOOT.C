// Macro to display FOOT MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAFOeventDisplayMC.hxx"
#endif

//void DisplayMcFOOT(TString file = "12C_C_200shoe.root", TString expName = "12C_200", Int_t runNumber = 1, Int_t type = 2)
void DisplayMcFOOT(TString file = "12C_C_200_1.root", TString expName = "12C_200", Int_t runNumber = 1, Int_t type = 1)
{
  // type = 1: Fluka MC structure
  // type = 2: Shoe MC structure
   TAFObaseEventDisplay::SetVtxTrackingAlgo('F');
//   TAFObaseEventDisplay::EnableItrTracking();
//   TAFObaseEventDisplay::DisableTracking();
   
   TAFOeventDisplayMC::Instance(expName, type)->ShowDisplay(file, runNumber);
}


