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

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TATWparCal.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"

#include "TAGcampaignManager.hxx"
#include "TATWactNtuMC.hxx"
#include "TATWactNtuPoint.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter* outFile = 0x0;
TATWactNtuMC* twActRaw = 0x0;
TATWactNtuPoint* twActRec = 0x0;

void FillMCTw(EVENT_STRUCT *myStr, Int_t runNumber) {
   
   TAGparaDsc* gGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeoG = (TAGparGeo*)gGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
   parGeoG->FromFile(parFileName.Data());
   
   Int_t Z_beam = parGeoG->GetBeamPar().AtomicNumber;
   Int_t A_beam = parGeoG->GetBeamPar().AtomicMass;
   TString ion_name = parGeoG->GetBeamPar().Material;
   Float_t kinE_beam = parGeoG->GetBeamPar().Energy; //GeV/u
   
   TAGparaDsc* twGeo    = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* geomap   = (TATWparGeo*) twGeo->Object();
   parFileName = campManager->GetCurGeoFile(TATWparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGparaDsc* twCal   = new TAGparaDsc("twCal", new TATWparCal());
   TATWparCal* parCal   = (TATWparCal*) twCal->Object();
   parFileName = campManager->GetCurMapFile(TATWparGeo::GetBaseName(), runNumber);
   parCal->FromBarStatusFile(parFileName.Data());

   Bool_t isTof_calib = false;
   parFileName = campManager->GetCurCalFile(TATWparGeo::GetBaseName(), runNumber,
                                             isTof_calib,false);
   parCal->FromCalibFile(parFileName.Data(),isTof_calib,false);

   isTof_calib = true;
   parFileName = campManager->GetCurCalFile(TATWparGeo::GetBaseName(), runNumber,
                                             isTof_calib,false);
   parCal->FromCalibFile(parFileName.Data(),isTof_calib,false);

   parFileName = campManager->GetCurConfFile(TATWparGeo::GetBaseName(), runNumber,
                                              Form("%d%s", A_beam,ion_name.Data()),
                                              (int)(kinE_beam*TAGgeoTrafo::GevToMev()));
   parCal->FromFileZID(parFileName.Data(),Z_beam);
   
   TAGdataDsc* twRec = new TAGdataDsc("twPoint", new TATWntuPoint());
   TAGdataDsc* twRaw = new TAGdataDsc("twRaw", new TATWntuHit());
   
   twActRaw  = new TATWactNtuMC("twActRaw", twRaw, twCal, twGeo, myStr);
   twActRaw->CreateHistogram();

   twActRec  = new TATWactNtuPoint("twActRec", twRaw, twRec, twGeo, twCal);
   twActRec->CreateHistogram();

   
   //outFile->SetupElementBranch(twRaw, TATWntuHit::GetBranchName());
}

void ReadTwRawMC(TString name = "16O_C2H4_200_1.root", TString expName = "16O_200", Int_t runNumber = 1)
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
   
   FillMCTw(&evStr, runNumber);
   
   tagr.AddRequiredItem("twActRaw");
   tagr.AddRequiredItem("twActRec");
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_OutTw.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   twActRaw->SetHistogramDir(outFile->File());
   twActRec->SetHistogramDir(outFile->File());
   
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
      
      if (ientry == 5000)
//      if (ientry == 20000)
         break;
      
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


