// written by mtoppi for GSI2019 data analysis
// to be run in shoe/build/Reconstruction
// therein set-up the enviroment:  source ../../setupFOOT.sh
// and run with (for example): root -l -b -q AnalyzeTW_MC.cc++g\(\"../../../../rootfiles/outMC_16O_C_400_1_GSI.root\",1,10,\"\"\)

#include "AnalyzeTracks.h"

#define RAD2DEG (180./TMath::Pi())


struct PolynomialFit_t  {
PolynomialFit_t() : parameter_x(), parameter_y() {}
std::array<double, 4> parameter_x; ///< parameter x
std::array<double, 2> parameter_y; ///< parameter y
};

// main
void AnalyzeTracks(TString infile = "testMC.root", TString outfile = "AnaTW_MC.root", Int_t nev=1e9)
  
{

  
  TAGroot gTAGroot;

  TFile *inputFile = new TFile(infile.Data());
  runinfo=(TAGrunInfo*)(inputFile->Get("runinfo"));
  const TAGrunInfo construninfo(*runinfo);
  gTAGroot.SetRunInfo(construninfo);

  TString expName=runinfo->CampaignName();
  if(expName.EndsWith("/")) //fix a bug present in shoe
    expName.Remove(expName.Length()-1);
  
  Int_t runNumber=runinfo->RunNumber();
  TAGrecoManager::Instance(expName);
  
  TAGcampaignManager* campManager = new TAGcampaignManager(expName);
  campManager->FromFile();

  // retrieve info from run and geometry
  GetRunAndGeoInfo(campManager,runNumber);

  const Char_t* name = FootActionDscName("TAGactTreeReader");
  TAGactTreeReader* fActReader  = new TAGactTreeReader(name);
  // TAGactTreeReader* fActReader  = new TAGactTreeReader("ActEvtReader");
  // retrieve info from the Tree  
  SetTreeBranchAddress(fActReader);

  std::cout<<"Input file (-in) : "<<infile.Data()<<std::endl;
  std::cout<<"Output file (-out) : "<<outfile.Data()<<std::endl<<endl;


  Int_t  TG_region = parGeo->GetRegTarget();
  Int_t  AirPreTW_region = parGeo->GetRegAirPreTW();
  Int_t  AirTW_region = parGeo->GetRegAirTW();  
  Int_t  firstBarID(0), lastBarID(19);
  Int_t TW_regions[kTWreg] = {twparGeo->GetRegStrip((int)LayerY,firstBarID),
                              twparGeo->GetRegStrip((int)LayerY,lastBarID),
                              twparGeo->GetRegStrip((int)LayerX,firstBarID),
                              twparGeo->GetRegStrip((int)LayerX,lastBarID)};   // min and max rear and front TW regions
  
  printf("ExpName::%s  TG_region::%d  TW_regions: front=[%d,%d] rear=[%d,%d], AirPreTW::%d, AirTW::%d\n",expName.Data(),TG_region,TW_regions[2],TW_regions[3],TW_regions[0],TW_regions[1],AirPreTW_region,AirTW_region);
  // getchar();
  

  TFile *fout = new TFile(outfile.Data(),"RECREATE");
  fout->cd();

  BookHistograms();
  
  
  cout<<"Beam Z:: "<<parGeo->GetBeamPar().AtomicNumber<<" A:: "<<parGeo->GetBeamPar().AtomicMass<<" ion::"<<parGeo->GetBeamPar().Material<<endl;
  
  cout<<"TG center:: "<<geoTrafo->GetTGCenter().z()<<endl<<" TG thickness:: "<<parGeo->GetTargetPar().Size.z()<<" TG material:: "<<parGeo->GetTargetPar().Material<<endl;
  
  printf("TW center =  (%2.1f,%2.1f,%2.1f)\n",GetTwCenter().x(),GetTwCenter().y(),GetTwCenter().z());
  
  //cout<<"theta angle of TW  acceptance::"<<GetMaxAngle()*180/TMath::Pi()<<endl;
  
  if (nev == 0) {
    printf("number of event to be processed is %d...set a number > 0",nev);
    return;
  }
  
  fActReader->Open(infile);
  gTAGroot.AddRequiredItem(fActReader);
  

  Int_t nentries = fActReader->NEvents();
  Int_t nevMax = (nev<nentries) ? nev : nentries;

  printf("Max available number of Entries in this Tree is::%d, going to analyze %d events\n\n",(int)nentries,nevMax);
  
  
  // Loop over the TTree
  gTAGroot.BeginEventLoop();
  Int_t ev = -1;
  
  while (gTAGroot.NextEvent() && ev!=nevMax){

    ev++;
    
    if(debug)
      printf("\n Event: %d\n", ev);
    else
      if( ev % 1000 == 0) printf("Event: %d\n", ev);
    
    

    Int_t nbmtrk =  bmNtuTrack->GetTracksN();
    if(nbmtrk!=1) continue;
    TVector3  dirbm = bmNtuTrack->GetTrack(0)->GetSlope();
    TVector3  rbm = bmNtuTrack->GetTrack(0)->GetOrigin();


    Int_t nGlbTracks=glbntutrk->GetTracksN();
    for(int itr=0;itr<nGlbTracks;itr++){
      
      //reco quantities
      TAGtrack *glbtrk = glbntutrk->GetTrack(itr);
      Int_t Zreco = glbtrk->GetFitChargeZ(); 
      Int_t ZTW = glbtrk->GetTwChargeZ(); 
      Int_t gtrkId = glbtrk->GetTrackId();
      Int_t gmctrkId = glbtrk->GetMcMainTrackId();
      TVector3 rTGT = glbtrk->GetTgtPosition();
      TVector3 rTW = glbtrk->GetTwPosition();
      TVector3 dirTGT = glbtrk->GetTgtDirection();
      Double_t thetaTGT = glbtrk->GetTgtTheta()*RAD2DEG;
      Double_t thetaTGTbm = glbtrk->GetTgtThetaBm()*RAD2DEG;
      Double_t  pval = glbtrk->GetPval();
      TVector3 poca = GetPOCA(rTGT,dirTGT, rbm, dirbm);

      //true quantities
      TAMCpart *mctrk = mcNtuPart->GetTrack(gmctrkId);
      Int_t mothId = mctrk->GetMotherID();
      TVector3 initPos = mctrk->GetInitPos();
      TVector3 initP = mctrk->GetInitP();
      TVector3 primaryMomentum = mcNtuPart->GetTrack(0)->GetInitP();
      Int_t reg = mctrk->GetRegion();  // region where track is generated
      int Ztrue = mctrk->GetCharge();
      int baryon = mctrk->GetBaryon();
      double mass = mctrk->GetMass();
      double ekin = (sqrt(initP.Mag2()+mass*mass)-mass)/(double)baryon*TAGgeoTrafo::GevToMev(); 
      double beta=(initP.Mag()/sqrt(initP.Mag2()+mass*mass));
      Double_t thetamc = initP.Theta()*RAD2DEG;
      Double_t thetamcbm = initP.Angle(primaryMomentum)*RAD2DEG;

      Bool_t recoFromTGT = (poca.Z()<0.5 && poca.Z()>0.5); 
      
      hZRecoVsTrue->Fill(Zreco,Ztrue);
      if(recoFromTGT)hZRecoVsTrueCut->Fill(Zreco,Ztrue);
      hPOCA->Fill(poca.X(), poca.Y(), poca.Z());
      hPosTGT->Fill(rTGT.X(),rTGT.Y(),rTGT.Z());
      if(Ztrue>=1 && Ztrue<=kCharges){
	hPOCA_Z[Ztrue-1]->Fill(poca.X(), poca.Y(), poca.Z());
	hPosTGT_Z[Ztrue-1]->Fill(rTGT.X(),rTGT.Y(),rTGT.Z());
      }
      

      Int_t nPoints = glbtrk->GetPointsN();
      for(int ipo=0;ipo<nPoints;ipo++){
	TAGpoint *trkpoi = glbtrk->GetPoint(ipo);
	Int_t ipla = trkpoi->GetSensorIdx();
	string dev = trkpoi->GetDevName();
	TVector3 cluPos = trkpoi->GetMeasPosition();
	TVector3 fitPos = trkpoi->GetFitPosition();
	if(Ztrue>=1 && Ztrue<=kCharges){
	  if(dev=="VT"){
	    //	    cout << "clus::" << cluPos.X() << "   fit::" << fitPos.X() << endl;
	    hResxVT_plane[ipla]->Fill(cluPos.X()-fitPos.X());
	    hResyVT_plane[ipla]->Fill(cluPos.Y()-fitPos.Y());
	    hReszVT_plane[ipla]->Fill(cluPos.Z()-fitPos.Z());
	    hResxVT_plane_Z[ipla][Ztrue-1]->Fill(cluPos.X()-fitPos.X());
	    hResyVT_plane_Z[ipla][Ztrue-1]->Fill(cluPos.Y()-fitPos.Y());
	    hReszVT_plane_Z[ipla][Ztrue-1]->Fill(cluPos.Z()-fitPos.Z());
	  }
	  if(dev=="MSD"){
	    hResxMSD_plane[ipla]->Fill(cluPos.X()-fitPos.X());
	    hResyMSD_plane[ipla]->Fill(cluPos.Y()-fitPos.Y());
	    hReszMSD_plane[ipla]->Fill(cluPos.Z()-fitPos.Z());
	    hResxMSD_plane_Z[ipla][Ztrue-1]->Fill(cluPos.X()-fitPos.X());
	    hResyMSD_plane_Z[ipla][Ztrue-1]->Fill(cluPos.Y()-fitPos.Y());
	    hReszMSD_plane_Z[ipla][Ztrue-1]->Fill(cluPos.Z()-fitPos.Z());
	  }
	  if(dev=="TW"){
	    hResxTW->Fill(cluPos.X()-fitPos.X());
	    hResyTW->Fill(cluPos.Y()-fitPos.Y());
	    hReszTW->Fill(cluPos.Z()-fitPos.Z());
	    hResxTW_Z[Ztrue-1]->Fill(cluPos.X()-fitPos.X());
	    hResyTW_Z[Ztrue-1]->Fill(cluPos.Y()-fitPos.Y());
	    hReszTW_Z[Ztrue-1]->Fill(cluPos.Z()-fitPos.Z());
	  }
	}
      }
      

      if(reg == TG_region){
	hResTGT->Fill(rTGT.X() - initPos.X(), rTGT.Y() - initPos.Y());
	hResThetaTGT->Fill(thetaTGT-thetamc);
	hResThetaTGTbm->Fill(thetaTGTbm-thetamcbm);
	hResThetaVsThetaTGT->Fill(thetamc, thetaTGT-thetamc);
	hResThetaVsThetaTGTbm->Fill(thetamcbm, thetaTGTbm-thetamcbm);
	hThetaRecoVsTrueTGT->Fill(thetamc, thetaTGT);
	hThetaRecoVsTrueTGTbm->Fill(thetamcbm, thetaTGTbm);
	if(Ztrue>=1 && Ztrue<=kCharges){
	  hResTGT_Z[Ztrue-1]->Fill(rTGT.X() - initPos.X(), rTGT.Y() - initPos.Y());
	  hResThetaTGT_Z[Ztrue-1]->Fill(thetaTGT-thetamc);
	  hResThetaTGTbm_Z[Ztrue-1]->Fill(thetaTGTbm-thetamcbm);
	  hResThetaVsThetaTGT_Z[Ztrue-1]->Fill(thetamc, thetaTGT-thetamc);
	  hResThetaVsThetaTGTbm_Z[Ztrue-1]->Fill(thetamcbm, thetaTGTbm-thetamcbm);
	  hThetaRecoVsTrueTGT_Z[Ztrue-1]->Fill(thetamc, thetaTGT);
	  hThetaRecoVsTrueTGTbm_Z[Ztrue-1]->Fill(thetamcbm, thetaTGTbm);
	}
      }
    }







    int nMCregions = mcNtuRegion->GetRegionsN();
    // loop over MC crossed regions        
    for(int imcreg=0; imcreg<nMCregions; imcreg++) {
      
        TAMCregion *CrossReg = mcNtuRegion->GetRegion(imcreg);
	Int_t trkId = CrossReg->GetTrackIdx()-1;
        Int_t nCross = CrossReg->GetCrossN();  // id crossed region
        Int_t nCrossOld = CrossReg->GetOldCrossN();  // id old crossed region
        Double_t posIn_z = CrossReg->GetPosition().Z();
        Double_t mass = CrossReg->GetMass();
        Double_t Z = CrossReg->GetCharge();
        Double_t time = CrossReg->GetTime()*TAGgeoTrafo::SecToNs();
	TAMCpart *mctrk = mcNtuPart->GetTrack(trkId);
        TVector3 initPos = mctrk->GetInitPos();
        TVector3 initP = mctrk->GetInitP();
        int mothId = mctrk->GetMotherID();
        int reg = mctrk->GetRegion();  // region where track is generated
        double beta=(initP.Mag()/sqrt(initP.Mag2()+mass*mass));
        double ekin_Z = (1/sqrt(1-beta*beta) - 1)*1000;  //is similar to Ekin/A in MeV
        double ekin=sqrt(initP.Mag2()+pow(mctrk->GetMass(),2))-mctrk->GetMass();
        if(debug)
          printf("reg::%d, trkId::%d, Z::%1.0f, A::%f, pos::%f, t::%f, cross::%d, old::%d\n",imcreg,trkId,Z,mass,posIn_z,time,nCross,nCrossOld);            
    } 
    

    // N of MC tracks per event
    int nMCtrk = mcNtuPart->GetTracksN();
    
    for(int imctr=0; imctr<nMCtrk; imctr++) {
      TAMCpart *mctrk = mcNtuPart->GetTrack(imctr);
      int mothId = mctrk->GetMotherID();
      TVector3 initPos = mctrk->GetInitPos();
      TVector3 initP = mctrk->GetInitP();
      TVector3 finalPos = mctrk->GetFinalPos();
      TVector3 finalP = mctrk->GetFinalP();
      int reg = mctrk->GetRegion();  // region where track is generated
      Int_t reg_dead = mctrk->GetDead(); // region where track die
      int Z = mctrk->GetCharge();
      int baryon = mctrk->GetBaryon();
      double mass = mctrk->GetMass();
      double ekin = (sqrt(initP.Mag2()+mass*mass)-mass)/(double)baryon*TAGgeoTrafo::GevToMev(); // Ekin/A in MeV
      double beta=(initP.Mag()/sqrt(initP.Mag2()+mass*mass));
      double ekin_Z = (1/sqrt(1-beta*beta) - 1)*1000;  //is similar to Ekin/A in MeV
      if(debug) {
        cout<<"imctrk::"<<imctr<<" mothId::"<<mothId<<" Z::"<<Z<<endl;
        cout<<"reg::"<<reg<<" posZ::"<<initPos.Z()<<" F_posZ::"<<finalPos.Z()<<" mass::"<<mass<<endl;
        cout<<"Z::"<<Z<<"  p::"<<initP.Mag()*1000<<" E::"<<sqrt(initP.Mag2()+mass*mass)*1000<<" beta::"<<beta<<"  ekin_Z::"<<ekin_Z<<" ekin::"<<ekin<<endl<<endl;
      }
    }
    
      // fragmentation In-target from primaries
    //    if(Z>0 && Z<GetZbeam()+1 && mothId==(Int_t)kPrimaryID && reg==TG_region) {
    //if(finalPos.z() > geoTrafo->GetTGCenter().z()+parGeo->GetTargetPar().Size.z()/2 ) { // if the fragment exit from the target
    //}
    //}
      
  }


  cout<<endl<<"Job Done!"<<endl;   


  fout->cd();
  fout->Write();
  fout->Close();

  return;

}

