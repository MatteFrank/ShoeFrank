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
  trueEvents = 0;
  istrueEvent = false;
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

    if (currEvent >= nTotEv)
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
      recoEvents++; //! modifica

    // TAWDntuTrigger *wdTrig = 0x0;
    // if (fFlagMC == false)
    // {
    //   wdTrig = (TAWDntuTrigger *)fpNtuWDtrigInfo->GenerateObject(); // trigger from hardware
    // }

    m_nClone.clear();

    //studies concerning tw points
    hasSameTwPoint.clear();
    hasSameTwPoint = CheckTwPointInMoreTracks();

    // study of origin of the particle crossing the tw for every track (MC)
    if (fFlagMC)
      {
        isParticleBorninTG.clear();
        isParticleBorninTG = CheckTwParticleOrigin();
      }

    Int_t nt_TW = 0; // number of reconstructed tracks with TW point for every event
    for (int it = 0; it < nt; it++)
    {
      fGlbTrack = myGlb->GetTrack(it);
      if (fGlbTrack->HasTwPoint())
        nt_TW++;
    }

    //*********************************************************************************** begin loop on global tracks *********************************************
    // if ) cout << "event " << currEvent << " has more tw points in tracks " << endl;

    for (int it = 0; it < nt; it++)
    {
      fGlbTrack = myGlb->GetTrack(it);
      // if (fGlbTrack->GetPointsN() < 9)
      //   cout << "n points of track: " << fGlbTrack->GetPointsN() << endl;
      if (!fGlbTrack->HasTwPoint() /*|| fGlbTrack->GetPointsN() < 11*/)
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

        // compute MC VT match
        bool VTMatch = true;
        bool VTZ8Match = true;

        if( vecVtZMC.size() > 0 )
        {
          for (int i = 0; i < vecVtZMC.size(); i++)
          {
            if (std::find(vecVtZMC.at(i).begin(), vecVtZMC.at(i).end(), 8) != vecVtZMC.at(i).end()) // if the cluster of the VTX contains Z=8
              VTZ8Match = VTZ8Match && true;
            else
              VTZ8Match = VTZ8Match && false;
          }


          if (VTZ8Match == true)
          {                   // if a primary entered the first layer of the vtx and did not fragmented up to its last plane...
            if (Z_meas > 7)   // ... and the charge reconstructed in TW is higher than 7
              VTMatch = true; // --> it means there is no fragmentation between VTX and up to TW
            else
              VTMatch = false;
          }

          if ( (std::find(vecVtZMC.at(0).begin(), vecVtZMC.at(0).end(), 8) != vecVtZMC.at(0).end()) && (VTZ8Match == false))
          // if the first cluster was a Z=8 but then some fragmentation happened...
          {
            VTMatch = false;
          }
        }

        // compute MC MSD match
        bool MSDMatch = true;

        if( vecMsdZMC.size() > 0 )
        {
          for (int i = 0; i < vecMsdZMC.size(); i++)
          {
            if (std::find(vecMsdZMC.at(i).begin(), vecMsdZMC.at(i).end(), Z_meas) != vecMsdZMC.at(i).end()) // if all the cluster of the MSD contains Z
              MSDMatch = MSDMatch && true;                                                                  // reconstructed by the TW
            else
              MSDMatch = MSDMatch && false;
          }
        }


        // // // =================== VTX MATCH
        // if (VTMatch == true)
        //   MyReco("VT");

        // // // =================== MSD MATCH
        // if (MSDMatch == true)
        //   MyReco("MSD");

        // // // =================== VTX + MSD  MATCH
        // if (MSDMatch == true && VTMatch == true)
        //   MyReco("VTX_MSD");
        
        // // // ===================ALL TRACKS with TRACK QUALITY = 1
        // if (fGlbTrack->GetQuality() == 1)
        //   MyReco("Q1");

        // // // =================== MC VTX + Chi2 cuts
        // if (VTMatch == true && fGlbTrack->GetChi2() < 2 && abs(residual)  < 0.01)
        //   MyReco("VTChi2");

        // // // =================== MC VTX + Chi2 cuts + multitrack
        // if (VTMatch == true && fGlbTrack->GetChi2() < 2 && abs(residual)  < 0.01 && nt > 1)
        //   MyReco("MVTChi2");

        // // // =================== Chi2 cuts + multitrack + NO TW in multitracks + MSD CUT
        // if (fGlbTrack->GetChi2() < 2 && abs(residual)  < 0.01 && nt > 1 && hasSameTwPoint.at(it) == false && MSDMatch == true)
        //   MyReco("MChi2MSDTWT");
        }

        // compute chi2 and residuals
        residual = 0;
        float res_temp = 0;
        for (int i = 0; i < fGlbTrack->GetPointsN(); i++)
        {
          auto point = fGlbTrack->GetPoint(i);
          if ((string)point->GetDevName() == "MSD")
          {
            if (point->GetSensorIdx() % 2 == 0) // if it is the sensor x of the msd
              res_temp = point->GetMeasPosition().X() - point->GetFitPosition().X();
            else
              res_temp = point->GetMeasPosition().Y() - point->GetFitPosition().Y();
          }

          if ((string)point->GetDevName() == "VT")
          {
            res_temp = (point->GetMeasPosition() - point->GetFitPosition()).Mag();
          }
          if (abs(res_temp) > abs(residual) )
            residual = res_temp;
        }

        // cout << abs(residual) << endl;
        bool residualCut = true;
        // bool residualCut = abs(residual) < 0.01;
        // bool chi2Cut = fGlbTrack->GetChi2() < 2;
        bool chi2Cut = fGlbTrack->GetPval() > 0.01;

        //// =================== RECO CHI2 cut
        if (chi2Cut && residualCut)
          MyReco("Chi2");

        //// =================== RECO CHI2 cut + multitrack
        if (chi2Cut && residualCut && nt > 1)
          MyReco("MChi2");

        // // =================== Chi2 cuts + multitrack + NO TW in multitracks
        if (chi2Cut && residualCut && nt > 1 && hasSameTwPoint.at(it) == false)
          MyReco("MChi2TWT");


        // // =================== Chi2 cuts + multitrack + NO TW in multitracks + n_tracks == n_twpoints
        if (chi2Cut && residualCut && nt > 1 && hasSameTwPoint.at(it) == false && nt_TW == myTWNtuPt->GetPointsN())
          MyReco("MChi2TWTngt");
        //Check if the vertex of the interaction is inside the target or not
        bool VTok = true;
        float fVtVtxTolerance = .05;
        for(int iVt = 0; iVt < myVtNtuVtx->GetVertexN(); ++iVt)
        {
          TAVTvertex* vt = myVtNtuVtx->GetVertex(iVt);
          if( !vt->IsBmMatched() )
            continue;

          TVector3 vtPos = GetGeoTrafo()->FromGlobalToTGLocal( GetGeoTrafo()->FromVTLocalToGlobal( vt->GetPosition() ) ); //vertex position in TG frame
          TVector3 tgSize = GetParGeoG()->GetTargetPar().Size;
          if( TMath::Abs(vtPos.X()) > tgSize.X()/2 + fVtVtxTolerance || TMath::Abs(vtPos.Y()) > tgSize.Y()/2 + fVtVtxTolerance || TMath::Abs(vtPos.Z()) > tgSize.Z()/2 + fVtVtxTolerance)
          {
            VTok = false;
            break;
          }
        }

        if (chi2Cut && residualCut && nt > 1 && hasSameTwPoint.at(it) == false && nt_TW == myTWNtuPt->GetPointsN() && VTok)
          MyReco("MChi2TWTngt_VTok");

        // // // =================== Chi2 cuts +  NO TW in multitracks
        // if (fGlbTrack->GetChi2() < 2 && abs(residual)  < 0.01 && hasSameTwPoint.at(it) == false)
        //   MyReco("ChiTWT");

        if (fFlagMC)
    {
      if (chi2Cut && residualCut && nt > 1 && hasSameTwPoint.at(it) == false && nt_TW == myTWNtuPt->GetPointsN() && VTok && isParticleBorninTG.at(it) == true)
        MyReco("MChi2TWTngt_originOk");
    }

        //// ===================ALL TRACKS
        MyReco("reco");

      ntracks++;

    } //********* end loop on global tracks ****************

    if (fFlagMC)
    {
    //Increment the number of track clones found in the event
      for( auto itZ : m_nClone ) //Loop on Z_true
      {
        for( auto itMCid : itZ.second ) //Loop on MCtrackID
        {
          if (itMCid.second > 1)
            n_clones[ itZ.first ] += itMCid.second - 1;
        }
      }

      // MCParticleStudies();
      //***** loop on every TAMCparticle:
      FillMCPartYields(); // N_ref
      if (istrueEvent)
        trueEvents ++;
      istrueEvent = false;
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

    // //vt mc cuts
    // MyRecoBooking("VT");

    // // MSD mc cuts
    // MyRecoBooking("MSD");

    // // VT + MSD mc cuts
    // MyRecoBooking("VTX_MSD");

    // //track quality 1
    // MyRecoBooking("Q1");

    // // VT +  chi2 cuts on track
    // MyRecoBooking("VTChi2");

    // // VT +  chi2 + multitrack cuts on track
    // MyRecoBooking("MVTChi2");

    // // chi2 + multitrack cuts on track  + no more tw points in tracks + MSD MC cut
    // MyRecoBooking("MChi2MSDTWT");
  }

  // chi2 cuts on track
  MyRecoBooking("Chi2");

  // chi2 cuts on track + multitrack
  MyRecoBooking("MChi2");

  // chi2 + multitrack cuts on track  + no more tw points in tracks
  MyRecoBooking("MChi2TWT");

  // // chi2  + no more tw points in tracks
  // MyRecoBooking("ChiTWT");


  // chi2 + multitrack cuts on track  + no more tw points in tracks + n_glb= n_twpoints
  MyRecoBooking("MChi2TWTngt");

  MyRecoBooking("MChi2TWTngt_VTok");
  MyRecoBooking("MChi2TWTngt_originOk");

  // all tracks
  MyRecoBooking("reco");
  // }
  // else
  // {

  //   // Cross section recostruction histos from REAL DATA
  //   MyRecoBooking("yield-trkREAL");
  // }
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
    fpNtuRecTw = new TAGdataDsc(new TATWntuPoint());
    gTAGroot->AddRequiredItem("twpt");
    myReader->SetupBranch(fpNtuRecTw);

    if (!fFlagMC)
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

  myTWNtuPt = (TATWntuPoint *)fpNtuRecTw->GenerateObject();
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

  // Set fixed region values
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
  for (int i = 1; i <= fPrimaryCharge; i++)
    n_clones[i] = 0;

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
  ((TH1D *)gDirectory->Get(luminosity_name.c_str()))->SetBinContent(1, Ntg * trueEvents); // recoEvents
  cout << "Reconstructed events: " << recoEvents << endl;
  cout << "True events for the analysis: " << trueEvents << endl;
  cout << "Total input events: " << nTotEv << endl;

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
    ((TH2D *)gDirectory->Get(path.c_str()))->Fill(Z_meas, Z_true);

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

      h = new TH1D("theta_", "", 150, 0, 90.); // bin width of 0.6 deg

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

