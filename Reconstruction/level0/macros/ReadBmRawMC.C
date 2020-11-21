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
#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparConf.hxx"
#include "TABMdatRaw.hxx"
#include "TABMntuRaw.hxx"

#include "TABMactNtuMC.hxx"
#include "TABMactNtuTrack.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGactTreeWriter* outFile   = 0x0;
TABMactNtuMC* bmActRaw      = 0x0;
TABMactNtuTrack* bmActTrack = 0x0;

void FillMCMsd(EVENT_STRUCT *myStr) {
   
   /*Ntupling the MC Vertex information*/
   TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
   parGeo->FromFile();
   
   TAGparaDsc* bmGeo    = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
   TABMparGeo* geomap   = (TABMparGeo*) bmGeo->Object();
   TString  parFileName = "./geomaps/TABMdetector.geo";  
   geomap->FromFile(parFileName.Data());

   TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
   TABMparConf* parConf = (TABMparConf*)bmConf->Object();
   parFileName = "./config/TABMdetector.cfg";
   parConf->FromFile(parFileName.Data());
   
   TAGdataDsc* bmRaw    = new TAGdataDsc("bmRaw", new TABMntuRaw());
   bmActRaw  = new TABMactNtuMC("bmActRaw", bmRaw, bmConf, bmGeo, myStr);
   bmActRaw->CreateHistogram();
   
   TAGdataDsc* bmTrack = new TAGdataDsc("bmTrack", new TABMntuTrack());
   bmActTrack  = new TABMactNtuTrack("bmActTrack", bmTrack, bmRaw, bmGeo, bmConf, tgGeo);
   bmActTrack->CreateHistogram();

   outFile->SetupElementBranch(bmRaw, TABMntuRaw::GetBranchName());
   outFile->SetupElementBranch(bmTrack, TABMntuTrack::GetBranchName());
}

void ReadBmRawMC(TString name = "./data/flukasim/footC200_C2H4.root")
//void ReadMsdRawMC(TString name = "p_80_vtx.root")
//void ReadMsdRawMC(TString name = "12C_80_vtx.root")
//void ReadMsdRawMC(TString name = "12C_400_vtx.root")
{  Long64_t nev=1000;
  
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();

   TAGroot tagr;
   TAGgeoTrafo geoTrafo;
   geoTrafo.FromFile();

   tagr.SetCampaignNumber(-1);
   tagr.SetRunNumber(1);
   
   
   TFile* f = new TFile(name.Data());
   f->ls();
   
   TTree* tree = (TTree*)gDirectory->Get("EventTree");
   
   Evento *ev  = new Evento();
   
   EVENT_STRUCT evStr;
   
   ev->FindBranches(tree,&evStr);
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillMCMsd(&evStr);
   
   tagr.AddRequiredItem("bmActRaw");
   tagr.AddRequiredItem("bmActTrack");
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t posend = name.Last('.');
   Int_t posinit = name.Last('/');
   TString nameOut = name;
   nameOut.Replace(posend,5,"_Out.root");
   if(posinit)
     nameOut.Remove(0,posinit+1);
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   bmActRaw->SetHistogramDir(outFile->File());
   bmActTrack->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Long64_t nentries = tree->GetEntries();
   
   Long64_t nbytes = 0, nb = 0, maxevents=min(nentries, nev);
   cout<<"Running against ntuple with:: "<<nentries<<" entries, maxevents="<<maxevents<<endl;
   
   for (Long64_t ientry = 0; ientry < maxevents; ientry++) {
      
      nb = tree->GetEntry(ientry);
      nbytes += nb;
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
            
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
   cout<<"job done, the output file is: "<<nameOut.Data()<<endl;
}