//-----------------------------------------------------------------------------

void BookHistograms() {
  
  for(int Z=1;Z<=kCharges;Z++){
    gDirectory->mkdir(Form("Z_%d",Z));
    gDirectory->cd(Form("Z_%d",Z));
    
    hResTGT_Z[Z-1] = new TH2D(Form("ResidualsTruePosTGT_Z%d",Z),"",1000,-5,5,1000,-5,5);
    hResThetaTGT_Z[Z-1] = new TH1D(Form("ResidualsThetaTGT_Z%d",Z),"",1000,-5,5);//deg
    hResThetaTGTbm_Z[Z-1] = new TH1D(Form("ResidualsThetaTGTbm_Z%d",Z),"",1000,-5,5);//deg
    hResThetaVsThetaTGT_Z[Z-1] = new TH2D("ResoThetaVsThetaTGT","",60,0,30,1000,-5,5);//deg
    hResThetaVsThetaTGTbm_Z[Z-1] = new TH2D("ResoThetaVsThetaTGTbm","",60,0,30,1000,-5,5);//deg
    hThetaRecoVsTrueTGT_Z[Z-1] = new TH2D(Form("ThetaRecoVsTrueTGT_Z%d",Z),"",60,0,30,60,0,30);//deg
    hThetaRecoVsTrueTGTbm_Z[Z-1] = new TH2D(Form("ThetaRecoVsTrueTGTbm_Z%d",Z),"",60,0,30,60,0,30);//deg
    hPOCA_Z[Z-1] = new TH3D(Form("POCA_Z%d",Z),"",50,-5,5,50,-5,5,300, -10,20);
    hPosTGT_Z[Z-1] = new TH3D(Form("PosTGT_Z%d",Z),"",50,-5,5,50,-5,5,300, -10,20);
    for(int ipla=0;ipla<4;ipla++){
      hResxVT_plane_Z[ipla][Z-1]=new TH1D(Form("Residuals_x_VT_pla%d_Z%d",ipla,Z),"",1000,-0.1,0.1);//deg
      hResyVT_plane_Z[ipla][Z-1]=new TH1D(Form("Residuals_y_VT_pla%d_Z%d",ipla,Z),"",1000,-0.1,0.1);//deg
      hReszVT_plane_Z[ipla][Z-1]=new TH1D(Form("Residuals_z_VT_pla%d_Z%d",ipla,Z),"",1000,-0.1,0.1);//deg
    }
    for(int ipla=0;ipla<6;ipla++){
      hResxMSD_plane_Z[ipla][Z-1]=new TH1D(Form("Residuals_x_MSD_pla%d_Z%d",ipla,Z),"",1000,-1,1);//deg
      hResyMSD_plane_Z[ipla][Z-1]=new TH1D(Form("Residuals_y_MSD_pla%d_Z%d",ipla,Z),"",1000,-1,1);//deg
      hReszMSD_plane_Z[ipla][Z-1]=new TH1D(Form("Residuals_z_MSD_pla%d_Z%d",ipla,Z),"",1000,-1,1);//deg
    }
    hResxTW_Z[Z-1]=new TH1D(Form("Residuals_x_TW_Z%d",Z),"",1000,-5,5);//deg
    hResyTW_Z[Z-1]=new TH1D(Form("Residuals_y_TW_Z%d",Z),"",1000,-5,5);//deg
    hReszTW_Z[Z-1]=new TH1D(Form("Residuals_z_TW_Z%d",Z),"",1000,-5,5);//deg


    gDirectory->cd("..");
  }

  
  
  hResTGT = new TH2D("ResidualsTruePosTGT","",1000,-5,5,1000,-5,5);
  hResThetaTGT = new TH1D("ResidualsThetaTGT","",1000,-5,5);//deg
  hResThetaTGTbm = new TH1D("ResidualsThetaTGTbm","",1000,-5,5);//deg
  hResThetaVsThetaTGT = new TH2D("ResoThetaVsThetaTGT","",60,0,30,1000,-5,5);//deg
  hResThetaVsThetaTGTbm = new TH2D("ResoThetaVsThetaTGTbm","",60,0,30,1000,-5,5);//deg
  hThetaRecoVsTrueTGT = new TH2D("ThetaRecoVsTrueTGT","",60,0,30,60,0,30);//deg
  hThetaRecoVsTrueTGTbm = new TH2D("ThetaRecoVsTrueTGTbm","",60,0,30,60,0,30);//deg
  hZRecoVsTrue = new TH2D("ZRecoVsTrue","",8,0.5,8.5,8,0.5,8.5);//deg
  hZRecoVsTrueCut = new TH2D("ZRecoVsTrueCut","",8,0.5,8.5,8,0.5,8.5);//deg
  hPOCA = new TH3D("POCA","",50,-5,5,50,-5,5,300, -10,20);
  hPosTGT = new TH3D("PosTGT","",50,-5,5,50,-5,5,300, -10,20);
  for(int ipla=0;ipla<4;ipla++){  
    hResxVT_plane[ipla]=new TH1D(Form("Residuals_x_VT_pla%d",ipla),"",1000,-0.1,0.1);//deg
    hResyVT_plane[ipla]=new TH1D(Form("Residuals_y_VT_pla%d",ipla),"",1000,-0.1,0.1);//deg
    hReszVT_plane[ipla]=new TH1D(Form("Residuals_z_VT_pla%d",ipla),"",1000,-0.1,0.1);//deg
  }
  for(int ipla=0;ipla<6;ipla++){
    hResxMSD_plane[ipla]=new TH1D(Form("Residuals_x_MSD_pla%d",ipla),"",1000,-1,1);//deg
    hResyMSD_plane[ipla]=new TH1D(Form("Residuals_y_MSD_pla%d",ipla),"",1000,-1,1);//deg
    hReszMSD_plane[ipla]=new TH1D(Form("Residuals_z_MSD_pla%d",ipla),"",1000,-1,1);//deg
  }
  hResxTW =new TH1D(Form("Residuals_x_TW"),"",1000,-5,5);//deg
  hResyTW =new TH1D(Form("Residuals_y_TW"),"",1000,-5,5);//deg
  hReszTW =new TH1D(Form("Residuals_z_TW"),"",1000,-5,5);//deg
    

  return;
  
}

