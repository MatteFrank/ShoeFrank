#include "BMAnalysis.h"

// nameFile=Input file name
// entries: number of events to be processed (use 0 to process the whole file)
// printFile: redirect cout to an external txt file
void BMAnalysis(TString nameFile = "", Int_t nentries = 0)

{

  TFile *inputFile = new TFile(nameFile.Data());
  
  if(OpenInputFile(inputFile))
    return;
  if(ChargeCampaignParameters(inputFile))
    return;
  ChargeParFiles(nentries);
  SetOutputFiles(nameFile);  //define the output root filename
  BookHisto();
  InitializeVariables();

  cout<<"input file="<<nameFile.Data()<<endl;
  cout<<"I'll process "<<maxentries<<" events. The input tree contains a total of "<<tree->GetEntries()<<" events."<<endl;

  for (evnum = 0; evnum < maxentries; ++evnum) {

    if(evnum%100==0)
      printf("Processed Events: %d\n", evnum);

    tree->GetEntry(evnum);

    if(IncludeBM)
      BeamMonitor();
    if(IncludeMC && IncludeREG)
      BMC();
   

  }//Loop on events

  PostLoopAnalysis();
  outputFile->Write();
  outputFile->Close();
  DeleteVariables();

  cout<<"program executed; output file= "<<outputFile->GetName()<<endl;
}
