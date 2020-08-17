// Macro to display FOOT MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAFOeventDisplayMC.hxx"
#endif

void DisplayMcFOOT(TString file = "12C_C_200_1.root")
{
   TAFOeventDisplay::SetVtxTrackingAlgo('F');
//   TAFOeventDisplay::EnableItrTracking();
//   TAFOeventDisplay::DisableTracking();
   
   TAFOeventDisplayMC::Instance(1, "12C_200")->ShowDisplay(file, 1);
}