//-----------------------------------------------------------------------------

void  ProjectTracksOnTw(int Z, TVector3 initPos, TVector3 initP) {
      // Ogni traccia è una retta nello spazio di eq parametriche (espr. vettoriale) X = initPos + initP * t (parametro t reale) e si interseca con il piano z=z_TW (centro tra i due layer in z del TW) nei punti:

      Int_t x_intTW = initPos.x()+initP.x()/initP.z()*(GetTwCenter().z()-initPos.z());
      Int_t y_intTW = initPos.y()+initP.y()/initP.z()*(GetTwCenter().z()-initPos.z());

      //Select only TW portion of the plan z=z_TW:
      Int_t TwHalfLength = (nBarsPerLayer*twparGeo->GetBarWidth())/2;
      if(TMath::Abs(x_intTW - GetTwCenter().x())<=TwHalfLength && TMath::Abs(y_intTW - GetTwCenter().y())<=TwHalfLength)



      return;
}

//-----------------------------------------------------------------------------

Bool_t IsVTregion(int reg) {

  TString firstVTregName = "VTXE0";
  TString lastVTregName  = "VTXP3";

  // first and last VT region
  Int_t VT_regions[kVTreg] = {parGeo->GetCrossReg(firstVTregName),
                              parGeo->GetCrossReg(lastVTregName)};

  bool isvtreg=false;
  
  if(VT_regions[0]<reg<VT_regions[1])
    isvtreg = true;
  else
    isvtreg = false;
  
  return isvtreg;
  
}

