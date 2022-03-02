// Macro to display FOOT raw data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAGeventDisplay.hxx"
#endif

void DisplayFOOT(TString file = "dataRaw/data_test.00004303.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", TString expName = "GSI2021", Int_t runNumber = 4303)
//void DisplayFOOT(TString file = "dataRaw/data_built.2211.physics_foot.daq.VTX.1.dat", TString expName = "GSI", Int_t runNumber = 2211)
//void DisplayFOOT(TString file = "./run_2211", TString expName = "GSI", Int_t runNumber = 2211)
{
   //TAGbaseEventDisplay::SetVtxTrackingAlgo('F');
   //TAGbaseEventDisplay::EnableStdAlone();
   TAGeventDisplay::Instance(expName, runNumber)->ShowDisplay(file);
}
