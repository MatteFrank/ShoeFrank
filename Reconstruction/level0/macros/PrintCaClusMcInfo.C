// Macro to read back MC info from reconstructed cluster of calorimeter
// L. Scavarda, Oct 20.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
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


// main
void PrintCaClusMcInfo(TString nameFile = "200_C_TG_TW_CALO_1e5_Out_0thr_3.root", Int_t nentries = 0)
{
   TAGroot gTAGroot;

   TCanvas *cHitPerClus = new TCanvas("cHitPerClus", "cHitPerClus");
   TH1F* hEnDepPerHit = new TH1F("hEnDepPerHit", "Energy deposition per Cry", 1000, 0, 5);
   TH1F* hHitPerClus = new TH1F("hHitPerClus", "Crystals Hit per Cluster", 50, 0, 50);
   
   TAGparaDsc* parGeoTW = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* twparGeo = (TATWparGeo*)parGeoTW->Object();
   twparGeo->FromFile();

   TAGparaDsc* parGeoCa = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
   TACAparGeo* caparGeo = (TACAparGeo*)parGeoCa->Object();
   caparGeo->FromFile();
   
   TTree *tree = 0;
   TFile *f = new TFile(nameFile.Data());
   tree = (TTree*)f->Get("tree");
   
   TACAntuCluster *caClus = new TACAntuCluster();
   tree->SetBranchAddress(TACAntuCluster::GetBranchName(), &caClus);
   
   TAMCntuEve *eve = new TAMCntuEve();
   tree->SetBranchAddress(TAMCntuEve::GetBranchName(), &eve);
   tree->SetBranchAddress("mctrack.", &eve);
   
   TAMCntuHit *caMc = new TAMCntuHit();
   tree->SetBranchAddress(TAMCntuHit::GetCalBranchName(), &caMc);
   tree->SetBranchAddress("mcca.", &caMc);
   
   if (nentries == 0)
      nentries = tree->GetEntries();
   printf("nentries %d\n", nentries);
   
   Int_t wrong_events[10000];
   Int_t count_cluster=0;
   Int_t count_bad_cluster=0;
   Int_t count_bad_cluster_1=0;
   Int_t count_bad_cluster_2=0;
   Int_t count_good_cluster=0;
   Int_t count_fg_cal_det=0;  // total number of fragments detected by the Calo
   Int_t count_fg_cal_arr=0;  // total number of fragments arrived at the Calo
   Int_t count_tot_fg=0;      // toal number of fragments
   Int_t count_protons=0;      // toal number of protons
   Int_t count_heavy_ions=0;      // toal number of heavy ions
   Int_t count_protons_cal=0;      // toal number of protons arrived at the Calo
   Int_t count_heavy_ions_cal=0;      // toal number of heavy ions arrived at the Calo
   

   for (Int_t ev = 0; ev < nentries; ++ev) {
      
      // printf("### Event: %d\n", ev);
      if (ev % 1000 == 0) printf("### Event: %d\n", ev);
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
         // printf(" id: %d - charge %d - mass %g - motherID: %d - p_in: %f\n", ii, track->GetCharge(), track->GetMass(), track->GetMotherID(), track->GetInitP()[2]);
         TVector3 pos_in_track = track->GetInitPos();
         TVector3 pos_out_track = track->GetFinalPos();
         if (ii==0){
           z_mother_out = pos_out_track[2]; //mi salvo la z out della madre, per capire se ha interagito con il target
           z_mother_in = pos_in_track[2]; //mi salvo la z out della madre, per capire se ha interagito con il target 
         } 

         //Count how many fragments arrive to the calorimeter
         if (track->GetRegion() == 14){                                 //frammento nato nel target
            count_tot_fg++;                                             //conta il numero TOTALE di particelle prodotte nel target
            // Calo                                             
            if(pos_out_track[2] > 101){                                 //frammento morto con una z > 101
               if (pos_out_track[0] > -30 && pos_out_track[0] < 30){    //frammento morto con  -30 < x < 30
                  if (pos_out_track[1] > -30 && pos_out_track[1] < 30){ //frammento morto con  -30 < y < 30
                     count_fg_cal_arr++;
                     // printf("    -----> Add particle to count!\n");
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
         
         Int_t nParTot =0.;

         /////// loop over number of clusters
         for(Int_t i = 0; i < nClus; ++i){
            TACAcluster* clus = caClus->GetCluster(i);
            Float_t clus_en = clus->GetCharge();          //energy released in the cluster
            Float_t hit_en = 0;                           //energy released in the crystals
            Bool_t isGoodClus = kFALSE;
            Int_t count_fg_cal_rec_clus=0;                //number of fragments seen, from target to cal per clus
            
            Int_t nHits = clus->GetHitsN();               //number of crystals hit in that cluster
            hHitPerClus->Fill(nHits);
            // printf(" ### CLUSTER: %d\n", i);
            // printf("    Number of crystals hit: %d\n", nHits);
            /////// loop over crystals hit
            // Float_t e_kin = 0.;

            Int_t idx_arr[nHits];
            for (Int_t j = 0; j < nHits; ++j){
               // printf("  ## Hit: %d\n", j);
               TACAntuHit* hit = clus->GetHit(j);
               Int_t cry_id = hit->GetCrystalId();
               Float_t enDep = hit->GetCharge();
               hit_en += hit->GetCharge();

               // printf("    ID Crystal hit: %d\n",cry_id);
               // printf("    Energy Released in the cry: %f MeV\n",enDep);
               // printf("    Number of tracks (particles): %d\n", hit->GetMcTracksN());
               nParTot += hit->GetMcTracksN();
               Int_t nTracks = hit->GetMcTracksN();
               Bool_t isGoodFrag=kFALSE;
               
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
                  
                  Float_t e_kin = TMath::Sqrt( p_init[2]*p_init[2] + track->GetMass()*track->GetMass() ) - track->GetMass();
                  // printf("     E_kin: %.f MeV\n", e_kin*1000);
                  
                  if (track->GetRegion() == 14){
                     idx_arr[j] = idx;
                     if (j==0){
                        count_fg_cal_det++;
                        // printf("    -----> Add particle to count!\n");
                     }
                     if (j>0){
                        for (Int_t ix=0; ix<j; ix++){
                           if (idx_arr[j] != idx_arr[ix]) isGoodFrag = kTRUE;
                        }
                     }
                     if (isGoodFrag==kTRUE){
                        count_fg_cal_det++;
                        // printf("    -----> Add particle to count!\n");
                     }
                     // if (track->GetCharge() == 1) count_protons_cal++;
                     // if (track->GetCharge() > 1) count_heavy_ions_cal++;
                     count_fg_cal_rec_clus++;
                     isGoodClus=kTRUE;
                  }
               }

               // printf("    *** PARTICLE STORY *** \n");
               //loop to reconstruct the story of the particles
               for (Int_t i_track = hit->GetMcTracksN()-1; i_track>=0; i_track--){
                  Int_t idx_mot = 0;
                  Int_t idx_dg = hit->GetMcTrackIdx(i_track);
                  // printf("    ### %d)\n",i_track);
                  TAMCeveTrack* track_dg = 0x0;
                  TAMCeveTrack* track_mot = 0x0;
                  while (idx_dg > 0){
                     track_dg = eve->GetTrack(idx_dg);
                     // printf("     Index of daugther: %d | charge: %d | mass: %g |", idx_dg, track_dg->GetCharge(), track_dg->GetMass());
                     TVector3 pos_in_dg = track_dg->GetInitPos();
                     TVector3 pos_out_dg = track_dg->GetFinalPos();
                     // printf(" pos in: %f %f %f | ",pos_in_dg[0],pos_in_dg[1],pos_in_dg[2]);
                     // printf(" pos out: %f %f %f\n",pos_out_dg[0],pos_out_dg[1],pos_out_dg[2]);
                     idx_mot = track_dg->GetMotherID();
                     track_mot = eve->GetTrack(idx_mot);
                     // printf("     Index of mother: %d | charge: %d | mass: %g |", idx_mot, track_mot->GetCharge(), track_mot->GetMass());
                     TVector3 pos_in_mot = track_mot->GetInitPos();
                     TVector3 pos_out_mot = track_mot->GetFinalPos();
                     // printf(" pos in: %f %f %f | ",pos_in_mot[0],pos_in_mot[1],pos_in_mot[2]);
                     // printf(" pos out: %f %f %f\n",pos_out_mot[0],pos_out_mot[1],pos_out_mot[2]);
                     idx_dg = idx_mot;
                  }
               } // end loop story particle
            
            } //end loop hit

            
            // Seleziono i cluster con + di un cristallo colpito e con almeno due frammenti prodotti nel target
            if (count_fg_cal_rec_clus>1 || isGoodClus==kFALSE){ 
               if (count_fg_cal_rec_clus>1) count_bad_cluster_1++;
               if (isGoodClus==kFALSE) count_bad_cluster_2++;
               wrong_events[count_bad_cluster] = ev;
               count_bad_cluster++;
            }
            
            // printf("    Cluster Energy:: %f  Hit Energy:: %f  Fragments Energy:: %f\n",clus_en,hit_en,e_kin*1000);
            // printf("    Cluster Energy:: %f  Hit Energy:: %f\n",clus_en,hit_en);

         } //end loop cluster
      } // end if condition z<1 && z>-1
   } //end loop events

   cHitPerClus->cd();
   hHitPerClus->Draw("hist");
   count_good_cluster = count_cluster - count_bad_cluster;
   Double_t pur = double(count_good_cluster)/double(count_cluster);
   Double_t eff = double(count_good_cluster)/double(count_fg_cal_arr);
   // for (Int_t i = 0; i<count_bad_cluster; i++) printf("ev: %d\n", wrong_events[i]);
   printf("N. of fragments produced: %d\n",count_tot_fg);
   printf("N. of protons produced:: %d  N. of protons lost:: %d\n",count_protons, count_protons-count_protons_cal);
   printf("N. of heavy ions produced:: %d  N. of heavy ions lost:: %d\n",count_heavy_ions, count_heavy_ions-count_heavy_ions_cal);
   printf("N. of primary fragments arrived at the calorimeter: %d\n", count_fg_cal_arr);
   printf("N. of primary fragments detected at the calorimeter: %d\n", count_fg_cal_det);
   printf("N. clusters: %d\n",count_cluster);
   printf("N. bad clusters: %d\n",count_bad_cluster);
   printf("N. bad clusters from 2 fragments: %d\n",count_bad_cluster_1);
   printf("N. bad clusters from ghost particles: %d\n",count_bad_cluster_2);
   printf("N. good clusters: %d\n",count_good_cluster);
   printf("Efficiency of fragments reconstructed: %f\n", eff);
   printf("Purity of cluster: %f\n", pur);
  
}