//-----------------------------------------------------------------------------

void ComputeBinomialEfficiency(Int_t ibin, Double_t num, Double_t den, Double_t& eff, Double_t& e_eff) {

  if(den>0) {
    eff = num/den;
    if(eff > 1) {
      if(debugEff) cout <<"Warning: for the bin "<<ibin<<"  the efficiency is > 1 !!!!! ... Efficiency is set to: eff = 0 +/- 0"<<endl;
      //eff = 1;
      e_eff = 0;
    }
    else {
      e_eff = sqrt(eff*(1-eff)/den);  // pure Binomial                                                                                                                              
    }
  }
  else {
    if(debugEff) cout<<"Warning: for the bin "<<ibin<<"  the denominator is:: "<<den<<" !!!!! ... Efficiency is set to: eff = 0 +/- 0"<<endl;
    eff = 0;
    e_eff = 0;
  }

  return;
}

//-----------------------------------------------------------------------------

void ComputeBayesianEfficiency(Int_t ibin, Double_t num, Double_t den, Double_t& eff, Double_t& e_eff) {

  if(num>den) {
    if(debugEff) cout <<"Warning: for the bin "<<ibin<<"  numerator > denominator !!!!! ... Efficiency is set to: eff = 0 +/- 0"<<endl;
    eff = 0;
    e_eff = 0;
  }
  else {
    eff = (num+1)/(den+2);
    e_eff = eff*(num+2)/(den+3)-pow(eff,2);
    if(e_eff>0.)
      e_eff = sqrt(e_eff);
    else {
      cout<<"arg(sqrt)<0 :: "<<e_eff<<endl;
      e_eff= 1./(2.*(den+1.));
    }
  }

  return;
}

