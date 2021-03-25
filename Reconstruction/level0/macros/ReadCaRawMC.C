// Macro to make reconstruction from MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>

#include "Evento.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGparGeo.hxx"
#include "TACAparGeo.hxx"
#include "TACAntuHit.hxx"

#include "TACAactNtuMC.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGcampaignManager* campManager  = 0x0;
TAGactTreeWriter* outFile = 0x0;
TACAactNtuMC* caActRaw = 0x0;

void FillMCCa(EVENT_STRUCT *myStr, Int_t runNumber) {
   
   TAGparaDsc* gGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeoG = (TAGparGeo*)gGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
   parGeoG->FromFile(parFileName.Data());

   TAGparaDsc* caGeo    = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
   TACAparGeo* geomap   = (TACAparGeo*) caGeo->Object();
   parFileName = campManager->GetCurGeoFile(TACAparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName);
   
   TAGdataDsc* caRaw    = new TAGdataDsc("caRaw", new TACAntuHit());
   
   caActRaw  = new TACAactNtuMC("caActRaw", caRaw, caGeo, gGeo, myStr);
   caActRaw->CreateHistogram();
   
   outFile->SetupElementBranch(caRaw, TACAntuHit::GetBranchName());
}

void ReadCaRawMC(TString name = "16O_C2H4_200_1.root", TString expName = "16O_200", Int_t runNumber = 1)
{
   GlobalPar::Instance(expName);
   GlobalPar::GetPar()->FromFile();
   GlobalPar::GetPar()->Print();
   
   TAGroot tagr;
   
   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();
   
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   geoTrafo->FromFile(parFileName);
   
   TFile* f = new TFile(name.Data());
   f->ls();
   
   TTree* tree = (TTree*)gDirectory->Get("EventTree");
   
   Evento *ev  = new Evento();
   
   EVENT_STRUCT evStr;
   
   ev->FindBranches(tree,&evStr);
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillMCCa(&evStr, runNumber);
   
   tagr.AddRequiredItem("caActRaw");
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_OutCa.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   caActRaw->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Long64_t nentries = tree->GetEntries();
   
   cout<<"Running against ntuple with:: "<<nentries<<" entries!"<<endl;
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t ientry = 0; ientry < nentries; ientry++) {
      
      nb = tree->GetEntry(ientry);
      nbytes += nb;
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
//      if (ientry == 500)
      if (ientry == 50000)
         break;
      
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


