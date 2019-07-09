// Macro to display FOOT raw data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAFOeventDisplay.hxx"
#endif

void DisplayFOOT(TString file = "data/data_built.2211.physics_foot.daq.VTX.1.dat")
//void DisplayFOOT(TString file = "./run_2211")
{
   //TAFOeventDisplay::SetVtxTrackingAlgo('F');
   //TAFOeventDisplay::EnableStdAlone();
   TAFOeventDisplay::Instance(0, "GSI")->ShowDisplay(file);
}