//-----------------------------------------------------------------------------

void GetFOOTgeo( TAGcampaignManager* campManager,
                  Int_t runNumber) {

  // TAGgeoTrafo*
  geoTrafo = new TAGgeoTrafo();
  TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
  geoTrafo->FromFile(parFileName);
  printf("geoTrafo::  %s\n",parFileName.Data());
  
  // beam
  TAGparaDsc* fpParGeoG = new TAGparaDsc( new TAGparGeo());

  // TAGparGeo*
  parGeo = (TAGparGeo*)fpParGeoG->Object();
  parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
  parGeo->FromFile(parFileName.Data());
  printf("parGeo::  %s\n",parFileName.Data());
  
  // Get the detectors geo files
  
  // ST
  TAGparaDsc* parGeoST = new TAGparaDsc( new TASTparGeo());

  // TASTparGeo*
  stparGeo = (TASTparGeo*)parGeoST->Object();
  parFileName = campManager->GetCurGeoFile(TASTparGeo::GetBaseName(), runNumber);
  stparGeo->FromFile(parFileName);
  printf("stparGeo::  %s\n",parFileName.Data());
  
  // BM
  TAGparaDsc* parGeoBm = new TAGparaDsc( new TABMparGeo());

  bmparGeo = (TABMparGeo*)parGeoBm->Object();
  parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
  bmparGeo->FromFile(parFileName);
  printf("bmparGeo::  %s\n",parFileName.Data());
  
  // VTX
  TAGparaDsc* parGeoVtx = new TAGparaDsc( new TAVTparGeo());

  // TAVTparGeo*
  vtparGeo = (TAVTparGeo*)parGeoVtx->Object();
  parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
  vtparGeo->FromFile(parFileName);
  printf("vtparGeo::  %s\n",parFileName.Data());
  
  // IT
  TAGparaDsc* parGeoIt = new TAGparaDsc( new TAITparGeo());

  itparGeo = (TAITparGeo*)parGeoIt->Object();
  parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
  itparGeo->FromFile(parFileName);
  printf("itparGeo::  %s\n",parFileName.Data());
  
  // MSD
  TAGparaDsc* parGeoMsd = new TAGparaDsc( new TAMSDparGeo());

  msdparGeo = (TAMSDparGeo*)parGeoMsd->Object();
  parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
  msdparGeo->FromFile(parFileName);
  printf("msdparGeo::  %s\n",parFileName.Data());
  
  // TW
  TAGparaDsc* parGeoTW = new TAGparaDsc( new TATWparGeo());

  // TATWparGeo*
  twparGeo = (TATWparGeo*)parGeoTW->Object();
  parFileName = campManager->GetCurGeoFile(TATWparGeo::GetBaseName(), runNumber);
  twparGeo->FromFile(parFileName);
  printf("twparGeo::  %s\n",parFileName.Data());
  
  return;

}

