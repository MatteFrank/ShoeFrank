// Macro to print TOF geo parameters
// Ch. Finck

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TString.h>
#include "TAGaction.hxx"
#include "TAGroot.hxx"

#include "TAGrecoManager.hxx"
#include "TAGcampaignManager.hxx"

#endif

// main

void BenchMark(TString expName = "12C_200" )
{
  TAGroot tagr;
  TAGrecoManager::Instance(expName);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();
  
  TAGcampaignManager* man = new TAGcampaignManager(expName);
  man->FromFile();
  man->Print("");
}


