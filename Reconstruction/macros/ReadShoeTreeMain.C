#include "ReadShoeTreeFunc.h"

// nameFile=Input file name
// entries: number of events to be processed (use 0 to process the whole file)
// printFile: redirect cout to an external txt file
void ReadShoeTreeMain(TString nameFile = "", Int_t nentries = 0, Int_t debug_in=0)
{

  debug=debug_in;
  if(OpenInputFile(nameFile))
    return;
  if(ChargeCampaignParameters())
    return;
  ChargeParFiles(nentries);
  SetOutputFiles(nameFile);
  BookHisto();

  cout<<"input file="<<nameFile.Data()<<endl;
  cout<<"I'll process "<<maxentries<<" events. The input tree contains a total of "<<tree->GetEntries()<<" events."<<endl;

  //event loop
  for (evnum = 0; evnum < maxentries; ++evnum) {

    if(evnum%100==0)
      printf("Processed Events: %d\n", evnum);

    tree->GetEntry(evnum);

    if(IncludeVT)
      Vertex();
    if(IncludeGLB)
      GLBTRKstudies();
    if(IncludeDAQ)
      DataAcquisition();
    if(IncludeMC)
      MonteCarlo();

      CaloTest();
  }

  //write and close the input/output files
  inputFile->Close();
  outputFile->Write();
  outputFile->Close();

  cout<<"program executed; output file= "<<outputFile->GetName()<<endl;
}