//-----------------------------------------------------------------------------

void GetRunAndGeoInfo( TAGcampaignManager* campManager, Int_t runNumber) {

  IncludeReg=runinfo->GetGlobalPar().EnableRegionMc;
  IncludeMC=campManager->GetCampaignPar(campManager->GetCurrentCamNumber()).McFlag;
  IncludeDI=campManager->IsDetectorOn("DI");
  IncludeSC=campManager->IsDetectorOn("ST");
  IncludeBM=campManager->IsDetectorOn("BM");
  IncludeTG=campManager->IsDetectorOn("TG");
  IncludeVT=campManager->IsDetectorOn("VT");
  IncludeIT=campManager->IsDetectorOn("IT");
  IncludeMSD=campManager->IsDetectorOn("MSD");
  IncludeTW=campManager->IsDetectorOn("TW");
  IncludeCA=campManager->IsDetectorOn("CA");

  if(IncludeMC && IncludeDAQ){
    cout<<"IncludeMC and IncludeDAQ are both true... check your input file and the configuration files, this program will be ended"<<endl;
    return;
  }

  // global FOOT geometry
  GetFOOTgeo(campManager,runNumber);

  return;

}





//----------------------------------------------------------------------------


TVector3 GetPOCA(TVector3 p1,TVector3 n1, TVector3 p2,TVector3 n2){

  //Here we follow the prescription from
  //http://geomalgorithms.com/a07-_distance.html

  TVector3 POCA(-1000,-1000,-1000);
  TVector3 closest1,closest2;
  double  a,b,c,d,e,s,t;

      
  a = n2.Dot(n2);
  b = n2.Dot(n1);
  c = n1.Dot(n1);
  d = n2.Dot(p2 - p1);
  e = n1.Dot(p2 - p1);
 
  if(a*c-b*b) {
    
    s = (b*e-c*d)/(a*c-b*b);
    t = (a*e-b*d)/(a*c-b*b);
    
    closest1 = p1 + t*n1;
    closest2 = p2 + s*n2;
    
    POCA = (closest1 + closest2)*(1./2.); 
  }
  
  return POCA;
  
}







