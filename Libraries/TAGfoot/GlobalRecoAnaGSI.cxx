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
  nchargeok_it = 0;
  nchargeok_it_clu = 0;
  nchargeok_msd = 0;
  nchargeok_msd_clu = 0;
  nchargeok_tw = 0;
  nchargeall_vt = 0;
  nchargeall_vt_clu = 0;
  nchargeall_it = 0;
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
  purity_cut=0.51;
  clean_cut=1.;
  nTotEv=innTotEv;

  outfile = fileNameout;

  Th_true = -999.; //from TAMCparticle
  Th_meas = -999.; //from TWpoint
  Th_reco = -999.; //from global tracking
  Th_recoBM = -999.; //from global tracking wrt BM direction


}

GlobalRecoAnaGSI::~GlobalRecoAnaGSI()
{
}

void GlobalRecoAnaGSI::LoopEvent() {

  if(FootDebugLevel(1))
    cout<<"GlboalRecoAna::LoopEvent start"<<endl;
  if (fSkipEventsN>0)
  {
  currEvent=fSkipEventsN;
  nTotEv+=  fSkipEventsN;
  }
  else currEvent=0;

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
  while(gTAGroot->NextEvent()) { //for every event
    //fFlagMC = false;     //N.B.: for MC FAKE REAL

  if (currEvent % frequency == 0 || FootDebugLevel(1))
    cout << "Load Event: " << currEvent << endl;

  int evtcutstatus = ApplyEvtCuts();
  if (evtcutstatus) {
    ++currEvent;
    continue;
  }

      Int_t nt = myGlb->GetTracksN(); // number of reconstructed tracks for every event
      if (nt > 0)
        recoEvents++;

      TAWDntuTrigger *wdTrig = 0x0;
      if (fFlagMC == false)
      {
        wdTrig = (TAWDntuTrigger *)fpNtuWDtrigInfo->GenerateObject(); // trigger from hardware
      }

      m_nClone.clear();
      //*********************************************************************************** begin loop on global tracks **********************************************
      for (int it = 0; it < nt; it++)
      {
        fGlbTrack = myGlb->GetTrack(it);
        if (!fGlbTrack->HasTwPoint()) {
          ntracks++;
          continue;
        }
        // Set the reconstructed quantities
        RecoGlbTrkLoopSetVariables();

        if (fFlagMC)
        {
          MCGlbTrkLoopSetVariables();

          //study of MC clones: different glb tracks with the same MC ID
          m_nClone[Z_true][TrkIdMC]++;
          if (m_nClone[Z_true][TrkIdMC] > 1)
            n_clones[Z_true] = m_nClone[Z_true][TrkIdMC];

          if (isGoodReco(TrkIdMC))
          {
            FillYieldMC("yield-N_GoodReco", Z_true, Th_true, Ek_tot); // N_GoodReco
          }
          FillYieldMC("yield-N_AllReco", Z_true, Th_true, Ek_tot); // N_AllReco

          FillYieldReco("yield-N_recoTracks", Z_meas, 0, Th_recoBM); // all reconstructed tracks
        }

         


        


        // //fill the yields plots
        // if (fFlagMC){
        // FillMCGlbTrkYields();
        // }

        // else
        //   FillDataGlbTrkYields();
        ntracks++;

      } //********* end loop on global tracks ****************

      if (fFlagMC){
        //MCParticleStudies(); 
        //***** loop on every TAMCparticle:
        FillMCPartYields();  //N_ref
      }

      ++currEvent;
      if (currEvent == nTotEv)
        break;
    } // end of loop event

  return;
}

