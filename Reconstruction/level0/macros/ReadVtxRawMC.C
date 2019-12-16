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
#include "TAVTparGeo.hxx"
#include "TAVTparMap.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"
#include "TAMCntuEve.hxx"

#include "TAMCactNtuEve.hxx"
#include "TAVTactNtuMC.hxx"
#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"
#include "TAVTactNtuVertex.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGactTreeWriter* outFile = 0x0;
TAVTactNtuMC* vtActRaw = 0x0;
TAVTactNtuClusterF* vtActClus = 0x0;
TAVTactNtuTrackF*    vtActTrck = 0x0;
TAVTactNtuVertex*    vtActVtx  = 0x0;

void FillMCVertex(EVENT_STRUCT *myStr) {
   
   /*Ntupling the MC Vertex information*/
   TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeoG = (TAGparGeo*)tgGeo->Object();
   parGeoG->FromFile();
   
   TAGparaDsc* vtGeo    = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   geomap->FromFile();
   
   TAGdataDsc* vtRaw    = new TAGdataDsc("vtRaw", new TAVTntuRaw());
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
   TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());
   TAGdataDsc* vtVtx    = new TAGdataDsc("vtVtx", new TAVTntuVertex());
   TAGdataDsc* eveMc    = new TAGdataDsc("eveMc", new TAMCntuEve());

   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parconf->FromFile("./config/TAVTdetector.cfg");

   TAVTparConf::SetHistoMap();
   TAMCactNtuEve* actNtuMcEve = new TAMCactNtuEve("eveActNtuMc", eveMc, myStr);

   vtActRaw  = new TAVTactNtuMC("vtActRaw", vtRaw, vtGeo, myStr);
   vtActRaw->CreateHistogram();
   
   vtActClus = new TAVTactNtuClusterF("vtActCluster", vtRaw, vtClus, vtConf, vtGeo);
   vtActClus->CreateHistogram();
   
   vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
   vtActTrck->CreateHistogram();
   
   vtActVtx = new TAVTactNtuVertex("vtActVtx", vtTrck, vtVtx, vtConf, vtGeo, tgGeo);
   vtActVtx->CreateHistogram();
   
   // outFile->SetupElementBranch(vtRaw, TAVTntuRaw::GetBranchName());
   // outFile->SetupElementBranch(vtClus, TAVTntuCluster::GetBranchName());
   outFile->SetupElementBranch(vtTrck, TAVTntuTrack::GetBranchName());
   outFile->SetupElementBranch(eveMc, TAMCntuEve::GetBranchName());

}

//void ReadVtxRawMC(TString name = "16O_C2H4_200_1.root")
//void ReadVtxRawMC(TString name = "p_80_vtx.root")
void ReadVtxRawMC(TString name = "ionEventoC_IR_100k.root")
//void ReadVtxRawMC(TString name = "4He_400_vtx_10k.root")
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
   
   FillMCVertex(&evStr);
   tagr.AddRequiredItem("eveActNtuMc");
   tagr.AddRequiredItem("vtActRaw");
   tagr.AddRequiredItem("vtActCluster");
   tagr.AddRequiredItem("vtActTrck");
   tagr.AddRequiredItem("vtActVtx");
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_Out.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   vtActRaw->SetHistogramDir(outFile->File());
   vtActClus->SetHistogramDir(outFile->File());
   vtActTrck->SetHistogramDir(outFile->File());
   vtActVtx->SetHistogramDir(outFile->File());
   
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
      if (ientry == 200000)
         break;
      
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