vector<bool> GlobalRecoAnaGSI::CheckTwPointInMoreTracks()
{
  vector<Int_t> vectTwId;         // vector of the twpoint reco ID
  vector<Int_t> vecSameTWid;      // vector of the twpoint reco ID wich are the same in more than a track
  vector<bool> hasSameTWid;       // vector of bool of tracks with same tw point
  Int_t nt = myGlb->GetTracksN(); // number of reconstructed tracks for every event

  for (int it = 0; it < nt; it++)
  {
    fGlbTrack = myGlb->GetTrack(it);
    if (!fGlbTrack->HasTwPoint())
    {
      vectTwId.push_back(-1);
      continue;
    }

    TAGpoint *twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);

    if ((std::find(vectTwId.begin(), vectTwId.end(), twpoint->GetClusterIdx()) != vectTwId.end())) // if twpoint id is already in the vector
      vecSameTWid.push_back(twpoint->GetClusterIdx());
    vectTwId.push_back(twpoint->GetClusterIdx());
  }

  for (auto allTWid : vectTwId)
  {
    if ((std::find(vecSameTWid.begin(), vecSameTWid.end(), allTWid) != vecSameTWid.end()))
      hasSameTWid.push_back(1);
    else
      hasSameTWid.push_back(0);
  }

  return hasSameTWid;
}
vector<bool> GlobalRecoAnaGSI::CheckTwParticleOrigin()
{
  vector<Double_t> trackpos;         // vector of the twpoint origin of every track
  vector<bool> hasOrigininTG;     // if a track has a twpoint particle originated far from the TG
  Int_t nt = myGlb->GetTracksN(); // number of reconstructed tracks for every event

  for (int it = 0; it < nt; it++)
  {
    fGlbTrack = myGlb->GetTrack(it);
    if (fGlbTrack->HasTwPoint())
    {
      auto twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);
      auto initTWPosition = GetNtuMcTrk()->GetTrack(twpoint->GetMcTrackIdx(0))->GetInitPos().Z();

      trackpos.push_back(initTWPosition);
      continue;
    }
    else
      trackpos.push_back(-99);
}