void GlobalRecoAnaGSI:: Booking(){
//binning of theta, ekin, A
th_nbin = 20;
theta_binning = new double *[th_nbin];
for (int i = 0; i<th_nbin; i++) {
  theta_binning[i] = new double [2];
  theta_binning[i][0] = double (i)*0.5;
  theta_binning[i][1] = double (i+1)*0.5;
  //cout << " theta binning "<< i << " "<< theta_binning[i][0] << " "<< theta_binning[i][1];
}
//theta_binning[60] = new double [2];    //!hard coded
//theta_binning[60][0] = double(30);
//theta_binning[60][1] =  double(90);
//cout << " theta binning "<< 10 << " "<< theta_binning[10][0] << " "<< theta_binning[10][1];

ek_nbin = 1;
ek_binning = new double *[ek_nbin];
for (int i = 0; i<ek_nbin; i++) {
  ek_binning[i] = new double [2];
  ek_binning[i][0] = double (i*100);
  ek_binning[i][1] = double ((i+1)*20000);
}
  //ek_binning[4] = new double [2];
  //ek_binning[4][0] = double(400);
  //ek_binning[4][1] =  double(1000);

mass_nbin = 1;
mass_binning = new double *[mass_nbin];
for (int i = 0; i<mass_nbin; i++) {
  mass_binning[i] = new double [2];
  mass_binning[i][0] = double (i*2);
  mass_binning[i][1] = double ((i+1)*100);
}


// Cross section recostruction histos MC
if(fFlagMC){

BookYield("yield-N_ref", false);
BookYield("yield-N_GoodReco", false);
BookYield("yield-N_AllReco", false);
BookYield("yield-N_recoTracks", false);

// BookYield("yield-trkMC", true);

// // Cross section recostruction histos MC + GHOST HITS FIXED
// BookYield ("yield-trkGHfixMC", true);

// // Cross section recostruction histos MC + ALL TW HITS FIXED
// BookYield ("yield-trkcutsMC", true);

// // Cross section recostruction histos from REAL DATA
// BookYield ("yield-trkReco");

// // Cross section TRUE histos
// BookYield ("yield-true_cut");

// // Cross section TRUE histos DETECTABLE (PARTICLES WHICH REACH THE TW)
// BookYield ("yield-true_DET");

// // Cross section TRUE histos reco real
// TString name = GetTitle();
// Int_t pos = name.Last('.');
// string name_ = "yield-trkREAL";
// name_ += name[pos-1];
// BookYield (name_.c_str());
} else {

// Cross section recostruction histos from REAL DATA
BookYield ("yield-trkREAL");
}
return;
}


int GlobalRecoAnaGSI::ApplyEvtCuts(){
  if(FootDebugLevel(1))
    cout<<"GlobalRecoAnaGSI::ApplyEvtCuts start"<<endl;

  if (myBMNtuTrk->GetTracksN() !=1)
    return 1;

  return 0;
}



void GlobalRecoAnaGSI::FillMCGlbTrkYields(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAnaGSI::FillMCGlbTrkYields start"<<endl;

  //-------------------------------------------------------------
  //--Yield for CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA
  if ( Z_true >0. && Z_true <= primary_cha /*&& TriggerCheckMC() == true*/)
    FillYieldReco("yield-trkMC",Z_true,Z_meas,Th_true );

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
          if (Z_true > 0. && Z_true <= primary_cha /*&& TriggerCheckMC() == true*/)
          {
             FillYieldReco("yield-trkcutsMC", Z_true, Z_meas, Th_true);
             //((TH1D *)gDirectory->Get("ThReco_fragMC"))->Fill(Th_recoBM);
          }
        }
      }
    }
  }

  // //-------------------------------------------------------------
  // //--CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA + GHOST HITS FIX : i don't want not fragmented primary
  // if (N_TrkIdMC_TW == 1)
  //   if (Z_true >0. && Z_true <= primary_cha /*&& TriggerCheckMC() == true*/)
  //     FillYieldReco("yield-trkGHfixMC",Z_true,Z_meas,Th_true );

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation for trigger efficiency   (comparing triggercheck with TAWDntuTrigger )
  if (Z_meas >0. && Z_meas <= primary_cha /*&& TriggerCheck() == true*/)
    FillYieldReco("yield-trkReco",Z_meas,0,Th_recoBM );

  //-------------------------------------------------------------
  //--CROSS SECTION reco
  if (Z_meas >0. && Z_meas <= primary_cha /*&& TriggerCheck() == true*/){

    TString name = GetTitle();
    Int_t pos = name.Last('.');
    string name_ = "yield-trkREAL";
    name_ += name[pos-1];
    FillYieldReco(name_.c_str(),Z_meas,0,Th_recoBM );












  }



return;
}

