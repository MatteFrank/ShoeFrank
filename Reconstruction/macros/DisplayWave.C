// Macro to display FOOT MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TACEwaveDisplay.hxx"
#endif

void DisplayWave(TString file = "./data/Run_1200plas_1500plasnew_backTipex_2plas_coinc_24.6MeV_Data_12_10_2021_Binary.bin", TString exp ="CLINM")
{
   TACEwaveDisplay::Instance(file, exp);
}


