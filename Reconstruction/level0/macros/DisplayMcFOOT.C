// Macro to display FOOT MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAFOeventDisplayMC.hxx"
#endif

//void DisplayMcFOOT(TString file = "12C_400_vtx.root")
void DisplayMcFOOT(TString file = "16O_C2H4_200_1.root")
{
   TAFOeventDisplay::SetVtxTrackingAlgo('F');
//   TAFOeventDisplay::DisableTracking();
   
   TAFOeventDisplayMC::Instance(1)->ShowDisplay(file);
}