void GlobalRecoAnaGSI::FillDataGlbTrkYields(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAnaGSI::FillDataGlbTrkYields start"<<endl;

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation- RECO PARAMETERS FROM REAL DATA : i don't want not fragmented primary
  if ( Z_meas >0. && Z_meas <= primary_cha /*&& wdTrig -> GetTriggersStatus()[1] == 1*/)     //fragmentation hardware trigger ON
  //&& TriggerCheck(fGlbTrack) == true  //NB.: for MC FAKE REAL
   {
    //cout << "inside " <<endl;
    FillYieldReco("yield-trkREAL",Z_meas,0,Th_recoBM );
    //cout << "thBM: "<< Th_recoBM <<endl;
    // ((TH1D*)gDirectory->Get("ThReco_frag"))->Fill(Th_recoBM);
    // ((TH1D*)gDirectory->Get("Charge_trk_frag"))->Fill(Z_meas);
  }
  return;
}


void GlobalRecoAnaGSI::SetupTree(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAnaGSI::SetupTree start"<<endl;

  myReader = new TAGactTreeReader("myReader");
  fpNtuGlbTrack = new TAGdataDsc("glbTrack",new TAGntuGlbTrack());
  gTAGroot->AddRequiredItem("glbTrack");
  myReader->SetupBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());

  if(TAGrecoManager::GetPar()->IncludeBM()){
  fpNtuTrackBm = new TAGdataDsc("bmtrack" , new TABMntuTrack());
  gTAGroot->AddRequiredItem("bmtrack");
  myReader->SetupBranch(fpNtuTrackBm);

  }

  if(TAGrecoManager::GetPar()->IncludeVT()){
    fpNtuClusVtx = new TAGdataDsc("vtclus",new TAVTntuCluster());
    fpNtuTrackVtx = new TAGdataDsc("vttrack",new TAVTntuTrack());
    fpNtuVtx = new TAGdataDsc("vtvtx",new TAVTntuVertex());
    gTAGroot->AddRequiredItem("vtclus");
    gTAGroot->AddRequiredItem("vttrack");
    gTAGroot->AddRequiredItem("vtvtx");
    myReader->SetupBranch(fpNtuClusVtx);
    myReader->SetupBranch(fpNtuTrackVtx);
    myReader->SetupBranch(fpNtuVtx);
    if(fFlagMC){
      fpNtuMcVt = new TAGdataDsc(FootDataDscMcName(kVTX),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcvt");
      myReader->SetupBranch(fpNtuMcVt, FootBranchMcName(kVTX));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeIT()){
    fpNtuClusIt = new TAGdataDsc(new TAITntuCluster());
    gTAGroot->AddRequiredItem("itclus");
    myReader->SetupBranch(fpNtuClusIt);
    if(fFlagMC){
      fpNtuMcIt = new TAGdataDsc(FootDataDscMcName(kITR), new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcit");
      myReader->SetupBranch(fpNtuMcIt, FootBranchMcName(kITR));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeMSD()){
    fpNtuClusMsd = new TAGdataDsc(new TAMSDntuCluster());
    gTAGroot->AddRequiredItem("msdclus");
    myReader->SetupBranch(fpNtuClusMsd);
    fpNtuRecMsd = new TAGdataDsc(new TAMSDntuPoint());
    gTAGroot->AddRequiredItem("msdpoint");
    myReader->SetupBranch(fpNtuRecMsd);
    if(fFlagMC){
      fpNtuMcMsd = new TAGdataDsc(FootDataDscMcName(kMSD),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcmsd");
      myReader->SetupBranch(fpNtuMcMsd, FootBranchMcName(kMSD));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeTW()){
    fpNtuRecTw = new TAGdataDsc(new TATWntuPoint());
    gTAGroot->AddRequiredItem("twpt");
    myReader->SetupBranch(fpNtuRecTw);

    if (fFlagMC == false){
      fpNtuWDtrigInfo = new TAGdataDsc(new TAWDntuTrigger());
      gTAGroot->AddRequiredItem("WDtrigInfo");
      myReader->SetupBranch(fpNtuWDtrigInfo);
    }
    if(fFlagMC){
      fpNtuMcTw = new TAGdataDsc(FootDataDscMcName(kTW),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mctw");
      myReader->SetupBranch(fpNtuMcTw, FootBranchMcName(kTW));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeCA()){
    fpNtuClusCa = new TAGdataDsc(new TACAntuCluster());
    gTAGroot->AddRequiredItem("caclus");
    myReader->SetupBranch(fpNtuClusCa);
    if(fFlagMC){
      fpNtuMcCa = new TAGdataDsc(FootDataDscMcName(kCAL),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcca");
      myReader->SetupBranch(fpNtuMcCa, FootBranchMcName(kCAL));
    }
  }

  if(fFlagMC){
    fpNtuMcEvt = new TAGdataDsc(new TAMCntuEvent());
    fpNtuMcTrk = new TAGdataDsc(new TAMCntuPart());
    gTAGroot->AddRequiredItem("mcevt");
    gTAGroot->AddRequiredItem("mctrack");
    myReader->SetupBranch(fpNtuMcEvt);
    myReader->SetupBranch(fpNtuMcTrk);
    if(TAGrecoManager::GetPar()->IsRegionMc()){
      fpNtuMcReg = new TAGdataDsc(new TAMCntuRegion());
      gTAGroot->AddRequiredItem("mcreg");
      myReader->SetupBranch(fpNtuMcReg);
    }
  }

  gTAGroot->AddRequiredItem("myReader");
  gTAGroot->Print();

  return;
}


void GlobalRecoAnaGSI::BeforeEventLoop(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAnaGSI::BeforeEventLoop start"<<endl;


  ReadParFiles();
  CampaignChecks();
  SetupTree();
  cout <<"fSkipEventsN :" << fSkipEventsN <<endl;
  myReader->Open(GetName(), "READ", "tree");
  if (fSkipEventsN > 0){
       myReader->Reset(fSkipEventsN);
     }

  file_out = new TFile(GetTitle(),"RECREATE");

  cout<<"Going to create "<<GetTitle()<<" outfile "<<endl;
//  SetRunNumber(runNb); //serve veramente?
  //  myReader->GetTree()->Print();

  //initialization of several objects needed for the analysis
  gTAGroot->BeginEventLoop();
  mass_ana=new GlobalRecoMassAna();
  myGlb = (TAGntuGlbTrack*)fpNtuGlbTrack->GenerateObject();
  //myVtNtuClus = (TAVTntuCluster*)fpNtuClusVtx->GenerateObject();
  myVtNtuVtx = (TAVTntuVertex*)fpNtuVtx->GenerateObject();

  myTWNtuPt = (TATWntuPoint*)fpNtuRecTw->GenerateObject();
  myMSDNtuHit = (TAMSDntuHit*)fpNtuRecTw->GenerateObject();
  pCaNtuClu = (TACAntuCluster*)fpNtuClusCa->GenerateObject();
  myBMNtuTrk = (TABMntuTrack*) fpNtuTrackBm->GenerateObject();

  if(fFlagMC){
    myMcNtuEvent = (TAMCntuEvent*)fpNtuMcEvt->GenerateObject();
    myMcNtuPart = (TAMCntuPart*)fpNtuMcTrk->GenerateObject();
  }else{
    wdTrig = (TAWDntuTrigger*)fpNtuWDtrigInfo->GenerateObject();
  }


  //set variables
  primary_cha=GetParGeoG()->GetBeamPar().AtomicNumber;
  Double_t beam_mass_number=GetParGeoG()->GetBeamPar().AtomicMass*TAGgeoTrafo::GetMassFactorMeV(); //primary mass number in mev
  Double_t beam_energy=GetParGeoG()->GetBeamPar().Energy*GetParGeoG()->GetBeamPar().AtomicMass*TAGgeoTrafo::GevToMev(); //Total kinetic energy (MeV)
  Double_t beam_speed = sqrt( beam_energy*beam_energy + 2.*beam_mass_number*beam_energy )/(beam_mass_number + beam_energy)*TAGgeoTrafo::GetLightVelocity(); //cm/ns
  primary_tof=(fpFootGeo->GetTGCenter().Z()-fpFootGeo->GetSTCenter().Z())/beam_speed; //ns
  Booking();
  pure_track_xcha.clear();
  pure_track_xcha.resize(primary_cha+1,std::make_pair(0,0));
  Ntg=GetParGeoG()->GetTargetPar().Density*TMath::Na()*GetParGeoG()->GetTargetPar().Size.Z()/GetParGeoG()->GetTargetPar().AtomicMass;

  if(FootDebugLevel(1)) {
    cout<<"primary_cha="<<primary_cha<<"  beam_mass_number="<<beam_mass_number<<"  beam_energy="<<beam_energy<<"  beam_speed="<<beam_speed<<"  primary_tof="<<primary_tof<<endl;
    cout<<"N_target="<< Ntg << endl;
    cout <<"target density="<< GetParGeoG()->GetTargetPar().Density << endl;
    cout << "target z=" << GetParGeoG()->GetTargetPar().Size.Z() << endl;
    cout << "target A=" << GetParGeoG()->GetTargetPar().AtomicMass << endl;
  }

  n_clones.clear();
  return;
}

void GlobalRecoAnaGSI::AfterEventLoop(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAnaGSI::AfterEventLoop start"<<endl;

  //stamp luminosity
  string luminosity_name="";
    if (fFlagMC == true){
      luminosity_name = "luminosityMC";
    }else {  // real data
      luminosity_name = "luminosityREAL";
    }
    h = new TH1D(luminosity_name.c_str(), "", 1, 0., 1.);
    ((TH1D *)gDirectory->Get(luminosity_name.c_str()))->SetBinContent(1, Ntg * recoEvents);
    cout << "Reconstructed events: " << recoEvents << endl;

  if (fFlagMC){
      h = new TH1D("trkclone", "number of tracks with the same mc id", 8, 0.5, 8.5);
      for (int i = 1; i <= primary_cha; i++){
      ((TH1D *)gDirectory->Get("trkclone"))->SetBinContent(i, n_clones[i]);
      }
  } 


  gTAGroot->EndEventLoop();



  cout <<"Writing..." << endl;
  file_out->Write();

  cout <<"Closing..." << endl;
  file_out->Close();

  return;
}


void GlobalRecoAnaGSI::FillYieldReco(string folderName, Int_t Z,Int_t Z_meas, Double_t Th, Double_t Ek){
        string path = folderName+"/charge";
        ((TH1D*)gDirectory->Get(path.c_str()))->Fill(Z);
      //((TH1D*)gDirectory->Get(Form("xsec_rec/Z_%d-%d_%d/Theta_meas",Z_meas,Z_meas,(Z_meas+1))))->Fill(Th_meas);
      //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/Ek_meas",Z_meas,Z_meas,(Z_meas+1))))->Fill(Ek_meas*fpFootGeo->GevToMev());
      //((TH2D*)gDirectory->Get(Form("xsec_rec/Z_%d-%d_%d/Theta_vs_Ekin",Z_meas,Z_meas,(Z_meas+1))))->Fill(Ek_meas*fpFootGeo->GevToMev(),Th_meas);



        for (int i = 0; i<th_nbin; i++) {
         if(Th>=theta_binning[i][0] && Th<theta_binning[i][1]){
            //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Ek_bin",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(Ek_meas*fpFootGeo->GevToMev());
            //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Mass_bin",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(M_meas);

            //if (M_meas <0) cout <<" M MEAS NEGATIVE" << endl;
            //string pathmigz = "xsecrec-trkMC/Z_" + to_string(Z_true) +"-"+to_string(Z_true)+"_"+to_string(Z_true+1)+"/migMatrix";
            //((TH2D*)gDirectory->Get(pathmigz.c_str()))->Fill(Z_true,Z_meas);
            path = folderName+"/Z_" + to_string(Z-1) +"#"+to_string(Z-0.5)+"_"+to_string(Z+0.5)+"/theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1])+"/theta_";
            ((TH1D*)gDirectory->Get(path.c_str()))->Fill(Th);

            string path_matrix = folderName+"/Z_" + to_string(Z-1) +"#"+to_string(Z-0.5)+"_"+to_string(Z+0.5)+"/theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1])+"/migMatrix_Z";
            if (!(Z_meas==0))
            ((TH2D*)gDirectory->Get(path_matrix.c_str()))->Fill(Z,Z_meas);


           /*for (int j=0; j < ek_nbin; j++) {
             if((Ek_meas*fpFootGeo->GevToMev())>=ek_binning[j][0] && (Ek_meas*fpFootGeo->GevToMev())<ek_binning[j][1]) {

                //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/Mass_bin_",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]))))->Fill(M_meas);

                  for (int k=0; k < mass_nbin; k++) {
                   if(M_meas>=mass_binning[k][0] && M_meas <mass_binning[k][1]) {

                     ((TH1D*)gDirectory->Get(Form("xsecrec-trkMC/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(M_meas);



                    }
                  }


              }

            }*/
          }
        //}
        }
}

void GlobalRecoAnaGSI::FillYieldMC(string folderName, Int_t charge_tr, Double_t theta_tr, Double_t Ek){

                          /*
                          ((TH1D*)gDirectory->Get("MC_check/Charge_MC_tg")) -> Fill(particle-> GetCharge());
                          ((TH1D*)gDirectory->Get("MC_check/Mass_MC_tg")) -> Fill(particle-> GetMass());
                          ((TH1D*)gDirectory->Get("MC_check/Ek_tot_MC_tg")) -> Fill(Ek_tr_tot);
                          ((TH1D*)gDirectory->Get("MC_check/InitPosZ_MC_tg")) -> Fill(particle-> GetInitPos().Z() );
                          ((TH1D*)gDirectory->Get("MC_check/FinalPosZ_MC_tg")) -> Fill(particle-> GetFinalPos().Z() );
                          ((TH1D*)gDirectory->Get("MC_check/TrkLength_MC_tg")) -> Fill(particle-> GetTrkLength());
                          ((TH1D*)gDirectory->Get("MC_check/Type_MC_tg")) -> Fill(particle-> GetType());
                          ((TH1D*)gDirectory->Get("MC_check/FlukaID_MC_tg")) -> Fill(particle->  GetFlukaID());
                          ((TH1D*)gDirectory->Get("MC_check/MotherID_MC_tg")) -> Fill(particle->  GetMotherID());
                          ((TH1D*)gDirectory->Get("MC_check/Theta_MC_tg")) -> Fill(particle->  GetInitP().Theta()*180./TMath::Pi());

                          */
                          string path = folderName + "/charge";
                          ((TH1D*)gDirectory->Get(path.c_str()))->Fill(charge_tr);

                          for (int i = 0; i<th_nbin; i++) {

                          if(theta_tr>=theta_binning[i][0] && theta_tr<theta_binning[i][1]){

                             path = folderName+"/Z_" + to_string(int(charge_tr)-1) +"#"+to_string(int(charge_tr)-0.5)+"_"+to_string(int(charge_tr)+0.5)+"/theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1])+"/theta_";
                            ((TH1D*)gDirectory->Get(path.c_str()))->Fill(theta_tr);
                            //if ((theta_binning[i][1] <=1) && charge_tr == 6) cout <<"MC : " <<particle->GetFlukaID() << endl;

                            //((TH1D*)gDirectory->Get(Form("xsecrec-true_cut/Z_%d-%d_%d/theta_%d-%d_%d/theta_",int(charge_tr),int(charge_tr),int(charge_tr+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(theta_tr);
                            /*for (int j=0; j < ek_nbin; j++) {


                              if(Ek_tr_tot >=ek_binning[j][0] && Ek_tr_tot<ek_binning[j][1]) {

                                    for (int k=0; k < mass_nbin; k++) {
                                      Float_t mass_tr = particle -> GetMass();
                                    if(mass_tr>=mass_binning[k][0] && mass_tr <mass_binning[k][1]) {

                                      ((TH1D*)gDirectory->Get(Form("xsecrec-true_cut/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",int(charge_tr),int(charge_tr),int(charge_tr+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(mass_tr);



                                      }
                                    }


                                }

                              }*/
                            }
                          }


}


void GlobalRecoAnaGSI:: BookYield (string path, bool enableMigMatr) {

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAnaGSI::BookYield start"<<endl;

  gDirectory->mkdir(path.c_str());
  gDirectory->cd(path.c_str());
  h = new TH1D("charge","",8, 0.5 ,8.5);

  for(int iz=1; iz<=primary_cha; iz++){
    string name = "Z_" + to_string(iz-1) +"#"+to_string(iz-0.5)+"_"+to_string(iz+0.5);
    gDirectory->mkdir(name.c_str());
    gDirectory->cd(name.c_str());
    name = "";

    for (int i = 0; i<th_nbin; i++) {
      string path = "theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1]);
      gDirectory->mkdir(path.c_str());
      gDirectory->cd(path.c_str());

      h = new TH1D("theta_","",180, 0 ,90.);
      if (enableMigMatr)
      h2 = new TH2D("migMatrix_Z", "migMatrix_Z; Z_{true}; Z_{reco}",8,0.5,8.5,8,0.5,8.5);

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
    //  Int_t TG_region = -1;         //! hard coded
    //  if(fExpName.IsNull())
    //  TG_region = 59; // true in newgeom setup
    //  else if(!fExpName.CompareTo("GSI/") || !GlobalRecoAna::fExpName.CompareTo("GSI_MC/"))
    //  TG_region = 48;  //  GSI-2019
    //  else if(!fExpName.CompareTo("CNAO2020/"))
    //  TG_region = 50;  //  CNAO-2020
    //  else if(!fExpName.CompareTo("GSI2021_MC/"))
    //  TG_region = 50; //   GSI2021_MC

    //-------------  MC TOTAL CROSS SECTION
    // if the particle is generated in the target and it is the fragment of a primary
    // if Z<8 and A<30, so if it is a fragment (not the primitive projectile, nor detector fragments)
    // ifenough energy/n to go beyond the target
    if (Mid == 0 && Reg == GetParGeoG()->GetRegTarget() && particle->GetCharge() > 0 && particle->GetCharge() <= primary_cha && Ek_true > 100)
      FillYieldMC("yield-true_cut", charge_tr, theta_tr, Ek_tr_tot);

    //-------------  MC FIDUCIAL CROSS SECTION (<8 deg)
    if (Mid == 0 && Reg == GetParGeoG()->GetRegTarget() && particle->GetCharge() > 0 && particle->GetCharge() <= primary_cha && Ek_true > 100 && theta_tr <= 8.)
      FillYieldMC("yield-true_DET", charge_tr, theta_tr, Ek_tr_tot);

    //-------------  MC FIDUCIAL CROSS SECTION (<2 deg)
    /*
    if (  Mid==0 && Reg == GetParGeoG()->GetRegTarget() && particle->GetCharge()>0 && particle->GetCharge()<=primary_cha && Ek_true>100
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
      if (fpNtuMcReg->GetCharge() > 0 && fpNtuMcReg->GetMass() > 0 && fpNtuMcReg->GetOldCrossN() == GetParGeoG()->GetRegTarget() && fpNtuMcReg->GetCrossN() == GetParGeoG()->GetRegAirPreTW())
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
  // take the direction of the beam in global SdR
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

void GlobalRecoAnaGSI::MCGlbTrkLoopSetVariables()
{

  if (FootDebugLevel(1))
    cout << "GlobalRecoAna::MCGlbTrkLoopSetVariables start" << endl;

  Tof_true = -1.;
  Tof_startmc = -1.;
  Beta_true = -1.;
  P_cross.SetXYZ(-999., -999., -999.); // also MS contribution in target!

  TrkIdMC = fGlbTrack->GetMcMainTrackId(); // associo l'IdMC alla particella più frequente della traccia  (prima era ottenuto tramite studio purity)
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
      for (int icr = 0; icr < GetNtuMcReg()->GetRegionsN(); icr++)
      {
      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
      if ((fpNtuMcReg->GetTrackIdx() - 1) == TrkIdMC)
      {
          // particle exit from target
          if (fpNtuMcReg->GetCrossN() == GetParGeoG()->GetRegAirPreTW() && fpNtuMcReg->GetOldCrossN() == GetParGeoG()->GetRegTarget())
          {
             P_cross = fpNtuMcReg->GetMomentum();
             Tof_startmc = fpNtuMcReg->GetTime() * fpFootGeo->SecToNs();
          }
          // particle crossing in the first TW layer
          if (fpNtuMcReg->GetCrossN() <= GetParGeoTw()->GetRegStrip(1, GetParGeoTw()->GetNBars() - 1) && fpNtuMcReg->GetCrossN() >= GetParGeoTw()->GetRegStrip(1, 0))
          {
             Tof_true = fpNtuMcReg->GetTime() * fpFootGeo->SecToNs();
             Beta_true = fpNtuMcReg->GetMomentum().Mag() / sqrt(fpNtuMcReg->GetMass() * fpNtuMcReg->GetMass() + fpNtuMcReg->GetMomentum().Mag() * fpNtuMcReg->GetMomentum().Mag());
          }
          // particle crossing in the calo
          if (fpNtuMcReg->GetCrossN() >= GetParGeoCa()->GetRegCrystal(0) && fpNtuMcReg->GetCrossN() <= GetParGeoCa()->GetRegCrystal(GetParGeoCa()->GetCrystalsN() - 1))
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

      Th_true = P_true.Theta() * 180. / TMath::Pi();
      // cout << "theta true: "<< Th_true <<endl;
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

  for (int iCross = 0; iCross < nCross; iCross++)
  {
    TAMCregion *cross = pNtuReg->GetRegion(iCross); // Gets the i-crossing
    TVector3 crosspos = cross->GetPosition();       // Get CROSSx, CROSSy, CROSSz
    Int_t OldReg = cross->GetOldCrossN();
    Int_t NewReg = cross->GetCrossN();
    Double_t time_cross = cross->GetTime();
    TVector3 mom_cross = cross->GetMomentum();                                                                                // retrieves P at crossing
    Double32_t Mass = cross->GetMass();                                                                                       // retrieves Mass of track
    Double_t ekin_cross = pow(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2) + pow(Mass, 2), 0.5) - Mass; // Kinetic energy at crossing
    Double_t beta_cross = pow(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2), 0.5) / (ekin_cross + Mass);

    // if ((cross->GetTrackIdx() - 1) != Id_part)
    // { // if it is not the particle I want to check, continue the loop
    //   continue;
    // }

    TAMCpart *particle = myMcNtuPart->GetTrack(cross->GetTrackIdx() - 1); // retrievs TrackID
    Int_t particle_ID = cross->GetTrackIdx() - 1;                     // TrackID
    Int_t target_region = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetRegTarget();
    Int_t primary_charge = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber;
    TString regvtxname("VTXP3");
    Int_t last_vtx_plane_region = GetParGeoG()->GetCrossReg(regvtxname);
    Int_t AirAfterTW_region = GetParGeoG()->GetRegAirTW();
    TString regname("AIR1");
    Int_t RegAirAfterVtx = GetParGeoG()->GetCrossReg(regname);
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
    Float_t theta_tr = particle->GetInitP().Theta() * (180. / TMath::Pi()); // in deg
    Float_t charge_tr = particle->GetCharge();
    if (
        (particle_ID == 0                      // if the particle is a primary OR
         || (Mid == 0 && Reg == target_region) // if the particle is a primary fragment born in the target
         ) &&
        particle->GetCharge() > 0 &&
        particle->GetCharge() <= primary_charge && // with reasonable charge
        // Ek_true > 100 &&                           // with enough initial energy to go beyond the vtx    //! is it true?
        // theta_tr <= 30. && // inside the angular acceptance of the vtxt   //!hard coded for GSI2021_MC
        (OldReg >= GetParGeoTw()->GetRegStrip(0, 0) && OldReg <= GetParGeoTw()->GetRegStrip(1, GetParGeoTw()->GetNBars() - 1)) && NewReg == AirAfterTW_region // it crosses the two planes of the TW and go beyond  (one of the bar of the two layers - region from 81 to 120)

    )
    {
      FillYieldMC("yield-N_ref", charge_tr, theta_tr, Ek_tr_tot);
    }
  }

  return;
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
    TVector3 mom_cross = cross->GetMomentum();                                                                                // retrieves P at crossing
    Double32_t Mass = cross->GetMass();                                                                                       // retrieves Mass of track
    Double_t ekin_cross = pow(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2) + pow(Mass, 2), 0.5) - Mass; // Kinetic energy at crossing
    Double_t beta_cross = pow(pow(mom_cross(0), 2) + pow(mom_cross(1), 2) + pow(mom_cross(2), 2), 0.5) / (ekin_cross + Mass);

    if ((cross->GetTrackIdx() - 1) != Id_part)
    { // if it is not the particle I want to check, continue the loop
       continue;
    }

    TAMCpart *particle = myMcNtuPart->GetTrack(cross->GetTrackIdx() - 1); // retrievs TrackID
    Int_t particle_ID = cross->GetTrackIdx() - 1;                         // TrackID
    Int_t target_region = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetRegTarget();
    Int_t primary_charge = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber;
    TString regvtxname("VTXP3");
    Int_t last_vtx_plane_region = GetParGeoG()->GetCrossReg(regvtxname);
    Int_t AirAfterTW_region = GetParGeoG()->GetRegAirTW();
    TString regname("AIR1");
    Int_t RegAirAfterVtx = GetParGeoG()->GetCrossReg(regname);
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
        particle->GetCharge() <= primary_charge && // with reasonable charge
        // Ek_true > 100 &&                           // with enough initial energy to go beyond the vtx    //! is it true?
        // theta_tr <= 30. && // inside the angular acceptance of the vtxt   //!hard coded for GSI2021_MC
        (OldReg >= GetParGeoTw()->GetRegStrip(0, 0) && OldReg <= GetParGeoTw()->GetRegStrip(1, GetParGeoTw()->GetNBars() - 1)) && NewReg == AirAfterTW_region // it crosses the two planes of the TW and go beyond  (one of the bar of the two layers - region from 81 to 120)
    )
       return true;
  }
  return false;
  }
