// Macro to read back MC info from reconstructed cluster of calorimeter
// L. Scavarda, Oct 20.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <set> 
#include <iostream> 
#include <iterator> 
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>

#include "TAMCntuEve.hxx"
#include "TAMCntuHit.hxx"

#include "TACAparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TAVTntuRaw.hxx"
#include "TACAntuRaw.hxx"
#include "TACAntuCluster.hxx"
#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#endif

#define MAX_ZNUMBER 7

TString thr = "3thr";

// main
void PrintCaClusMcInfo_v4(TString nameFile = Form("200_C_TG_TW_CALO_1e5_Out_%s.root",thr.Data()), Int_t nentries = 0)
{
   
   printf("ANALISI CON SOGLIA: %s\n",thr.Data());

   TAGroot gTAGroot;

   set <int, greater <int> >::iterator itr; 

   TCanvas *cHitPerClus = new TCanvas("cHitPerClus", "cHitPerClus");
   TH1F* hEnDepPerHit = new TH1F("hEnDepPerHit", "Energy deposition per Cry", 1000, 0, 5);
   TH1F* hHitPerClus = new TH1F("hHitPerClus", "Crystals Hit per Cluster", 50, 0, 50);
   TH1F* hChargeIon_D[MAX_ZNUMBER];
   TH1F* hChargeIon_N[MAX_ZNUMBER];

   for (int c=0; c<MAX_ZNUMBER; c++){
      hChargeIon_D[c] = new TH1F(Form("hChargeIon_D_%d",c), Form("Z: %d", c), 250, 0, 2.5);
      hChargeIon_N[c] = new TH1F(Form("hChargeIon_N_%d",c), Form("Z: %d", c), 250, 0, 2.5);
   } 
   
   TAGparaDsc* parGeoTW = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* twparGeo = (TATWparGeo*)parGeoTW->Object();
   twparGeo->FromFile();

   TAGparaDsc* parGeoCa = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
   TACAparGeo* caparGeo = (TACAparGeo*)parGeoCa->Object();
   caparGeo->FromFile();
   
   TTree *tree = 0;
   TFile *f = new TFile(nameFile.Data());
   tree = (TTree*)f->Get("tree");
   
   // TATWntuCluster *twClus = new TATWntuCluster();
   // tree->SetBranchAddress(TATWntuCluster::GetBranchName(), &twClus);

   // TAMCntuHit *twMc = new TAMCntuHit();
   // tree->SetBranchAddress(TAMCntuHit::GetTofBranchName(), &twMc);
   // tree->SetBranchAddress("mctw.", &twMc);

   TACAntuCluster *caClus = new TACAntuCluster();
   tree->SetBranchAddress(TACAntuCluster::GetBranchName(), &caClus);
   
   TAMCntuHit *caMc = new TAMCntuHit();
   tree->SetBranchAddress(TAMCntuHit::GetCalBranchName(), &caMc);
   tree->SetBranchAddress("mcca.", &caMc);
   
   TAMCntuEve *eve = new TAMCntuEve();
   tree->SetBranchAddress(TAMCntuEve::GetBranchName(), &eve);
   tree->SetBranchAddress("mctrack.", &eve);
   
   if (nentries == 0)
      nentries = tree->GetEntries();
   printf("nentries %d\n", nentries);
   Int_t wrong_events[10000];
   Int_t count_cluster=0;

   for (Int_t ev = 0; ev < nentries; ++ev) {
      
      // printf("### Event: %d\n", ev);
      if (ev % 1000 == 0) printf("### Event: %d\n", ev);
      
      // twClus->Clear();
      // twMc->Clear();
      caClus->Clear();
      caMc->Clear();
      eve->Clear();

      tree->GetEntry(ev);
      // cout << "Numero di part: " << eve->GetTracksN() << " nell'evento: " << ev << endl;
      
      Double_t z_mother_out=0.;
      Double_t z_mother_in=0.;


      //loop of MC truth
      for (int ii = 0; ii<eve->GetTracksN(); ii++){
         TAMCeveTrack* track = eve->GetTrack(ii);
         TVector3 pos_in_track = track->GetInitPos();
         TVector3 pos_out_track = track->GetFinalPos();
         if (ii==0){
           z_mother_out = pos_out_track[2]; //mi salvo la z out della madre, per capire se ha interagito con il target
           z_mother_in = pos_in_track[2];   //mi salvo la z out della madre, per capire se ha interagito con il target 
         } 

         Double_t p = sqrt( track->GetInitP()[0]*track->GetInitP()[0]  + 
                         track->GetInitP()[1]*track->GetInitP()[1]  +
                         track->GetInitP()[2]*track->GetInitP()[2]  );
         Double_t ek = sqrt( p*p + track->GetMass()*track->GetMass() ) - track->GetMass();

         //Count how many fragments arrive to the calorimeter
         if (track->GetRegion() == 14){                                 //frammento nato nel target
            // Calo                                             
            if(pos_out_track[2] > 101){                                 //frammento morto con una z > 101
               if (pos_out_track[0] > -30 && pos_out_track[0] < 30){    //frammento morto con  -30 < x < 30
                  if (pos_out_track[1] > -30 && pos_out_track[1] < 30){ //frammento morto con  -30 < y < 30
                     if (track->GetCharge() < 7.){
                        hChargeIon_D[track->GetCharge()]->Fill(ek);
                        // printf("MC truth - id: %d - charge %d - mass %g - e_kin: %f\n", ii, track->GetCharge(), track->GetMass(), ek);
                     }
                  }
               }
            }
         }
      } //end loop of MC truth


      //Calorimeter Analysis
      if (z_mother_out<1 && z_mother_out>-1){
         
         Int_t nClus = caClus->GetClustersN();
         count_cluster+=nClus;

         // printf(" nClusters %d\n", nClus);
         set<int, greater <int>> idx_fg_clus;

         /////// loop over number of clusters
         for(Int_t i = 0; i < nClus; ++i){
            TACAcluster* clus = caClus->GetCluster(i);
            Float_t clus_en = clus->GetCharge();          //energy released in the cluster
            Float_t hit_en = 0;                           //energy released in the crystals
            
            Int_t nHits = clus->GetHitsN();               //number of crystals hit in that cluster
            hHitPerClus->Fill(nHits);

            // printf(" ### CLUSTER: %d\n", i);
            // printf("    Number of crystals hit: %d\n", nHits);
            
            /////// loop over crystals hit
            for (Int_t j = 0; j < nHits; ++j){
               // printf("  ## HIT: %d\n", j);
               TACAntuHit* hit = clus->GetHit(j);
               Int_t cry_id = hit->GetCrystalId();
               Float_t enDep = hit->GetCharge();
               hit_en += hit->GetCharge();

               // printf("    ID Crystal hit: %d\n",cry_id);
               // printf("    Energy Released in the cry: %f MeV\n",enDep);
               // printf("    Number of tracks (particles): %d\n", hit->GetMcTracksN());
         
               Int_t nTracks = hit->GetMcTracksN();
               ///////// loop over number of particle in that hit of that cluster
               for (Int_t k = 0; k < nTracks; ++k) {
                  Int_t idx = hit->GetMcTrackIdx(k); // index of the tracks in the track branch
                  Int_t idx_ca = hit->GetMcIndex(k); // index of the tracks in the calo branch
                  // printf("    %d) Part ID %d ", k, idx);
                  TAMCeveTrack* track = eve->GetTrack(idx);
                  // printf("charge %d - mass %g - motherID: %d", track->GetCharge(), track->GetMass(), track->GetMotherID());
                  TAMChit* mcHit = caMc->GetHit(idx_ca);
                  Double_t deltaE = mcHit->GetDeltaE();
                  // printf(" - ∆E: %f MeV\n", deltaE*1000);
                  TVector3 pos_in_ca = mcHit->GetInPosition();
                  TVector3 pos_out_ca = mcHit->GetOutPosition();
                  // printf("     MC pos ca in: (%.4f %.4f %.4f) |", pos_in_ca[0], pos_in_ca[1], pos_in_ca[2]);
                  // printf(" MC pos ca out: (%.4f %.4f %.4f)\n", pos_out_ca[0], pos_out_ca[1], pos_out_ca[2]);
                  TVector3 pos_in_track = track->GetInitPos();
                  TVector3 pos_out_track = track->GetFinalPos();
                  
                  TVector3 p_init = track->GetInitP();
                  TVector3 p_fin = track->GetFinalP();
                  
                  // printf("     MC pos track in: (%.4f %.4f %.4f) |", pos_in_track[0], pos_in_track[1], pos_in_track[2]);
                  // printf(" MC pos track out: (%.4f %.4f %.4f)\n", pos_out_track[0], pos_out_track[1], pos_out_track[2]);
                  
                  Double_t p = sqrt( p_init[0]*p_init[0] + p_init[1]*p_init[1] +p_init[2]*p_init[2]  );
                  Double_t ek = sqrt( p*p + track->GetMass()*track->GetMass() ) - track->GetMass();
                  // printf("     E_kin: %f GeV\n", ek);
                  
                  //se la particella nata nel target
                  if (track->GetRegion() == 14 && 
                     ( (pos_out_track[0]<30. && pos_out_track[0]>-30.) && 
                        (pos_out_track[1]<30. && pos_out_track[1]>-30.) )){

                     idx_fg_clus.insert(idx);

                  } //end if particelle nate nel target

               }//end loop tracce
            
            } //end loop hit
            
         } //end loop cluster

         for(itr = idx_fg_clus.begin(); itr!= idx_fg_clus.end(); ++itr){
            Int_t idxxx = *itr;
            TAMCeveTrack* track = eve->GetTrack(idxxx);
            Double_t p = sqrt( track->GetInitP()[0]*track->GetInitP()[0] + track->GetInitP()[1]*track->GetInitP()[1] + track->GetInitP()[2]*track->GetInitP()[2]);
            Double_t ek =sqrt(p*p + track->GetMass()*track->GetMass()) - track->GetMass();
            if (track->GetCharge()<7.) hChargeIon_N[track->GetCharge()]->Fill(ek);
         }

      } // end if condition z<1 && z>-1

   } //end loop events

   printf("\n-------- OUTPUT ---------\n");
   printf("p arrived: %f\n", hChargeIon_D[1]->GetEntries());
   printf("He arrived: %f\n", hChargeIon_D[2]->GetEntries());
   printf("Li arrived: %f\n", hChargeIon_D[3]->GetEntries());
   printf("Be arrived: %f\n", hChargeIon_D[4]->GetEntries());
   printf("B arrived: %f\n", hChargeIon_D[5]->GetEntries());
   printf("C arrived: %f\n", hChargeIon_D[6]->GetEntries());
   printf("N arrived: %f\n", hChargeIon_D[0]->GetEntries());

   printf("p seen by Calo: %f\n", hChargeIon_N[1]->GetEntries());
   printf("He seen by Calo: %f\n", hChargeIon_N[2]->GetEntries());
   printf("Li seen by Calo: %f\n", hChargeIon_N[3]->GetEntries());
   printf("Be seen by Calo: %f\n", hChargeIon_N[4]->GetEntries());
   printf("B seen by Calo: %f\n", hChargeIon_N[5]->GetEntries());
   printf("C seen by Calo: %f\n", hChargeIon_N[6]->GetEntries());
   printf("N seen by Calo: %f\n", hChargeIon_N[0]->GetEntries());

   cHitPerClus->cd();
   hHitPerClus->Draw("hist");

   TFile* file_out = new TFile(Form("histo_ions_%s_prova.root",thr.Data()), "RECREATE");
   file_out->cd();
   for (int z=0; z<MAX_ZNUMBER; z++){
      hChargeIon_D[z]->Write();
      hChargeIon_N[z]->Write();
   }
   file_out->Write();
   file_out->Close();
  
}