for (auto alltrackpos : trackpos)
{
  if (alltrackpos > -(GetParGeoG()->GetTargetPar().Size.Z() / 2) && alltrackpos < (GetParGeoG()->GetTargetPar().Size.Z() / 2))
    hasOrigininTG.push_back(1);
  else
    hasOrigininTG.push_back(0);
}
return hasOrigininTG;
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
    point = fGlbTrack->GetPoint(i);

    if ((string)point->GetDevName() == "VT")
    {                        //... i take the vt cluster
      vector<Int_t> vecVT_z; // vector of all Z of a cluster of the vtx
      vecVT_z.clear();
      for (int j = 0; j < point->GetMcTracksN(); j++)
      {

        TAMCpart *particleMC = GetNtuMcTrk()->GetTrack(point->GetMcTrackIdx(j));
        vecVT_z.push_back(particleMC->GetCharge()); // i take all the charges of a cluster
      }
      vecVtZMC.push_back(vecVT_z); // and i put all in a vector
    }

    if ((string)point->GetDevName() == "MSD")
    {
      // cout << " it is a MSD" << endl;
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
  if (TAGrecoManager::GetPar()->IsRegionMc() == false)
  {
    Error("FillMCPartYields", "IsRegionMc() needed for the analysis!");
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
    // TVector3 crosspos = cross->GetPosition();       // Get CROSSx, CROSSy, CROSSz
    Int_t OldReg = cross->GetOldCrossN();
    Int_t NewReg = cross->GetCrossN();
    // Double_t time_cross = cross->GetTime();
    // TVector3 mom_cross = cross->GetMomentum();                                                                            // retrieves P at crossing
    // Double32_t Mass = cross->GetMass();                                                                                   // retrieves Mass of track
    // Double_t ekin_cross = sqrt(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2) + pow(Mass, 2)) - Mass; // Kinetic energy at crossing
    // Double_t beta_cross = sqrt(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2)) / (ekin_cross + Mass);

    // particle description
    TAMCpart *particle = myMcNtuPart->GetTrack(cross->GetTrackIdx() - 1); // retrievs TrackID
    Int_t particle_ID = cross->GetTrackIdx() - 1;                         // TrackID
    // Int_t target_region = fRegTG;
    auto Mid = particle->GetMotherID();
    // double mass = particle->GetMass(); // Get TRpaid-1
    auto Reg = particle->GetRegion();
    // auto finalPos = particle->GetFinalPos();
    // int baryon = particle->GetBaryon();
    // TVector3 initMom = particle->GetInitP();
    // double InitPmod = sqrt(pow(initMom(0), 2) + pow(initMom(1), 2) + pow(initMom(2), 2));
    // Float_t Ek_tr_tot = (sqrt(pow(InitPmod, 2) + pow(mass, 2)) - mass);
    // Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
    // Float_t Ek_true = Ek_tr_tot / (double)baryon;                           // MeV/n
    // Float_t theta_tr = particle->GetInitP().Theta() * (180. / TMath::Pi()); // in deg
    Float_t charge_tr = particle->GetCharge();
    Int_t oldPid = particle_ID;

    bool isParticleGood = false;
    bool gammaDecayHappened = false;
    if( particle->GetCharge() > 0 &&
        particle->GetCharge() <= fPrimaryCharge && // with reasonable charge
        (NewReg >= fRegFirstTWbar && NewReg <= fRegLastTWbar) && OldReg == fRegAirAfterTW) // it creaches the TW (one of the bar of the two layers - region from 81 to 120)
    {
      if( particle_ID == 0 ) //primary -> ok!
        isParticleGood = true;
      else if( Reg == fRegTG && Mid == 0 ) //born in target from primary
          isParticleGood = true;
      else //Check for radiative decay in FLUKA!
        isParticleGood = CheckRadiativeDecayChain(particle, &particle_ID);
    }

    TVector3 P_crossOld(-999,-999,-999);
    if ( isParticleGood )
    {
      // I want to measure the angle of emission of this particle:  i need to loop again the regions
      for (int iiCross = 0; iiCross < nCross; iiCross++)
      {
        // region description
        TAMCregion *cross2 = pNtuReg->GetRegion(iiCross); // Gets the i-crossing
        // TVector3 crosspos = cross->GetPosition();       // Get CROSSx, CROSSy, CROSSz
        Int_t OldReg2 = cross2->GetOldCrossN();
        Int_t NewReg2 = cross2->GetCrossN();

        // particle entering into target
        if (NewReg2 == fRegTG && OldReg2 == fRegAirBeforeTW)
        {
          P_beforeTG = cross2->GetMomentum();
        }

        if ((cross2->GetTrackIdx() - 1) == particle_ID)
        {
          // particle exit from target
          if (NewReg2 == fRegAirBeforeTW && OldReg2 == fRegTG)
          {
            P_cross = cross2->GetMomentum();
          }
        }

      }

      Th_BM = P_cross.Angle(P_beforeTG) * 180. / TMath::Pi();
      if (charge_tr > 0 && charge_tr <= fPrimaryCharge){
        FillYieldMC("yield-N_ref", charge_tr, charge_tr, Th_BM, Th_BM, false);
        istrueEvent = istrueEvent || true;
      }

      Th_BM = -999;
      P_cross.SetXYZ(-999., -999., -999.); // also MS contribution in target!
      P_beforeTG.SetXYZ(-999., -999., -999.);
    }
  }
}

