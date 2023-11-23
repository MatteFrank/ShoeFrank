#define GlobalRecoAna_cxx
#include <TVector3.h>
#include <TRandom.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include <TMultiGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include "BaseReco.hxx"
#include "TAVTtrack.hxx"
#include "TAMCntuPart.hxx"
#include "RecoRaw.hxx"
#include "RecoMC.hxx"

#include "GlobalRecoAnaGSI.hxx"

using namespace std;

GlobalRecoAnaGSI::GlobalRecoAnaGSI(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, Int_t innTotEv) : RecoRaw(expName, runNumber, fileNameIn, fileNameout)
{
  nchargeok_vt = 0;
  nchargeok_vt_clu = 0;
  TrkIdMC_TW = 0;
  nchargeall_it_clu = 0;
  nchargeall_msd = 0;
  nchargeall_msd_clu = 0;
  nchargeall_tw = 0;

  npure_vt = 0;
  npure_it = 0;
  npure_msd = 0;
  npure = 0;
  nclean = 0;
  ntracks = 0;
  recoEvents = 0;
  fFlagMC = isMC;
  purity_cut = 0.51;
  clean_cut = 1.;
  nTotEv = innTotEv;

  outfile = fileNameout;

  Th_true = -999.;   // from TAMCparticle
  Th_meas = -999.;   // from TWpoint
  Th_reco = -999.;   // from global tracking
  Th_recoBM = -999.; // from global tracking wrt BM direction
  Th_BM = -999;      // angle wrt to track before impining on tg ( mc version of th_recoBM)
}

GlobalRecoAnaGSI::~GlobalRecoAnaGSI()
{
}