//-----------------------------------------------------------------------------

void SetTreeBranchAddress(TAGactTreeReader *actTreeReader) {


  // blocco traccia MC
  mcNtuPart = new TAMCntuPart();
  TAGdataDsc* mcPart    = new TAGdataDsc(mcNtuPart);
  actTreeReader->SetupBranch(mcPart);

  TString name(mcNtuPart->ClassName());
  const char* branch = TAGnameManager::GetBranchName(name);
  // cout<<branch<<endl;
  printf("%s\n",TAGnameManager::GetBranchName(name).Data());

  // blocco crossings MC
  mcNtuRegion = new TAMCntuRegion();
  TAGdataDsc* mcRegion    = new TAGdataDsc(mcNtuRegion);
  actTreeReader->SetupBranch(mcRegion);
  cout<<TAGnameManager::GetBranchName(mcNtuRegion->ClassName())<<endl;

  // ST
  stNtuHit = new TASTntuHit();   // blocco hit ST reco
  TAGdataDsc* stHit    = new TAGdataDsc(stNtuHit);
  actTreeReader->SetupBranch(stHit);
  cout<<TAGnameManager::GetBranchName(stNtuHit->ClassName())<<endl;

  stMcNtuHit = new TAMCntuHit();
  TAGdataDsc* stMcHit    = new TAGdataDsc(FootBranchMcName(kST), stMcNtuHit);
  actTreeReader->SetupBranch(stMcHit);
  cout<<TAGnameManager::GetBranchMcName(kST)<<endl;

  // TW
  twNtuHit = new TATWntuHit();   // blocco hit TW reco
  TAGdataDsc* twHit    = new TAGdataDsc(twNtuHit);
  actTreeReader->SetupBranch(twHit);

  twNtuPoint = new TATWntuPoint();
  TAGdataDsc* twPoint    = new TAGdataDsc(twNtuPoint);
  actTreeReader->SetupBranch(twPoint);

  twMcNtuHit = new TAMCntuHit();     // blocco hit TW MC
  TAGdataDsc* twMcHit    = new TAGdataDsc(FootBranchMcName(kTW),twMcNtuHit);
  actTreeReader->SetupBranch(twMcHit);


  // BM
  bmNtuHit = new TABMntuHit();
  TAGdataDsc* bmHit    = new TAGdataDsc(bmNtuHit);
  actTreeReader->SetupBranch(bmHit);

  bmNtuTrack = new TABMntuTrack();
  TAGdataDsc* bmTrack    = new TAGdataDsc(bmNtuTrack);
  actTreeReader->SetupBranch(bmTrack);

  if(IncludeMC){
    bmMcNtuHit = new TAMCntuHit();
    TAGdataDsc* bmMcHit    = new TAGdataDsc(FootBranchMcName(kBM),bmMcNtuHit);
    actTreeReader->SetupBranch(bmMcHit);
  }

  // VT
  vtxNtuVertex = new TAVTntuVertex();
  TAGdataDsc* vtVertex    = new TAGdataDsc(vtxNtuVertex);
  actTreeReader->SetupBranch(vtVertex);

  vtxNtuTrack = new TAVTntuTrack();
  TAGdataDsc* vtTrack    = new TAGdataDsc(vtxNtuTrack);
  actTreeReader->SetupBranch(vtTrack);

  vtxNtuCluster = new TAVTntuCluster();
  vtxNtuCluster->SetParGeo(vtparGeo);
  TAGdataDsc* vtCluster    = new TAGdataDsc(vtxNtuCluster);
  actTreeReader->SetupBranch(vtCluster);

  if(IncludeMC){
    vtMcNtuHit = new TAMCntuHit();
    TAGdataDsc* vtMcHit    = new TAGdataDsc(FootBranchMcName(kVTX),vtMcNtuHit);
    actTreeReader->SetupBranch(vtMcHit);
  }

  // IT
  //itNtuClus = new TAITntuCluster();
  //TAGdataDsc* itClus    = new TAGdataDsc(itNtuClus);
  //actTreeReader->SetupBranch(itClus);

  // MSD
  msdNtuClus= new TAMSDntuCluster();
  TAGdataDsc* msdClus    = new TAGdataDsc(msdNtuClus);
  actTreeReader->SetupBranch(msdClus);

  msdNtuPoint= new TAMSDntuPoint();
  TAGdataDsc* msdPoint    = new TAGdataDsc(msdNtuPoint);
  actTreeReader->SetupBranch(msdPoint);

  msdNtuHit= new TAMSDntuHit();
  TAGdataDsc* msdHit    = new TAGdataDsc(msdNtuHit);
  actTreeReader->SetupBranch(msdHit);

  // DAQ
  //tgNtuEvent= new TAGntuEvent();
  //TAGdataDsc* tgEvent    = new TAGdataDsc(tgNtuEvent);
  //actTreeReader->SetupBranch(tgEvent);


  glbntutrk= new TAGntuGlbTrack();
  TAGdataDsc* ntuTrk    = new TAGdataDsc(glbntutrk);
  actTreeReader->SetupBranch(ntuTrk);
  
  return;
  
}