bool GlobalRecoAnaGSI::isGoodReco(Int_t Id_part)
{
  if (TAGrecoManager::GetPar()->IsRegionMc() == false)
  {
    Error("isGoodReco", "IsRegionMc() needed for the analysis!");
    return false;
  }

  TAMCntuRegion *pNtuReg = GetNtuMcReg();
  Int_t nCross = pNtuReg->GetRegionsN();

  for (int iCross = 0; iCross < nCross; iCross++)
  {
    TAMCregion *cross = pNtuReg->GetRegion(iCross); // Gets the i-crossing
    if ((cross->GetTrackIdx() - 1) != Id_part)
    { // if it is not the particle I want to check, continue the loop
      continue;
    }

    Int_t OldReg = cross->GetOldCrossN();
    Int_t NewReg = cross->GetCrossN();

    TAMCpart *particle = myMcNtuPart->GetTrack(cross->GetTrackIdx() - 1); // retrievs TrackID
    Int_t particle_ID = cross->GetTrackIdx() - 1;                         // TrackID
    auto Mid = particle->GetMotherID();
    auto Reg = particle->GetRegion();

    if( particle->GetCharge() > 0 &&
        particle->GetCharge() <= fPrimaryCharge && // with reasonable charge
        (NewReg >= fRegFirstTWbar && NewReg <= fRegLastTWbar) && OldReg == fRegAirAfterTW) // it crosses the two planes of the TW and go beyond  (one of the bar of the two layers - region from 81 to 120)
    {
      bool isParticleGood = false;
      if( particle_ID == 0 ) //primary -> ok!
        isParticleGood = true;
      else if( Reg == fRegTG && Mid == 0) //born in target from primary
        isParticleGood = true;
      else //Check for radiative decay in FLUKA!
        isParticleGood = CheckRadiativeDecayChain(particle, &particle_ID);

      return isParticleGood;
    }
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
      TAGpoint *point = fGlbTrack->GetPoint(i);
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

void GlobalRecoAnaGSI::FragmentationStudies(string path, TAGtrack *fGlbTrack)
{
  for (int charge = 1; charge < 9; charge++)
  {
    if (Z_true == charge)
    {
      auto twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);
      auto initTWPosition = GetNtuMcTrk()->GetTrack(twpoint->GetMcTrackIdx(0))->GetInitPos().Z();
      auto initMostFreq = GetNtuMcTrk()->GetTrack(fGlbTrack->GetMcMainTrackId())->GetInitPos().Z();
      Double_t chi2 = fGlbTrack->GetChi2();
      string path_ = "";

      path_ = path + "/" + to_string(charge) + "/initposition_twpoint";
      ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(initTWPosition);

      path_ = path + "/" + to_string(charge) + "/initposition_mostfrequent";
      ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(initMostFreq);

      path_ = path + "/" + to_string(charge) + "/reducedChi2";
      ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(chi2);

      path_ = path + "/" + to_string(charge) + "/posvsreducedChi2";
      ((TH2D *)gDirectory->Get(path_.c_str()))->Fill(initTWPosition,chi2);

      path_ = path + "/" + to_string(charge) + "/residual";
      ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(residual);

      path_ = path + "/" + to_string(charge) + "/posvsres";
      ((TH2D *)gDirectory->Get(path_.c_str()))->Fill(initTWPosition, residual);

      path_ = path + "/" + to_string(charge) + "/trackquality";
      ((TH1D *)gDirectory->Get(path_.c_str()))->Fill(fGlbTrack->GetQuality());
    }
  }
}

void GlobalRecoAnaGSI::BookFragmentationStudies(string path)
{ // study of Z=8
  gDirectory->mkdir(path.c_str());
  gDirectory->cd(path.c_str());

  for (int charge = 1; charge < 9; charge++)
  {
    gDirectory->mkdir(to_string(charge).c_str());
    gDirectory->cd(to_string(charge).c_str());
    h = new TH1D("initposition_twpoint", "origin of the particle crossing TW for TW matched tracks; pos(Z) [cm]; events ", 3050, -105., 200.);
    h = new TH1D("initposition_mostfrequent", "origin of the most frequent particle of track; pos(Z) [cm]; events ", 3050, -105., 200.);
    h = new TH1D("reducedChi2", "\\Chi^2_{reduced} \\, for \\, every \\, track ; value", 400,0,20);
    h2 = new TH2D("posvsreducedChi2", "particle origin vs \\$Chi^2_{reduced}\\$ for every track ; pos(Z) [cm]; \\Chi^2_{reduced}", 100, -105., 200., 20, 0, 20);
    h = new TH1D("residual", "worst residual for every track fit - meas", 1000, -0.1,0.1);
    h2 = new TH2D("posvsres", "particle origin vs worst residual for every track fit ; pos(Z) [cm]; res [cm] ", 100, -105., 200., 200, -0.1, 0.1);
    h = new TH1D("trackquality", "Track quality", 11, 0,1.1);
    gDirectory->cd("..");
  }
  gDirectory->cd("..");
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

void GlobalRecoAnaGSI::MyReco(string path_name)
{
  string name = "";
  if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
  {
    name = "yield-N_" + path_name + "_Z_reco_Th_Reco";
    FillYieldReco(name, Z_meas, Th_recoBM); // all reconstructed tracks
  }

  if(fFlagMC)
  {
    if (isGoodReco(TrkIdMC))
    {
      if (Z_true > 0. && Z_true <= fPrimaryCharge)
      { 
        name = "yield-N_"+path_name+"GoodReco";
        FillYieldMC(name, Z_true, Z_meas, Th_BM, Th_recoBM, true);                              // N_GoodReco MC
        name = "MigMatrix" + path_name + "GoodReco";
        MigMatrixPlots(name, Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots
      }
    }
    if (Z_true > 0. && Z_true <= fPrimaryCharge){
      name = "yield-N_" + path_name + "AllReco";
      FillYieldMC(name, Z_true, Z_meas, Th_BM, Th_recoBM, true); // N_AllReco MC
    }

    if (Z_true > 0. && Z_true <= fPrimaryCharge){
      name = "MigMatrix" + path_name;
      MigMatrixPlots(name, Z_true, Z_meas, Th_BM, Th_recoBM, true); // migration matrix plots
    }

    if (Z_true > 0. && Z_true <= fPrimaryCharge)
    {
      name = "yield-N_" + path_name + "_Z_true_Th_Reco";
      FillYieldReco(name, Z_true, Th_recoBM); // all reconstructed tracks but with real Z
    }

    if (Z_meas > 0. && Z_meas <= fPrimaryCharge)
    {
      name = "yield-N_" + path_name + "_Z_meas_Th_True";
      FillYieldReco(name, Z_meas, Th_BM); // all reconstructed tracks but with real theta
    }

    if (Z_meas > 0. && Z_meas <= fPrimaryCharge && Z_meas == Z_true)
    {
      name = "yield-N_" + path_name + "_Z_measEqualTrue_Th_True";
      FillYieldReco(name, Z_meas, Th_BM); // all reconstructed tracks with z_reco = z_true with rea theta (for purity purposes)
    }

  name = "Z_" + path_name + "_match";
  FragmentationStudies(name, fGlbTrack);
  }
}

void GlobalRecoAnaGSI::MyRecoBooking(string path_name)
{
  string name = "";
  name = "yield-N_" + path_name + "_Z_reco_Th_Reco";
  BookYield(name, false);
  if( fFlagMC )
  {
    name = "yield-N_" + path_name + "_Z_true_Th_Reco";
    BookYield(name, false);
    name = "yield-N_" + path_name + "GoodReco";
    BookYield(name, true);
    name = "yield-N_" + path_name + "AllReco";
    BookYield(name, true);
    name = "yield-N_" + path_name + "_Z_meas_Th_True";
    BookYield(name, false);
    name = "yield-N_" + path_name + "_Z_measEqualTrue_Th_True";
    BookYield(name, false);
    name = "MigMatrix" + path_name;
    BookMigMatrix(name, true);
    name = "MigMatrix" + path_name + "GoodReco";
    BookMigMatrix(name, true);
    name = "Z_" + path_name + "_match";
    BookFragmentationStudies(name);
  }
}



Bool_t GlobalRecoAnaGSI::CheckRadiativeDecayChain(TAMCpart* part, Int_t* part_ID)
{
  TAMCpart* partMoth = myMcNtuPart->GetTrack( part->GetMotherID() );
  if ( !partMoth )
    return false;
  
  bool isGammaDecay = false;

  if( partMoth->GetCharge() == part->GetCharge() && //if Z and A are the same
      partMoth->GetBaryon() == part->GetBaryon() )
  { //check if there is a gamma from same particle
    for(int iGamma =0; iGamma<myMcNtuPart->GetTracksN(); ++iGamma)
    {
      TAMCpart* maybeGamma = myMcNtuPart->GetTrack(iGamma);
      if( maybeGamma->GetMotherID() == part->GetMotherID() && maybeGamma->GetFlukaID() == 7 )
      { //there is a gamma with same mother id!
        isGammaDecay = true; //Particle comes from radiative decay! -> ok!
        break;
      }
    }
  }
  else
    return false;
  
  if( isGammaDecay )
  {
    if( partMoth->GetMotherID() == 0 && partMoth->GetRegion() == fRegTG ) // mother particle comes from primary and is born in target
    {
      TVector3 posMoth = GetGeoTrafo()->FromGlobalToTGLocal( partMoth->GetFinalPos() ); // final position of mother particle in TG frame
      TVector3 tgSize = GetParGeoG()->GetTargetPar().Size;
      if( TMath::Abs(posMoth.X()) > tgSize.X()/2 || TMath::Abs(posMoth.Y()) > tgSize.Y()/2 || TMath::Abs(posMoth.Z()) > tgSize.Z()/2 )
        *part_ID = part->GetMotherID(); //if mother particle "dies" outside target, change particle ID in output for P calculation

      return true;
    }
    else
      return CheckRadiativeDecayChain(partMoth, part_ID); //Check next step of chain
  }
  else
    return false;
}