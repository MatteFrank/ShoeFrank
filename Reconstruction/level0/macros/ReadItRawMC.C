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

#include "TAITparGeo.hxx"
#include "TAITparMap.hxx"
#include "TAITparConf.hxx"
#include "TAITntuRaw.hxx"
#include "TAITntuCluster.hxx"
#include "TAITntuTrack.hxx"

#include "TAITactNtuMC.hxx"
#include "TAITactNtuClusterF.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGactTreeWriter* outFile = 0x0;
TAITactNtuMC* itActRaw = 0x0;
TAITactNtuClusterF* itActClus = 0x0;
TAITactNtuTrackF*    itActTrck = 0x0;

void FillMCInner(EVENT_STRUCT *myStr) {
   
   /*Ntupling the MC Vertex information*/
   TAGparaDsc* itGeo    = new TAGparaDsc(TAITparGeo::GetItDefParaName(), new TAITparGeo());
   TAITparGeo* geomap   = (TAITparGeo*) itGeo->Object();
   geomap->FromFile();
   
   TAGdataDsc* itRaw    = new TAGdataDsc("itRaw", new TAITntuRaw());
   TAGdataDsc* itClus   = new TAGdataDsc("itClus", new TAITntuCluster());
   
   TAGparaDsc*  itConf  = new TAGparaDsc("itConf", new TAITparConf());
   TAITparConf* parconf = (TAITparConf*) itConf->Object();
   parconf->FromFile("./config/TAITdetector.cfg");
   
   TAITparConf::SetHistoMap();
   itActRaw  = new TAITactNtuMC("itActRaw", itRaw, itGeo, myStr);
   itActRaw->CreateHistogram();
   
   
   itActClus = new TAITactNtuClusterF("itActCluster", itRaw, itClus, itConf, itGeo);
   itActClus->CreateHistogram();
   
//   itActTrck = new TAITactNtuTrackF("itActTrck", itClus, itTrck, itConf, itGeo);
//   itActTrck->CreateHistogram();
//   
   
   // outFile->SetupElementBranch(itRaw, TAITntuRaw::GetBranchName());
  //outFile->SetupElementBranch(itClus, TAITntuCluster::GetBranchName());
   // outFile->SetupElementBranch(itTrck, TAITntuTrack::GetBranchName());
   
}

//void ReadItRawMC(TString name = "16O_C2H4_200_1.root")
//void ReadItRawMC(TString name = "p_80_vtx.root")
//void ReadItRawMC(TString name = "12C_80_vtx.root")
void ReadItRawMC(TString name = "12C_400_vtx.root")
{
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
   
   FillMCInner(&evStr);
   
   tagr.AddRequiredItem("itActRaw");
   tagr.AddRequiredItem("itActCluster");
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_Out.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   itActRaw->SetHistogramDir(outFile->File());
   itActClus->SetHistogramDir(outFile->File());
   
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
      
      if (ientry == 500)
//      if (ientry == 200000)
         break;
      
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();

   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


