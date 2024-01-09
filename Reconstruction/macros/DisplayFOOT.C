// Macro to display FOOT raw data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAGeventDisplay.hxx"
#endif

//void DisplayFOOT(TString file = "dataRaw/data_test.00006115.physics_foot.daq.RAW_synched.0001.data", TString expName = "CNAO2023")
//void DisplayFOOT(TString file = "dataRaw/data_test.00006086.physics_foot.daq.RAW_synched.0001.data", TString expName = "CNAO2023")
//void DisplayFOOT(TString file = "dataRaw/data_test.00006309.physics_foot.daq.RAW_synched.0001.data", TString expName = "CNAO2023")
void DisplayFOOT(TString file = "dataRaw/data_test.00006144.physics_foot.daq.RAW_synched.0001.data", TString expName = "CNAO2023")
//void DisplayFOOT(TString file = "dataRaw/data_test.00005300.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", TString expName = "CNAO2022")
//void DisplayFOOT(TString file = "dataRaw/data_test.00004303.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", TString expName = "GSI2021")
//void DisplayFOOT(TString file = "dataRaw/data_built.2211.physics_foot.daq.VTX.1.dat", TString expName = "GSI")
//void DisplayFOOT(TString file = "./run_2211", TString expName = "GSI", Int_t runNumber = 2211)
{
   //TAGbaseEventDisplay::EnableStdAlone();
   TAGeventDisplay::Instance(expName)->ShowDisplay(file);
}
