#define GlobalRecoAna_cxx
#include <TVector3.h>
#include <TRandom.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include <TMultiGraph.h>
#include <TLegend.h>


#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "GlobalRecoAna.hxx"

using namespace std;

GlobalRecoAna::GlobalRecoAna(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, Int_t innTotEv) : BaseReco(expName, runNumber, fileNameIn, fileNameout)
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
  fFlagMC = isMC;
  purity_cut=0.51;
  clean_cut=1.;
  nTotEv=innTotEv;


  Th_meas = -999.; //maybe we should use a config file?

  f = new TFile(fileNameIn.Data(),"READ");


}

GlobalRecoAna::~GlobalRecoAna()
{
}

void GlobalRecoAna::LoopEvent() {

  Int_t currEvent=0;

  TATWparGeo* TWparGeo = (TATWparGeo*)fpParGeoTw->Object();

  while(gTAGroot->NextEvent()) {

    DiffApp_trkIdx = false; 

    if (currEvent % 100 == 0)
      printf("Event: %d\n", currEvent);

    TAGntuGlbTrack *myGlb = (TAGntuGlbTrack*)fpNtuGlbTrack->Object();
    resetStatus();
    TATWntuPoint* myTWNtuPt = (TATWntuPoint*)fpNtuRecTw->GenerateObject();
    TACAntuCluster* pCaNtuClu = (TACAntuCluster*)fpNtuClusCa->GenerateObject();
    if(fFlagMC){
      TAMCntuEvent* myMcNtuEvent = (TAMCntuEvent*)fpNtuMcEvt->GenerateObject();
      TAMCntuPart* myMcNtuPart = (TAMCntuPart*)fpNtuMcTrk->GenerateObject();
    }

    
    Int_t nt =  myGlb->GetTracksN();
    ((TH1D*)gDirectory->Get("ntrk"))->Fill(nt);
    //    if(myVtTr->GetTracksN()>1){
    fEvtGlbTrkVec.clear();
    //*************  loop on global tracks *****************

    
    for(int it=0;it<nt;it++){

      fGlbTrack = myGlb->GetTrack(it);
      if(isnan(fGlbTrack->GetCalEnergy())){//check if tof + track arc_length + mass hyp are ok
      	cout<<"TRK energy ISNAN! -> TOF value = "<<fGlbTrack->GetTwTof()<<endl;
      	continue;
      }

      //loop on meas points of a glb track
      int charge = -100;
      if(FootDebugLevel(1))
      	cout<<"===NEW TRACK with "<<fGlbTrack->GetPointsN()<<" points =="<<endl;

      map<int, int> mapclu;
      mapclu.clear();
      fGlbTrkVec.clear();
      vector<Int_t> vtxpoints, itpoints, msdpoints, twpoints, calpoints; // in principle twpoints and calpoints should be 0 or 1

      for(int ic=0;ic<fGlbTrack->GetPointsN();ic++) {

      	TAGpoint *tmp_poi = fGlbTrack->GetPoint(ic);
      	TString str = tmp_poi->GetDevName();
      	Int_t cluID = -1;

      	if(str.Contains(TAVTparGeo::GetBaseName())){//vtx
      	  VTanal(tmp_poi, cluID);
          if(tmp_poi->GetClusterIdx()>=0)
            vtxpoints.push_back(ic);
      	  if(cluID!=-1)
      	    mapclu[cluID]++;
      	}
      	if(str.Contains(TAITparGeo::GetBaseName())){//it
      	  ITanal(tmp_poi, cluID);
          if(tmp_poi->GetClusterIdx()>=0)
            itpoints.push_back(ic);
      	  if(cluID!=-1)
      	    mapclu[cluID]++;
      	}
      	if(str.Contains(TAMSDparGeo::GetBaseName())){//msd
      	  MSDanal(tmp_poi, cluID);
          if(tmp_poi->GetClusterIdx()>=0)
            msdpoints.push_back(ic);
      	  if(cluID!=-1)
      	    mapclu[cluID]++;
      	}
      	if(str.Contains(TATWparGeo::GetBaseName())){//tw
          TWanal(tmp_poi, cluID);
          if(tmp_poi->GetClusterIdx()>=0){
            twpoints.push_back(ic);
            calpoints.push_back(max(GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetMatchCalIdx(),-1));
            if(GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetMatchCalIdx()>=0){
              CALOanal(GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetMatchCalIdx());
            }
          }

      	  if(cluID!=-1)
      	    mapclu[cluID]++;
      	}
        // if(str.Contains(TACAparGeo::GetBaseName()))// actually the calorimeter is not used by the trackers!
        //   CALOanal(tmp_poi,cluID);
      }//end loop on meas points
      fGlbTrkVec.push_back(vtxpoints);
      fGlbTrkVec.push_back(itpoints);
      fGlbTrkVec.push_back(msdpoints);
      fGlbTrkVec.push_back(twpoints);
      fGlbTrkVec.push_back(calpoints);

      if(FootDebugLevel(1))
        cout<<"loop on global track hit done"<<endl;

      //Assignment of a true track idx (TrkIdMC)  to global reco track
      Int_t tmp_size = 0;
      Int_t my_mult(-1);
      Int_t TrkIdMC = -1;
      bool pure_trk = false;
      bool clean_trk = false;
      for(map<int,int>::iterator it=mapclu.begin(); it!=mapclu.end(); it++){
      	if(FootDebugLevel(1))
      	  cout<<mapclu.size()<<" :: MAPtrk ["<<it->first<<"]["<<it->second<<"]"<<endl;
      	tmp_size = it->second;
      	if(tmp_size > my_mult){
      	  TrkIdMC = it->first;
      	  my_mult = tmp_size;
      	}
      }//close loop on mapclu

      if(fFlagMC && TrkIdMC>=0){
        if(((Double_t)mapclu.at(TrkIdMC))/fGlbTrack->GetPointsN()>=purity_cut){
	    pure_trk = true;
	    npure++;
        }

      if(((Double_t)mapclu.at(TrkIdMC))/fGlbTrack->GetPointsN()==clean_cut){//100% pure track
        clean_trk = true;
        nclean++;
      }
      }
      if(FootDebugLevel(1) && fFlagMC)
      	cout<<"IDX TRK = "<<TrkIdMC<<" "<<endl;
	    //	if(TrkIdMC != )//to do ::: check if our trk index is different than the true (MC) one assigned by toe/genfit

      //------------TRUE MC VALUES-------------
      Int_t Z_true = -999;
      Double_t Ek_true = -1., Ek_cross = -1., Ek_cross_calo = -1., Ek_true_tot;
      Double_t M_true = -1., M_cross = -1., M_cross_calo = -1.;
      Double_t Tof_true = -1.;
      Double_t Tof_startmc = -1.;
      Double_t Beta_true = -1.;
      TVector3 P_true, P_cross, P_cross_calo;
      double Th_true, Th_cross;
      P_true.SetXYZ(-999.,-999.,-999.);
      P_cross.SetXYZ(-999.,-999.,-999.);
      P_cross_calo.SetXYZ(-999.,-999.,-999.);

      	// ComputeMCtruth(TrkIdMC, Z_true, P_true, P_cross, Ek_true);

      TrkIdMC = (fGlbTrack->GetMcTrackIdx())[0];      // i take as id the one given by the kalman filter algoritm
      if(fFlagMC){
      	if(TrkIdMC !=-1){
      	  TAMCpart *pNtuMcTrk = GetNtuMcTrk()->GetTrack(TrkIdMC);

      	  Z_true = pNtuMcTrk->GetCharge();
      	  P_true = pNtuMcTrk->GetInitP();//also MS contribution in target!
          M_true = pNtuMcTrk->GetMass();
      	  Ek_true_tot = (sqrt(pow(pNtuMcTrk->GetMass(),2) + pow((pNtuMcTrk->GetInitP()).Mag(),2)) - pNtuMcTrk->GetMass());
      	  Ek_true = Ek_true_tot/M_true;

      	  if(TAGrecoManager::GetPar()->IsRegionMc()){
      	    for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
      	      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
      	      if((fpNtuMcReg->GetTrackIdx()-1)==TrkIdMC){
            		if(fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirPreTW() && fpNtuMcReg->GetOldCrossN()==GetParGeoG()->GetRegTarget()){
            		  M_cross = fpNtuMcReg->GetMass();
            		  P_cross = fpNtuMcReg->GetMomentum();
            		  Ek_cross = sqrt(P_cross*P_cross + M_cross*M_cross) - M_cross;
            		  Ek_cross = Ek_cross/M_cross;
            		  Tof_startmc = fpNtuMcReg->GetTime()*fpFootGeo->SecToNs();
            		}
            		if(fpNtuMcReg->GetCrossN()<=GetParGeoTw()->GetRegStrip(1,GetParGeoTw()->GetNBars()-1) && fpNtuMcReg->GetCrossN()>=GetParGeoTw()->GetRegStrip(1,0)){//particle crossing in the first TW layer
                              Tof_true = fpNtuMcReg->GetTime()*fpFootGeo->SecToNs();
                              Beta_true=fpNtuMcReg->GetMomentum().Mag()/sqrt(fpNtuMcReg->GetMass()*fpNtuMcReg->GetMass()+fpNtuMcReg->GetMomentum().Mag()*fpNtuMcReg->GetMomentum().Mag());
            		}

            		// if(fpNtuMcReg->GetCrossN()>=GetParGeoCa()->GetRegCrystal(0) && fpNtuMcReg->GetCrossN()<=GetParGeoCa()->GetRegCrystal(GetParGeoCa()->GetCrystalsN()-1)){//particle crossing in the calo
                //particle exit from the tw
            		if(fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirTW() && fpNtuMcReg->GetOldCrossN()<=GetParGeoTw()->GetRegStrip(0,GetParGeoTw()->GetNBars()-1) && fpNtuMcReg->GetOldCrossN()>=GetParGeoTw()->GetRegStrip(0,0)){//particle crossing in the calo
                  M_cross_calo = fpNtuMcReg->GetMass();
            		  P_cross_calo = fpNtuMcReg->GetMomentum();
            		  Ek_cross_calo = sqrt(P_cross_calo*P_cross_calo + M_cross_calo*M_cross_calo) - M_cross_calo;
            		  Ek_cross_calo = Ek_cross_calo/M_cross_calo;

            		}

            	}
      	    }
      	  }
          if(FootDebugLevel(1)){
            cout<<"Z_true="<<Z_true<<"  M_true="<<M_true<<"  P_true.Mag()="<<P_true.Mag()<<"  Ek_true="<<Ek_true<<"  Ek_true_tot="<<Ek_true_tot<<endl;
            if(TAGrecoManager::GetPar()->IsRegionMc()){
              cout<<"Target exit crossings:"<<endl<<"  M_cross="<<M_cross<<"  P_cross.Mag()="<<P_cross.Mag()<<"  Ek_cross="<<Ek_cross<<"  Tof_startmc="<<Tof_startmc<<endl;
              cout<<"TOF mesurements: particle total time of flight="<<Tof_true<<"  Beta_true="<<Beta_true<<"  primary_tof=Tof_startmc="<<Tof_startmc<<"  particle real mc tof="<<Tof_true-Tof_startmc<<endl;
            }
          }
      	}

        if(FootDebugLevel(1))
          cout<<"GlobalRecoAna::myMcNtuPart loop done"<<endl;

        Th_true = P_true.Theta()*TMath::RadToDeg();
        Th_cross = P_cross.Theta()*TMath::RadToDeg();
      }//close if MC
      //------------END of TRUE MC VALUES-------------

      //-------------------------   SET TRACK VALUES     ----------------
      if(FootDebugLevel(1))
        cout<<"Reco analysis: retrieve measured quantities"<<endl;

      Int_t trkid = fGlbTrack->GetTrackId();
      Int_t Z_meas = fGlbTrack->GetTwChargeZ();

      

      Double_t P_meas = fGlbTrack->GetMomentum();  // Wrong method of TOE!! but it does not crash
      //Double_t P_meas = GetMomentumAtZ(fGlbTrack, 20.).Z();  //TOE right method... but it has to be fixed
      //Double_t P_meas = GetMomentumAtZ(fGlbTrack,fpFootGeo->GetTGCenter().Z()+GetParGeoG()->GetTargetPar().Size.Z()/2.).Z();//p meas at target exit

      //set infos from tw
      if(twstatus!=0){
        if(FootDebugLevel(1))
          cout<<"twstatus="<<twstatus<<"  this track analysis will be skipped"<<endl;
        continue;
      }

      Double_t Tof_tw=fGlbTrack->GetTwTof();
      // TVector3 TWhitpos = fGlbTrack->GetTwPosition(); //not working in tatoe
      TVector3 TWhitpos(-999,-999,-999);
      if(fGlbTrkVec.at(3).size()==1){
        TAGpoint *tmp_poi = fGlbTrack->GetPoint(fGlbTrkVec.at(3).at(0));
        // TWhitpos=tmp_poi->GetPositionG();//not working in tatoe
        if(tmp_poi->GetClusterIdx()>=0)
          TWhitpos = fpFootGeo->FromTWLocalToGlobal(GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetPosition());
        Tof_tw=GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetToF()-primary_tof;
      }else
        cout<<"GlobalRecoAna::warning! No one tw point attached; number of tw point="<<fGlbTrkVec.at(3).size()<<endl;

      Tof_true-=Tof_startmc;

      Double_t P_tg=fGlbTrack->GetMomentum(); // Wrong method of TOE!! but it does not crash
      // Double_t P_tg=GetMomentumAtZ(fGlbTrack,fpFootGeo->GetTGCenter().Z()+GetParGeoG()->GetTargetPar().Size.Z()/2.).Z(); //particle momentum at target out
      Double_t P_tw_front=fGlbTrack->GetMomentum(); // Wrong method of TOE!! but it does not crash
      // Double_t P_tw_front=GetMomentumAtZ(fGlbTrack,fpFootGeo->GetTWCenter().Z()-GetParGeoTw()->GetBarThick()/2.).Z(); //particle momentum at TW first layer

      //------------Ekin measured from calo associated to the glb track
      //      Double_t Ek_meas = fGlbTrack->GetCalEnergy()/atomassu;
      // Double_t Ek_meas_tot =  fGlbTrack->GetCalEnergy(); //not workiong in TATOE
      // Double_t Ek_meas_tot =  fGlbTrack->GetFitEnergy(); //not workiong in TATOE
      Double_t Ek_meas_tot;
      if(fGlbTrkVec.at(4).size()==1){
        if(fGlbTrkVec.at(4).at(0)>=0){
          TACAcluster *pCaClu=GetNtuClusterCa()->GetCluster(fGlbTrkVec.at(4).at(0));
          Ek_meas_tot=pCaClu->GetEnergy()*fpFootGeo->MevToGev();
        }
      }

      //------------end of compute Ekin

      //Evaluate the mass:
      if(FootDebugLevel(1))
        cout<<"Reco analysis: Mass analysis"<<endl;
      //THE TRACK lENGTH SHOULD BE GIVEN FROM THE TRACK RECONSTRUCTION TO TAKE INTO ACCOUNT THE CURVATURE!!!!!
      Double_t length=(TWhitpos-fpFootGeo->GetTGCenter()).Mag();
      Double_t beta=length/Tof_tw/TAGgeoTrafo::GetLightVelocity();
      Double_t M_meas = fGlbTrack->GetMass(); //It's the track mass hp, cannot be used as mass measurement
      // TVector3 vP_meas = ((TAGpoint*)(fGlbTrack->GetMeasPoint(0)))->GetMomentum();

      mass_ana->NewMass(P_tw_front, length, Tof_tw,Ek_meas_tot, 0.07, P_tw_front*0.05, Ek_meas_tot*0.015);// sigma values should be defined in a better way.
      M_meas=mass_ana->GetMassWavg()/TAGgeoTrafo::AmuToGev();
      if(mass_ana->GetInputStatus()==30){
        mass_ana->FitChiMass();
        mass_ana->FitAlmMass();
        mass_ana->CombineChi2Alm();
        M_meas=mass_ana->GetMassBest()/TAGgeoTrafo::AmuToGev();
      }
      FillMassPlots();
      FillGlbTrackPlots();

      Double_t Ek_meas = Ek_meas_tot/M_meas;//Energy (GeV/u)

      if(FootDebugLevel(1)){
        cout<<"GlobalRecoAna::Z_meas="<<Z_meas<<"  P_meas="<<P_meas<<"  P_tg="<<P_tg<<"  P_tw_front="<<P_tw_front<<"  primary_tof="<<primary_tof<<"  Tof_tw="<<Tof_tw<<"  beta="<<beta<<"  M_meas="<<M_meas<<"  Ek_meas="<<Ek_meas<<"  Ek_meas_tot="<<Ek_meas_tot<<endl;
        cout<<"MassPb="<<mass_ana->GetMassPb()<<"  MassPe="<<mass_ana->GetMassPe()<<"  MassBe="<<mass_ana->GetMassBe()<<endl;
      }
      //      Double_t Th_meas = vP_meas.Theta()*TMath::RadToDeg();

      //------- study on ek binning -------
      double Dtwpos = 0.01;//m
      double vlight = fpFootGeo->GetLightVelocity()/100.;//m/ns
      double tof_res = 0.6;//ns
      double gamma = 1./sqrt(1. - beta*beta);
      double Dbeta=sqrt((Dtwpos*Dtwpos)/(Tof_tw*vlight*Tof_tw*vlight)+TWhitpos.Z()*fpFootGeo->CmToM()*TWhitpos.Z()*fpFootGeo->CmToM()*(pow(tof_res*vlight,2)/(pow(Tof_tw*vlight,4)))); // delta beta
      double Dg=beta*pow((1-beta*beta),-3./2.)*Dbeta; //delta gamma
      double DE=M_meas*Dg; //delta mass
      //----------

      
      ((TH2D*)gDirectory->Get(Form("Ekin/Z%d/DE_vs_Ekin",Z_meas)))->Fill(Ek_meas*fpFootGeo->GevToMev(),DE*fpFootGeo->GevToMev());
      ((TH1D*)gDirectory->Get(Form("Ekin/Z%d/Ek_meas",Z_meas)))->Fill(Ek_meas*fpFootGeo->GevToMev());
      ((TH2D*)gDirectory->Get("Z_truevsZ_reco"))->Fill(Z_true,Z_meas);


      //--CROSS SECTION - RECO PARAMETERS
      if ((Z_meas >=0.) && (Th_meas >=0.) && (Ek_meas >=0.) && (M_meas >=0. ) && (M_meas <=90. )) {   
        ((TH1D*)gDirectory->Get("xsecrec-/charge"))->Fill(Z_meas); 
      //((TH1D*)gDirectory->Get(Form("xsec_rec/Z_%d-%d_%d/Theta_meas",Z_meas,Z_meas,(Z_meas+1))))->Fill(Th_meas);
      //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/Ek_meas",Z_meas,Z_meas,(Z_meas+1))))->Fill(Ek_meas*fpFootGeo->GevToMev());
      //((TH2D*)gDirectory->Get(Form("xsec_rec/Z_%d-%d_%d/Theta_vs_Ekin",Z_meas,Z_meas,(Z_meas+1))))->Fill(Ek_meas*fpFootGeo->GevToMev(),Th_meas);


            
        for (int i = 0; i<th_nbin; i++) {   
          
         if ( Z_meas>0 && Z_meas<=primary_cha){


         if(Th_meas>=theta_binning[i][0] && Th_meas<theta_binning[i][1]){
            //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Ek_bin",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(Ek_meas*fpFootGeo->GevToMev());
            //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Mass_bin",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(M_meas);

            //if (M_meas <0) cout <<" M MEAS NEGATIVE" << endl;
          
                    
           for (int j=0; j < ek_nbin; j++) {
             if((Ek_meas*fpFootGeo->GevToMev())>=ek_binning[j][0] && (Ek_meas*fpFootGeo->GevToMev())<ek_binning[j][1]) {
              
                //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/Mass_bin_",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]))))->Fill(M_meas);
             
                  for (int k=0; k < mass_nbin; k++) {
                   if(M_meas>=mass_binning[k][0] && M_meas <mass_binning[k][1]) {
              
                     ((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(M_meas);
                      
                                          

                    }
                  }


              }

            }
          }
         }
        }
      }

      //--END CROSS SECTION - RECO PARAMETERS


      
      /*


      //--CROSS SECTION - Z_ EFFICIENCY
      if ((Z_meas >=0) && (Th_meas >=0) && (Ek_meas >=0) && (M_meas >=0 ) && (M_meas <=30 )) {    
      
               
        for (int i = 0; i<th_nbin; i++) {        
         if(Th_true>=theta_binning[i][0] && Th_true<theta_binning[i][1]){
                      
                    
           for (int j=0; j < ek_nbin; j++) {
             if((Ek_true*fpFootGeo->GevToMev())>=ek_binning[j][0] && (Ek_true*fpFootGeo->GevToMev())<ek_binning[j][1]) {            
                             
                  for (int k=0; k < mass_nbin; k++) {
                   if(M_true>=mass_binning[k][0] && M_true <mass_binning[k][1]) {

                     if (Z_meas == Z_true)
                     ((TH1D*)gDirectory->Get(Form("xsecrec-Z_eff/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(M_meas);
                      
                                          

                    }
                  }
                     ((TH1D*)gDirectory->Get(Form("xsecrec-true/Z_%d-%d_%d/theta_


              }

            }
          }
        }
      }
       //--END CROSS SECTION - Z_ EFFICIENCY
      


      //--CROSS SECTION - TRUE PARAMETERS
      if ((Z_meas >=0) && (Th_meas >=0) && (Ek_meas >=0) && (M_meas >=0 ) && (M_meas <=30 )) {    
      
               
        for (int i = 0; i<th_nbin; i++) {        
         if(Th_true>=theta_binning[i][0] && Th_true<theta_binning[i][1]){
                      
                    
           for (int j=0; j < ek_nbin; j++) {
             if((Ek_true*fpFootGeo->GevToMev())>=ek_binning[j][0] && (Ek_true*fpFootGeo->GevToMev())<ek_binning[j][1]) {            
                             
                  for (int k=0; k < mass_nbin; k++) {
                   if(M_true>=mass_binning[k][0] && M_true <mass_binning[k][1]) {
              
                     ((TH1D*)gDirectory->Get(Form("xsecrec-true/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",Z_true,Z_true,(Z_true+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(M_true);
                    }
                  }


              }

            }
          }
        }
      }

      //--END CROSS SECTION - TRUE PARAMETERS
     
      */
        

       

      ((TH1D*)gDirectory->Get("Energy"))->Fill(Ek_meas*fpFootGeo->GevToMev());
      ((TH1D*)gDirectory->Get("Charge"))->Fill(Z_meas);
      ((TH1D*)gDirectory->Get("Charge_True"))->Fill(Z_true);
      // charge efficiency
      if (Z_meas == Z_true){
        
        ((TH1D*)gDirectory->Get("Charge_purity")) -> Fill(Z_meas);
        //((TH1D*)gDirectory->Get("Charge_efficiency")) -> Divide(((TH1D*)gDirectory->Get("Charge_True")));
      }


      ((TH1D*)gDirectory->Get("Mass"))->Fill(M_meas);
      ((TH1D*)gDirectory->Get("Mass_True"))->Fill(M_true);
      ((TH1D*)gDirectory->Get("ThReco"))->Fill(Th_meas);
      ((TH1D*)gDirectory->Get("ThTrue"))->Fill(Th_true);
      ((TH1D*)gDirectory->Get("Tof_tw"))->Fill(Tof_tw);
      ((TH1D*)gDirectory->Get("Beta"))->Fill(beta);
      if(fFlagMC && Beta_true>=0){
        ((TH1D*)gDirectory->Get("Tof_true"))->Fill(Tof_true);
        ((TH1D*)gDirectory->Get("Beta_true"))->Fill(Beta_true);
        
        
      }

      ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass",Z_meas)))->Fill(M_meas);

      //if(fFlagMC && Z_true>0 && Z_true<=primary_cha)
        //((TH1D*)gDirectory->Get(Form("Zrec%d/Track_purity",Z_meas)))->Fill(((Double_t)mapclu.at(TrkIdMC))/fGlbTrack->GetPointsN());

      if(Z_meas>0 && Z_meas<=primary_cha){

      	if(fFlagMC && clean_trk){//do it only for tracks made by clusters of same idx
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/ChargeZ_reso",Z_meas)))->Fill(Z_meas - Z_true);
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Mass_reso",Z_meas)))->Fill(M_meas - M_true/TAGgeoTrafo::AmuToGev());
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Mom_reso",Z_meas)))->Fill((P_meas - P_true.Mag())*fpFootGeo->GevToMev());
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_reso",Z_meas)))->Fill((Ek_meas - Ek_true)*fpFootGeo->GevToMev());
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_reso_cross",Z_meas)))->Fill((Ek_meas - Ek_cross_calo)*fpFootGeo->GevToMev());
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_reso",Z_meas)))->Fill((Ek_meas - Ek_true)*fpFootGeo->GevToMev());
            if(fGlbTrkVec.at(4).size()==1)
              if(fGlbTrkVec.at(4).at(0)>=0)
                if(GetNtuClusterCa()->GetCluster(fGlbTrkVec.at(4).at(0))->GetHitsN()==1){//only 1 crystal fired
                  ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_calo1cls_reso",Z_meas)))->Fill((Ek_meas - Ek_true)*fpFootGeo->GevToMev());
                  ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_calo1cls_reso_cross",Z_meas)))->Fill((Ek_meas - Ek_cross_calo)*fpFootGeo->GevToMev());

                  ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_calo1cls_reso",Z_meas)))->Fill(Th_meas - Th_true);
                  ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_calo1cls_reso_cross",Z_meas)))->Fill(Th_meas - Th_cross);
                }
      	    ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_reso",Z_meas)))->Fill(Th_meas - Th_true);
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross",Z_meas)))->Fill(Th_meas - Th_cross);
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_true_cross_vs_meas",Z_meas)))->Fill(Th_meas, Th_cross);
            ((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_th",Z_meas)))->Fill(Th_cross, Th_meas - Th_cross);
            ((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_thmeas",Z_meas)))->Fill(Th_meas, Th_meas - Th_cross);
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Tof_reso",Z_meas)))->Fill(Tof_tw - Tof_true);
            if(TAGrecoManager::GetPar()->IsRegionMc())
              ((TH1D*)gDirectory->Get(Form("MC/Z%d/Mom_reso_cross",Z_meas)))->Fill((P_meas - P_cross.Mag())*fpFootGeo->GevToMev());
            if(Z_true>0 && Z_true<=primary_cha){
              ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_clean",Z_meas)))->Fill(M_meas);
              ((TH1D*)gDirectory->Get(Form("Zrec%d/Zmc%d/Mass_clean",Z_meas, Z_true)))->Fill(M_meas);
      	    }
      	}

	      if(pure_trk){
          pure_track_xcha.at(Z_meas).first++;
          ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_pure",Z_meas)))->Fill(M_meas);

	        if(Z_true>0 && Z_true<=primary_cha)
      	    ((TH1D*)gDirectory->Get(Form("Zrec%d/Zmc%d/Mass_pure",Z_meas, Z_true)))->Fill(M_meas);

        }else{
          ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_impure",Z_meas)))->Fill(M_meas);
	        if(mapclu.size()==2)
            ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_impure_2",Z_meas)))->Fill(M_meas);

	        if(Z_true>0 && Z_true<=primary_cha)
            ((TH1D*)gDirectory->Get(Form("Zrec%d/Zmc%d/Mass_impure",Z_meas, Z_true)))->Fill(M_meas);
        }
        pure_track_xcha.at(Z_meas).second++;
      }

      if(fGlbTrkVec.at(4).size()!=fGlbTrkVec.at(3).size() || fGlbTrkVec.at(3).size()>1){
        cout<<"WARNING in global reco:: the number of tw or calo hits associated to a global track is not the expected one:"<<endl;
        cout<<"number of associated twhits="<<fGlbTrkVec.at(3).size()<<"; number of associated calo hits:"<<fGlbTrkVec.at(4).size()<<endl;
        cout<<"event number="<<nTotEv<<"  global track:"<<it<<endl;
      }

      fEvtGlbTrkVec.push_back(fGlbTrkVec);
      ntracks++;

    }
    //end loop on ntracks
      
    

   
    if (fFlagMC){
          
          // Definisco la regione target all'inizializzazione
        
        //! da riguardare gli oggetti
          //TAGrunInfo* runinfo=(TAGrunInfo*)(f->Get("runinfo"));

          
          //TString regnameTg="TARGET";
          //auto RegTarg = runinfo->GetRegion(regnameTg);
          

          //TAMCntuPart* m_trueParticleRep = (TAMCntuPart*)   gTAGroot->FindDataDsc("mctrack", "TAMCntuPart")->Object(); //container of all particles of an event

          TAMCntuPart* m_trueParticleRep = (TAMCntuPart*)fpNtuMcTrk->GenerateObject();
          Int_t n_particles = m_trueParticleRep -> GetTracksN();        // n° of particles of an event

        
          for (Int_t i= 0 ; i < n_particles; i++) {                         // for every particle
            TAMCpart* particle = m_trueParticleRep->GetTrack(i);
            auto  Mid = particle->GetMotherID();              // Get TRpaid-1
            auto Reg = particle->GetRegion();
            auto finalPos = particle-> GetFinalPos();
            
            if (  Mid==0 && Reg == 50 &&           // if the particle is generated in the target and it is the fragment of a primary
                  particle->GetCharge()>0 && particle->GetCharge()<=8 &&                       //if Z<8 and A<30, so if it is a fragment (not the primitive projectile, nor detector fragments)
                  particle->GetMass()>0.8 && particle->GetMass()<30
                  )  {                            
                        
                          Float_t charge_tr = particle-> GetCharge();

                          ((TH1D*)gDirectory->Get(Form("xsecrec-true/Z_true")))->Fill(charge_tr);

                          for (int i = 0; i<th_nbin; i++) {
                          
                          
                          Float_t theta_tr = particle->GetInitP().Theta()*(180/TMath::Pi());          
                          if(theta_tr>=theta_binning[i][0] && theta_tr<theta_binning[i][1]){
                                                
                            for (int j=0; j < ek_nbin; j++) {
                              Float_t Ek_tr_tot = (sqrt(pow(particle->GetMass(),2) + pow((particle->GetInitP()).Mag(),2)) - particle->GetMass());
                              Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
                              
                              if(Ek_tr_tot >=ek_binning[j][0] && Ek_tr_tot<ek_binning[j][1]) {
                                            
                                    for (int k=0; k < mass_nbin; k++) {
                                      Float_t mass_tr = particle -> GetMass();
                                    if(mass_tr>=mass_binning[k][0] && mass_tr <mass_binning[k][1]) {
                                
                                      ((TH1D*)gDirectory->Get(Form("xsecrec-true/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",int(charge_tr),int(charge_tr),int(charge_tr+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(mass_tr);
                                      
                                                            

                                      }
                                    }


                                }

                              }
                            }
                          }
                        }

          //true detectable
                    //! NB: 50 IN GSI2021_MC
          if (  (Mid==0 ) && Reg == 50  &&  finalPos.Z() > 190     &&    // if the particle is generated in the target and it is the fragment of a primary AND DIES IN THE TW
                  particle->GetCharge()>0 && particle->GetCharge()<=8 &&                       //if Z<8 and A<30, so if it is a fragment (not the primitive projectil)
                  particle->GetMass()>0.8 && particle->GetMass()<30
                  )  {                            
                        
                          Float_t charge_tr = particle-> GetCharge();
                          for (int i = 0; i<th_nbin; i++) {
                          
                          
                          Float_t theta_tr = particle->GetInitP().Theta()*(180/TMath::Pi());          
                          if(theta_tr>=theta_binning[i][0] && theta_tr<theta_binning[i][1]){
                                                
                            for (int j=0; j < ek_nbin; j++) {
                              Float_t Ek_tr_tot = (sqrt(pow(particle->GetMass(),2) + pow((particle->GetInitP()).Mag(),2)) - particle->GetMass());
                              Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
                              
                              if(Ek_tr_tot >=ek_binning[j][0] && Ek_tr_tot<ek_binning[j][1]) {
                                            
                                    for (int k=0; k < mass_nbin; k++) {
                                      Float_t mass_tr = particle -> GetMass();
                                    if(mass_tr>=mass_binning[k][0] && mass_tr <mass_binning[k][1]) {
                                
                                      ((TH1D*)gDirectory->Get(Form("xsecrec-true_DET/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",int(charge_tr),int(charge_tr),int(charge_tr+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(mass_tr);
                                      
                                                            

                                      }
                                    }


                                }

                              }
                            }
                          }
                        }

          //--END CROSS SECTION - TRUE PARAMETERS


          }


        



    }
    

    
    FullCALOanal();

    Int_t exitfragnum=0;    //number of fragmengs exit from the target
    Int_t exitfrag10anglenum=0;    //number of fragmengs exit from the target
    if(TAGrecoManager::GetPar()->IsRegionMc() && fFlagMC){
      for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
        TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
          if( fpNtuMcReg->GetCharge()>0 && fpNtuMcReg->GetMass()>0  && fpNtuMcReg->GetOldCrossN()==GetParGeoG()->GetRegTarget() && fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirPreTW()){ //a particle exit from the target
            exitfragnum++;
          if(fpNtuMcReg->GetMomentum().Theta()*TMath::RadToDeg()<10)
            exitfrag10anglenum++;
        }
      }
    }
    //((TH2D*)gDirectory->Get("MC/MCpartVsGlbtrackNum"))->Fill(exitfragnum,myGlb->GetTracksN());
    //((TH2D*)gDirectory->Get("MC/MCpartVsGlbtrackNum_angle10"))->Fill(exitfrag10anglenum,myGlb->GetTracksN());

    //    }//close if >=2 tracks in vtx => no primary without interactions!
    
    ++currEvent;

    if (currEvent == nTotEv) {
      
      break;
    } 
  }//end event loop
  
  return;
}


void GlobalRecoAna:: Booking(){


  if(FootDebugLevel(1))
    cout<<"start GlboalRecoAna::Booking"<<endl;

  char name[200],title[200];
  file_out->cd();


  h = new TH1D("ntrk","",10, 0 ,10.);
  h = new TH1D("Energy","",100, 0 ,800.);
  h = new TH1D("Charge","",10, 0 ,10.);
  h = new TH1D("Charge_True","",10, 0 ,10.);
  h = new TH1D("Charge_purity","",10, 0 ,10.);
  h = new TH1D("Mass","Mass [amu]",200, 0 ,20.);
  h = new TH1D("Mass_True","Mass_True [amu]",200, 0 ,20.);
  h = new TH1D("ThReco","",200, 0 ,20.);
  h = new TH1D("ThTrue","",200, 0 ,20.);
  h = new TH1D("Tof_tw","TOF from TW center; [ns]",200, 0., 10.);
  h = new TH1D("Beta","Beta;",200, 0., 1.);
  h2  = new TH2D("Z_truevsZ_reco","",10, 0 ,10., 10, 0 ,10.);

  if(fFlagMC){
    h = new TH1D("Tof_true","TOF from MC truth; [ns]",200, 0., 10.);
    h = new TH1D("Beta_true","Beta from MC truth; ",200, 0., 1.);
  }


  for(int ic=0; ic<=primary_cha; ic++){
    gDirectory->mkdir(Form("Zrec%d",ic));
    gDirectory->cd(Form("Zrec%d",ic));
    int iz=max(ic,1);
      h = new TH1D("Mass","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_clean","Reconstructed mass spectra for pure tracks;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_pure","Reconstructed mass spectra for pure tracks;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_impure","Reconstructed mass spectra not pure tracks;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_impure_2","Reconstructed mass spectra for tracks with multiplicity 2;Mass[GeV];",iz*200, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Track_purity","Number of right clusters over total number of clusters;Mass[GeV];Tracks",120, -0.1 ,1.1);
      for(int kz=0; kz<=primary_cha; kz++){
        gDirectory->mkdir(Form("Zmc%d",kz));
        gDirectory->cd(Form("Zmc%d",kz));
          h = new TH1D("Mass_clean","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
	  h = new TH1D("Mass_pure","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
	  h = new TH1D("Mass_impure","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
        gDirectory->cd("..");
      }
    gDirectory->cd("..");

  }

  gDirectory->mkdir("Ekin");
  gDirectory->cd("Ekin");

  for(int iz=0; iz<=primary_cha; iz++){
    gDirectory->mkdir(Form("Z%d",iz));
    gDirectory->cd(Form("Z%d",iz));

    h2 = new TH2D("DE_vs_Ekin","", 200, 0.,600., 200,0.,600.);
    h = new TH1D("Ek_meas","",100, 0 ,800.);

    gDirectory->cd("..");
  }
  gDirectory->cd("..");



//prova binning = 1
/*
th_nbin = 1;
theta_binning = new double *[th_nbin];
theta_binning[0] = new double [2];
theta_binning[0][0] = 0.;
theta_binning[0][1] = 90.;

ek_nbin = 1;
ek_binning = new double *[ek_nbin];
ek_binning[0] = new double [2];
ek_binning[0][0] = 0.;
ek_binning[0][1] = 2000.;

mass_nbin = 1;
mass_binning = new double *[mass_nbin];
mass_binning[0] = new double [2];
mass_binning[0][0] = 0.;
mass_binning[0][1] = 50.;
*/



//binning of theta, ekin, A
th_nbin = 11;
theta_binning = new double *[th_nbin];
for (int i = 0; i<th_nbin-1; i++) {
  theta_binning[i] = new double [2];
  theta_binning[i][0] = double (i);
  theta_binning[i][1] = double (i+1);
  //cout << " theta binning "<< i << " "<< theta_binning[i][0] << " "<< theta_binning[i][1];
}
theta_binning[10] = new double [2];
theta_binning[10][0] = double(10);
theta_binning[10][1] =  double(90);
//cout << " theta binning "<< 10 << " "<< theta_binning[10][0] << " "<< theta_binning[10][1];

ek_nbin = 10;
ek_binning = new double *[ek_nbin];
for (int i = 0; i<ek_nbin; i++) {
  ek_binning[i] = new double [2];
  ek_binning[i][0] = double (i*100);
  ek_binning[i][1] = double ((i+1)*100);
}
  //ek_binning[4] = new double [2];
  //ek_binning[4][0] = double(400);
  //ek_binning[4][1] =  double(1000);

mass_nbin = 10;
mass_binning = new double *[mass_nbin];
for (int i = 0; i<mass_nbin; i++) {
  mass_binning[i] = new double [2];
  mass_binning[i][0] = double (i*2);
  mass_binning[i][1] = double ((i+1)*2);
}
 

// Cross section recostruction histos
  
  gDirectory->mkdir("xsecrec-");
  gDirectory->cd("xsecrec-");
  h = new TH1D("charge","",10, 0. ,10.);

  for(int iz=0; iz<=primary_cha; iz++){
    gDirectory->mkdir(Form("Z_%d-%d_%d",iz, iz , iz+1));
    gDirectory->cd(Form("Z_%d-%d_%d",iz, iz , iz+1));

    
    //h = new TH1D("Theta_meas","",200, 0 ,20.);
    //h = new TH1D("Ek_meas","",100, 0 ,2000.);
   // h2 = new TH2D("Theta_vs_Ekin","", 200, 0.,600., 200,0.,20.);


    for (int i = 0; i<th_nbin; i++) {
      gDirectory->mkdir(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
      gDirectory->cd(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
     // h = new TH1D(Form("Ek_bin"),"",100, 0 ,2000.);
     // h = new TH1D(Form("Mass_bin"),"",200, 0 ,90.);

      for (int j=0; j <ek_nbin; j++) {
        gDirectory->mkdir(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        gDirectory->cd(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        //h = new TH1D(Form("Mass_bin_"),"",200, 0 ,90.);

          for (int k=0; k<mass_nbin; k++) {
            gDirectory->mkdir(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            gDirectory->cd(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            h = new TH1D("A_","",200, 0 ,90.);

            /*
            gDirectory->mkdir("Efficiencies");
            gDirectory->cd("Efficiencies");
            h = new TH1D("Z_eff","",10, 0 ,10.);
            gDirectory->cd("..");
            */

          gDirectory->cd("..");
          }
      gDirectory->cd("..");
      }
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  gDirectory->cd("..");

//----------- end cross section recostruction


  // Cross section TRUE histos
  
  gDirectory->mkdir("xsecrec-true");
  gDirectory->cd("xsecrec-true");

  h = new TH1D("Z_true","",10, 0 ,10.);



  for(int iz=0; iz<=primary_cha; iz++){
    gDirectory->mkdir(Form("Z_%d-%d_%d",iz, iz , iz+1));
    gDirectory->cd(Form("Z_%d-%d_%d",iz, iz , iz+1));

    
   // h = new TH1D("Theta_meas","",200, 0 ,20.);
   // h = new TH1D("Ek_meas","",100, 0 ,800.);
   // h2 = new TH2D("Theta_vs_Ekin","", 200, 0.,600., 200,0.,20.);


    for (int i = 0; i<th_nbin; i++) {
      gDirectory->mkdir(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
      gDirectory->cd(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
      //h = new TH1D(Form("Ek_bin"),"",100, 0 ,800.);
      //h = new TH1D(Form("Mass_bin"),"",200, 0 ,30.);

      for (int j=0; j <ek_nbin; j++) {
        gDirectory->mkdir(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        gDirectory->cd(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        //h = new TH1D(Form("Mass_bin_"),"",200, 0 ,30.);

          for (int k=0; k<mass_nbin; k++) {
            gDirectory->mkdir(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            gDirectory->cd(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            h = new TH1D("A_","",200, 0 ,30.);

          gDirectory->cd("..");
          }
      gDirectory->cd("..");
      }
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  gDirectory->cd("..");

//----------- end cross section TRUE 

 // Cross section TRUE histos DETECTABLE (PARTICLES WHICH REACH THE TW)
  
  gDirectory->mkdir("xsecrec-true_DET");
  gDirectory->cd("xsecrec-true_DET");

  for(int iz=0; iz<=primary_cha; iz++){
    gDirectory->mkdir(Form("Z_%d-%d_%d",iz, iz , iz+1));
    gDirectory->cd(Form("Z_%d-%d_%d",iz, iz , iz+1));

    
   // h = new TH1D("Theta_meas","",200, 0 ,20.);
   // h = new TH1D("Ek_meas","",100, 0 ,800.);
   // h2 = new TH2D("Theta_vs_Ekin","", 200, 0.,600., 200,0.,20.);


    for (int i = 0; i<th_nbin; i++) {
      gDirectory->mkdir(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
      gDirectory->cd(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
      //h = new TH1D(Form("Ek_bin"),"",100, 0 ,800.);
      //h = new TH1D(Form("Mass_bin"),"",200, 0 ,30.);

      for (int j=0; j <ek_nbin; j++) {
        gDirectory->mkdir(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        gDirectory->cd(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        //h = new TH1D(Form("Mass_bin_"),"",200, 0 ,30.);

          for (int k=0; k<mass_nbin; k++) {
            gDirectory->mkdir(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            gDirectory->cd(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            h = new TH1D("A_","",200, 0 ,30.);

          gDirectory->cd("..");
          }
      gDirectory->cd("..");
      }
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  gDirectory->cd("..");

//----------- end cross section TRUE 

/*



// Cross section Z_eff
  
  gDirectory->mkdir("xsecrec-Z_eff");
  gDirectory->cd("xsecrec-Z_eff");

  for(int iz=0; iz<=primary_cha; iz++){
    gDirectory->mkdir(Form("Z_%d-%d_%d",iz, iz , iz+1));
    gDirectory->cd(Form("Z_%d-%d_%d",iz, iz , iz+1));

    
   // h = new TH1D("Theta_meas","",200, 0 ,20.);
   // h = new TH1D("Ek_meas","",100, 0 ,800.);
   // h2 = new TH2D("Theta_vs_Ekin","", 200, 0.,600., 200,0.,20.);


    for (int i = 0; i<th_nbin; i++) {
      gDirectory->mkdir(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
      gDirectory->cd(Form("theta_%d-%d_%d",i,int(theta_binning[i][0]),int(theta_binning[i][1])));
      //h = new TH1D(Form("Ek_bin"),"",100, 0 ,800.);
      //h = new TH1D(Form("Mass_bin"),"",200, 0 ,30.);

      for (int j=0; j <ek_nbin; j++) {
        gDirectory->mkdir(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        gDirectory->cd(Form("Ek_%d-%d_%d",j,int(ek_binning[j][0]),int(ek_binning[j][1])));
        //h = new TH1D(Form("Mass_bin_"),"",200, 0 ,30.);

          for (int k=0; k<mass_nbin; k++) {
            gDirectory->mkdir(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            gDirectory->cd(Form("A_%d-%d_%d",k,int(mass_binning[k][0]),int(mass_binning[k][1])));
            h = new TH1D(Form("A_"),"",200, 0 ,30.);

          gDirectory->cd("..");
          }
      gDirectory->cd("..");
      }
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  gDirectory->cd("..");

//----------- end cross section Z_eff
*/


  //  int NB_ek = 10;
  //  double binV[NB_ek];
  //  for(int ib=0;ib<NB_ek;ib++){
    //    binV[ib] = 1.;
  //  }

 

  if(fFlagMC){
    gDirectory->mkdir("MC");
    gDirectory->cd("MC");
    h2 = new TH2D("ChargePoi_vs_ChargeVT","",11, -1. ,10.,11, -1. ,10.);
    h2 = new TH2D("ChargePoi_vs_ChargeIT","",10, 0. ,10.,10, 0. ,10.);
    h2 = new TH2D("ChargePoi_vs_ChargeMSD","",10, 0. ,10.,10, 0. ,10.);
    h2 = new TH2D("ChargePoi_vs_ChargeTW","",10, 0. ,10.,10, 0. ,10.);
    if(TAGrecoManager::GetPar()->IsRegionMc()){
      h2 = new TH2D("MCpartVsGlbtrackNum","Number of MC particles exit from target Vs number of reconstructed tracks;",11, -0.5, 10.5,11, -0.5, 10.5);
      h2 = new TH2D("MCpartVsGlbtrackNum_angle10","Number of MC particles exit from target with angle <10 Vs number of reconstructed tracks;",11, -0.5, 10.5,11, -0.5, 10.5);
    }

    for(int iz=0; iz<=primary_cha; iz++){
      gDirectory->mkdir(Form("Z%d",iz));
      gDirectory->cd(Form("Z%d",iz));
      h = new TH1D("ChargeZ_reso","",11, -5.5, 5.5);
      h = new TH1D("Mass_reso","Mass resolution;Mass measured - MC true [Amu]",600, -3, 3.);
      h = new TH1D("Mom_reso","Momentum resolution with MC initial P; measured - MC true [MeV]",400, -2000., 2000.);
      h = new TH1D("Mom_reso_cross","Momentum resolution with MC crossing P; measured - MC true [MeV]",400, -2000., 2000.);
      h = new TH1D("Ekin_reso","Ekin resolution with MC initial Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Ekin_reso_cross","Ekin resolution with MC crossing Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Ekin_calo1cls_reso","Ekin resolution with MC initial Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Ekin_calo1cls_reso_cross","Ekin resolution with MC crossing Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Theta_reso","",200, -2., 2.);
      h = new TH1D("Theta_reso_cross","",200, -2., 2.);
      h2 = new TH2D("Theta_reso_cross_vs_th","",50, 0 ,20., 100, -2., 2.);
      h2 = new TH2D("Theta_reso_cross_vs_thmeas","",50, 0 ,20., 100, -2., 2.);
      h2 = new TH2D("Theta_true_cross_vs_meas","Theta measured [deg]; Theta MC cross [deg]",200, 0 ,20., 200, 0., 20.);

      h = new TH1D("Theta_calo1cls_reso","",200, -2., 2.);
      h = new TH1D("Theta_calo1cls_reso_cross","",200, -2., 2.);


      h = new TH1D("Tof_reso","TOF Resolution; MC TOF - Reco TOF [ns]",200, -1., 1.);
      //for hres studies
      gDirectory->mkdir("ThetaRes");
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }

  gDirectory->mkdir("MassReco");
  gDirectory->cd("MassReco");
  h = new TH1D("InputStatus","InputStatus; 0=error, 1=no input, 2=Momentum ok, 3=Beta Ok, 5=Energy ok, other values=prod ",31, -0.5, 30.5);
  h = new TH1D("ChiMassStatus","ChiMassStatus; 0=ok, -1=singular matrix for chi2, >0 error ",7, -1.5, 5.5);
  h = new TH1D("AlmMassStatus","AlmMassStatus; 0=ok, -1=singular matrix for chi2, >0 error ",7, -1.5, 5.5);
    for(int ic=0; ic<=primary_cha; ic++){
      gDirectory->mkdir(Form("Zreco%d",ic));
      gDirectory->cd(Form("Zreco%d",ic));
      h = new TH1D("chi2min","chi2 value",250, 0., 2.);
      h = new TH1D("almmin","almmin value",250, 0., 2.);
      h = new TH1D("mom_bestvsmeas","chi2 estimated momentum vs measured;chi2-measured [MeV]",400, -100., 100.);
      h = new TH1D("ekin_bestvsmeas","chi2 estimated ekin vs measured;ekin-measured [MeV]",400, -100., 100.);
      h = new TH1D("tof_bestvsmeas","chi2 estimated tof vs measured;tof-measured [ns]",100, -1., 1.);
      h = new TH1D("Mass_pb","Reconstructed mass spectra with momentum and tof;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_pberr","Mass error for momentum and tof mass estimate;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_be","Reconstructed mass spectra with tof and kinetic energy;Mass number;",1000, 0. ,20.);
      h = new TH1D("Mass_beerr","Mass error for energy and tof mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mass_pe","Reconstructed mass spectra with momentum and kinetic energy;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_peerr","Mass error for energy and momentum mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mass_wavg","Reconstructed mass spectra using weighted average;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_wavgerr","Mass error for weighted averate mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mass_chi","Reconstructed mass spectra using chi square method;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_chierr","Mass error for chi square mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mom_chi","momentum optimized from chi square method;Momentum [GeV];",1000, 0. ,20);
      h = new TH1D("Mom_chierr","Error for the momentum optimized from chi square method;Momentum [GeV];",200, 0. ,1);
      h = new TH1D("Ekin_chi","Ekin optimized from chi square method;Ekin [GeV];",1000, 0. ,20.);
      h = new TH1D("Ekin_chierr","Error for the Ekin optimized from chi square method;Ekin [GeV];",200, 0. ,0.5);
      h = new TH1D("Tof_chi","TOf optimized from chi square method;Tof [ns];",200, 0. ,15.);
      h = new TH1D("Tof_chierr","Error for the TOf optimized from chi square method;Tof [ns];",200, 0. ,1.);
      h = new TH1D("Mass_alm","Reconstructed mass spectra using alm method;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_almerr","Mass error for alm mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mom_alm","momentum optimized from alm method;Momentum [GeV];",1000, 0. ,20);
      h = new TH1D("Mom_almerr","Error for the momentum optimized from alm method;Momentum [GeV];",200, 0. ,1);
      h = new TH1D("Ekin_alm","Ekin optimized from alm method;Ekin [GeV];",1000, 0. ,20.);
      h = new TH1D("Ekin_almerr","Error for the Ekin optimized from alm method;Ekin [GeV];",200, 0. ,0.1);
      h = new TH1D("Tof_alm","TOf optimized from alm method;Tof [ns];",200, 0. ,15.);
      h = new TH1D("Tof_almerr","Error for the TOf optimized from alm method;Tof [ns];",200, 0. ,1.);
      gDirectory->cd("..");
    }
    gDirectory->cd("..");

    gDirectory->mkdir("CALO");
    gDirectory->cd("CALO");

    for(int iz=0; iz<=primary_cha; iz++){
      gDirectory->mkdir(Form("Zreco%d",iz));
      gDirectory->cd(Form("Zreco%d",iz));
      h = new TH1D("ClusSize","Calo cluster size;Number of hits x cluster;Number of clusters",21, -0.5, 20.5);
      h = new TH1D("ClusCharge","Calo cluster total charge;Cluster charge [MeV];Number of clusters",101, -0.5, 100.5);
      gDirectory->cd("..");
    }
    for(int iz=0; iz<=primary_cha; iz++){
      gDirectory->mkdir(Form("Zmc%d",iz));
      gDirectory->cd(Form("Zmc%d",iz));
      h = new TH1D("partNumXCluster","Number of MC particles exit from tw and arriving in calo x cluster",11, -0.5, 10.5);
      h = new TH1D("partChaXCluster","Charge of MC particle exit from tw and arriving in calo x cluster",21, -0.5, 20.5);
      h2 = new TH2D("EkinVsCluscharge","MC particle ekin vs calo cluster total charge;Kinetic energy per nucleon [MeV/n];Cluster total charge [MeV/n]",200, 0., 1000.,200,0.,1000.);
      h2 = new TH2D("ClusSizeVsTheta","Cluster size vs MC crossing in calo theta for each MC particle;Cluster size;Theta [deg]",21, -0.5, 20.5,180,0.,30.);
      gDirectory->cd("..");
    }

    gDirectory->mkdir("GlbTrk");
    gDirectory->cd("GlbTrk");
    h = new TH1D("ClusNum_glbtrk","Number of calo clusters with a glb track per event;Number of cluster;Events",51, -0.5, 50.5);
    h = new TH1D("ClusEfficiency","Number of glb tracks with a Calo cluster over total number of glb tracks;Calo efficiency;Events",100, 0., 1.);
    h = new TH1D("ClusSize_glbtrk","Calo glb tracks cluster size;Number of hits x cluster;Number of clusters",21, -0.5, 20.5);
    gDirectory->cd("..");

    h = new TH1D("MultiPartDistance","Distance btw the particles that enter in the calo in the same event from MC;Distance [cm];Number of particles",600, 0., 200.);
    h = new TH1D("clus2HitDist","Distance btw the calo hits when a cluster have 2 hits;Distance [cm];Number of particles",250, 0., 5.);
    h2 = new TH2D("MCpartNumvscluster","Number of MC particle exit from tw entering in calo vs number of calo clusters per event; Number of MC particles; Number of Calo clusters",11, -0.5, 10.5,11, -0.5, 10.5);
    h2 = new TH2D("MCpartChavsClusSize","MC Charge vs calo cluster size; Charge; Calo cluster size [Number of crystals]",11, -0.5, 10.5,21, -0.5, 20.5);
    h = new TH1D("MCpartNumXHit","Number of MC particle x hit",11, -0.5, 10.5);
    h = new TH1D("ClusNum","Number of calo clusters per event;Number of cluster;Events",51, -0.5, 50.5);
    h = new TH1D("ClusSize","Calo cluster size;Number of hits x cluster;Number of clusters",21, -0.5, 20.5);
    h = new TH1D("ClusCharge","Calo cluster total charge;Cluster charge [MeV];Number of clusters",101, -0.5, 100.5);
    h = new TH1D("distminMag","Distance between track projection and CALO cluster position",100, -1., 10.);
    h = new TH1D("DistX_trkclu","Distance X trk int w calo AND calo clus",200, -20., 20.);
    h = new TH1D("DistY_trkclu","Distance Y trk int w calo AND calo clus",200, -20., 20.);
    h = new TH1D("DistZ_trkclu","Distance Z trk int w calo AND calo clus",200, -20., 20.);
  gDirectory->cd("..");

  gDirectory->mkdir("FIT_vs_Meas");
  gDirectory->cd("FIT_vs_Meas");
  h = new TH1D("Ekin","Fitted Ekin - Measured Ekin;[MeV]",600, -100., 100.);
  h = new TH1D("Tof","Fitted Tof - Measured Tof [ns]",600, -100., 100.);
  h = new TH1D("Mass","Fitted Mass - Measured Mass [MeV]",600, -100., 100.);
  gDirectory->cd("..");

  if(FootDebugLevel(1))
    cout<<"GlboalRecoAna::Booking done"<<endl;

  return;
}

void GlobalRecoAna::SetupTree(){

  myReader = new TAGactTreeReader("myReader");
  fpNtuGlbTrack = new TAGdataDsc("glbTrack",new TAGntuGlbTrack());
  gTAGroot->AddRequiredItem("glbTrack");
  myReader->SetupBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());

  if(TAGrecoManager::GetPar()->IncludeVT()){
    fpNtuClusVtx = new TAGdataDsc("vtclus",new TAVTntuCluster());
    fpNtuTrackVtx = new TAGdataDsc("vttrack",new TAVTntuTrack());
    fpNtuVtx = new TAGdataDsc("vtvtx",new TAVTntuVertex());
    gTAGroot->AddRequiredItem("vtclus");
    gTAGroot->AddRequiredItem("vttrack");
    gTAGroot->AddRequiredItem("vtvtx");
    myReader->SetupBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
    myReader->SetupBranch(fpNtuTrackVtx, TAVTntuTrack::GetBranchName());
    myReader->SetupBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
    if(fFlagMC){
      fpNtuMcVt = new TAGdataDsc("mcvt",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcvt");
      myReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeIT()){
    fpNtuClusIt = new TAGdataDsc("itclus",new TAITntuCluster());
    gTAGroot->AddRequiredItem("itclus");
    myReader->SetupBranch(fpNtuClusIt, TAITntuCluster::GetBranchName());
    if(fFlagMC){
      fpNtuMcIt = new TAGdataDsc("mcit",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcit");
      myReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeMSD()){
    fpNtuClusMsd = new TAGdataDsc("msdclus",new TAMSDntuCluster());
    gTAGroot->AddRequiredItem("msdclus");
    myReader->SetupBranch(fpNtuClusMsd, TAMSDntuCluster::GetBranchName());
    if(fFlagMC){
      fpNtuMcMsd = new TAGdataDsc("mcmsd",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcmsd");
      myReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeTW()){
    fpNtuRecTw = new TAGdataDsc("twpt",new TATWntuPoint());
    gTAGroot->AddRequiredItem("twpt");
    myReader->SetupBranch(fpNtuRecTw, TATWntuPoint::GetBranchName());
    if(fFlagMC){
      fpNtuMcTw = new TAGdataDsc("mctw",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mctw");
      myReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeCA()){
    fpNtuClusCa = new TAGdataDsc("caclus",new TACAntuCluster());
    gTAGroot->AddRequiredItem("caclus");
    myReader->SetupBranch(fpNtuClusCa, TACAntuCluster::GetBranchName());
    if(fFlagMC){
      fpNtuMcCa = new TAGdataDsc("mcca",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcca");
      myReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
    }
  }

  if(fFlagMC){
    fpNtuMcEvt = new TAGdataDsc("mcevt",new TAMCntuEvent());
    fpNtuMcTrk = new TAGdataDsc("mctrack",new TAMCntuPart());
    gTAGroot->AddRequiredItem("mcevt");
    gTAGroot->AddRequiredItem("mctrack");
    myReader->SetupBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
    myReader->SetupBranch(fpNtuMcTrk, TAMCntuPart::GetBranchName());
    if(TAGrecoManager::GetPar()->IsRegionMc()){
      fpNtuMcReg = new TAGdataDsc("mcreg",new TAMCntuRegion());
      gTAGroot->AddRequiredItem("mcreg");
      myReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
    }
  }

  gTAGroot->AddRequiredItem("myReader");
  gTAGroot->Print();



  return;
}


void GlobalRecoAna::VTanal(TAGpoint *tmp_poi, Int_t &idxCLU){

  if(FootDebugLevel(1))
    cout<<"VTanal start"<<endl;

  Double_t xp = (tmp_poi->GetPosition()).X();
  Double_t yp = (tmp_poi->GetPosition()).Y();
  Double_t zp = (tmp_poi->GetPosition()).Z();
  Int_t charge = fGlbTrack->GetTwChargeZ();
  TAVTntuCluster *myVTclus = (TAVTntuCluster*)fpNtuClusVtx->GenerateObject();

  if(tmp_poi->GetSensorIdx()>=0 && tmp_poi->GetClusterIdx()>=0){
    TAVTcluster *tmp_vtclu = myVTclus->GetCluster(tmp_poi->GetSensorIdx(), tmp_poi->GetClusterIdx());
    TAVTntuVertex *myVtVtx = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
    for(int ivtx = 0;ivtx<myVtVtx->GetVertexN(); ivtx++){
      TAVTvertex *tmp_vtvtx = myVtVtx->GetVertex(ivtx);
      for(int ivtr = 0;ivtr<tmp_vtvtx->GetTracksN();ivtr++){
        TAVTtrack *tmp_vtvtr = tmp_vtvtx->GetTrack(ivtr);
        for(int ivtcl = 0;ivtcl<tmp_vtvtr->GetClustersN();ivtcl++){
          TAVTbaseCluster *tmp_c = tmp_vtvtr->GetCluster(ivtcl);
          if(tmp_c->GetPosition() == tmp_vtclu->GetPosition())
          	Th_meas = tmp_vtvtr->GetTheta();
        }
      }
    }
    TVector3 clPos = tmp_vtclu->GetPosition();
    TVector3 GLclPos = fpFootGeo->FromVTLocalToGlobal(clPos);
    if(FootDebugLevel(1))
      cout<<" CHECK POS "<<GLclPos.X()<<" "<<xp<<" "<<GLclPos.Y()<<" "<<yp<<" "<<GLclPos.Z()<<" "<<zp<<endl;

    
    if(fFlagMC){
      map<int, map<int, map<int,vector<int>>>> mapall;
      mapall.clear();
      for(int ipix=0;ipix<tmp_vtclu->GetPixelsN();ipix++){
        TAVThit* pixel = tmp_vtclu->GetPixel(ipix);
          if(pixel->GetMcTracksN()>0){
            for(int imcpa = 0; imcpa<pixel->GetMcTracksN();imcpa++){
              
            	int ipa = pixel->GetMcTrackIdx(imcpa);
              
            /*	if(ipa>=0){                
            	  TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(ipa-1);     //Error in <TClonesArray::operator[]>: out of bounds at -1 in ...                
            	  Int_t charge_vt_2 = tmp_mctrack->GetCharge();
                
                
            	  if(FootDebugLevel(1))
            	    cout<<"isen "<<tmp_poi->GetSensorIdx()<<" : vtcl "<<tmp_poi->GetClusterIdx()<<" : pix "<<ipix<<" ::: "<<"Poi MC "<<imcpa<<" = "<<charge<<" - "<<charge_vt_2 <<" "<<pixel->GetMcTrackIdx(imcpa)<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;
                
                
            	  ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeVT"))->Fill(charge_vt_2,charge);
                
              
            	  mapall[pixel->GetMcTrackIdx(imcpa)][charge_vt_2][tmp_mctrack->GetMotherID()+1].push_back(1);

            	  //this is ok to check the "clustering" efficiency at step 0
            	  if(charge_vt_2==charge)
            	    nchargeok_vt++;
            	  nchargeall_vt++;

            	}*/
          }
        }//close if MCtracks
      }//close loop on npix


      if(mapall.size()>1){
        if(FootDebugLevel(1))
          cout<<"MORE than 1 idx : ev = "<<myReader->GetCurrentTriger()<<endl; //to be checked
      }else if(mapall.size()==1){
        npure_vt++;
      }
      if(FootDebugLevel(1)){
        for( const auto& x : mapall ){
          cout << "idx: " << x.first << "  ch: [  ";
          for( const auto& d : x.second ){
          	cout << d.first<<" ] idpa [ ";
          	for( const auto& e : d.second ) cout << e.first<<"] size ["<<e.second.size();
          	cout << "]"<<endl;
          }
        }
      }

      Int_t charge_vt_clu = -999;
      Int_t idx_vt_clu = -1;
      Int_t charge_vt_clu_bis = -999;
      Int_t idx_vt_clu_bis = -1;
      SetMCtruth(mapall, idx_vt_clu, charge_vt_clu);
      SetMCtruthBIS(mapall, idx_vt_clu_bis, charge_vt_clu_bis);

      idxCLU = idx_vt_clu;

      if(FootDebugLevel(1))
        cout<<"CLUSTER Zmc "<<charge_vt_clu<<" Idx "<<idx_vt_clu<<endl;

      if(idx_vt_clu!=idx_vt_clu_bis){
        cout<<"DIFFERENT APPROACHES VT:: "<<GetNtuMcEvt()->GetEventNumber()<<endl;
        cout<<idx_vt_clu<<" "<<idx_vt_clu_bis<<endl;
        cout<<charge_vt_clu<<" "<<charge_vt_clu_bis<<endl;
	DiffApp_trkIdx = true;
      }

      if(charge_vt_clu==charge)
        nchargeok_vt_clu++;
    }

    nchargeall_vt_clu++;
  }

  if(FootDebugLevel(1))
    cout<<"VTanal done"<<endl;

  return;
}


void GlobalRecoAna::ITanal(TAGpoint *tmp_poi, Int_t &idxCLU){

  if(FootDebugLevel(1))
    cout<<"ITanal start"<<endl;

  Double_t xp = (tmp_poi->GetPosition()).X();
  Double_t yp = (tmp_poi->GetPosition()).Y();
  Double_t zp = (tmp_poi->GetPosition()).Z();
  Int_t charge = fGlbTrack->GetTwChargeZ();

  TAITntuCluster *myITclus = (TAITntuCluster*)fpNtuClusIt->GenerateObject();
  if(tmp_poi->GetSensorIdx()>=0 && tmp_poi->GetClusterIdx()>=0){
    TAITcluster *tmp_itclu = myITclus->GetCluster(tmp_poi->GetSensorIdx(), tmp_poi->GetClusterIdx());
    if(fFlagMC) {
      //  TAITparGeo* ITparGeo = (TAITparGeo*)fpParGeoIt->Object();
      TVector3 clPos = tmp_itclu->GetPosition();
      TVector3 GLclPos = fpFootGeo->FromITLocalToGlobal(clPos);
      if(FootDebugLevel(1))
      cout<<"CHECK POS "<<GLclPos.X()<<" "<<xp<<" "<<GLclPos.Y()<<" "<<yp<<" "<<GLclPos.Z()<<" "<<zp<<endl;

      map<int, map<int, map<int,vector<int>>>> mapall;
      mapall.clear();


      for(int ipix=0;ipix<tmp_itclu->GetPixelsN();ipix++){

        TAVThit* pixel_it = tmp_itclu->GetPixel(ipix);

        if(pixel_it->GetMcTracksN()>0){

          for(int imcpa = 0; imcpa<pixel_it->GetMcTracksN();imcpa++){
          	Int_t ipa = pixel_it->GetMcTrackIdx(imcpa);
          	if(ipa>=0){
          	  TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(ipa-1);
          	  Int_t charge_it = tmp_mctrack->GetCharge();
          	  ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeIT"))->Fill(charge_it,charge);
          	  mapall[pixel_it->GetMcTrackIdx(imcpa)][charge_it][tmp_mctrack->GetMotherID()+1].push_back(1);

          	  if(charge_it==charge)
          	    nchargeok_it++;
          	  nchargeall_it++;

              if(FootDebugLevel(1))
                cout<<"sen "<<tmp_poi->GetSensorIdx()<<" : itcl "<<tmp_poi->GetClusterIdx()<<" : pix "<<ipix<<" ::: "<<"Poi MC "<<imcpa<<" = "<<charge<<" - "<<charge_it <<" "<<pixel_it->GetMcTrackIdx(imcpa)<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;

          	}
          }
        }//close if MCtracks

      }//close loop on npix

      if(mapall.size()>1){
        if(FootDebugLevel(1))
          cout<<"MORE than 1 idx : ev = "<<GetNtuMcEvt()->GetEventNumber()<<endl;
      }
      if(mapall.size()==1){
        npure_it++;
      }


      if(FootDebugLevel(1)){
          for( const auto& x : mapall ){
          cout << "idx: " << x.first << "  ch: [  ";
          for( const auto& d : x.second ){
          	cout << d.first<<" ] idpa [ ";
          	for( const auto& e : d.second )
              cout << e.first<<"] size ["<<e.second.size();
          	cout << "]"<<endl;
          }
        }
      }

      Int_t charge_it_clu = -999;
      Int_t idx_it_clu = -1;
      Int_t charge_it_clu_bis = -999;
      Int_t idx_it_clu_bis = -1;
      SetMCtruth(mapall, idx_it_clu, charge_it_clu);
      SetMCtruthBIS(mapall, idx_it_clu_bis, charge_it_clu_bis);
      idxCLU = idx_it_clu;
      if(FootDebugLevel(1))
        cout<<"CLUSTER Zmc "<<charge_it_clu<<" Idx "<<idx_it_clu<<endl;

      if(idx_it_clu!=idx_it_clu_bis){
        cout<<"DIFFERENT APPROACHES IT:: "<<GetNtuMcEvt()->GetEventNumber()<<endl;
        cout<<idx_it_clu<<" "<<idx_it_clu_bis<<endl;
        cout<<charge_it_clu<<" "<<charge_it_clu_bis<<endl;
	DiffApp_trkIdx = true;
      }

      if(charge_it_clu==charge)
        nchargeok_it_clu++;
    }

      nchargeall_it_clu++;
  }

  if(FootDebugLevel(1))
    cout<<"ITanal done"<<endl;

  return;

}


  void GlobalRecoAna::MSDanal(TAGpoint *tmp_poi, Int_t &idxCLU) {

    if(FootDebugLevel(1))
      cout<<"MSDanal start"<<endl;

    Double_t xp = (tmp_poi->GetPosition()).X();
    Double_t yp = (tmp_poi->GetPosition()).Y();
    Double_t zp = (tmp_poi->GetPosition()).Z();

    Int_t charge = fGlbTrack->GetTwChargeZ();

    TAMSDparGeo* MSDparGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
    TAMSDntuCluster *myMSDclus = (TAMSDntuCluster*)fpNtuClusMsd->GenerateObject();

    if(tmp_poi->GetSensorIdx()>=0 && tmp_poi->GetClusterIdx()>=0){
      TAMSDcluster *tmp_msdclu = myMSDclus->GetCluster(tmp_poi->GetSensorIdx(), tmp_poi->GetClusterIdx());

      TVector3 clPos = tmp_msdclu->GetPosition();
      TVector3 GLclPos = fpFootGeo->FromMSDLocalToGlobal(clPos);

      if(fFlagMC) {
        map<int, map<int, map<int,vector<int>>>> mapall;
        mapall.clear();

        if(FootDebugLevel(1))
          cout<<"CHECK POS "<<GLclPos.X()<<" "<<xp<<" "<<GLclPos.Y()<<" "<<yp<<" "<<GLclPos.Z()<<" "<<zp<<endl;

        for(int istri=0;istri<tmp_msdclu->GetStripsN();istri++){

          TAMSDhit* strip = tmp_msdclu->GetStrip(istri);
          if(strip->GetMcTracksN()>0){
            for(int imcpa = 0; imcpa<strip->GetMcTracksN();imcpa++){
            	int ipa = strip->GetMcTrackIdx(imcpa);
            	if(ipa>=0){
            	  TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(ipa-1);
            	  Int_t charge_msd = tmp_mctrack->GetCharge();
            	  if(FootDebugLevel(1))
            	    cout<<"sen "<<tmp_poi->GetSensorIdx()<<" : msdcl "<<tmp_poi->GetClusterIdx()<<" : stri "<<istri<<" ::: "<<"Poi MC "<<imcpa<<" = "<<charge<<" - "<<charge_msd<<" "<<strip->GetMcTrackIdx(imcpa)<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;

            	  ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeMSD"))->Fill(charge_msd,charge);
            	  mapall[strip->GetMcTrackIdx(imcpa)][charge_msd][tmp_mctrack->GetMotherID()+1].push_back(1);
            	  if(charge_msd==charge)
            	    nchargeok_msd++;
            	  nchargeall_msd++;
            	}
            }
          }//close if MCtracks

        }//close loop on strips

        if(mapall.size()>1){
          if(FootDebugLevel(1))
            cout<<"MORE than 1 idx : ev = "<<GetNtuMcEvt()->GetEventNumber()<<endl;
        }
        if(mapall.size()==1)
          npure_msd++;


        if(FootDebugLevel(1)){
          for( const auto& x : mapall ){
            cout << "idx: " << x.first << "  ch: [  ";
            for( const auto& d : x.second ){
            	cout << d.first<<" ] idpa [ ";
            	for( const auto& e : d.second )
                cout << e.first<<"] size ["<<e.second.size();
            	cout << "]"<<endl;
            }
          }
        }

        Int_t charge_msd_clu = -999;
        Int_t idx_msd_clu = -1;
        Int_t charge_msd_clu_bis = -999;
        Int_t idx_msd_clu_bis = -1;
        SetMCtruth(mapall, idx_msd_clu, charge_msd_clu);
        SetMCtruthBIS(mapall, idx_msd_clu_bis, charge_msd_clu_bis);

        idxCLU = idx_msd_clu;

        if(FootDebugLevel(1))
          cout<<"CLUSTER Zmc "<<charge_msd_clu<<" Idx "<<idx_msd_clu<<endl;

        if(idx_msd_clu!=idx_msd_clu_bis){
          cout<<"DIFFERENT APPROACHES MSD:: "<<GetNtuMcEvt()->GetEventNumber()<<endl;
          cout<<idx_msd_clu<<" "<<idx_msd_clu_bis<<endl;
          cout<<charge_msd_clu<<" "<<charge_msd_clu_bis<<endl;
	  DiffApp_trkIdx = true;
        }

        if(charge_msd_clu==charge)
          nchargeok_msd_clu++;

     }

    nchargeall_msd_clu++;
  }

  if(FootDebugLevel(1))
    cout<<"MSDanal done"<<endl;

  return;
}

void GlobalRecoAna::TWanal(TAGpoint *tmp_poi, Int_t &idxCLU){

  if(FootDebugLevel(1))
    cout<<"TWanal start"<<endl;

  Double_t xp = (tmp_poi->GetPosition()).X();
  Double_t yp = (tmp_poi->GetPosition()).Y();
  Double_t zp = (tmp_poi->GetPosition()).Z();

  Int_t charge = fGlbTrack->GetTwChargeZ();

  if(tmp_poi->GetClusterIdx()>=0){
    TATWpoint *tw_point = GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx());
    TVector3 clPos = tw_point->GetPosition();
    TVector3 GLclPos = fpFootGeo->FromTWLocalToGlobal(clPos);

    Int_t charge_tw = tw_point->GetChargeZ();
    twstatus=0;

    if(fFlagMC){
      if(tw_point->GetPointMatchMCtrkID()>=0){
        TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(tw_point->GetPointMatchMCtrkID()-1);

        if(FootDebugLevel(1))
        	cout<<tmp_poi->GetClusterIdx()<<" : Poi MC "<<tw_point->GetPointMatchMCtrkID()<<" = "<<charge<<" - "<<charge_tw <<" - "<<tmp_mctrack->GetCharge()<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;

        ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeTW"))->Fill(charge_tw,charge);

        idxCLU = tw_point->GetPointMatchMCtrkID();
      }

      if(FootDebugLevel(1))
        cout<<"CLUSTER Zmc "<<charge_tw<<endl;

      if(charge_tw==charge)
        nchargeok_tw++;
    }

    nchargeall_tw++;
  }

  if(FootDebugLevel(1))
    cout<<"TWanal done"<<endl;

  return;
}


void GlobalRecoAna::CALOanal(Int_t clusidx){

  if(FootDebugLevel(1))
    cout<<"CALOanal start"<<endl;

    // Bool_t  VtOneTrack = (myVtTr->GetTracksN()==1);
    Int_t Zreco=fGlbTrack->GetTwChargeZ();
    TACAcluster *pCaClu=GetNtuClusterCa()->GetCluster(clusidx);
    ((TH1D*)gDirectory->Get("CALO/GlbTrk/ClusSize_glbtrk"))->Fill(pCaClu->GetHitsN());
    ((TH1D*)gDirectory->Get(Form("CALO/Zreco%d/ClusSize",Zreco)))->Fill(pCaClu->GetHitsN());
    ((TH1D*)gDirectory->Get(Form("CALO/Zreco%d/ClusCharge",Zreco)))->Fill(pCaClu->GetEnergy());

    //
    //
    // ((TH1D*)gDirectory->Get("FIT_vs_Meas/Ekin"))->Fill(fGlbTrack->GetFitEnergy()-pCaClu->GetEnergy());

  if(FootDebugLevel(1))
    cout<<"CALOanal done"<<endl;

  return;
}






void GlobalRecoAna::FullCALOanal(){

  if(FootDebugLevel(1))
    cout<<"FullCALOanal start"<<endl;


  TAVTntuTrack *myVtTr = (TAVTntuTrack*)fpNtuTrackVtx->GenerateObject();

  Double_t caloCharge = 0.;

  ((TH1D*)gDirectory->Get("CALO/ClusNum"))->Fill(GetNtuClusterCa()->GetClustersN());


  map<int,int> MCtwexitpart; //key:TAMCpart index for all the particle exit from tw, value=crossing index for the same particle entering for the first time in calo or -1 if not
  Int_t caloinpartnum=0; //number of particles entering in the calo and coming from the tw
  if(TAGrecoManager::GetPar()->IsRegionMc() && fFlagMC){
    for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
      if(fpNtuMcReg->GetCharge()>0 && fpNtuMcReg->GetMass()>0.9 && fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirTW() && fpNtuMcReg->GetOldCrossN()<=GetParGeoTw()->GetRegStrip(0,GetParGeoTw()->GetNBars()-1) && fpNtuMcReg->GetOldCrossN()>=GetParGeoTw()->GetRegStrip(0,0)) //a particle exit from the tw
        MCtwexitpart[fpNtuMcReg->GetID()-1]=-1;
      if(fpNtuMcReg->GetCharge()>0 && fpNtuMcReg->GetMass()>0.9 && fpNtuMcReg->GetCrossN()>=GetParGeoCa()->GetRegCrystal(0) && fpNtuMcReg->GetCrossN()<=GetParGeoCa()->GetRegCrystal(GetParGeoCa()->GetCrystalsN()-1) && MCtwexitpart.count(fpNtuMcReg->GetID()-1)==1) //a particle entering in the calo that also exit from the tw
        if(MCtwexitpart[fpNtuMcReg->GetID()-1]==-1){
          MCtwexitpart[fpNtuMcReg->GetID()-1]=icr;
          caloinpartnum++;
        }
    }
    ((TH2D*)gDirectory->Get("CALO/MCpartNumvscluster"))->Fill(caloinpartnum,GetNtuClusterCa()->GetClustersN());
    for(auto it = MCtwexitpart.begin() ; it != MCtwexitpart.end() ; ++it)
      if(it->second>=0)
        for(auto iu = std::next(it) ; iu !=MCtwexitpart.end() ; ++iu)
          if(iu->second>=0)
            ((TH1D*)gDirectory->Get("CALO/MultiPartDistance"))->Fill((GetNtuMcReg()->GetRegion(it->second)->GetPosition()-GetNtuMcReg()->GetRegion(iu->second)->GetPosition()).Mag());

  }

  if(FootDebugLevel(1))
    cout<<"FullCALOanal:start loop on all the calo clusters"<<endl;

  for(Int_t i=0;i<GetNtuClusterCa()->GetClustersN();i++){

    // TAMCpart *highChaPart;
    map<int,int> MCtrkXClus; //key: number of MC particles arrived from tw and entering in calo, value: number of crystals with a hit from that particle
    // Int_t partfind=0;
    TAMCregion *ntumchighchreg, *ntumcreg;
    Int_t partcharge=-1;

    TACAcluster *pCaClu=GetNtuClusterCa()->GetCluster(i);
    ((TH1D*)gDirectory->Get("CALO/ClusSize"))->Fill(pCaClu->GetHitsN());

    if(FootDebugLevel(1))
      cout<<"FullCALOanal:start loop on hits, cluster number="<<i<<"/"<<GetNtuClusterCa()->GetClustersN()<<endl;

    if(pCaClu->GetHitsN()==2)
      ((TH1D*)gDirectory->Get("CALO/clus2HitDist"))->Fill((pCaClu->GetHit(0)->GetPosition()-pCaClu->GetHit(1)->GetPosition()).Mag());

    for(Int_t k=0;k<pCaClu->GetHitsN();k++){
      TACAhit* pCaHit=pCaClu->GetHit(k);
      if(fFlagMC){
        ((TH1D*)gDirectory->Get("CALO/MCpartNumXHit"))->Fill(pCaHit->GetMcTracksN());
        if(TAGrecoManager::GetPar()->IsRegionMc()){
          for(Int_t m=0;m<pCaHit->GetMcTracksN();m++){
            if(MCtwexitpart.count(pCaHit->GetMcTrackIdx(m)-1)==1){  //the particle arrived from the tw
              if(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1)>=0){  //the particle arrived from tw and crossed the  calo
                MCtrkXClus[pCaHit->GetMcTrackIdx(m)-1]= MCtrkXClus.count(pCaHit->GetMcTrackIdx(m)-1)+1;
                ntumcreg = GetNtuMcReg()->GetRegion(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1));
                if(m==0)
                  ntumchighchreg=GetNtuMcReg()->GetRegion(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1));
                if(std::round(ntumchighchreg->GetCharge())>partcharge){
                  partcharge=std::round(ntumchighchreg->GetCharge());
                  ntumchighchreg=GetNtuMcReg()->GetRegion(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1));
                }
              }
            }
          }
        }
      }
    }//end of loop on hits

    if(FootDebugLevel(1))
      cout<<"FullCALOanal:start loop on hits done, cluster number="<<i<<"/"<<GetNtuClusterCa()->GetClustersN()<<endl;

    ((TH1D*)gDirectory->Get("CALO/MCpartChavsClusSize"))->Fill(partcharge,pCaClu->GetHitsN());
    if(partcharge>=0 && partcharge<=primary_cha){
      ((TH1D*)gDirectory->Get(Form("CALO/Zmc%d/partNumXCluster",partcharge)))->Fill(MCtrkXClus.size());
      for(const auto& x : MCtrkXClus)
        ((TH1D*)gDirectory->Get(Form("CALO/Zmc%d/partChaXCluster",partcharge)))->Fill(GetNtuMcTrk()->GetTrack(x.first)->GetCharge());

      Double_t mcekin = (sqrt(pow(ntumchighchreg->GetMass(),2) + pow((ntumchighchreg->GetMomentum()).Mag(),2)) - ntumchighchreg->GetMass())/ntumchighchreg->GetMass()*fpFootGeo->GevToMev();
      ((TH2D*)gDirectory->Get(Form("CALO/Zmc%d/EkinVsCluscharge",partcharge)))->Fill(mcekin,pCaClu->GetEnergy()/ntumchighchreg->GetMass());
      ((TH2D*)gDirectory->Get(Form("CALO/Zmc%d/ClusSizeVsTheta",partcharge)))->Fill(pCaClu->GetHitsN(),ntumchighchreg->GetMomentum().Theta());
    }
    ((TH1D*)gDirectory->Get("CALO/ClusCharge"))->Fill(pCaClu->GetEnergy());

    caloCharge += pCaClu->GetEnergy();
  }//end of loop on clusters

  if(fEvtGlbTrkVec.size()>0){
    Int_t caloasscls=0;
    for(Int_t i=0;i<fEvtGlbTrkVec.size();i++){
      if(fEvtGlbTrkVec.at(i).at(4).size()==1){
        if(fEvtGlbTrkVec.at(i).at(4).at(0)>=0)
        caloasscls++;
      }
    }
    ((TH1D*)gDirectory->Get("CALO/GlbTrk/ClusNum_glbtrk"))->Fill(caloasscls);
    ((TH1D*)gDirectory->Get("CALO/GlbTrk/ClusEfficiency"))->Fill((double)caloasscls/fEvtGlbTrkVec.size());
  }


  if(FootDebugLevel(1))
    cout<<"FullCALOanal done"<<endl;

  return;
}

void GlobalRecoAna::SetMCtruth(map<int, map<int, map<int,vector<int>>>> mapall, int &idx, int &charge){

  Int_t my_mult(-1);
  Int_t my_charge(-999);
  Int_t my_idx(-999);
  Int_t my_idpa(-999);

  Int_t tmp_size = 0;

  for(map<int,map<int,map<int,vector<int>>>>::iterator it=mapall.begin(); it!=mapall.end(); it++){
    for(map<int,map<int,vector<int>>>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++){
      for(map<int,vector<int>>::iterator it3=it2->second.begin(); it3!=it2->second.end(); it3++){
      	tmp_size = it3->second.size();
      	if(tmp_size==my_mult){
      	  if(FootDebugLevel(1))
      	    cout<<"same mult"<<endl;
      	  if(it3->first == (my_idx+1)){
      	    break;
      	  }else{
      	    //take the particle with highest charge, but not the nuclear evaporated frag (idpa=0 & icharge != primary charge!
      	    if(it2->first > my_charge && (it3->first==0 && it2->first == primary_cha)){
      	      my_idx = it->first;
      	      my_charge = it2->first;
      	    }else if(it2->first == my_charge && (it3->first==0 && it2->first == primary_cha)){
      	      if(it->first < my_idx){//or take the particle with lowest index -> arbitrary choice!
		my_idx = it->first;
		my_charge = it2->first;
	      }
	    }
	  }

	}
	if(tmp_size > my_mult) {
      	  if(it3->first == (my_idx+1))//if particle is daughter of the previous one
      	    break;
      	  my_idx = it->first;
      	  my_charge = it2->first;
      	  my_idpa = it3->first;
      	  my_mult=tmp_size;
      	}
      }
    }
  }


  idx = my_idx;
  charge = my_charge;

  return;

}

void GlobalRecoAna::SetMCtruthBIS(map<int, map<int, map<int,vector<int>>>> mapall, int &idx, int &charge){

  Int_t my_mult(-1);
  Int_t my_charge(-999);
  Int_t my_idx(-999);
  Int_t my_idpa(-999);

  Int_t tmp_size = 0;

  for(map<int,map<int,map<int,vector<int>>>>::iterator it=mapall.begin(); it!=mapall.end(); it++){
    for(map<int,map<int,vector<int>>>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++){
      for(map<int,vector<int>>::iterator it3=it2->second.begin(); it3!=it2->second.end(); it3++){
	tmp_size = it3->second.size();
      	if(tmp_size > my_mult) {
      	  my_idx = it->first;
      	  my_charge = it2->first;
      	  my_idpa = it3->first;
      	  my_mult=tmp_size;
      	}
      }
    }
  }

  idx = my_idx;
  charge = my_charge;

  return;
}




void GlobalRecoAna::ComputeMCtruth(Int_t trkid, int &cha, TVector3 &mom, TVector3 &mom_cross, double &ek){

  cha = -999, mom.SetXYZ(-999.,-999.,-999.), ek = -999.,  mom_cross.SetXYZ(-999.,-999.,-999.);

  if(trkid !=-1){
    TAMCpart *pNtuMcTrk = GetNtuMcTrk()->GetTrack(trkid);

    cha = pNtuMcTrk->GetCharge();
    mom = pNtuMcTrk->GetInitP();//also MS contribution in target!
    //  mom*=fpFootGeo->GevToMev();
    ek = (sqrt(pow(pNtuMcTrk->GetMass(),2) + pow((pNtuMcTrk->GetInitP()).Mag(),2)) - pNtuMcTrk->GetMass())/pNtuMcTrk->GetMass();
    //  ek*=fpFootGeo->GevToMev();
  }

  if(TAGrecoManager::GetPar()->IsRegionMc()){
    for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
      if((fpNtuMcReg->GetTrackIdx()-1)==trkid){
        if(fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirPreTW() && fpNtuMcReg->GetOldCrossN()==GetParGeoG()->GetRegTarget()){
          mom_cross = fpNtuMcReg->GetMomentum();
        }
      }
    }
  }

  if(FootDebugLevel(1))
    cout<<"TRUE MC info:: Z = "<<cha<<"; |P| = "<<mom.Mag()<<"; Ek = "<<ek<<"; |Pcross| = "<<mom_cross.Mag()<<endl;

  return;
}



Double_t GlobalRecoAna::ComputeTrkEkin(TAGtrack *fGlbTrack){

  //Double_t Ek_meas = fGlbTrack->GetCalEnergy()/atomassu;
  //Double_t Ek_meas = -1.;
  Double_t Ek_meas = 0.;

  Double_t ZCalo = fpFootGeo->GetCACenter().Z()-GetParGeoCa()->GetCrystalThick();
  if(FootDebugLevel(1))
    cout<<"Zcalo = "<<ZCalo<<" "<<fpFootGeo->GetCACenter().Z()<<" "<<GetParGeoCa()->GetCrystalThick()<<endl;

  TAGtrack::polynomial_fit_parameters trk_param = fGlbTrack->GetParameters();

  Double_t Yint_Calo = trk_param.parameter_y[1] * ZCalo + trk_param.parameter_y[0];
  Double_t Xint_Calo = trk_param.parameter_x[3] * pow(ZCalo,3) + trk_param.parameter_x[2] * pow(ZCalo,2) + trk_param.parameter_x[1] * ZCalo + trk_param.parameter_x[0];

  TVector3 Rint(0.,0.,0.), eRint(0.,0.,0.);
  Double_t distmin = 999.;
  Rint.SetXYZ(Xint_Calo,Yint_Calo,ZCalo);
  eRint.SetXYZ(2.*sqrt(2)*2.,2.*sqrt(2)*2.,2.);//test 0 : diagonale 2 cristalli + 2 cm in z (~ from minZ:maxZ calo)

  TACAntuCluster *myCAntuclu = (TACAntuCluster*)fpNtuClusCa->GenerateObject();

  for(int ica = 0; ica< myCAntuclu->GetClustersN();ica++){

	TACAcluster *myCAclu = myCAntuclu->GetCluster(ica);
	TVector3 caclu_local = myCAclu->GetPosition();
	TVector3 caclu_pos = fpFootGeo->FromCALocalToGlobal(caclu_local);
	TVector3 caclu_epos = myCAclu->GetPosError();

	//	for(int ih = 0; ih < myCAclu->GetHitsN();ih++){

	//	  TACAhit *myCAhit = myCAclu->GetHit(ih);

	//	  TVector3 cahit_local = myCAhit->GetPosition();
	//	  TVector3 cahit_pos = fpFootGeo->FromCALocalToGlobal(cahit_local);
	  //	  TVector3 cahit_epos = myCAhit->GetPosError();//non esiste. va implementata
	//	  TVector3 cahit_epos(0.,0.,0.);
	//	  cahit_epos.SetXYZ(1.,1.,1.);//half of crystal side

	  if(FootDebugLevel(1)){
	    cout<<"Rint hit pos = "<<Rint.X()<<" "<<Rint.Y()<<" "<<Rint.Z()<<endl;
	    //	  cout<<"calo hit local = "<<cahit_local.X()<<" "<<cahit_local.Y()<<" "<<cahit_local.Z()<<endl;
	    cout<<"calo clus pos = "<<caclu_pos.X()<<" "<<caclu_pos.Y()<<" "<<caclu_pos.Z()<<endl;
	    //	    cout<<"calo hit pos = "<<cahit_pos.X()<<" "<<cahit_pos.Y()<<" "<<cahit_pos.Z()<<endl;
	  //	  cout<<"calo hitepos = "<<cahit_epos.X()<<" "<<cahit_epos.Y()<<" "<<cahit_epos.Z()<<endl;
	  }
	  //	  if( (Rint-caclu_pos).Mag() <= (eRint).Mag() ){
	  if( (Rint-caclu_pos).Mag() <= distmin && distmin <= caclu_epos.Mag()){
	    if(FootDebugLevel(1))
	      //	      cout<<"IN hit "<<cahit_pos.X()<<" "<<cahit_pos.Y()<<" "<<cahit_pos.Z()<<endl;
	      cout<<"IN clu "<<caclu_pos.X()<<" "<<caclu_pos.Y()<<" "<<caclu_pos.Z()<<endl;
	    //sum of the energy in each crystal (hit) within a certain distance from the glb track extrapolation to the calo
	    //	    Ek_meas += (myCAhit->GetCharge()*fpFootGeo->MevToGev());
	    //	    Ek_meas += (myCAclu->GetEnergy()*fpFootGeo->MevToGev());
	    distmin = (Rint-caclu_pos).Mag();
	    Ek_meas = (myCAclu->GetEnergy()*fpFootGeo->MevToGev());

	  }

	  ((TH1D*)gDirectory->Get("CALO/distminMag"))->Fill(distmin<999 ? distmin:-0.5);
	  ((TH1D*)gDirectory->Get("CALO/DistX_trkclu"))->Fill(Rint.X()-caclu_pos.X());
	  ((TH1D*)gDirectory->Get("CALO/DistY_trkclu"))->Fill(Rint.Y()-caclu_pos.Y());
	  ((TH1D*)gDirectory->Get("CALO/DistZ_trkclu"))->Fill(Rint.Z()-caclu_pos.Z());

	  //      }close loop on Nhits

      }//close loop on n clusters

      return Ek_meas;//Energy in GeV

}

void GlobalRecoAna::PrintNCharge(){

  cout<<"-----------NchargeOK/NchargeALL-----------"<<endl;
  if(nchargeall_vt)
    cout<<"VT pix:: "<<((double)nchargeok_vt/(double)nchargeall_vt)*100.<<endl;
  if(nchargeall_vt_clu){
    cout<<"VT clu:: "<<((double)nchargeok_vt_clu/(double)nchargeall_vt_clu)*100.<<endl;
    cout<<"VT clu pure :: "<<((double)npure_vt/(double)nchargeall_vt_clu)*100.<<endl;
    cout<<endl;
  }
  if(nchargeall_it)
    cout<<"IT pix :: "<<((double)nchargeok_it/(double)nchargeall_it)*100.<<endl;
  if(nchargeall_it_clu){
    cout<<"IT clu :: "<<((double)nchargeok_it_clu/(double)nchargeall_it_clu)*100.<<endl;
    cout<<"IT clu pure :: "<<((double)npure_it/(double)nchargeall_it_clu)*100.<<endl;
    cout<<endl;
  }
  if(nchargeall_msd)
    cout<<"MSD strip :: "<<((double)nchargeok_msd/(double)nchargeall_msd)*100.<<endl;
  if(nchargeall_msd_clu){
    cout<<"MSD clu :: "<<((double)nchargeok_msd_clu/(double)nchargeall_msd_clu)*100.<<endl;
    cout<<"MSD clu pure :: "<<((double)npure_msd/(double)nchargeall_msd_clu)*100.<<endl;
    cout<<endl;
  }
  if(nchargeall_tw)
    cout<<"TW :: "<<((double)nchargeok_tw/(double)nchargeall_tw)*100.<<endl;
  cout<<endl;
  if(ntracks)
    cout<<"Npure/Ntracks :: "<<((double)npure/(double)ntracks)*100.<<endl<<endl;

  for(Int_t i=1;i<pure_track_xcha.size();i++)
    if(pure_track_xcha.at(i).second)
      cout<<"Npure/Ntracks Z="<<i<<" :: "<<((double)pure_track_xcha.at(i).first/(double)pure_track_xcha.at(i).second)*100.<<endl;

  return;

}


void GlobalRecoAna::FillGlbTrackPlots(){
  ((TH1D*)gDirectory->Get("FIT_vs_Meas/Ekin"))->Fill(fGlbTrack->GetFitEnergy()-fGlbTrack->GetCalEnergy());
  ((TH1D*)gDirectory->Get("FIT_vs_Meas/Tof"))->Fill(fGlbTrack->GetFitTof()-(fGlbTrack->GetTwTof()));
  ((TH1D*)gDirectory->Get("FIT_vs_Meas/Mass"))->Fill(fGlbTrack->GetFitMass()-mass_ana->GetMassBest()*TAGgeoTrafo::AmuToGev());
  return;
}

void GlobalRecoAna::FillMassPlots(){

  Int_t Z_meas=fGlbTrack->GetTwChargeZ();
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/chi2min",Z_meas)))->Fill(mass_ana->GetChiValue());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/almmin",Z_meas)))->Fill(mass_ana->GetAlmValue());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/mom_bestvsmeas",Z_meas)))->Fill((mass_ana->GetMomBest()-mass_ana->GetMomMeas())*TAGgeoTrafo::GevToMev());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/ekin_bestvsmeas",Z_meas)))->Fill((mass_ana->GetEkinBest()-mass_ana->GetEkinMeas())*TAGgeoTrafo::GevToMev());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/tof_bestvsmeas",Z_meas)))->Fill(mass_ana->GetTofBest()-mass_ana->GetTofMeas());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_pb",Z_meas)))->Fill(mass_ana->GetMassPb());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_pberr",Z_meas)))->Fill(mass_ana->GetMassErrPb());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_be",Z_meas)))->Fill(mass_ana->GetMassBe());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_beerr",Z_meas)))->Fill(mass_ana->GetMassErrBe());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_pe",Z_meas)))->Fill(mass_ana->GetMassPe());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_peerr",Z_meas)))->Fill(mass_ana->GetMassErrPe());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_wavg",Z_meas)))->Fill(mass_ana->GetMassWavg());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_wavgerr",Z_meas)))->Fill(mass_ana->GetMassErrWavg());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_chi",Z_meas)))->Fill(mass_ana->GetMassChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_chierr",Z_meas)))->Fill(mass_ana->GetMassErrChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_chi",Z_meas)))->Fill(mass_ana->GetMomChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_chierr",Z_meas)))->Fill(mass_ana->GetMomErrChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_chi",Z_meas)))->Fill(mass_ana->GetEkinChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_chierr",Z_meas)))->Fill(mass_ana->GetEkinErrChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_chi",Z_meas)))->Fill(mass_ana->GetTofChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_chierr",Z_meas)))->Fill(mass_ana->GetTofErrChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_alm",Z_meas)))->Fill(mass_ana->GetMassAlm());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_almerr",Z_meas)))->Fill(mass_ana->GetMassErrAlm());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_alm",Z_meas)))->Fill(mass_ana->GetMomAlm());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_almerr",Z_meas)))->Fill(mass_ana->GetMomErrAlm());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_alm",Z_meas)))->Fill(mass_ana->GetEkinChi());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_almerr",Z_meas)))->Fill(mass_ana->GetEkinErrAlm());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_alm",Z_meas)))->Fill(mass_ana->GetTofAlm());
  ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_almerr",Z_meas)))->Fill(mass_ana->GetTofErrAlm());
  ((TH1D*)gDirectory->Get("MassReco/InputStatus"))->Fill(mass_ana->GetInputStatus());
  ((TH1D*)gDirectory->Get("MassReco/ChiMassStatus"))->Fill(mass_ana->GetChiFitStatus());
  ((TH1D*)gDirectory->Get("MassReco/AlmMassStatus"))->Fill(mass_ana->GetAlmFitStatus());

  return;
}
void GlobalRecoAna::StudyThetaReso(){

  if(FootDebugLevel(1))
    cout<<"StudyThetaReso start"<<endl;

  TH1D *hres, *mhres;
  TF1 *fgaus = new TF1("fgaus","gaus(0)",-1,1);
  fgaus->SetParameters(1000.,0.,0.5);

  Double_t binwidth = (((TH2D*)gDirectory->Get("MC/Z1/Theta_reso_cross_vs_th"))->GetXaxis())->GetBinWidth(1);
  const Int_t nbinsx = ((TH2D*)gDirectory->Get("MC/Z1/Theta_reso_cross_vs_th"))->GetNbinsX();

  //  cout<<"BIN WIDTH THETA"<<binwidth<<endl;


  Double_t mean[nbinsx], sigma[nbinsx];
  Double_t emean[nbinsx], esigma[nbinsx];

  //vs th true
  Double_t tmp_thcross[nbinsx], tmp_ethcross[nbinsx];
  vector<TGraphErrors*> gr_mea, gr_sig;

  //vs th meas
  Double_t tmp_thm[nbinsx], tmp_ethm[nbinsx];
  vector<TGraphErrors*> grm_mea, grm_sig;

  TCanvas *cc = new TCanvas("cc","",1000.,1000.);
  cc->Divide(10,10);

  for(Int_t iz=1;iz<=primary_cha;iz++){

    for(Int_t ibin = 0; ibin < nbinsx; ibin++){

      mean[ibin] = -999., sigma[ibin] = -999.;
      emean[ibin] = -999., esigma[ibin] = -999.;
      tmp_thcross[ibin] = -999., tmp_ethcross[ibin] = -999.;

      tmp_thm[ibin] = -999., tmp_ethm[ibin] = -999.;

      tmp_thcross[ibin] = (((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_th",iz)))->GetXaxis())->GetBinCenter(ibin+1);
      tmp_ethcross[ibin] = binwidth/sqrt(12.);

      tmp_thm[ibin] = (((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_thmeas",iz)))->GetXaxis())->GetBinCenter(ibin+1);
      tmp_ethm[ibin] = binwidth/sqrt(12.);

      if( (ibin+2) <= nbinsx ){

      	hres = ((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_th",iz)))->ProjectionY("",ibin+1, ibin+2,"");
      	hres->Fit("fgaus","","",-1,1);

	      mean[ibin] = fgaus->GetParameter(1);
      	emean[ibin] = fgaus->GetParError(1);
      	sigma[ibin] = fgaus->GetParameter(2);
      	esigma[ibin] = fgaus->GetParError(2);

	      if(hres->GetEntries()<30.){
      	  mean[ibin] = -999.;
      	  emean[ibin] = 0.;
      	  sigma[ibin] = -999.;
      	  esigma[ibin] = 0.;
      	}

      	cc->cd(ibin+1);
      	hres->Draw();
      	cc->Update();

        gDirectory->cd(Form("MC/Z%d/ThetaRes",iz));
      	hres->Write(Form("hres_%d",ibin+1));
      	gDirectory->cd("../../..");


      }
    }//close loop on nbinsx

    cc->SaveAs(Form("hres_Z%d.pdf",iz));

    gr_mea.push_back(new TGraphErrors(nbinsx, tmp_thcross, mean, tmp_ethcross, emean));
    gr_sig.push_back(new TGraphErrors(nbinsx, tmp_thcross, sigma, tmp_ethcross, esigma));

    grm_mea.push_back(new TGraphErrors(nbinsx, tmp_thm, mean, tmp_ethm, emean));
    grm_sig.push_back(new TGraphErrors(nbinsx, tmp_thm, sigma, tmp_ethm, esigma));

  }//close loop on Zid

  TMultiGraph *mulmea = new TMultiGraph();
  TLegend* legmea = new TLegend(0.1,0.7,0.48,0.9);

  TMultiGraph *mmulmea = new TMultiGraph();
  TLegend* mlegmea = new TLegend(0.1,0.7,0.48,0.9);

  for(Int_t iz=0;iz<primary_cha;iz++){
    gr_mea[iz]->SetMarkerColor(iz+1);
    mulmea->Add(gr_mea[iz],"pe");
    legmea->AddEntry(gr_mea[iz],Form("Z=%d",iz+1));

    grm_mea[iz]->SetMarkerColor(iz+1);
    mmulmea->Add(grm_mea[iz],"pe");
    legmea->AddEntry(grm_mea[iz],Form("Z=%d",iz+1));
  }

  mulmea->Draw("A");
  legmea->Draw();
  mulmea->Write("restheta_mean");

  mmulmea->Draw("A");
  mlegmea->Draw();
  mmulmea->Write("resthetameas_mean");


  TMultiGraph *mulsig = new TMultiGraph();
  TLegend* legsig = new TLegend(0.1,0.7,0.48,0.9);

  TMultiGraph *mmulsig = new TMultiGraph();
  TLegend* mlegsig = new TLegend(0.1,0.7,0.48,0.9);

  for(Int_t iz=0;iz<primary_cha;iz++){
    gr_sig[iz]->SetMarkerColor(iz+1);
    mulsig->Add(gr_sig[iz],"pe");
    legsig->AddEntry(gr_sig[iz],Form("Z=%d",iz+1));

    grm_sig[iz]->SetMarkerColor(iz+1);
    mmulsig->Add(grm_sig[iz],"pe");
    mlegsig->AddEntry(grm_sig[iz],Form("Z=%d",iz+1));

  }
  mulsig->Draw("A");
  legsig->Draw();
  mulsig->Write("restheta_sigma");

  mmulsig->Draw("A");
  mlegsig->Draw();
  mmulsig->Write("resthetameas_sigma");


  if(FootDebugLevel(1))
    cout<<"StudyThetaReso done"<<endl;

  return;
}

void GlobalRecoAna::BeforeEventLoop(){

  CampaignChecks();
  ReadParFiles();
  SetupTree();
  myReader->Open(GetName(), "READ", "tree");

  file_out = new TFile(GetTitle(),"RECREATE");
  cout<<"Going to create "<<GetTitle()<<" outfile "<<endl;
//  SetRunNumber(runNb); //serve veramente?
  //  myReader->GetTree()->Print();
  gTAGroot->BeginEventLoop();
  mass_ana=new GlobalRecoMassAna();

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

  
  

  if(FootDebugLevel(1))
    cout<<"primary_cha="<<primary_cha<<"  beam_mass_number="<<beam_mass_number<<"  beam_energy="<<beam_energy<<"  beam_speed="<<beam_speed<<"  primary_tof="<<primary_tof<<endl;
    cout<<"N_target="<< Ntg << endl;
    cout <<"target density="<< GetParGeoG()->GetTargetPar().Density << endl;
    cout << "target z=" << GetParGeoG()->GetTargetPar().Size.Z() << endl;
    cout << "target A=" << GetParGeoG()->GetTargetPar().AtomicMass << endl;
  return;
}

void GlobalRecoAna::AfterEventLoop(){

  //stamp luminosity
      
      h = new TH1D("luminosity","",1, 0. ,1.);
      ((TH1D*)gDirectory->Get("luminosity"))->SetBinContent(1,Ntg*nTotEv  );
  

  gTAGroot->EndEventLoop();

  //if(fFlagMC)
    //StudyThetaReso(); //to be fixed!!!
  //PrintNCharge();

  cout <<" Writing..." << endl;
  file_out->Write();
  
  cout <<"Closing..." << endl;
  file_out->Close();
  
  return;
}

void GlobalRecoAna::resetStatus(){
twstatus=-1;
return;
}
