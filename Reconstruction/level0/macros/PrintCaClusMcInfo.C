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
void PrintCaClusMcInfo(TString nameFile = "200_C_TG_TW_CALO_Out_25thr.root", Int_t nentries = 0)
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
   Int_t count_good_cluster=0;
   Int_t count_fg_cal_rec=0;  // total number of fragments reconstructed well
   Int_t count_fg_cal=0;      // total number of fragments from target to cal
   Int_t count_tot_fg=0;      // toal number of fragments
   for (Int_t ev = 0; ev < nentries; ++ev) {
      
      printf("### Event: %d\n", ev);
      caClus->Clear();
      caMc->Clear();
      eve->Clear();

      tree->GetEntry(ev);
      cout << "Numero di part: " << eve->GetTracksN() << " nell'evento: " << ev << endl;
      
      Int_t count_fg_ev=0; //number of fragments from target per event
      Int_t count_fg_cal_ev=0; //number of fragments from target to cal per event
      Int_t count_fg_cal_rec_ev=0; //number of fragments seen from target to cal per event
      Double_t z_mother_out=0.;

      //loop of MC truth
      for (int ii = 0; ii<eve->GetTracksN(); ii++){
         TAMCeveTrack* track = eve->GetTrack(ii);
         // printf("id: %d - charge %d - mass %g - motherID: %d - p_in: %f\n", ii, track->GetCharge(), track->GetMass(), track->GetMotherID(), track->GetInitP()[2]);
         TVector3 pos_in_track = track->GetInitPos();
         TVector3 pos_out_track = track->GetFinalPos();
         if (ii==0) z_mother_out = pos_out_track[2]; //mi salvo la z out della madre, per capire se ha interagito con il target
         // printf(" MC pos track in: (%.4f %.4f %.4f) |", pos_in_track[0], pos_in_track[1], pos_in_track[2]);
         // printf(" MC pos track out: (%.4f %.4f %.4f)\n", pos_out_track[0], pos_out_track[1], pos_out_track[2]);
         // printf(" -----\n");

         //Count how many fragments are produced in the target
         if (pos_in_track[2]<1. && pos_in_track[2]>-1.){
            count_tot_fg++;  //conta il numero TOTALE di particelle prodotte nel target
            count_fg_ev++;  //conta il numero di particelle prodotte nel target per ogni evento
         }

         //Count how many fragments arrive to the calorimeter
         if (pos_in_track[2]<1. && pos_in_track[2]>-1.){                //frammento nato nel target
            if(pos_out_track[2] > 101){                                 //frammento morto con una z > 101
               if (pos_out_track[0] > -22 && pos_out_track[0] < 22){    //frammento morto con  -22 < x < 22
                  if (pos_out_track[1] > -22 && pos_out_track[1] < 22){ //frammento morto con  -22 < y < 22
                     count_fg_cal++;
                     count_fg_cal_ev++;
                  }
               }
            }
         }

      } //end loop of MC truth

      if (z_mother_out<1 && z_mother_out>-1){
         
         Int_t nClus = caClus->GetClustersN();
         count_cluster+=nClus;
         printf(" nClusters %d\n", nClus);
         
         /////// loop over number of clusters
         for(Int_t i = 0; i < nClus; ++i){
            TACAcluster* clus = caClus->GetCluster(i);
            Int_t nHits = clus->GetHitsN(); //conta quanti Hit (= cry) ci sono in quel cluster
            hHitPerClus->Fill(nHits);
            printf(" ### CLUSTER: %d\n", i);
            printf("    Number of crystals hit: %d\n",nHits);
            
            Bool_t WrongCluster=kFALSE;
            // Double_t x_in_track[nHits];
            // Double_t y_in_track[nHits];
            // Double_t z_in_track[nHits];

            /////// loop over crystals hit
            for (Int_t j = 0; j < nHits; ++j){
               printf("  ## Hit: %d\n", j);
               TACAntuHit* hit = clus->GetHit(j);
               Int_t cry_id = hit->GetCrystalId();

               printf("    Number of tracks (particles): %d\n", hit->GetMcTracksN());
               printf("    ID Crystal hit: %d\n",cry_id);
               
               /////// loop over number of particle in that hit of that cluster
               for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
                  Int_t idx = hit->GetMcTrackIdx(k); // index of the tracks in the track branch
                  Int_t idx_ca = hit->GetMcIndex(k); // index of the tracks in the calo branch
                  printf("    Part ID %d ", idx);
                  TAMCeveTrack* track = eve->GetTrack(idx);
                  printf("charge %d - mass %g - motherID: %d", track->GetCharge(), track->GetMass(), track->GetMotherID());
                  TAMChit* mcHit = caMc->GetHit(idx_ca);
                  Double_t deltaE = mcHit->GetDeltaE();
                  printf(" - ∆E: %f GeV\n", deltaE);
                  TVector3 pos_in_ca = mcHit->GetInPosition();
                  TVector3 pos_out_ca = mcHit->GetOutPosition();
                  printf("    MC pos ca in: (%.4f %.4f %.4f) |", pos_in_ca[0], pos_in_ca[1], pos_in_ca[2]);
                  printf(" MC pos ca out: (%.4f %.4f %.4f)\n", pos_out_ca[0], pos_out_ca[1], pos_out_ca[2]);
                  TVector3 pos_in_track = track->GetInitPos();
                  TVector3 pos_out_track = track->GetFinalPos();
                  if (pos_in_track[2] < 101){
                     count_fg_cal_rec++;
                     count_fg_cal_rec_ev++;
                  }


                  // printf("    MC pos track in: (%.4f %.4f %.4f) |", pos_in_track[0], pos_in_track[1], pos_in_track[2]);
                  // printf(" MC pos track out: (%.4f %.4f %.4f)\n", pos_out_track[0], pos_out_track[1], pos_out_track[2]);
               }

               printf("    *** PARTICLE STORY *** \n");
               //loop to reconstruct the story of the particles
               for (Int_t i_track = hit->GetMcTracksN()-1; i_track>=0; i_track--){
                  Int_t idx_mot = 0;
                  Int_t idx_dg = hit->GetMcTrackIdx(i_track);
                  printf("    - Particle n. %d\n",i_track);
                  TAMCeveTrack* track_dg = 0x0;
                  TAMCeveTrack* track_mot = 0x0;
                  while (idx_dg > 0){
                     track_dg = eve->GetTrack(idx_dg);
                     printf("     Index of daugther: %d | charge: %d | mass: %g |", idx_dg, track_dg->GetCharge(), track_dg->GetMass());
                     TVector3 pos_in_dg = track_dg->GetInitPos();
                     TVector3 pos_out_dg = track_dg->GetFinalPos();
                     printf(" pos in: %f %f %f | ",pos_in_dg[0],pos_in_dg[1],pos_in_dg[2]);
                     printf(" pos out: %f %f %f\n",pos_out_dg[0],pos_out_dg[1],pos_out_dg[2]);
                     idx_mot = track_dg->GetMotherID();
                     track_mot = eve->GetTrack(idx_mot);
                     printf("     Index of mother: %d | charge: %d | mass: %g |", idx_mot, track_mot->GetCharge(), track_mot->GetMass());
                     TVector3 pos_in_mot = track_mot->GetInitPos();
                     TVector3 pos_out_mot = track_mot->GetFinalPos();
                     printf(" pos in: %f %f %f | ",pos_in_mot[0],pos_in_mot[1],pos_in_mot[2]);
                     printf(" pos out: %f %f %f\n",pos_out_mot[0],pos_out_mot[1],pos_out_mot[2]);
                     idx_dg = idx_mot;
                  }
               } // end loop story particle
            
            } //end loop hit

            // if (nHits>1 && count_fg_ev>1){// Seleziono i cluster con + di un cristallo colpito e con almeno due frammenti prodotti nel target
            if (nHits>1 && count_fg_cal_ev>1){ //|| count_fg_cal_ev > count_fg_cal_rec_ev)){
               wrong_events[count_bad_cluster] = ev;
               WrongCluster = kTRUE;                         
               count_bad_cluster++;
            }
         } //end loop cluster

      } // end if condition z<1 && z>-1
   } //end loop events

   cHitPerClus->cd();
   hHitPerClus->Draw("hist");
   count_good_cluster = count_cluster - count_bad_cluster;
   Double_t eff = double(count_good_cluster)/double(count_cluster);
   // Double_t pur = double(count_fg_cal-count_bad_cluster)/double(count_fg_cal);
   Double_t eff_fg = double(count_fg_cal_rec)/double(count_tot_fg);
   Double_t pur_fg = double(count_fg_cal_rec)/double(count_fg_cal);
   // for (Int_t i = 0; i<count_bad_cluster; i++) printf("ev: %d\n", wrong_events[i]);
   printf("N. of fragments produced: %d\n",count_tot_fg);
   printf("N. of fragments produced and arrived to the calorimeter: %d\n",count_fg_cal);
   printf("N. of fragments seen by the calorimeter: %d\n", count_fg_cal_rec);
   printf("N. of escaping fragments: %d\n", count_tot_fg-count_fg_cal);
   printf("Efficency of fragments reconstructed: %f\n", eff_fg);
   printf("Purity of fragments reconstructed: %f\n", pur_fg);
   printf("N. clusters: %d\n",count_cluster);
   printf("N. bad clusters: %d\n",count_bad_cluster);
   printf("Efficency of cluster: %f\n", eff);
   // printf("Number of BAD cluster in %d events: %d\n", nentries, count_bad_cluster);
   // printf("Number of GOOD cluster in %d events: %d\n", nentries, count_good_cluster);
   // printf("Efficency: %f  Purity: %f\n", eff, pur);
   // printf("Number of entries of histogram: %f\n",hHitPerClus->GetEntries());
   // printf("Number of events with nhits>1: %f\n",hHitPerClus->Integral(0,50));
}


