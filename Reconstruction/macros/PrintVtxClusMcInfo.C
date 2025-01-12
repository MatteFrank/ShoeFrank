// Macro to read back MC info from reconstructed cluster of vertex
// Ch. Finck, Jan 19.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>

#include "TAMCntuPart.hxx"
#include "TAMCntuHit.hxx"

#include "TAITparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#endif


// main
void PrintVtxClusMcInfo(TString nameFile = "12C_400_vtx_Out.root", Int_t nentries = 2, Int_t plane = 0)
{
   TAGroot gTAGroot;
   
   TAGparaDsc* parGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
   TAVTparGeo* parGeo = (TAVTparGeo*)parGeoVtx->Object();
   parGeo->FromFile();
   Int_t vtSensorsN = parGeo->GetSensorsN();
   
   TAGparaDsc* parGeoIt = new TAGparaDsc(TAITparGeo::GetDefParaName(), new TAITparGeo());
   TAITparGeo* iparGeo = (TAITparGeo*)parGeoIt->Object();
   iparGeo->FromFile();
   Int_t itSensorsN = iparGeo->GetSensorsN();

   TAGparaDsc* parGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
   TAMSDparGeo* msdparGeo = (TAMSDparGeo*)parGeoMsd->Object();
   msdparGeo->FromFile();
   Int_t msSensorsN = msdparGeo->GetSensorsN();

   TAGparaDsc* parGeoTW = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* twparGeo = (TATWparGeo*)parGeoTW->Object();
   twparGeo->FromFile();
   
   TTree *tree = 0;
   TFile *f = new TFile(nameFile.Data());
   tree = (TTree*)f->Get("tree");
   
   TAVTntuCluster *vtClus = new TAVTntuCluster(vtSensorsN);
   vtClus->SetParGeo(parGeo);
   tree->SetBranchAddress(TAVTntuCluster::GetBranchName(), &vtClus);
   
   TAITntuCluster *itClus = new TAITntuCluster(itSensorsN);
   tree->SetBranchAddress(TAITntuCluster::GetBranchName(), &itClus);
   
   TAMSDntuCluster *msdClus = new TAMSDntuCluster(msSensorsN);
   tree->SetBranchAddress(TAMSDntuCluster::GetBranchName(), &msdClus);
   
   TAMCntuPart *eve = new TAMCntuPart();
   tree->SetBranchAddress(TAMCntuPart::GetBranchName(), &eve);
   
   TAMCntuHit *vtMc = new TAMCntuHit();
   tree->SetBranchAddress(TAMCntuHit::GetVtxBranchName(), &vtMc);
   
   if (nentries == 0)
      nentries = tree->GetEntriesFast();
   printf("nentries %d\n", nentries);
   
   printf("plane %d\n", plane);
   
   for (Int_t ev = 0; ev < nentries; ++ev) {
      
      printf("Event: %d\n", ev);
      vtClus->Clear();
      itClus->Clear();
      msdClus->Clear();
      
      vtMc->Clear();
      eve->Clear();
      
      tree->GetEntry(ev);
      
      Int_t nClus = vtClus->GetClustersN(plane);
      printf("nClusters %d\n", nClus);
      
      for(Int_t i = 0; i < nClus; ++i){
         printf("Clusters# %d\n", i);
         TAVTcluster* clus = vtClus->GetCluster(plane, i);
         Int_t nHits = clus->GetPixelsN();
         
         for (Int_t j = 0; j < nHits; ++j) {
            TAVThit* hit = clus->GetPixel(j);
            for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
               Int_t idx = hit->GetMcTrackIdx(k);
               printf("TrackMcId %d ", idx);
               TAMCpart* track = eve->GetTrack(idx);
               printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
               /*
                Int_t idx = hit->GetMcIndex(k);
                TAMChit* mcHit = vtMc->GetHit(idx);
                TVector3 pos = mcHit->GetPosition();
                printf("MC pos (%.4f %.4f %.4f)\n", pos[0], pos[1], pos[2]);
                */
            }
         }
      }
   }
}