void GlobalRecoAnaGSI::LoopEvent()
{

  if (FootDebugLevel(1))
    cout << "GlboalRecoAna::LoopEvent start" << endl;
  if (fSkipEventsN > 0)
  {
    currEvent = fSkipEventsN;
    nTotEv += fSkipEventsN;
  }
  else
    currEvent = 0;

  Int_t frequency = 1;
  if (nTotEv >= 100000)
    frequency = 10000;
  else if (nTotEv >= 10000)
    frequency = 1000;
  else if (nTotEv >= 1000)
    frequency = 100;
  else if (nTotEv >= 100)
    frequency = 10;
  else if (nTotEv >= 10)
    frequency = 1;

  //*********************************************************************************** begin loop on every event **********************************************
  while (gTAGroot->NextEvent())
  { // for every event
    // fFlagMC = false;     //N.B.: for MC FAKE REAL

    if (currEvent == nTotEv)
      break;

    if (currEvent % frequency == 0 || FootDebugLevel(1))
      cout << "Load Event: " << currEvent << endl;

    int evtcutstatus = ApplyEvtCuts();
    if (evtcutstatus)
    {
      ++currEvent;
      continue;
    }

    Int_t nt = myGlb->GetTracksN(); // number of reconstructed tracks for every event
    if (nt > 0)
       recoEvents++;    //! modifica

    // TAWDntuTrigger *wdTrig = 0x0;
    // if (fFlagMC == false)
    // {
    //   wdTrig = (TAWDntuTrigger *)fpNtuWDtrigInfo->GenerateObject(); // trigger from hardware
    // }

    m_nClone.clear();
    
    //*********************************************************************************** begin loop on global tracks **********************************************
    bool moreTWpoints = CheckTwPointInMoreTracks();
    //if ) cout << "event " << currEvent << " has more tw points in tracks " << endl;
    
    for (int it = 0; it < nt; it++)
    {
      fGlbTrack = myGlb->GetTrack(it);
      if (!fGlbTrack->HasTwPoint())
      {
        ntracks++;
        continue;
      }
      // Set the reconstructed quantities
      RecoGlbTrkLoopSetVariables();

      if (fFlagMC)
      { 
        
        MCGlbTrkLoopSetVariables();
        

        // study of MC clones: different glb tracks with the same MC ID
        m_nClone[Z_true][TrkIdMC]++;
        if (m_nClone[Z_true][TrkIdMC] > 1)
          n_clones[Z_true] = m_nClone[Z_true][TrkIdMC];


        // ===================TWMATCH 
        TAMCpart *particle = myMcNtuPart->GetTrack(TrkIdMC);
        if ((std::find(vecTwTrkId.begin(), vecTwTrkId.end(), TrkIdMC) != vecTwTrkId.end()) // if main track id in twpoint id
            // && (particle->GetInitPos().Z() < 40.6 ||        particle->GetInitPos().Z() > 41))     // a hard coded way to take out fragm from 1 layer of msd...
            // if (TrkIdMC == TrkIdMC_TW)
        )
        {

          if (isGoodReco(TrkIdMC))
          {
            if (Z_true > 0. && Z_true <= fPrimaryCharge)
            {
              FillYieldMC("yield-N_TWGoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
            }
          }
          if (Z_true > 0. && Z_true <= fPrimaryCharge)
            FillYieldMC("yield-N_TWAllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

          // if (Z_true > 0. && Z_true <= fPrimaryCharge)
          MigMatrixPlots("MigMatrixTW", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

          if (Z_true > 0. && Z_true <= fPrimaryCharge)
          {
            FillYieldReco("yield-N_TW_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
          }
          if (Z_true > 0. && Z_true <= fPrimaryCharge)
          {
            FillYieldReco("yield-N_TW_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
          }

          if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
          {
            FillYieldReco("yield-N_TW_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
          }

          if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
          {
            FillYieldReco("yield-N_TW_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
          }

          // if (initTWPosition< 40){
          //   FillYieldReco("yield-N_TWCutZ_reco_Th_Reco", Z_meas, Th_recoBM);
          //   FillYieldReco("yield-N_TWCutZ_true_Th_Reco", Z_true, Th_recoBM);
          // }

          ChargeStudies("Z8_TWmatch", 8, fGlbTrack);


        }
        else
        ChargeStudies("Z8_TW_noTWmatch", 8, fGlbTrack);

        // // ===================VT MATCH
        bool VTMatch = true;
        bool VTZ8Match = true;

        for (int i = 0; i < vecVtZMC.size(); i++)
        {
          if (std::find(vecVtZMC.at(i).begin(), vecVtZMC.at(i).end(), 8) != vecVtZMC.at(i).end()) // if the cluster of the VTX contains Z=8
            VTZ8Match = VTZ8Match && true; 
          else
            VTZ8Match = VTZ8Match && false;
        }

        if (VTZ8Match == true) {   // if a primary entered the first layer of the msd and did not fragmented up to its last plane...
          if (Z_meas > 7)    // ... and the charge reconstructed in TW is higher than 7
            VTMatch = true;  // --> it means there is no fragmentation between VTX and up to TW
          else
            VTMatch = false;
        }

        if ((std::find(vecVtZMC.at(0).begin(), vecVtZMC.at(0).end(), 8) != vecVtZMC.at(0).end()) && (VTZ8Match == false)) 
        // if the first cluster was a Z=8 but then some fragmentation happened...
        {
          VTMatch = false;
        }

        if (VTMatch == true)
        {

          if (isGoodReco(TrkIdMC))
          {
            if (Z_true > 0. && Z_true <= fPrimaryCharge)
            {
              FillYieldMC("yield-N_VTGoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
            }
          }
          if (Z_true > 0. && Z_true <= fPrimaryCharge)
            FillYieldMC("yield-N_VTAllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

          // if (Z_true > 0. && Z_true <= fPrimaryCharge)
          MigMatrixPlots("MigMatrix_VT", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

          if (Z_true > 0. && Z_true <= fPrimaryCharge)
          {
            FillYieldReco("yield-N_VT_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
          }
          if (Z_true > 0. && Z_true <= fPrimaryCharge)
          {
            FillYieldReco("yield-N_VT_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
          }

          if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
          {
            FillYieldReco("yield-N_VT_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
          }

          if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
          {
            FillYieldReco("yield-N_VT_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
          }

          // if (initTWPosition< 40){
          //   FillYieldReco("yield-N_TWCutZ_reco_Th_Reco", Z_meas, Th_recoBM);
          //   FillYieldReco("yield-N_TWCutZ_true_Th_Reco", Z_true, Th_recoBM);
          // }

          ChargeStudies("Z8_VTmatch", 8, fGlbTrack);
        }
        else
          ChargeStudies("Z8_VT_nomatch", 8, fGlbTrack);

        // // ===================MSD MATCH
        bool MSDMatch = true;
        bool MsdZ8Match = true;

        for (int i = 0; i < vecMsdZMC.size(); i++)
        {
          if (std::find(vecMsdZMC.at(i).begin(), vecMsdZMC.at(i).end(), 8) != vecMsdZMC.at(i).end()) // if the cluster of the MSD contains Z=8
            MsdZ8Match = MsdZ8Match && true; 
          else
            MsdZ8Match = MsdZ8Match && false;
        }

        if (MsdZ8Match == true ){  // if a primary entered the first layer of the msd and did not fragmented up to its last plane...
            if (Z_meas > 7)   // ... and the charge reconstructed in TW is higher than 7
              MSDMatch = true; // --> it means there is no fragmentation between VTX and up to TW
            else
              MSDMatch = false;
          }

          if ((std::find(vecMsdZMC.at(0).begin(), vecMsdZMC.at(0).end(), 8) != vecMsdZMC.at(0).end()) && (MsdZ8Match == false)) // if the first cluster was a Z=8 but then some fragmentation happened...
          {
            MSDMatch = false;
          }

            if (MSDMatch == true)
            {

              if (isGoodReco(TrkIdMC))
              {
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldMC("yield-N_MSDGoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                }
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
                FillYieldMC("yield-N_MSDAllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

              // if (Z_true > 0. && Z_true <= fPrimaryCharge)
              MigMatrixPlots("MigMatrixMSD", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

              if (Z_true > 0. && Z_true <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_MSD_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_MSD_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_MSD_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
              {
                FillYieldReco("yield-N_MSD_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
              }

              // if (initTWPosition< 40){
              //   FillYieldReco("yield-N_TWCutZ_reco_Th_Reco", Z_meas, Th_recoBM);
              //   FillYieldReco("yield-N_TWCutZ_true_Th_Reco", Z_true, Th_recoBM);
              // }

              ChargeStudies("Z8_MSDmatch", 8, fGlbTrack);
            }
            else
              ChargeStudies("Z8_MSD_nomatch", 8, fGlbTrack);



            // // =================== VTX + MSD  MATCH

            if (MSDMatch == true && VTMatch == true)
            {

              if (isGoodReco(TrkIdMC))
              {
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldMC("yield-N_VTX_MSDGoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                }
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
                FillYieldMC("yield-N_VTX_MSDAllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

              // if (Z_true > 0. && Z_true <= fPrimaryCharge)
              MigMatrixPlots("MigMatrixVTX_MSD", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

              if (Z_true > 0. && Z_true <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_VTX_MSD_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_VTX_MSD_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_VTX_MSD_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
              {
                FillYieldReco("yield-N_VTX_MSD_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
              }

              // if (initTWPosition< 40){
              //   FillYieldReco("yield-N_TWCutZ_reco_Th_Reco", Z_meas, Th_recoBM);
              //   FillYieldReco("yield-N_TWCutZ_true_Th_Reco", Z_true, Th_recoBM);
              // }

              ChargeStudies("Z8_VTX_MSDmatch", 8, fGlbTrack);
            }
            else
              ChargeStudies("Z8_VTX_MSD_nomatch", 8, fGlbTrack);

           
            // // ===================ALL TRACKS with TRACK QUALITY = 1
            if (fGlbTrack->GetQuality() == 1)
            {
              if (isGoodReco(TrkIdMC))
              {
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldMC("yield-N_Q1GoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                }
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
                FillYieldMC("yield-N_Q1AllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

              // if (Z_true > 0. && Z_true <= fPrimaryCharge)
              MigMatrixPlots("MigMatrixQ1", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
              {
                FillYieldReco("yield-NQ1_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
              {
                FillYieldReco("yield-NQ1_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
              {
                FillYieldReco("yield-NQ1_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
              {
                FillYieldReco("yield-NQ1_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with real theta (for purity purposes)
              }
              ChargeStudies("Z8_Q1all", 8, fGlbTrack);
            }

            //// =================== RECO CHI2 cut

            float res = 0;
            float res_temp = 0;
            for (int i = 0; i < fGlbTrack->GetPointsN(); i++){
              auto point = fGlbTrack->GetPoint(i);
              if ((string)point->GetDevName() == "MSD")
              {
                if (point->GetSensorIdx()%2 == 0)  // if it is the sensor x of the msd 
                  res_temp = point->GetMeasPosition().X() - point->GetFitPosition().X();
                else
                  res_temp = point->GetMeasPosition().Y() - point->GetFitPosition().Y();
              }

              if ((string)point->GetDevName() == "VT")
              {
                res_temp = (point->GetMeasPosition() - point->GetFitPosition()).Mag();
              }
              if (res_temp > res) res=res_temp;
            }

            if (fGlbTrack->GetChi2() < 2 && res < 0.01)
            {
              if (isGoodReco(TrkIdMC))
              {
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldMC("yield-N_Chi2GoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                }
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
                FillYieldMC("yield-N_Chi2AllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

              // if (Z_true > 0. && Z_true <= fPrimaryCharge)
              MigMatrixPlots("MigMatrixChi2", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_Chi2_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
              }
              if (Z_true > 0. && Z_true <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_Chi2_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
              {
                FillYieldReco("yield-N_Chi2_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
              }

              if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
              {
                FillYieldReco("yield-N_Chi2_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with real theta (for purity purposes)
              }
              ChargeStudies("Z8_Chi2all", 8, fGlbTrack);
              }

              //// =================== RECO CHI2 cut + multitrack

              if (fGlbTrack->GetChi2() < 2 && res < 0.01 && nt>1)
              {
                if (isGoodReco(TrkIdMC))
                {
                  if (Z_true > 0. && Z_true <= fPrimaryCharge)
                  {
                    FillYieldMC("yield-N_MChi2GoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                  }
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                  FillYieldMC("yield-N_MChi2AllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

                // if (Z_true > 0. && Z_true <= fPrimaryCharge)
                MigMatrixPlots("MigMatrixMChi2", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MChi2_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MChi2_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MChi2_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
                {
                  FillYieldReco("yield-N_MChi2_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with real theta (for purity purposes)
                }
                ChargeStudies("Z8_MChi2all", 8, fGlbTrack);
              }

              // // =================== MC VTX + Chi2 cuts

              if (VTMatch == true && fGlbTrack->GetChi2() < 2 && res < 0.01)
              {

                if (isGoodReco(TrkIdMC))
                {
                  if (Z_true > 0. && Z_true <= fPrimaryCharge)
                  {
                    FillYieldMC("yield-N_VTChi2GoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                  }
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                  FillYieldMC("yield-N_VTChi2AllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

                // if (Z_true > 0. && Z_true <= fPrimaryCharge)
                MigMatrixPlots("MigMatrixVTChi2", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_VTChi2_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_VTChi2_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_VTChi2_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
                {
                  FillYieldReco("yield-N_VTChi2_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
                }

                // if (initTWPosition< 40){
                //   FillYieldReco("yield-N_TWCutZ_reco_Th_Reco", Z_meas, Th_recoBM);
                //   FillYieldReco("yield-N_TWCutZ_true_Th_Reco", Z_true, Th_recoBM);
                // }

                ChargeStudies("Z8_VTChi2_match", 8, fGlbTrack);
              }
              else
                ChargeStudies("Z8_VTChi2_nomatch", 8, fGlbTrack);

              // // =================== MC VTX + Chi2 cuts + multitrack

              if (VTMatch == true && fGlbTrack->GetChi2() < 2 && res < 0.01 && nt > 1)
              {

                if (isGoodReco(TrkIdMC))
                {
                  if (Z_true > 0. && Z_true <= fPrimaryCharge )
                  {
                    FillYieldMC("yield-N_MVTChi2GoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                  }
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                  FillYieldMC("yield-N_MVTChi2AllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

                // if (Z_true > 0. && Z_true <= fPrimaryCharge)
                MigMatrixPlots("MigMatrixMVTChi2", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MVTChi2_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MVTChi2_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MVTChi2_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
                {
                  FillYieldReco("yield-N_MVTChi2_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
                }

                // if (initTWPosition< 40){
                //   FillYieldReco("yield-N_TWCutZ_reco_Th_Reco", Z_meas, Th_recoBM);
                //   FillYieldReco("yield-N_TWCutZ_true_Th_Reco", Z_true, Th_recoBM);
                // }

                ChargeStudies("Z8_MVTChi2_match", 8, fGlbTrack);
              }
              else
                ChargeStudies("Z8_MVTChi2_nomatch", 8, fGlbTrack);








 // // =================== Chi2 cuts + multitrack + NO TW in multitracks

              if (fGlbTrack->GetChi2() < 2 && res < 0.01 && nt > 1 && (moreTWpoints == false ))
              {

                if (isGoodReco(TrkIdMC))
                {
                  if (Z_true > 0. && Z_true <= fPrimaryCharge )
                  {
                    FillYieldMC("yield-N_MChi2TWTGoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                  }
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                  FillYieldMC("yield-N_MChi2TWTAllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

                // if (Z_true > 0. && Z_true <= fPrimaryCharge)
                MigMatrixPlots("MigMatrixMChi2TWT", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MChi2TWT_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
                }
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MChi2TWT_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
                {
                  FillYieldReco("yield-N_MChi2TWT_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
                }

                if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
                {
                  FillYieldReco("yield-N_MChi2TWT_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
                }

                // if (initTWPosition< 40){
                //   FillYieldReco("yield-N_TWCutZ_reco_Th_Reco", Z_meas, Th_recoBM);
                //   FillYieldReco("yield-N_TWCutZ_true_Th_Reco", Z_true, Th_recoBM);
                // }

                ChargeStudies("Z8_MChi2TWT_match", 8, fGlbTrack);
              }
              else
                ChargeStudies("Z8_MChi2TWT_nomatch", 8, fGlbTrack);















              //// ===================ALL TRACKS
              ChargeStudies("Z8_all", 8, fGlbTrack);

              if (isGoodReco(TrkIdMC))
              {
                if (Z_true > 0. && Z_true <= fPrimaryCharge)
                {
                  FillYieldMC("yield-N_GoodReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_GoodReco MC
                }
            }
            if (Z_true > 0. && Z_true <= fPrimaryCharge)
              FillYieldMC("yield-N_AllReco", Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC

            // if (Z_true > 0. && Z_true <= fPrimaryCharge)
            MigMatrixPlots("MigMatrix", Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots

            if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
            {
              FillYieldReco("yield-N_Z_reco_Th_Reco", Z_meas, Th_recoBM); // all reconstructed tracks
            }
            if (Z_true > 0. && Z_true <= fPrimaryCharge)
            {
              FillYieldReco("yield-N_Z_true_Th_Reco", Z_true, Th_recoBM); // all reconstructed tracks but with real Z
            }

            if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
            {
              FillYieldReco("yield-N_Z_meas_Th_True", Z_meas, Th_BM); // all reconstructed tracks but with real theta
            }

            if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
            {
              FillYieldReco("yield-N_Z_measEqualTrue_Th_True", Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with real theta (for purity purposes)
            }
          }

      ntracks++;

    } //********* end loop on global tracks ****************

    //cout << "TWPoint ID in the event: "<< endl;
    
    //   for (int i = 0; i < vectTwId.size(); i++)
    //   {
    //     cout << vectTwId.at(i) << " ";
    //   }
    // cout << endl;
    

    // for (  Int_t id : vectTwId ) ++m_twId[id];
    // for (auto const& e : m_twId)
    // {
    //   if (e.second >1){
    //     cout << "event: " << currEvent << endl;
    //     cout << e.first << " : " << e.second << std::endl;
    //   }
    // }


        if (fFlagMC)
    {
      // MCParticleStudies();
      //***** loop on every TAMCparticle:
      FillMCPartYields(); // N_ref
    }

    ++currEvent;
  } // end of loop event

  return;
}

void GlobalRecoAnaGSI::Booking()
{
  // binning of theta, ekin, A
  th_nbin = 20;
  theta_binning = new double *[th_nbin];
  for (int i = 0; i < th_nbin; i++)
  {
    theta_binning[i] = new double[2];
    theta_binning[i][0] = double(i) * 0.6;
    theta_binning[i][1] = double(i + 1) * 0.6;
  }

  ek_nbin = 1;
  ek_binning = new double *[ek_nbin];
  for (int i = 0; i < ek_nbin; i++)
  {
    ek_binning[i] = new double[2];
    ek_binning[i][0] = double(i * 100);
    ek_binning[i][1] = double((i + 1) * 20000);
  }
  // ek_binning[4] = new double [2];
  // ek_binning[4][0] = double(400);
  // ek_binning[4][1] =  double(1000);

  mass_nbin = 1;
  mass_binning = new double *[mass_nbin];
  for (int i = 0; i < mass_nbin; i++)
  {
    mass_binning[i] = new double[2];
    mass_binning[i][0] = double(i * 2);
    mass_binning[i][1] = double((i + 1) * 100);
  }

  // Cross section recostruction histos MC
  if (fFlagMC)
  {

    BookYield("yield-N_ref", false);

    //no cuts
    BookYield("yield-N_GoodReco", true);
    BookYield("yield-N_AllReco", true);
    BookYield("yield-N_Z_reco_Th_Reco", false);
    BookYield("yield-N_Z_true_Th_Reco", false);
    BookYield("yield-N_Z_meas_Th_True", false);
    BookYield("yield-N_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrix", true);
    BookChargeStudies("Z8_all");

    //tw mc cuts
    BookYield("yield-N_TW_Z_reco_Th_Reco", false);
    BookYield("yield-N_TW_Z_true_Th_Reco", false);
    BookYield("yield-N_TWGoodReco", true);
    BookYield("yield-N_TWAllReco", true);
    BookYield("yield-N_TW_Z_meas_Th_True", false);
    BookYield("yield-N_TW_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixTW", true);
    BookChargeStudies("Z8_TWmatch");
    BookChargeStudies("Z8_TW_noTWmatch");

    //vt mc cuts
    BookYield("yield-N_VT_Z_reco_Th_Reco", false);
    BookYield("yield-N_VT_Z_true_Th_Reco", false);
    BookYield("yield-N_VTGoodReco", true);
    BookYield("yield-N_VTAllReco", true);
    BookYield("yield-N_VT_Z_meas_Th_True", false);
    BookYield("yield-N_VT_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrix_VT", true);
    BookChargeStudies("Z8_VTmatch");
    BookChargeStudies("Z8_VT_nomatch");

    // MSD mc cuts
    BookYield("yield-N_MSD_Z_reco_Th_Reco", false);
    BookYield("yield-N_MSD_Z_true_Th_Reco", false);
    BookYield("yield-N_MSDGoodReco", true);
    BookYield("yield-N_MSDAllReco", true);
    BookYield("yield-N_MSD_Z_meas_Th_True", false);
    BookYield("yield-N_MSD_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixMSD", true);
    BookChargeStudies("Z8_MSDmatch");
    BookChargeStudies("Z8_MSD_nomatch");

    // VT + MSD mc cuts

    BookYield("yield-N_VTX_MSD_Z_reco_Th_Reco", false);
    BookYield("yield-N_VTX_MSD_Z_true_Th_Reco", false);
    BookYield("yield-N_VTX_MSDGoodReco", true);
    BookYield("yield-N_VTX_MSDAllReco", true);
    BookYield("yield-N_VTX_MSD_Z_meas_Th_True", false);
    BookYield("yield-N_VTX_MSD_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixVTX_MSD", true);
    BookChargeStudies("Z8_VTX_MSDmatch");
    BookChargeStudies("Z8_VTX_MSD_nomatch");

    //track quality 1
    BookYield("yield-NQ1_Z_reco_Th_Reco", false);
    BookYield("yield-NQ1_Z_true_Th_Reco", false);
    BookYield("yield-N_Q1GoodReco", true);
    BookYield("yield-N_Q1AllReco", true);
    BookYield("yield-NQ1_Z_meas_Th_True", false);
    BookYield("yield-NQ1_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixQ1", true);
    BookChargeStudies("Z8_Q1all");

    // chi2 cuts on track
    BookYield("yield-N_Chi2_Z_reco_Th_Reco", false);
    BookYield("yield-N_Chi2_Z_true_Th_Reco", false);
    BookYield("yield-N_Chi2GoodReco", true);
    BookYield("yield-N_Chi2AllReco", true);
    BookYield("yield-N_Chi2_Z_meas_Th_True", false);
    BookYield("yield-N_Chi2_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixChi2", true);
    BookChargeStudies("Z8_Chi2all");


    // chi2 cuts on track + multitrack
    BookYield("yield-N_MChi2_Z_reco_Th_Reco", false);
    BookYield("yield-N_MChi2_Z_true_Th_Reco", false);
    BookYield("yield-N_MChi2GoodReco", true);
    BookYield("yield-N_MChi2AllReco", true);
    BookYield("yield-N_MChi2_Z_meas_Th_True", false);
    BookYield("yield-N_MChi2_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixMChi2", true);
    BookChargeStudies("Z8_MChi2all");


    // VT +  chi2 cuts on track
    BookYield("yield-N_VTChi2_Z_reco_Th_Reco", false);
    BookYield("yield-N_VTChi2_Z_true_Th_Reco", false);
    BookYield("yield-N_VTChi2GoodReco", true);
    BookYield("yield-N_VTChi2AllReco", true);
    BookYield("yield-N_VTChi2_Z_meas_Th_True", false);
    BookYield("yield-N_VTChi2_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixVTChi2", true);
    BookChargeStudies("Z8_VTChi2_match");
    BookChargeStudies("Z8_VTChi2_nomatch");

    // VT +  chi2 + multitrack cuts on track
    BookYield("yield-N_MVTChi2_Z_reco_Th_Reco", false);
    BookYield("yield-N_MVTChi2_Z_true_Th_Reco", false);
    BookYield("yield-N_MVTChi2GoodReco", true);
    BookYield("yield-N_MVTChi2AllReco", true);
    BookYield("yield-N_MVTChi2_Z_meas_Th_True", false);
    BookYield("yield-N_MVTChi2_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixMVTChi2", true);
    BookChargeStudies("Z8_MVTChi2_match");
    BookChargeStudies("Z8_MVTChi2_nomatch");


    // chi2 + multitrack cuts on track  + no more tw points in tracks
    BookYield("yield-N_MChi2TWT_Z_reco_Th_Reco", false);
    BookYield("yield-N_MChi2TWT_Z_true_Th_Reco", false);
    BookYield("yield-N_MChi2TWTGoodReco", true);
    BookYield("yield-N_MChi2TWTAllReco", true);
    BookYield("yield-N_MChi2TWT_Z_meas_Th_True", false);
    BookYield("yield-N_MChi2TWT_Z_measEqualTrue_Th_True", false);
    BookMigMatrix("MigMatrixMChi2TWT", true);
    BookChargeStudies("Z8_MChi2TWT_match");
    BookChargeStudies("Z8_MChi2TWT_nomatch");

  }
  else
  {

    // Cross section recostruction histos from REAL DATA
    BookYield("yield-trkREAL");
  }
  return;
}

int GlobalRecoAnaGSI::ApplyEvtCuts() // up to now, check if there is only one track in BM
{
  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::ApplyEvtCuts start" << endl;

  if (myBMNtuTrk->GetTracksN() != 1)
    return 1;

  return 0;
}

void GlobalRecoAnaGSI::FillMCGlbTrkYields()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::FillMCGlbTrkYields start" << endl;

  //-------------------------------------------------------------
  //--Yield for CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA
  if (Z_true > 0. && Z_true <= fPrimaryCharge /*&& TriggerCheckMC() == true*/)
    FillYieldReco("yield-trkMC", Z_true, Th_true);

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA + ALLTW FIX : i don't want not fragmented primary

  // check del TAGpoint del TW
  for (int ic = 0; ic < fGlbTrack->GetPointsN(); ic++)
  { // from all the points of the track...
    TAGpoint *tmp_poi = fGlbTrack->GetPoint(ic);
    TString str = tmp_poi->GetDevName();
    if (str.Contains(FootBaseName("TATWparGeo")))
    { //...i just want the TAGPOINT of TW
      if (tmp_poi->GetMcTracksN() == 1)
      { // if there is no ghost hits in the TW
        if (tmp_poi->GetMcTrackIdx(0) == fGlbTrack->GetMcMainTrackId())
        { // if there is match of MCId
          if (Z_true > 0. && Z_true <= fPrimaryCharge /*&& TriggerCheckMC() == true*/)
          {
            FillYieldReco("yield-trkcutsMC", Z_true, Th_true);
            //((TH1D *)gDirectory->Get("ThReco_fragMC"))->Fill(Th_recoBM);
          }
        }
      }
    }
  }

  // //-------------------------------------------------------------
  // //--CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA + GHOST HITS FIX : i don't want not fragmented primary
  // if (N_TrkIdMC_TW == 1)
  //   if (Z_true >0. && Z_true <= fPrimaryCharge /*&& TriggerCheckMC() == true*/)
  //     FillYieldReco("yield-trkGHfixMC",Z_true,Z_meas,Th_true );

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation for trigger efficiency   (comparing triggercheck with TAWDntuTrigger )
  if (Z_meas > 0. && Z_meas <= fPrimaryCharge /*&& TriggerCheck() == true*/)
    FillYieldReco("yield-trkReco", Z_meas, Th_recoBM);

  //-------------------------------------------------------------
  //--CROSS SECTION reco
  if (Z_meas > 0. && Z_meas <= fPrimaryCharge /*&& TriggerCheck() == true*/)
  {

    TString name = GetTitle();
    Int_t pos = name.Last('.');
    string name_ = "yield-trkREAL";
    name_ += name[pos - 1];
    FillYieldReco(name_.c_str(), Z_meas, Th_recoBM);
  }

  return;
}

void GlobalRecoAnaGSI::FillDataGlbTrkYields()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::FillDataGlbTrkYields start" << endl;

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation- RECO PARAMETERS FROM REAL DATA : i don't want not fragmented primary
  if (Z_meas > 0. && Z_meas <= fPrimaryCharge /*&& wdTrig -> GetTriggersStatus()[1] == 1*/) // fragmentation hardware trigger ON
  //&& TriggerCheck(fGlbTrack) == true  //NB.: for MC FAKE REAL
  {
    // cout << "inside " <<endl;
    FillYieldReco("yield-trkREAL", Z_meas, Th_recoBM);
    // cout << "thBM: "<< Th_recoBM <<endl
    //  ((TH1D*)gDirectory->Get("ThReco_frag"))->Fill(Th_recoBM);
    //  ((TH1D*)gDirectory->Get("Charge_trk_frag"))->Fill(Z_meas);
  }
  return;
}

void GlobalRecoAnaGSI::SetupTree()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::SetupTree start" << endl;

  myReader = new TAGactTreeReader("myReader");
  fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
  gTAGroot->AddRequiredItem("glbTrack");
  myReader->SetupBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());

  if (TAGrecoManager::GetPar()->IncludeBM())
  {
    fpNtuTrackBm = new TAGdataDsc("bmtrack", new TABMntuTrack());
    gTAGroot->AddRequiredItem("bmtrack");
    myReader->SetupBranch(fpNtuTrackBm);
  }

  if (TAGrecoManager::GetPar()->IncludeVT())
  {
    fpNtuClusVtx = new TAGdataDsc("vtclus", new TAVTntuCluster());
    // fpNtuTrackVtx = new TAGdataDsc("vttrack", new TAVTntuTrack());
    fpNtuVtx = new TAGdataDsc("vtvtx", new TAVTntuVertex());
    gTAGroot->AddRequiredItem("vtclus");
    // gTAGroot->AddRequiredItem("vttrack");
    gTAGroot->AddRequiredItem("vtvtx");
    myReader->SetupBranch(fpNtuClusVtx);
    // myReader->SetupBranch(fpNtuTrackVtx);
    myReader->SetupBranch(fpNtuVtx);
    // if (fFlagMC)
    // {
    //   fpNtuMcVt = new TAGdataDsc(FootDataDscMcName(kVTX), new TAMCntuHit());
    //   gTAGroot->AddRequiredItem("mcvt");
    //   myReader->SetupBranch(fpNtuMcVt, FootBranchMcName(kVTX));
    // }
  }
  // if (TAGrecoManager::GetPar()->IncludeIT())
  // {
  //   fpNtuClusIt = new TAGdataDsc(new TAITntuCluster());
  //   gTAGroot->AddRequiredItem("itclus");
  //   myReader->SetupBranch(fpNtuClusIt);
  //   if (fFlagMC)
  //   {
  //     fpNtuMcIt = new TAGdataDsc(FootDataDscMcName(kITR), new TAMCntuHit());
  //     gTAGroot->AddRequiredItem("mcit");
  //     myReader->SetupBranch(fpNtuMcIt, FootBranchMcName(kITR));
  //   }
  // }
  // if (TAGrecoManager::GetPar()->IncludeMSD())
  // {
  //   fpNtuClusMsd = new TAGdataDsc(new TAMSDntuCluster());
  //   gTAGroot->AddRequiredItem("msdclus");
  //   myReader->SetupBranch(fpNtuClusMsd);
  //   fpNtuRecMsd = new TAGdataDsc(new TAMSDntuPoint());
  //   gTAGroot->AddRequiredItem("msdpoint");
  //   myReader->SetupBranch(fpNtuRecMsd);
  //   if (fFlagMC)
  //   {
  //     fpNtuMcMsd = new TAGdataDsc(FootDataDscMcName(kMSD), new TAMCntuHit());
  //     gTAGroot->AddRequiredItem("mcmsd");
  //     myReader->SetupBranch(fpNtuMcMsd, FootBranchMcName(kMSD));
  //   }
  // }
  if (TAGrecoManager::GetPar()->IncludeTW())
  {
    // fpNtuRecTw = new TAGdataDsc(new TATWntuPoint());
    // gTAGroot->AddRequiredItem("twpt");
    // myReader->SetupBranch(fpNtuRecTw);

    if ( !fFlagMC )
    {
    //   fpNtuWDtrigInfo = new TAGdataDsc(new TAWDntuTrigger());
    //   gTAGroot->AddRequiredItem("WDtrigInfo");
    //   myReader->SetupBranch(fpNtuWDtrigInfo);
    }
    // if (fFlagMC)
    // {
    //   fpNtuMcTw = new TAGdataDsc(FootDataDscMcName(kTW), new TAMCntuHit());
    //   gTAGroot->AddRequiredItem("mctw");
    //   myReader->SetupBranch(fpNtuMcTw, FootBranchMcName(kTW));
    // }
  }
  if (TAGrecoManager::GetPar()->IncludeCA())
  {
    // fpNtuClusCa = new TAGdataDsc(new TACAntuCluster());
    // gTAGroot->AddRequiredItem("caclus");
    // myReader->SetupBranch(fpNtuClusCa);
    // if (fFlagMC)
    // {
    //   fpNtuMcCa = new TAGdataDsc(FootDataDscMcName(kCAL), new TAMCntuHit());
    //   gTAGroot->AddRequiredItem("mcca");
    //   myReader->SetupBranch(fpNtuMcCa, FootBranchMcName(kCAL));
    // }
  }

  if (fFlagMC)
  {
    fpNtuMcEvt = new TAGdataDsc(new TAMCntuEvent());
    fpNtuMcTrk = new TAGdataDsc(new TAMCntuPart());
    gTAGroot->AddRequiredItem("mcevt");
    gTAGroot->AddRequiredItem("mctrack");
    myReader->SetupBranch(fpNtuMcEvt);
    myReader->SetupBranch(fpNtuMcTrk);
    if (TAGrecoManager::GetPar()->IsRegionMc())
    {
      fpNtuMcReg = new TAGdataDsc(new TAMCntuRegion());
      gTAGroot->AddRequiredItem("mcreg");
      myReader->SetupBranch(fpNtuMcReg);
    }
  }

  gTAGroot->AddRequiredItem("myReader");
  gTAGroot->Print();

  return;
}

void GlobalRecoAnaGSI::BeforeEventLoop()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::BeforeEventLoop start" << endl;

  ReadParFiles();
  CampaignChecks();
  SetupTree();
  cout << "fSkipEventsN :" << fSkipEventsN << endl;
  myReader->Open(GetName(), "READ", "tree");
  if (fSkipEventsN > 0)
  {
    myReader->Reset(fSkipEventsN);
  }

  file_out = new TFile(GetTitle(), "RECREATE");

  cout << "Going to create " << GetTitle() << " outfile " << endl;
  //  SetRunNumber(runNb); //serve veramente?
  //  myReader->GetTree()->Print();

  // initialization of several objects needed for the analysis
  gTAGroot->BeginEventLoop();
  mass_ana = new GlobalRecoMassAna();
  myGlb = (TAGntuGlbTrack *)fpNtuGlbTrack->GenerateObject();
  // myVtNtuClus = (TAVTntuCluster*)fpNtuClusVtx->GenerateObject();
  myVtNtuVtx = (TAVTntuVertex *)fpNtuVtx->GenerateObject();

  // myTWNtuPt = (TATWntuPoint *)fpNtuRecTw->GenerateObject();
  // myMSDNtuHit = (TAMSDntuHit *)fpNtuRecTw->GenerateObject();
  // pCaNtuClu = (TACAntuCluster *)fpNtuClusCa->GenerateObject();
  myBMNtuTrk = (TABMntuTrack *)fpNtuTrackBm->GenerateObject();

  if (fFlagMC)
  {
    myMcNtuEvent = (TAMCntuEvent *)fpNtuMcEvt->GenerateObject();
    myMcNtuPart = (TAMCntuPart *)fpNtuMcTrk->GenerateObject();
  }
  // else
  // {
  //   wdTrig = (TAWDntuTrigger *)fpNtuWDtrigInfo->GenerateObject();
  // }

  // set variables
  fPrimaryCharge = GetParGeoG()->GetBeamPar().AtomicNumber;
  Double_t beam_mass_number = GetParGeoG()->GetBeamPar().AtomicMass * TAGgeoTrafo::GetMassFactorMeV();                                                              // primary mass number in mev
  Double_t beam_energy = GetParGeoG()->GetBeamPar().Energy * GetParGeoG()->GetBeamPar().AtomicMass * TAGgeoTrafo::GevToMev();                                       // Total kinetic energy (MeV)
  Double_t beam_speed = sqrt(beam_energy * beam_energy + 2. * beam_mass_number * beam_energy) / (beam_mass_number + beam_energy) * TAGgeoTrafo::GetLightVelocity(); // cm/ns
  primary_tof = (fpFootGeo->GetTGCenter().Z() - fpFootGeo->GetSTCenter().Z()) / beam_speed;                                                                         // ns
  Booking();
  pure_track_xcha.clear();
  pure_track_xcha.resize(fPrimaryCharge + 1, std::make_pair(0, 0));
  Ntg = GetParGeoG()->GetTargetPar().Density * TMath::Na() * GetParGeoG()->GetTargetPar().Size.Z() / GetParGeoG()->GetTargetPar().AtomicMass;

  if (FootDebugLevel(1))
  {
    cout << "fPrimaryCharge=" << fPrimaryCharge << "  beam_mass_number=" << beam_mass_number << "  beam_energy=" << beam_energy << "  beam_speed=" << beam_speed << "  primary_tof=" << primary_tof << endl;
    cout << "N_target=" << Ntg << endl;
    cout << "target density=" << GetParGeoG()->GetTargetPar().Density << endl;
    cout << "target z=" << GetParGeoG()->GetTargetPar().Size.Z() << endl;
    cout << "target A=" << GetParGeoG()->GetTargetPar().AtomicMass << endl;
  }

  //Set fixed region values
  fRegTG = GetParGeoG()->GetRegTarget();
  TString regvtxname("VTXP3");
  fRegLastVTplane = GetParGeoG()->GetCrossReg(regvtxname);
  TString regname("AIR1");
  fRegAirAfterVT = GetParGeoG()->GetCrossReg(regname);
  fRegFirstTWbar = GetParGeoTw()->GetRegStrip(0, 0);
  fRegFirstTWbarLay0 = GetParGeoTw()->GetRegStrip(1, 0);
  fRegLastTWbar = GetParGeoTw()->GetRegStrip(1, GetParGeoTw()->GetNBars() - 1);
  fRegFirstCAcrys = GetParGeoCa()->GetRegCrystal(0);
  fRegLastCAcrys = GetParGeoCa()->GetRegCrystal(GetParGeoCa()->GetCrystalsN() - 1);
  fRegAirBeforeTW = GetParGeoG()->GetRegAirPreTW();
  fRegAirAfterTW = GetParGeoG()->GetRegAirTW();

  n_clones.clear();
  return;
}

void GlobalRecoAnaGSI::AfterEventLoop()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::AfterEventLoop start" << endl;

  // stamp luminosity
  string luminosity_name = "";
  if (fFlagMC == true)
  {
    luminosity_name = "luminosityMC";
  }
  else
  { // real data
    luminosity_name = "luminosityREAL";
  }
  h = new TH1D(luminosity_name.c_str(), "", 1, 0., 1.);
  ((TH1D *)gDirectory->Get(luminosity_name.c_str()))->SetBinContent(1, Ntg * recoEvents);
  cout << "Reconstructed events: " << recoEvents << endl;

  if (fFlagMC)
  {
    h = new TH1D("trkclone", "number of tracks with the same mc id", 8, 0.5, 8.5);
    for (int i = 1; i <= fPrimaryCharge; i++)
    {
      ((TH1D *)gDirectory->Get("trkclone"))->SetBinContent(i, n_clones[i]);
    }
  }

  gTAGroot->EndEventLoop();

  cout << "Writing..." << endl;
  file_out->Write();

  cout << "Closing..." << endl;
  // file_out->Close();

  return;
}

void GlobalRecoAnaGSI::FillYieldReco(string folderName, Int_t Z, Double_t Th)
{
  string path = folderName + "/charge";
  ((TH1D *)gDirectory->Get(path.c_str()))->Fill(Z);
  for (int i = 0; i < th_nbin; i++)
  {
    if (Th >= theta_binning[i][0] && Th < theta_binning[i][1])
    {
      path = folderName + "/Z_" + to_string(Z - 1) + "#" + to_string(Z - 0.5) + "_" + to_string(Z + 0.5) + "/theta_" + to_string(i) + "#" + to_string(theta_binning[i][0]) + "_" + to_string(theta_binning[i][1]) + "/theta_";
      // cout << path << endl;
      ((TH1D *)gDirectory->Get(path.c_str()))->Fill(Th);
      string path_matrix = folderName + "/Z_" + to_string(Z - 1) + "#" + to_string(Z - 0.5) + "_" + to_string(Z + 0.5) + "/theta_" + to_string(i) + "#" + to_string(theta_binning[i][0]) + "_" + to_string(theta_binning[i][1]) + "/migMatrix_Z";
    }
  }
}

void GlobalRecoAnaGSI::FillYieldMC(string folderName, Int_t Z_true, Int_t Z_meas, Double_t Th_true, Double_t Th_meas, bool enableMigMatr = false)
{
  string path = folderName + "/charge";
  ((TH1D *)gDirectory->Get(path.c_str()))->Fill(Z_true);

  if (enableMigMatr)
  {
    string path = folderName + "/migMatrix_Z";
    ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Z_meas, Z_true);

    path = folderName + "/Z_" + to_string(int(Z_true) - 1) + "#" + to_string(int(Z_true) - 0.5) + "_" + to_string(int(Z_true) + 0.5) + "/migMatrix_theta_Z_" + to_string(Z_true - 1);
    ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Th_meas, Th_true);
  }

  for (int i = 0; i < th_nbin; i++)
  {
    if (Th_true >= theta_binning[i][0] && Th_true < theta_binning[i][1])
    {
      path = folderName + "/Z_" + to_string(int(Z_true) - 1) + "#" + to_string(int(Z_true) - 0.5) + "_" + to_string(int(Z_true) + 0.5) + "/theta_" + to_string(i) + "#" + to_string(theta_binning[i][0]) + "_" + to_string(theta_binning[i][1]) + "/theta_";
      ((TH1D *)gDirectory->Get(path.c_str()))->Fill(Th_true);
    }
  }
}

void GlobalRecoAnaGSI::MigMatrixPlots(string folderName, Int_t Z_true, Int_t Z_meas, Double_t Th_true, Double_t Th_meas, bool enableMigMatr = false)
{
  if (enableMigMatr)
  {
    string path = folderName + "/migMatrix_Z";
    ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Z_meas, Z_true );

    path = folderName + "/migMatrix_Ztrack_vs_Ztw";
    ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Z_true_TW, Z_true);

    path = folderName + "/migMatrix_theta";
    ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Th_meas, Th_true);

    if (Z_true > 0. && Z_true <= fPrimaryCharge)
    {
      path = folderName + "/Z_" + to_string(int(Z_true) - 1) + "#" + to_string(int(Z_true) - 0.5) + "_" + to_string(int(Z_true) + 0.5) + "/migMatrix_theta_Z_" + to_string(Z_true - 1);
      ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Th_meas, Th_true);

      for (int i = 0; i < th_nbin; i++)
      {
        if (Th_true >= theta_binning[i][0] && Th_true < theta_binning[i][1])
        {

          string path = folderName + "/theta_" + to_string(i) + "#" + to_string(theta_binning[i][0]) + "_" + to_string(theta_binning[i][1]) + "/migMatrix_Z";
          ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Z_meas, Z_true);
        }
      }
    }
  }
}

void GlobalRecoAnaGSI::BookMigMatrix(string path, bool enableMigMatr)
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::BookYield start" << endl;

  gDirectory->mkdir(path.c_str());
  gDirectory->cd(path.c_str());

  if (enableMigMatr)
  {
    h2 = new TH2D("migMatrix_Z", "migMatrix_Z;  Z_{reco}; Z_{true}", 8, 0.5, 8.5, 8, 0.5, 8.5);
    h2 = new TH2D("migMatrix_Ztrack_vs_Ztw", "migMatrix_Ztrack_vs_Ztw; Z_{TW}; Z_{track}", 8, 0.5, 8.5, 8, 0.5, 8.5);
    h2 = new TH2D("migMatrix_theta", "migMatrix_theta; \\theta_{reco} ; \\theta_{true} ", 20, 0., 12., 20, 0., 12.);
  }

  for (int iz = 1; iz <= fPrimaryCharge; iz++)
  {
    string name = "Z_" + to_string(iz - 1) + "#" + to_string(iz - 0.5) + "_" + to_string(iz + 0.5);
    gDirectory->mkdir(name.c_str());
    gDirectory->cd(name.c_str());
    name = "";

    if (enableMigMatr)
    {
      name = "migMatrix_theta_Z_" + to_string(iz - 1);
      string title = "migMatrix_theta_Z=" + to_string(iz) + "; #theta_{reco}; #theta_{true};";
      h2 = new TH2D(name.c_str(), title.c_str(), 20, 0., 12., 20, 0., 12.);
    }
    gDirectory->cd("..");
  }

  for (int i = 0; i < th_nbin; i++)
  {
    string path = "theta_" + to_string(i) + "#" + to_string(theta_binning[i][0]) + "_" + to_string(theta_binning[i][1]);
    gDirectory->mkdir(path.c_str());
    gDirectory->cd(path.c_str());
    string title = "migMatrix_Z with theta < " + to_string(theta_binning[i][1]) + "; Z_{reco}; Z_{true}";
    h2 = new TH2D("migMatrix_Z", title.c_str(), 8, 0.5, 8.5, 8, 0.5, 8.5);

    gDirectory->cd("..");
  }

  gDirectory->cd("..");
}

void GlobalRecoAnaGSI::BookYield(string path, bool enableMigMatr)
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAnaGSI::BookYield start" << endl;

  gDirectory->mkdir(path.c_str());
  gDirectory->cd(path.c_str());
  h = new TH1D("charge", "", 8, 0.5, 8.5);

  if (enableMigMatr)
    h2 = new TH2D("migMatrix_Z", "migMatrix_Z;Z_{reco};  Z_{true} ", 8, 0.5, 8.5, 8, 0.5, 8.5);

  for (int iz = 1; iz <= fPrimaryCharge; iz++)
  {
    string name = "Z_" + to_string(iz - 1) + "#" + to_string(iz - 0.5) + "_" + to_string(iz + 0.5);
    gDirectory->mkdir(name.c_str());
    gDirectory->cd(name.c_str());
    name = "";

    if (enableMigMatr)
    {
      name = "migMatrix_theta_Z_" + to_string(iz - 1);
      string title = "migMatrix_theta_Z=" + to_string(iz) + "; #theta_{reco}; #theta_{true};";
      h2 = new TH2D(name.c_str(), title.c_str(), 20, 0., 12., 20, 0., 12.);
    }

    for (int i = 0; i < th_nbin; i++)
    {
      string path = "theta_" + to_string(i) + "#" + to_string(theta_binning[i][0]) + "_" + to_string(theta_binning[i][1]);
      gDirectory->mkdir(path.c_str());
      gDirectory->cd(path.c_str());

      h = new TH1D("theta_", "", 180, 0, 90.);

      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  gDirectory->cd("..");
}

void GlobalRecoAnaGSI::MCParticleStudies()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAna::MCParticleStudies start" << endl;

  Int_t n_particles = myMcNtuPart->GetTracksN(); // n° of particles of an event
  //((TH1D *)gDirectory->Get("MC_check/TracksN_MC"))->Fill(myMcNtuPart->GetTracksN());

  //************************************************************* begin Loop on all MCparticles *************************************************************
  for (Int_t i = 0; i < n_particles; i++)
  { // for every particle in an event

    TAMCpart *particle = myMcNtuPart->GetTrack(i);
    auto Mid = particle->GetMotherID();
    double mass = particle->GetMass(); // Get TRpaid-1
    auto Reg = particle->GetRegion();
    auto finalPos = particle->GetFinalPos();
    int baryon = particle->GetBaryon();
    TVector3 initMom = particle->GetInitP();
    double InitPmod = sqrt(pow(initMom(0), 2) + pow(initMom(1), 2) + pow(initMom(2), 2));
    Float_t Ek_tr_tot = (sqrt(pow(InitPmod, 2) + pow(mass, 2)) - mass);
    Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
    Float_t Ek_true = Ek_tr_tot / (double)baryon;
    Float_t theta_tr = particle->GetInitP().Theta() * (180 / TMath::Pi()); // in deg
    Float_t charge_tr = particle->GetCharge();

    //! finalPos.Z() > 189.15 IN GSI2021_MC
    //! finalPos.Z() > 90 IN 16O_400

    // TO BE CHECKED
    //  Int_t fRegTG = -1;         //! hard coded
    //  if(fExpName.IsNull())
    //  fRegTG = 59; // true in newgeom setup
    //  else if(!fExpName.CompareTo("GSI/") || !GlobalRecoAna::fExpName.CompareTo("GSI_MC/"))
    //  fRegTG = 48;  //  GSI-2019
    //  else if(!fExpName.CompareTo("CNAO2020/"))
    //  fRegTG = 50;  //  CNAO-2020
    //  else if(!fExpName.CompareTo("GSI2021_MC/"))
    //  fRegTG = 50; //   GSI2021_MC

    //-------------  MC TOTAL CROSS SECTION
    // if the particle is generated in the target and it is the fragment of a primary
    // if Z<8 and A<30, so if it is a fragment (not the primitive projectile, nor detector fragments)
    // ifenough energy/n to go beyond the target
    if (Mid == 0 && Reg == fRegTG && particle->GetCharge() > 0 && particle->GetCharge() <= fPrimaryCharge && Ek_true > 100)
      FillYieldMC("yield-true_cut", charge_tr, charge_tr, theta_tr, theta_tr, false);

    //-------------  MC FIDUCIAL CROSS SECTION (<8 deg)
    if (Mid == 0 && Reg == fRegTG && particle->GetCharge() > 0 && particle->GetCharge() <= fPrimaryCharge && Ek_true > 100 && theta_tr <= 8.)
      FillYieldMC("yield-true_DET", charge_tr, charge_tr, theta_tr, theta_tr, false);

    //-------------  MC FIDUCIAL CROSS SECTION (<2 deg)
    /*
    if (  Mid==0 && Reg == fRegTG && particle->GetCharge()>0 && particle->GetCharge()<=fPrimaryCharge && Ek_true>100
    && theta_tr <= 2.  //  myangle // angular aperture < 8 deg
    )
    FillYieldMC("yield-true_DET2",charge_tr,theta_tr,Ek_tr_tot);
    */
  }

  // loop on crossings
  if (TAGrecoManager::GetPar()->IsRegionMc())
  {
    Int_t exitfragnum = 0;        // number of fragmengs exit from the target
    Int_t exitfrag10anglenum = 0; // number of fragmengs exit from the target
    for (int icr = 0; icr < GetNtuMcReg()->GetRegionsN(); icr++)
    {
      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
      if (fpNtuMcReg->GetCharge() > 0 && fpNtuMcReg->GetMass() > 0 && fpNtuMcReg->GetOldCrossN() == fRegTG && fpNtuMcReg->GetCrossN() == fRegAirBeforeTW)
      { // a particle exit from the target
        exitfragnum++;
        if (fpNtuMcReg->GetMomentum().Theta() * TMath::RadToDeg() < 10)
          exitfrag10anglenum++;
      }
    }
    // ((TH2D *)gDirectory->Get("MC/MCpartVsGlbtrackNum"))->Fill(exitfragnum, myGlb->GetTracksN());
    // ((TH2D *)gDirectory->Get("MC/MCpartVsGlbtrackNum_angle10"))->Fill(exitfrag10anglenum, myGlb->GetTracksN());
  }

  return;
}

void GlobalRecoAnaGSI::RecoGlbTrkLoopSetVariables()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAna::RecoGlbTrkLoopSetVariables start" << endl;

  Th_reco = fGlbTrack->GetTgtTheta() * TMath::RadToDeg();
  Z_meas = fGlbTrack->GetTwChargeZ();
  P_meas = fGlbTrack->GetMomentum(); // Wrong method of TOE!! but it does not crash
  Tof_meas = fGlbTrack->GetTwTof() - primary_tof;
  Ek_meas = fGlbTrack->GetCalEnergy() * fpFootGeo->MevToGev() / M_meas; // Energy (GeV/u)
  Beta_meas = fGlbTrack->GetLength() / Tof_meas / TAGgeoTrafo::GetLightVelocity();
  M_meas = fGlbTrack->GetMass(); // It's the track mass hp, cannot be used as mass measurement

  // take the vector direction of the fragment in global SdR wrt target
  TVector3 TgtMomentum = fGlbTrack->GetTgtMomentum().Unit();
  // take the direction ofpush_back the beam in global SdR
  TVector3 BMslope = fpFootGeo->VecFromBMLocalToGlobal(myBMNtuTrk->GetTrack(0)->GetSlope());
  // take the angle between these 2 vectors
  // Th_recoBM = BMslope.Angle( TgtMomentum )*TMath::RadToDeg();
  Th_recoBM = fGlbTrack->GetTgtThetaBm() * TMath::RadToDeg();
  if (FootDebugLevel(1))
    cout << "momX: " << TgtMomentum.X() << " momY: " << TgtMomentum.Y() << " momZ: " << TgtMomentum.Z() << endl
         << "BMX: " << BMslope.X() << " BMY: " << BMslope.Y() << " BMZ: " << BMslope.Z() << endl
         << "TH_mom: " << TgtMomentum.Theta() * TMath::RadToDeg() << " thBM: " << BMslope.Theta() * TMath::RadToDeg() << " th_recoBM: " << Th_recoBM << endl
         << endl;

  return;
}


bool GlobalRecoAnaGSI::CheckTwPointInMoreTracks(){
  vectTwId.clear();
  Int_t nt = myGlb->GetTracksN(); // number of reconstructed tracks for every event
  for (int it = 0; it < nt; it++)
  {
    fGlbTrack = myGlb->GetTrack(it);
    if (!fGlbTrack->HasTwPoint())
    {
      continue;
    }

  TAGpoint *twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);
  
  if ((std::find(vectTwId.begin(), vectTwId.end(), twpoint -> GetClusterIdx()) != vectTwId.end())) // if twpoin id already in the vector
    return true;
    else  vectTwId.push_back(twpoint -> GetClusterIdx());

  }
  return false;
}

void GlobalRecoAnaGSI::MCGlbTrkLoopSetVariables()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAna::MCGlbTrkLoopSetVariables start" << endl;

  Tof_true = -1.;
  Tof_startmc = -1.;
  initTWPosition = -999;
  Beta_true = -1.;
  P_cross.SetXYZ(-999., -999., -999.); // also MS contribution in target!
  P_beforeTG.SetXYZ(-999., -999., -999.);
  TrkIdMC = fGlbTrack->GetMcMainTrackId(); // associo l'IdMC alla particella più frequente della traccia  (prima era ottenuto tramite studio purity)
  TATWpoint *twpoint = (TATWpoint *)fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);
  TAGpoint *vtcluster;
  TAGpoint *msdcluster;
  TAGpoint *point;


  // cout << " track with " << fGlbTrack->GetPointsN ()<< " points " << endl;
  // for (int i = 0; i < 4; i++) // i put all charges of a vtx cluster in the vector
  // {
  //   vecVtZMC[i].clear();
  //   vtcluster = fGlbTrack->GetPoint(i);
  //   cout << "vtz point: "<< i << " is " << vtcluster -> GetDevName() << endl; 
  //   for (int j = 0; j < vtcluster->GetMcTracksN(); j++)
  //   {
  //     TAMCpart *particleMC = GetNtuMcTrk()->GetTrack(vtcluster->GetMcTrackIdx(j));
  //     vecVtZMC[i].push_back(particleMC->GetCharge());
  //   }
  // }




  // for (int i = 4; i<10; i++)  // i put all charges of a msd cluster in the vector
  // {
  //   vecMsdZMC[i-4].clear();
  //   msdcluster = fGlbTrack->GetPoint(i);
  //   cout << "msd point: " << i << " is " << msdcluster->GetDevName() << endl;
  //   for (int j = 0; j < msdcluster->GetMcTracksN(); j++)
  //   {
  //     TAMCpart *particleMC = GetNtuMcTrk()->GetTrack(msdcluster->GetMcTrackIdx(j));
  //     vecMsdZMC[i-4].push_back(particleMC->GetCharge());
  //   }
  // }
  vecVtZMC.clear();
  vecMsdZMC.clear();




  for (int i = 0; i < fGlbTrack->GetPointsN(); i++) // for all the points of a track...
  {
  point  = fGlbTrack->GetPoint(i);

  if ((string) point -> GetDevName() == "VT") { //... i take the vt cluster
    vector<Int_t> vecVT_z; // vector of all Z of a cluster of the vtx
    vecVT_z.clear();
    for (int j = 0; j < point->GetMcTracksN(); j++)
    {
      
      TAMCpart *particleMC = GetNtuMcTrk()->GetTrack(point->GetMcTrackIdx(j));
      vecVT_z.push_back(particleMC->GetCharge());     // i take all the charges of a cluster
    }
    vecVtZMC.push_back(vecVT_z); // and i put all in a vector
  }

  if ((string)point->GetDevName() == "MSD")
  {
    //cout << " it is a MSD" << endl;
    vector<Int_t> vecMSD_z; // vector of all Z of a cluster of the vtx
    vecMSD_z.clear();
    for (int j = 0; j < point->GetMcTracksN(); j++)
    {

      TAMCpart *particleMC = GetNtuMcTrk()->GetTrack(point->GetMcTrackIdx(j));
      vecMSD_z.push_back(particleMC->GetCharge());
    }
    vecMsdZMC.push_back(vecMSD_z);
  }


}

// if (vecVtZMC.size() < 4)
//   cout << "my vector of vector VT of charghe has size " << vecVtZMC.size() << endl;
// if (vecMsdZMC.size() < 6){
//   cout << "my vector of vector MSD of charghe has size " << vecMsdZMC.size() << endl;
//   cout << "vector 0 has " << vecMsdZMC.at(0).size() << " elements "<<endl;
// }

initTWPosition = GetNtuMcTrk()->GetTrack(twpoint->GetMcTrackIdx(0))->GetInitPos().Z();
Z_true_TW = GetNtuMcTrk()->GetTrack(twpoint->GetMcTrackIdx(0))->GetCharge();

// all mc track id of TW point
vecTwTrkId.clear();
for (int i = 0; i < twpoint->GetMcTracksN(); i++)
{
  vecTwTrkId.push_back(twpoint->GetMcTrackIdx(i));
  }
  TrkIdMC_TW = twpoint->GetMcTrackIdx(0); // associo l'IdMC alla particella che ha attraversato il TW --> in questo modo bypasso il problema di frammentazione secondaria
  //  for (int i = 0; i < fGlbTrack->GetPointsN(); i++){
  //    auto twpoint = fGlbTrack->GetPoint(i);
  //    cout << twpoint -> GetDevName() << endl;
  //    cout<< "tw points mc: " << twpoint ->GetMcTracksN() << endl;
  //  }

  if (TrkIdMC != -1)
  {
    TAMCpart *pNtuMcTrk = GetNtuMcTrk()->GetTrack(TrkIdMC);

    Z_true = pNtuMcTrk->GetCharge();
    P_true = pNtuMcTrk->GetInitP(); // also MS contribution in target!
    M_true = pNtuMcTrk->GetMass();
    Ek_tot = (sqrt(pow(pNtuMcTrk->GetMass(), 2) + pow((pNtuMcTrk->GetInitP()).Mag(), 2)) - pNtuMcTrk->GetMass());
    Ek_tot = Ek_tot * fpFootGeo->GevToMev();
    Ek_true = Ek_tot / pNtuMcTrk->GetBaryon();

    if (TAGrecoManager::GetPar()->IsRegionMc())
    {
      for (int icr = 0; icr < GetNtuMcReg()->GetRegionsN(); icr++) // loop on MC regions
      {
        TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);

        // particle entering into target
        if (fpNtuMcReg->GetCrossN() == fRegTG && fpNtuMcReg->GetOldCrossN() == fRegAirBeforeTW)
        {
          if ((fpNtuMcReg->GetTrackIdx() - 1) == 0) // it has to be the primary
            P_beforeTG = fpNtuMcReg->GetMomentum();
        }

        if ((fpNtuMcReg->GetTrackIdx() - 1) == TrkIdMC)
        {
          // particle exit from target
          if (fpNtuMcReg->GetCrossN() == fRegAirBeforeTW && fpNtuMcReg->GetOldCrossN() == fRegTG)
          {
            P_cross = fpNtuMcReg->GetMomentum();
            Tof_startmc = fpNtuMcReg->GetTime() * fpFootGeo->SecToNs();
          }
          // particle crossing in the first TW layer
          if (fpNtuMcReg->GetCrossN() <= fRegLastTWbar && fpNtuMcReg->GetCrossN() >= fRegFirstTWbarLay0)
          {
            Tof_true = fpNtuMcReg->GetTime() * fpFootGeo->SecToNs();
            Beta_true = fpNtuMcReg->GetMomentum().Mag() / sqrt(fpNtuMcReg->GetMass() * fpNtuMcReg->GetMass() + fpNtuMcReg->GetMomentum().Mag() * fpNtuMcReg->GetMomentum().Mag());
          }
          // particle crossing in the calo
          if (fpNtuMcReg->GetCrossN() >= fRegFirstCAcrys && fpNtuMcReg->GetCrossN() <= fRegLastCAcrys)
          {
            // if(fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirTW() && fpNtuMcReg->GetOldCrossN()<=GetParGeoTw()->GetRegStrip(0,GetParGeoTw()->GetNBars()-1) && fpNtuMcReg->GetOldCrossN()>=GetParGeoTw()->GetRegStrip(0,0)){//
            Double_t M_cross_calo = fpNtuMcReg->GetMass();
            TVector3 P_cross_calo = fpNtuMcReg->GetMomentum();
            Ek_cross_calo = sqrt(P_cross_calo * P_cross_calo + M_cross_calo * M_cross_calo) - M_cross_calo;
            Ek_cross_calo = Ek_cross_calo / M_cross_calo;
            // }
          }
        }
      }
      if (Tof_startmc >= 0)
        Tof_true -= Tof_startmc;
      // TODO: we need to define what happens when a particle isn't created in the target!
      if (FootDebugLevel(1))
      {
        cout << "Z_true=" << Z_true << "  M_true=" << M_true << "  P_true.Mag()=" << P_true.Mag() << "  Ek_true=" << Ek_true << endl;
        if (TAGrecoManager::GetPar()->IsRegionMc())
        {
          cout << "Target exit crossings:" << endl
               << "  P_cross.Mag()=" << P_cross.Mag() << "  Tof_startmc=" << Tof_startmc << endl;
          cout << "TOF mesurements: particle total time of flight=" << Tof_true << "  Beta_true=" << Beta_true << "  primary_tof=Tof_startmc=" << Tof_startmc << "  particle real mc tof=" << Tof_true - Tof_startmc << endl;
        }
      }

      Th_true = P_true.Theta() * TMath::RadToDeg();
      if (P_beforeTG.X() == -999 || P_cross.X() == -999)
        Th_BM = -999;
      else
        Th_BM = P_cross.Angle(P_beforeTG) * TMath::RadToDeg();
      Th_cross = P_cross.Theta() * TMath::RadToDeg();
    }
  }

  return;
}

void GlobalRecoAnaGSI::FillMCPartYields()
{
  if (TAGrecoManager::GetPar()->IsRegionMc() == false) {
    Error("FillMCPartYields","IsRegionMc() needed for the analysis!");
    return;
  }
   
  TAMCntuRegion *pNtuReg = GetNtuMcReg();
  Int_t nCross = pNtuReg->GetRegionsN();

  TVector3 P_cross;    // target exit momentum
  TVector3 P_beforeTG; // target entering momentum
  Double_t Th_BM = -999;
  P_cross.SetXYZ(-999., -999., -999.); // also MS contribution in target!
  P_beforeTG.SetXYZ(-999., -999., -999.);

  for (int iCross = 0; iCross < nCross; iCross++)
  {
    // region description
    TAMCregion *cross = pNtuReg->GetRegion(iCross); // Gets the i-crossing
    TVector3 crosspos = cross->GetPosition();       // Get CROSSx, CROSSy, CROSSz
    Int_t OldReg = cross->GetOldCrossN();
    Int_t NewReg = cross->GetCrossN();
    Double_t time_cross = cross->GetTime();
    TVector3 mom_cross = cross->GetMomentum();                                                                            // retrieves P at crossing
    Double32_t Mass = cross->GetMass();                                                                                   // retrieves Mass of track
    Double_t ekin_cross = sqrt(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2) + pow(Mass, 2)) - Mass; // Kinetic energy at crossing
    Double_t beta_cross = sqrt(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2)) / (ekin_cross + Mass);

    // if ((cross->GetTrackIdx() - 1) != Id_part)
    // { // if it is not the particle I want to check, continue the loop
    //   continue;
    // }

    // particle description
    TAMCpart *particle = myMcNtuPart->GetTrack(cross->GetTrackIdx() - 1); // retrievs TrackID
    Int_t particle_ID = cross->GetTrackIdx() - 1;                         // TrackID
    Int_t target_region = fRegTG;
    auto Mid = particle->GetMotherID();
    double mass = particle->GetMass(); // Get TRpaid-1
    auto Reg = particle->GetRegion();
    auto finalPos = particle->GetFinalPos();
    int baryon = particle->GetBaryon();
    TVector3 initMom = particle->GetInitP();
    double InitPmod = sqrt(pow(initMom(0), 2) + pow(initMom(1), 2) + pow(initMom(2), 2));
    Float_t Ek_tr_tot = (sqrt(pow(InitPmod, 2) + pow(mass, 2)) - mass);
    Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
    Float_t Ek_true = Ek_tr_tot / (double)baryon;                           // MeV/n
    Float_t theta_tr = particle->GetInitP().Theta() * (180. / TMath::Pi()); // in deg
    Float_t charge_tr = particle->GetCharge();

    if (
        (particle_ID == 0                      // if the particle is a primary OR
         || (Mid == 0 && Reg == target_region) // if the particle is a primary fragment born in the target
         ) &&
        particle->GetCharge() > 0 &&
        particle->GetCharge() <= fPrimaryCharge && // with reasonable charge
        // Ek_true > 100 &&                           // with enough initial energy to go beyond the vtx    //! is it true?
        // theta_tr <= 30. && // inside the angular acceptance of the vtxt   //!hard coded for GSI2021_MC
        (OldReg >= fRegFirstTWbar && OldReg <= fRegLastTWbar) && NewReg == fRegAirAfterTW 
        // it crosses the two planes of the TW and go beyond  (one of the bar of the two layers - region from 81 to 120)
    )
    {
      // I want to measure the angle of emission of this particle:  i need to loop again the regions
      for (int iCross = 0; iCross < nCross; iCross++)
      {
        // region description
        TAMCregion *cross = pNtuReg->GetRegion(iCross); // Gets the i-crossing
        TVector3 crosspos = cross->GetPosition();       // Get CROSSx, CROSSy, CROSSz
        Int_t OldReg = cross->GetOldCrossN();
        Int_t NewReg = cross->GetCrossN();

        // particle entering into target
        if (NewReg == fRegTG && OldReg == fRegAirBeforeTW)
        {
          P_beforeTG = cross->GetMomentum();
        }

        if ((cross->GetTrackIdx() - 1) == particle_ID)
        {
          // particle exit from target
          if (NewReg == fRegAirBeforeTW && OldReg == fRegTG)
          {
            P_cross = cross->GetMomentum();
          }
        }
      }

      Th_BM = P_cross.Angle(P_beforeTG) * 180. / TMath::Pi();
      if (charge_tr > 0 && charge_tr <= fPrimaryCharge)
        FillYieldMC("yield-N_ref", charge_tr, charge_tr, Th_BM, Th_BM, false);

      Th_BM = -999;
      P_cross.SetXYZ(-999., -999., -999.); // also MS contribution in target!
      P_beforeTG.SetXYZ(-999., -999., -999.);
    }
  }
}


bool GlobalRecoAnaGSI::isGoodReco(Int_t Id_part)
{
    if (TAGrecoManager::GetPar()->IsRegionMc() == false) {
      Error("isGoodReco","IsRegionMc() needed for the analysis!");
      return false;
    }

  TAMCntuRegion *pNtuReg = GetNtuMcReg();
  Int_t nCross = pNtuReg->GetRegionsN();

  for (int iCross = 0; iCross < nCross; iCross++)
  {
    TAMCregion *cross = pNtuReg->GetRegion(iCross); // Gets the i-crossing
    TVector3 crosspos = cross->GetPosition();       // Get CROSSx, CROSSy, CROSSz
    Int_t OldReg = cross->GetOldCrossN();
    Int_t NewReg = cross->GetCrossN();
    Double_t time_cross = cross->GetTime();
    TVector3 mom_cross = cross->GetMomentum();                                                                            // retrieves P at crossing
    Double32_t Mass = cross->GetMass();                                                                                   // retrieves Mass of track
    Double_t ekin_cross = sqrt(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2) + pow(Mass, 2)) - Mass; // Kinetic energy at crossing
    Double_t beta_cross = sqrt(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2)) / (ekin_cross + Mass);

    if ((cross->GetTrackIdx() - 1) != Id_part)
    { // if it is not the particle I want to check, continue the loop
      continue;
    }

    TAMCpart *particle = myMcNtuPart->GetTrack(cross->GetTrackIdx() - 1); // retrievs TrackID
    Int_t particle_ID = cross->GetTrackIdx() - 1;                         // TrackID
    Int_t target_region = fRegTG;
    auto Mid = particle->GetMotherID();
    double mass = particle->GetMass(); // Get TRpaid-1
    auto Reg = particle->GetRegion();
    auto finalPos = particle->GetFinalPos();
    int baryon = particle->GetBaryon();
    TVector3 initMom = particle->GetInitP();
    double InitPmod = sqrt(pow(initMom(0), 2) + pow(initMom(1), 2) + pow(initMom(2), 2));
    Float_t Ek_tr_tot = (sqrt(pow(InitPmod, 2) + pow(mass, 2)) - mass);
    Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
    Float_t Ek_true = Ek_tr_tot / (double)baryon;                          // MeV/n
    Float_t theta_tr = particle->GetInitP().Theta() * (180 / TMath::Pi()); // in deg
    Float_t charge_tr = particle->GetCharge();

    if (
        (particle_ID == 0                      // if the particle is a primary OR
         || (Mid == 0 && Reg == target_region) // if the particle is a primary fragment born in the target
         ) &&
        particle->GetCharge() > 0 &&
        particle->GetCharge() <= fPrimaryCharge && // with reasonable charge
        // Ek_true > 100 &&                           // with enough initial energy to go beyond the vtx    //! is it true?
        // theta_tr <= 30. && // inside the angular acceptance of the vtxt   //!hard coded for GSI2021_MC
        (OldReg >= fRegFirstTWbar && OldReg <= fRegLastTWbar) && NewReg == fRegAirAfterTW // it crosses the two planes of the TW and go beyond  (one of the bar of the two layers - region from 81 to 120)
    )
      return true;
  }
  return false;
}

void GlobalRecoAnaGSI::ChargeStudies(string path, Int_t charge, TAGtrack *fGlbTrack)
{
  if (Z_true == charge)
  {
    auto twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);
    auto initTWPosition = GetNtuMcTrk()->GetTrack(twpoint->GetMcTrackIdx(0))->GetInitPos().Z();
    string path_ = "";
    for (int i = 0; i < fGlbTrack->GetPointsN(); i++)
    {
      TAGpoint* point = fGlbTrack->GetPoint(i);
      string path_ = path + "/trackMCid";
      ((TH2D *)gDirectory->Get(path_.c_str()))->Fill(i, point->GetMcTrackIdx(0));
      path_ = path + "/trackMCid_multiplicity";
      ((TH2D *)gDirectory->Get(path_.c_str()))->Fill(i, point->GetMcTracksN());
    }
    path_ = path + "/trackquality";
    ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(fGlbTrack->GetQuality());
    path_ = path + "/trackqualityvsZmeas";
    ((TH2D *)gDirectory->Get(path_.c_str()))->Fill(fGlbTrack->GetQuality(), Z_meas);
    //}
    path_ = path + "/initposition_twpoint";
    ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(initTWPosition);

    path_ = path + "/initposition_twpoint(allMCID)";
    for (int i = 0; i < twpoint->GetMcTracksN(); i++)
    {
      ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(GetNtuMcTrk()->GetTrack(twpoint->GetMcTrackIdx(i))->GetInitPos().Z());
    }

    path_ = path + "/twpointposvscharge";
    ((TH2D *)gDirectory->Get(path_.c_str()))->Fill(initTWPosition, GetNtuMcTrk()->GetTrack(twpoint->GetMcTrackIdx(0))->GetCharge());
  }
}

void GlobalRecoAnaGSI::BookChargeStudies(string path)
{ // study of Z=8
  gDirectory->mkdir(path.c_str());
  gDirectory->cd(path.c_str());
  h2 = new TH2D("trackMCid", "MC id per point of the track; point; MC ID", 11, -0.5, 10.5, 300, -0.5, 300.5);
  h2 = new TH2D("trackMCid_multiplicity", "multiplicity of MC id per point of the track; point; MC ID multiplicity", 11, -0.5, 10.5, 30, -0.5, 30.5);
  h = new TH1D("trackquality", "track quality;", 11, 0., 1.1);
  h2 = new TH2D("trackqualityvsZmeas", "trackqualityvsZmeas; track quality; Z_meas", 11, 0., 1.1, 8, 0.5, 8.5);
  h = new TH1D("initposition_twpoint", "origin of the particle in TW for TW matched tracks; pos(Z) [cm]; events ", 2000, 0., 200.);
  h = new TH1D("initposition_twpoint(allMCID)", "origin of (all) the particle(s) in TW for TW matched tracks; pos(Z) [cm]; events ", 2000, 0., 200.);
  h2 = new TH2D("twpointposvscharge", "origin of tw particle vs the true charge; pos(Z) [cm]; charge", 2000, 0., 200., 8, 0.5, 8.5);
  gDirectory->cd("..");
}
