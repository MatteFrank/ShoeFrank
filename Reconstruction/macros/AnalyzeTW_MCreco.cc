// written by mtoppi for GSI2019 data analysis
// to be run in shoe/build/Reconstruction
// therein set-up the enviroment:  source ../../setupFOOT.sh
// and run with (for example): root -l -b -q AnalyzeTW_MC.cc++g\(\"../../../../rootfiles/outMC_16O_C_400_1_GSI.root\",1,10,\"\"\)

#include "AnalyzeTW_MCreco.h"


// main
void AnalyzeTW_MCreco(TString infile = "testMC.root", Bool_t isMax=kFALSE, Int_t nev = 10, TString outfile = "AnaTW_MC.root", Bool_t isNoTrig = true, Bool_t isPUtoBeRemoved = false, Int_t emin=0, Int_t emax=1000, Bool_t isBinomial=true, Bool_t isEfftoNoTGused = false)
  
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


  ////////////////////////////
  Int_t pos = outfile.Last('.');
  TString out = outfile(0, pos);
  // out.Append(Form("_Dx_%.1f",maxTGx));
  // out.Append(Form("_DE_%d-%d",emin,emax));
  // if(isBinomial)
  //   out.Append("_isBinEff");
    
  Int_t inpos = infile.Last('/');
  TString in = "_" + infile(inpos+1,infile.Length());
  cout<<endl<<"modified input name::  "<<in.Data()<<endl;
  
  out.Append(Form("%s",in.Data()));
  cout<<out.Data()<<endl;


  std::cout<<"Input file (-in) : "<<infile.Data()<<std::endl;
  std::cout<<"Output file (-out) : "<<out.Data()<<std::endl<<endl;


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


  TFile *fout = new TFile(out.Data(),"RECREATE");
  fout->cd();

  BookHistograms();
  
  TVecPair vPairWrongZ;
  vPairWrongZ.clear();
  
  map< Int_t, vector<TVector3> > pMap;  // primaries
  map< Int_t, vector<TVector3> > nMap;  // neutrons
  pMap.clear();
  nMap.clear();


  
  cout<<"Beam Z:: "<<parGeo->GetBeamPar().AtomicNumber<<" A:: "<<parGeo->GetBeamPar().AtomicMass<<" ion::"<<parGeo->GetBeamPar().Material<<endl;
   
  cout<<"TG center:: "<<geoTrafo->GetTGCenter().z()<<endl<<" TG thickness:: "<<parGeo->GetTargetPar().Size.z()<<" TG material:: "<<parGeo->GetTargetPar().Material<<endl;

  printf("TW center =  (%2.1f,%2.1f,%2.1f)\n",GetTwCenter().x(),GetTwCenter().y(),GetTwCenter().z());
       
  cout<<"theta angle of TW  acceptance::"<<GetMaxAngle()*180/TMath::Pi()<<endl;

  if (nev == 0) {
    printf("number of event to be processed is %d...set a number > 0",nev);
    return;
  }
  
  fActReader->Open(infile);
  gTAGroot.AddRequiredItem(fActReader);

  Int_t nentries = fActReader->NEvents();

  printf("Max available number of Entries in this Tree is::%d\n\n",(int)nentries);
  
  if(!isMax && nev>0) {
    if(nev<nentries)
      nentries = nev;
    else
      printf("Warning! nev (%d) has been set to a value > of maximum number of entries (%d), so take the maximum \n\n",nev,(int)nentries);
  }  

  if(isNoTrig) {
    Nprim = nentries;  
    Nprim_impingingOnTG = Nprim;
  }
  else {
    cout<<"Triggered simulation / data file...check the # of primaries against this macro default value of::"<<Nprim<<endl;
  }
  
  printf("Total Entries to be processed::%d\n\n",(int)nentries);
  

  // Loop over the TTree
  gTAGroot.BeginEventLoop();
  
  Int_t ev = -1;
  
  while (gTAGroot.NextEvent() && ev!=nentries){

     ev++;

    if(debug)
      printf("\n Event: %d\n", ev);
    else
      if( ev % 10000 == 0) printf("Event: %d\n", ev);


    map<Int_t,Int_t> mapTrkCrossTW;
    mapTrkCrossTW.clear();

    map<Int_t,Int_t> mapTrkCrossTW_inTG;
    mapTrkCrossTW_inTG.clear();

    map<Int_t,Int_t> mapIdTwZ;
    mapIdTwZ.clear();

    TMapMatch mapTwTrkId;
    mapTwTrkId.clear();


    // N of MC crossed regions per event
    int nMCregions = mcNtuRegion->GetRegionsN();
    if(debug)
      printf("nMCregions::%d\n",nMCregions);    

    if(StudyCrossings && StudyEffAndXS) {

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
        
        // if cross the tw regions coming from the air in front the TW
        // if(nCross>=TW_regions[2] && nCross<=TW_regions[3] && nCrossOld==AirTW_region) {
        if(nCross>TW_regions[0] && nCross<TW_regions[3] && nCrossOld==AirTW_region) {  // if cross the TW coming from the air in front or in the rear of the TW

          hyields_fromCrossings->Fill(Z);
          
          if(debugCMM) 
            printf("crossId::%d, trkId::%d, mothId::%d, reg::%d, Ekin::%f, Z::%1.0f, A::%f, pos::%f, t::%f, cross::%d, old::%d\n",imcreg,trkId,mothId,reg,ekin,Z,mass,posIn_z,time,nCross,nCrossOld);


          mapTrkCrossTW[trkId]=Z;

          
          //if is a primary or if is a fragment produced by a primary in the TG
          // if(mothId==kPrimaryID && reg==TG_region) {
          if(trkId==kPrimaryID || (mothId==kPrimaryID && reg==TG_region)) {
          
            // if(initP.Theta()>GetMaxAngle() || abs(initPos.X())>maxTGx || abs(initPos.Y())>maxTGy || ekin_Z<(double)emin || ekin_Z>(double)emax) continue;
            
            hyields_fromCrossings_inTG->Fill(Z);
            
            mapTrkCrossTW_inTG[trkId]=Z;
          }
        }     
      }      // for loop over MC regions
    }        // if(StudyCrossings)
    

    // N of MC tracks per event
    int nMCtrk = mcNtuPart->GetTracksN();
    if(debug)
      printf("nMCtrk::%d\n",nMCtrk);

    // loop over MC tracks
    // LoopOverMCtracks(emin,emax,isNoTrig);
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

      h_reg_vs_z->Fill(initPos.Z(),reg);


      double ekin = (sqrt(initP.Mag2()+mass*mass)-mass)/(double)baryon*TAGgeoTrafo::GevToMev(); // Ekin/A in MeV
      double beta=(initP.Mag()/sqrt(initP.Mag2()+mass*mass));
      double ekin_Z = (1/sqrt(1-beta*beta) - 1)*1000;  //is similar to Ekin/A in MeV
      if(debug) {
        cout<<"imctrk::"<<imctr<<" mothId::"<<mothId<<" Z::"<<Z<<endl;
        cout<<"reg::"<<reg<<" posZ::"<<initPos.Z()<<" F_posZ::"<<finalPos.Z()<<" mass::"<<mass<<endl;
        cout<<"Z::"<<Z<<"  p::"<<initP.Mag()*1000<<" E::"<<sqrt(initP.Mag2()+mass*mass)*1000<<" beta::"<<beta<<"  ekin_Z::"<<ekin_Z<<" ekin::"<<ekin<<endl<<endl;
      }

      if(!StudyEffAndXS) continue;

      // out of target fragmentation from primaries
      if(Z>0 && Z<GetZbeam()+1 && mothId==(Int_t)kPrimaryID && reg!=TG_region) {
	if(initPos.Z()>geoTrafo->GetTGCenter().z()-parGeo->GetTargetPar().Size.z()/2. && initPos.z() < GetTwCenter().z()-twparGeo->GetBarThick() ) {
	  double angle = TMath::ATan(((twparGeo->GetBarHeight()-2*twparGeo->GetBarWidth())/2)/(GetTwCenter().z()-initPos.z())); // rad --> in TW acceptance depending on te z position production
	  if(initP.Theta()<angle  && ekin_Z>(double)emin && ekin_Z<(double)emax)
	    hyields_trk_bkg->Fill(Z);  // denominator for bkg eff (out-of-target fragments eff)
	  
	}
      }
      
      // fragmentation In-target from primaries
      if(Z>0 && Z<GetZbeam()+1 && mothId==(Int_t)kPrimaryID && reg==TG_region) {
        
	if(finalPos.z() > geoTrafo->GetTGCenter().z()+parGeo->GetTargetPar().Size.z()/2 ) { // if the fragment exit from the target
	  hekin_vs_angle[Z-1]->Fill(initP.Theta()/TMath::Pi()*180,ekin);
	  hekin[Z-1]->Fill(ekin);
	  hangle[Z-1]->Fill(initP.Theta()/TMath::Pi()*180);
	}

	// if(!expName.CompareTo("GSI_MC") &&
        if((double)initP.Theta()<GetMaxAngle() && ekin>(double)emin && ekin<(double)emax)
	  hyields_XStrue->Fill(Z);	
      }
      
      if(debug)
        cout<<"reg_dead:: "<<reg_dead<<" TG_region:: "<<TG_region<<"  Z::"<<Z<<"  initPos:: "<<initPos.z()<<" finalPos:: "<<finalPos.z()<<endl;

      if(isNoTrig) {
        
	if(Z==GetZbeam()) {
          
	  if(imctr==0 && finalPos.z() < geoTrafo->GetTGCenter().z()-parGeo->GetTargetPar().Size.z()/2 ) { // if the primary died before the target it means it is fragmenting before interacting in the TG and it is producing pre target fragmentation. This contribution has to be removed
	    Nprim_impingingOnTG--;
            if(debug)
              cout<<"Nprim_impingingOnTG:: "<<(int)Nprim_impingingOnTG<<"  finalPos:: "<<finalPos.z()<<endl;
	  }
          
	  if(imctr==0 && finalPos.z() > geoTrafo->GetTGCenter().z()+parGeo->GetTargetPar().Size.z()/2 ) { // count the primaris not interacting in the TG for total cc XS asking for primaries not fragmenting before interacting in the TG and producing pre target fragmentation (request that simulate the BM request of have only a track)
	    Nprim_notInteracting++;
            if(debug)
              cout<<"Nprim_notInteracting:: "<<(int)Nprim_notInteracting<<"  finalPos:: "<<finalPos.z()<<endl;
	  }
	  if(imctr==0 && finalPos.z() > geoTrafo->GetTGCenter().z()-parGeo->GetTargetPar().Size.z()/2  && finalPos.z() < geoTrafo->GetTGCenter().z()+parGeo->GetTargetPar().Size.z()/2) {
	    Nprim_interacting++;
	    if(debug)
              cout<<"Nprim_Interacting:: "<<(int)Nprim_interacting<<"  finalPos:: "<<finalPos.z()<<endl;
	  }
	     
	  hyields_trk->Fill(Z); // denominator for oxygen efficiency
	  hyields_ang_trk[Z-1]->Fill(initP.Theta()/TMath::Pi()*180);
	  hyields_ekin_trk[Z-1]->Fill(ekin);
	  
	}
      }
      
      if(Z>0 && Z<GetZbeam() && reg==TG_region ) {
	
        // if(!expName.CompareTo("GSI_MC")
        if(initP.Theta()>GetMaxAngle() || abs(initPos.X())>maxTGx || abs(initPos.Y())>maxTGy || ekin_Z<(double)emin || ekin_Z>(double)emax) {

          if(dbg)
            printf("Cut selection: Z::%d, Angle::%f, ekin/nucl::%f, TG_x::%f, TG_y::%f\n",Z,initP.Theta()/TMath::Pi()*180,ekin,initPos.X(),initPos.Y());
          
          continue;    
        }


        hyields_trk_inTG->Fill(Z);  // denominator for efficiency


	if( mothId==(Int_t)kPrimaryID ) {
	  // if(reg_dead!=TG_region) {

	  hyields_trk->Fill(Z);  // denominator for efficiency
	  hyields_ang_trk[Z-1]->Fill(initP.Theta()/TMath::Pi()*180);	
	  hyields_ekin_trk[Z-1]->Fill(ekin);

          ProjectTracksOnTw(Z,initPos,initP);

	}

	if(debug) {
          printf("Z::%d  theta::%.3f  ekin::%.3f\n",Z,initP.Theta()/TMath::Pi()*180,ekin_Z);
          cout<<hyields_trk->GetEntries()<<endl;
          cout<<hyields_ang_trk[Z-1]->GetEntries()<<endl;	
          cout<<hyields_ekin_trk[Z-1]->GetEntries()<<endl;
          
          printf("id::%d  mothId::%d  reg::%d  x::%.f  y::%.f  z::%.f\n",imctr,mothId,reg,initPos.x(),initPos.y(),initPos.z());
          printf("id::%d  mothId::%d  Z::%d  A::%d  M::%f\n",imctr,mothId,Z,baryon,mass);
        } 
      }
      
    }  // close for loop on MC true tracks

    

    map<int,vector<int>> countMultHitsX;
    countMultHitsX.clear();
    map<int,vector<int>> countMultHitsY;
    countMultHitsY.clear();
    
    int NtwPoints = twNtuPoint->GetPointsN();
    if(debug) cout<<NtwPoints<<" TW points"<<endl;
    
    if(StudyEffAndXS) {
      
      for(int ipt=0; ipt<NtwPoints; ipt++) {
	
	TATWpoint *twpnt = twNtuPoint->GetPoint(ipt);
	TATWhit *hitX = twpnt->GetRowHit();
	TATWhit *hitY = twpnt->GetColumnHit();
	Int_t Z_hitX = hitX->GetChargeZ();
	Int_t Z_hitY = hitY->GetChargeZ();
	Int_t Z = twpnt->GetChargeZ();
	
	Int_t barX=hitX->GetBar();
	Int_t barY=hitY->GetBar();
	
	double eloss (-1);
	if(twpnt->GetMainLayer()==(Int_t)LayerX)
	  eloss = hitX->GetEnergyLoss();
	else if(twpnt->GetMainLayer()==(Int_t)LayerY)
	  eloss = hitY->GetEnergyLoss();
	
	Double_t tof = twpnt->GetMeanTof();
	// TVector3 glPntPos = twpnt->GetPositionGlb();

        TATWhit *hit;
        if(twpnt->GetMainLayer()==(Int_t)LayerX)
          hit=(TATWhit*)hitX;
        else
          hit=(TATWhit*)hitY;

        vector<Int_t> vecTwTrkId;
        vecTwTrkId.clear();
        
	Int_t NmcTrk = hit->GetMcTracksN();
        if(debug)
          cout<<"N trks in TW::"<<NmcTrk<<endl;
        
	for(int imctr=0; imctr<NmcTrk; imctr++) {
	  
	  Int_t mcId = hit->GetMcIndex(imctr);
	  Int_t mcTrkId = hit->GetMcTrackIdx(imctr);

          vecTwTrkId.push_back(mcTrkId);

          TAMCpart *mctrk = mcNtuPart->GetTrack(mcTrkId);
          TVector3 initP = mctrk->GetInitP();
          int mothId = mctrk->GetMotherID();
          int reg = mctrk->GetRegion();  // region where track is generate
          int zmc = mctrk->GetCharge();  // Z charge

          if(debugCMM && NmcTrk>1)
            cout<<"mcId::"<<mcId<<"  mcTrkId::"<<mcTrkId<<" mothId::"<<mothId<<" reg::"<<reg<<" Ekin::"<<sqrt(initP.Mag2()+pow(mctrk->GetMass(),2))-mctrk->GetMass()<<" Zmc::"<<zmc<<"  Z::"<<Z<<endl;
	  
	}


        mapIdTwZ[ipt] = Z;

        mapTwTrkId[ipt] = vecTwTrkId;
        

        if(debugCMM && NmcTrk>1)
          cout<<"Zx::"<<hitX->GetChargeZ()<<" Zy::"<<hitY->GetChargeZ()<<" Lay::"<<twpnt->GetMainLayer()<<"  Bar::"<<hit->GetBar()<<" ElossX::"<<hitX->GetEnergyLoss()<<" ElossY::"<<hitY->GetEnergyLoss()<<" TofX::"<<hitX->GetToF()<<" TofY::"<<hitY->GetToF()<<endl;
        
        if(StudyCrossings) {
          
          Int_t cntGoodZ(0), cntGoodZ_inTG(0);
          Int_t cntWrongZ(0), cntWrongZ_inTG(0);
          
          // loop over MC crossed regions        
          for(int imcreg=0; imcreg<nMCregions; imcreg++) {
            
            TAMCregion *CrossReg = mcNtuRegion->GetRegion(imcreg);
            
            Int_t nCross = CrossReg->GetCrossN();  // id crossed region
            Int_t nCrossOld = CrossReg->GetOldCrossN();  // id old crossed region

            // if cross the tw regions coming from the air in front the TW
            // if(nCross>=TW_regions[2] && nCross<=TW_regions[3] && nCrossOld==AirTW_region) {
            if(nCross>=TW_regions[0] && nCross<=TW_regions[3] && nCrossOld==AirTW_region) {

              Int_t trkId = CrossReg->GetTrackIdx()-1;
              Double_t Zmc = CrossReg->GetCharge();

              TAMCpart *mctrk = mcNtuPart->GetTrack(trkId);
              TVector3 initPos = mctrk->GetInitPos();
              TVector3 initP = mctrk->GetInitP();
              int mothId = mctrk->GetMotherID();
              int reg = mctrk->GetRegion();  // region where track is generated

              if(std::find(vecTwTrkId.begin(), vecTwTrkId.end(), trkId) != vecTwTrkId.end()) {
                
                hCMM_crossing->Fill(Z,Zmc);
                
                if(Z==Zmc) // diagonal element of the CMM matrix are the efficiencies' numerator
                  cntGoodZ++;
                else
                  cntWrongZ++;
               

                if(trkId==kPrimaryID || (mothId==kPrimaryID && reg==TG_region)) {
                  
                  hCMM_crossing_inTG->Fill(Z,Zmc);
                  
                  if(Z==Zmc) // diagonal element of the CMM matrix are the efficiencies' numerator
                    cntGoodZ_inTG++;
                  else
                    cntWrongZ_inTG++;
                }
                
              } else 
                if(debug) std::cout << "Element not found"<<endl;

            }     
          } // for loop over MC regions

          if(cntGoodZ>0)
            hyields_multiHitReco->Fill(Z);
          
          if(cntGoodZ_inTG>0)
            hyields_multiHitReco_inTG->Fill(Z);

          if(cntWrongZ>0 && cntGoodZ==0)
            hWrongZ->Fill(Z);                    

          if(cntWrongZ_inTG>0 && cntGoodZ_inTG==0)
            hWrongZ_inTG->Fill(Z);                    



        }  // if(StudyCrossings)
        


	Int_t trkId = twpnt->GetPointMatchMCtrkID();  // ID of the MC track associated to the TW point
	
	if(trkId<0) {
	  if(debug)
            printf("pile upped point...skip...\n");
	  continue;
	}
	
	
	TAMCpart *mctrk = mcNtuPart->GetTrack(trkId);
	
	int mothId = mctrk->GetMotherID();
	TVector3 initPos = mctrk->GetInitPos();
	TVector3 initP = mctrk->GetInitP();
	int reg = mctrk->GetRegion();  // region where track is generated
	Int_t reg_dead = mctrk->GetDead(); // region where track die
	int Zmc = mctrk->GetCharge();
	int baryon = mctrk->GetBaryon();
	double mass = mctrk->GetMass();
      
	
	double ekin = (sqrt(initP.Mag2()+mass*mass)-mass)/baryon*TAGgeoTrafo::GevToMev();
	double beta=(initP.Mag()/sqrt(initP.Mag2()+mass*mass));
	double ekin_Z = (1/sqrt(1-beta*beta) - 1)*1000;  //is similar to Ekin/A in MeV
	if(debug) cout<<"Z::"<<Z<<"  p::"<<initP.Mag()*1000<<" E::"<<sqrt(initP.Mag2()+mass*mass)*1000<<" beta::"<<beta<<"  ekin_Z::"<<ekin_Z<<" ekin::"<<ekin<<endl;
	
	if(Z>0 && Z<GetZbeam()+1) {
	  heloss_all->Fill(eloss);
	  heloss[Z-1]->Fill(eloss);
	  
	  // reconstructed yields data like
	  if(Z==GetZbeam()) {  // primari
	    
	    hCMM_rec->Fill(Z,Zmc);
	    // hyields_rec->Fill(Z);
            
	  } else {
	    
	    if(initPos.Z()>geoTrafo->GetTGCenter().z()-parGeo->GetTargetPar().Size.z()/2.) { // cut to simulate BM tracks selection not reproduced here
	      
	      // if(!expName.CompareTo("GSI_MC")
              if(initP.Theta()<GetMaxAngle() && abs(initPos.X())<maxTGx && abs(initPos.Y())<maxTGy && ekin_Z>(double)emin && ekin_Z<(double)emax) {
		if(dbg)
		  printf("GSI cut selection: Z::%d, Angle::%f, TG_x::%f,  TG_y::%f\n",Z,initP.Theta(),initPos.X(),initPos.Y());
		
		hCMM_rec->Fill(Z,Zmc);
                // hyields_rec->Fill(Z);
	      }
	    }
	  }
	}
	
	if(debug) {
          cout<<"trkId::"<<trkId<<"  mothId::"<<mothId<<endl;
          cout<<"fuori:  eloss::"<<eloss<<"  reg::"<<reg<<"  dead::"<<reg_dead<<"  Z::"<<Z<<endl;
        }
        
        //out of target fragmentation
	if(Z>0 && Z<GetZbeam()+1 && reg!=TG_region) {
	  
	  // reconstructed yields no TG (bkg) data like
	  if(initPos.Z()>geoTrafo->GetTGCenter().z()-parGeo->GetTargetPar().Size.z()/2. ) {
	    
	    hyields_bkg->Fill(Z);
	    
	    if(mothId==(Int_t)kPrimaryID) { // asking also for primary fragmentation for efficiencies for no TG case
	      
	      if(initPos.Z()>geoTrafo->GetTGCenter().z()-parGeo->GetTargetPar().Size.z()/2. && initPos.z() < GetTwCenter().z()-twparGeo->GetBarThick() ) {
		double angle = TMath::ATan(((twparGeo->GetBarHeight()-2*twparGeo->GetBarWidth())/2)/(GetTwCenter().z()-initPos.z())); // rad --> in TW acceptance depending on te z position production
		
		if(initP.Theta()<angle  && ekin_Z>(double)emin && ekin_Z<(double)emax)
		  hyields_prim_bkg->Fill(Z);
	      }
	    }
	    
	    // if(reg_dead!=reg) {	// remove very low energy fragments
	    // cout<<"noTG:  eloss::"<<eloss<<"  reg::"<<reg<<"  dead::"<<reg_dead<<endl;
	    
	    heloss_all_frg_noTG->Fill(eloss);
	    
	    if(mothId==(Int_t)kPrimaryID) {	
	      if(debug)
                cout<<"noTG:  eloss::"<<eloss<<"  reg::"<<reg<<"  dead::"<<reg_dead<<endl;
	      heloss_all_prim_noTG->Fill(eloss);
	      heloss_prim_noTG[Z-1]->Fill(eloss);
	      Tof_prim_noTG->Fill(tof);
	      Reg_prim_noTG->Fill(reg);
	      Z_prim_noTG->Fill(initPos.z());
	      
	    }
	  }	
	}
	
	if(Z>0 && Z<GetZbeam()+1 && mothId>(Int_t)kPrimaryID)  // secondary fragmentation
	  Z_sec_fragmentation->Fill(initPos.z());
	
	
	if(Z>0 && Z<GetZbeam()+1 && mothId==(Int_t)kPrimaryID && IsVTregion(reg)) {

	  // cout<<"VT:   eloss::"<<eloss<<"  reg::"<<reg<<"  dead::"<<reg_dead<<endl;
	  
	  heloss_all_primVT->Fill(eloss);
	  heloss_primVT[Z-1]->Fill(eloss);
	  
	}
	
        // IN-TARGET fragmentation requiring also fragmentation from a primary
	if(Z>0 && Z<GetZbeam()+1 && reg==TG_region && mothId==(Int_t)kPrimaryID) {
	  
          hyields_fromTG->Fill(Z);

          heloss_all_cut->Fill(eloss);
	  heloss_cut[Z-1]->Fill(eloss);
	  
	  // if(!expName.CompareTo("GSI_MC")
          if(initP.Theta()>GetMaxAngle() || abs(initPos.X())>maxTGx || abs(initPos.Y())>maxTGy || ekin_Z<(double)emin || ekin_Z>(double)emax) {
	    if(dbg)
	      printf("GSI cut selection: Z::%d, Angle::%f, TG_x::%f,  TG_y::%f\n",Z,initP.Theta(),initPos.X(),initPos.Y());
	    continue;
	  }
	  
	  if(hitX->GetEnergyLoss()<0 || hitY->GetEnergyLoss()<0) {
	    cout<<"Eloss_hitX::"<<hitX->GetEnergyLoss()<<"  Eloss_hitY::"<<hitY->GetEnergyLoss()<<endl;
	    // continue; //not a good TW cluster
	  }
	  
	  if(Z_hitX != Z_hitY) {
	    cout<<"Z_hitX::"<<Z_hitX<<"  Z_hitY::"<<Z_hitY<<endl;
	    // continue; //not a good TW cluster
	  }

	    
          // hCMM->Fill(Z,Zmc);
	    
          heloss_all_cutXS->Fill(eloss);
          heloss_cutXS[Z-1]->Fill(eloss);
          
          countMultHitsX[barX].push_back(Z);
          countMultHitsY[barY].push_back(Z);
	  
          hyields_allCuts->Fill(Z);
          hyields_ang[Z-1]->Fill(initP.Theta()/TMath::Pi()*180);
	  
          // printf("id::%d  mothId::%d  reg::%d  x::%.f  y::%.f  z::%.f\n",imctr,mothId,reg,initPos.x(),initPos.y(),initPos.z());
          // printf("id::%d  mothId::%d  Z::%d  A::%d  M::%f\n",imctr,mothId,charge,baryon,mass);
          
          // double Ltrk = TMath::Abs(geoTrafo->GetTGCenter().z()-geoTrafo->GetSTCenter().z()) + parGeo->GetTargetPar().Size.z()/2. + (initPos-trkInPos).Mag();  // cm
          
          // double c = TMath::C()*TAGgeoTrafo::MToCm()/TAGgeoTrafo::SecToNs(); //speed of light in cm/ns (tof is in ns):  c = 30 cm/ns
          // double beta=(Ltrk/(c*tof));  //beta = L/(c*tof)
          // // cout<<geoTrafo.GetTGCenter().z()<<" "<<geoTrafo.GetSTCenter().z()<<" "<<parGeo->GetTargetPar().Size.z()/2.<<" "<<(initPos-trkInPos).Mag()<<" "<<tof<<" "<<c<<" "<<beta<<" "<<Ltrk<<endl;
	  
          double beta=(initP.Mag()/sqrt(initP.Mag2()+mass*mass));
          double ekin_Z = (1/sqrt(1-beta*beta) - 1)*1000;  //is similar to Ekin/A in MeV
          // cout<<"Z::"<<Z<<"  p::"<<initP.Mag()*1000<<" E::"<<sqrt(initP.Mag2()+mass*mass)*1000<<" beta::"<<beta<<"  ekin_Z::"<<ekin_Z<<" ekin::"<<(sqrt(initP.Mag2()+mass*mass)-mass)/baryon*1000<<endl;
          // getchar();
	  
          hyields_ekin[Z-1]->Fill(ekin_Z);
	
	}
	
	// TH1D* heloss_sum = (TH1D*)heloss_all_cut->Clone("Eloss_Sum");
	// heloss_sum->Add(heloss_all_prim_noTG);
		
      }  // close for loop over TW points
      
    } // close if(StudyEffAndXS)



    // Needed to compute the reconstruction efficiency of TW and the CMM
    TMapMatch mapCntGoodZ;
    TMapMatch mapCntWrongZ;
    mapCntGoodZ.clear();
    mapCntWrongZ.clear();
    
    for(auto it_trk=mapTrkCrossTW.begin(); it_trk!=mapTrkCrossTW.end(); ++it_trk) {

      Int_t trkId = it_trk->first;
      Int_t Zmc = it_trk->second;

      hyields_crossTW->Fill(Zmc);
      
      if(debugCMM)
        cout<<"trkId::"<<trkId<<" Zmc::"<<Zmc<<endl;

      for(auto it_TwId=mapIdTwZ.begin(); it_TwId!=mapIdTwZ.end(); ++it_TwId) {

        Int_t IdTwHit = it_TwId->first;
        Int_t Zrec = it_TwId->second;

        if(debugCMM)
          cout<<"IdTwHit::"<<IdTwHit<<"  Zrec::"<<Zrec<<endl;

        vector<Int_t> vec_TrkIdTw = mapTwTrkId[IdTwHit];

        if(debugCMM) {
          for (auto it_v=vec_TrkIdTw.begin();it_v!=vec_TrkIdTw.end();++it_v){
            
            Int_t trkId_tw = *it_v;
            
            cout<<"trkId_tw::"<< *it_v<<endl<<endl;
          }
        }

        if(std::find(vec_TrkIdTw.begin(), vec_TrkIdTw.end(), trkId) != vec_TrkIdTw.end()) {
                
          hCMM->Fill(Zrec,Zmc);
          
          if(Zrec==Zmc)
            mapCntGoodZ[IdTwHit].push_back(Zmc);
          else 
            mapCntWrongZ[IdTwHit].push_back(Zmc);

        }        
      }
    }
    
    for(auto it_TwId=mapIdTwZ.begin(); it_TwId!=mapIdTwZ.end(); ++it_TwId) {
      
      Int_t idTwHit = it_TwId->first;
      Int_t Zrec = it_TwId->second;
      
      if (mapCntGoodZ[idTwHit].size()>0)
        hyields_rec->Fill(Zrec);
      if (mapCntWrongZ[idTwHit].size()>0 && mapCntGoodZ[idTwHit].size()==0) {

        hWrongZ_crossing->Fill(Zrec);
        hWrongZmult->Fill(Zrec,mapCntWrongZ[idTwHit].size());
        
      }
      if(debugCMM) {
        if (mapCntGoodZ[idTwHit].size()>1){
          
          for (auto it_v=mapCntGoodZ[idTwHit].begin();it_v!=mapCntGoodZ[idTwHit].end();++it_v)  {          
            cout<<"Zrec"<<Zrec<<"  Ztrue::"<<*it_v<<endl;
            getchar();
          }
        }
      }
      
      if(debugCMM) {
        if (mapCntWrongZ[idTwHit].size()>1){

          for (auto it_v=mapCntWrongZ[idTwHit].begin();it_v!=mapCntWrongZ[idTwHit].end();++it_v)   {
            cout<<"idTw::"<<idTwHit<<" Zrec::"<<Zrec<<"  Ztrue::"<<*it_v<<endl;
            getchar();
            
          }
        } 
      }

      pair<Int_t,Int_t> mypair;

      if(mapCntWrongZ[idTwHit].size()==2) {
        
        mypair=make_pair(mapCntWrongZ[idTwHit].at(0),mapCntWrongZ[idTwHit].at(1));
        vPairWrongZ.push_back(mypair);

        
      }
    }


    
    // remove manually pile-up for noTWpileUp production to check productions in which pile up has been already included
    for(auto it=countMultHitsX.begin(); it!=countMultHitsX.end(); ++it) {
	
      if((it->second).size()> 1) {
	// cout<<"N TW points::"<<NtwPoints<<endl;
	// cout<<"Hit barX::"<<it->first<<endl;
	// cout<<"Hit multiplicity::"<<(it->second).size()<<endl;

	vector<Int_t> vec_Z = it->second;
	  
	for (auto it_v=vec_Z.begin();it_v!=vec_Z.end();++it_v){
	  Int_t Z = *it_v;
	  // cout<<"Z frg::"<< *it_v<<endl;

	  hyieldsPU->Fill(Z);
	  
	}
	// getchar();
      }
    }
    
    for(auto it=countMultHitsY.begin(); it!=countMultHitsY.end(); ++it) {
      
      if((it->second).size()> 1) {
	// cout<<"N TW points::"<<NtwPoints<<endl;
	// cout<<"Hit barY::"<<it->first<<endl;
	// cout<<"Hit multiplicity::"<<(it->second).size()<<endl;
	
	vector<Int_t> vec_Z = it->second;
	
	for (auto it_v=vec_Z.begin();it_v!=vec_Z.end();++it_v){
	  Int_t Z = *it_v;
	  // cout<<"Z frg::"<< *it_v<<endl;
          
	  hyieldsPU->Fill(Z);
	}
	// getchar();
      }
    }
    
    
    if(StudyFragmentation) {	
      

      vector<TVector3> p_vec;
      vector<TVector3> n_vec;
      p_vec.clear();
      n_vec.clear();
      
      // retrieve SC MC info
      int Nsthits = stMcNtuHit->GetHitsN();
      double time_st = 999;
      if(Nsthits>0) {
        TAMChit * sthit = stMcNtuHit->GetHit(0);
        if(!sthit)
          cout<<"No SC MC hit for this event...set time ST to dummy::"<<time_st<<endl;
        else
          time_st = sthit->GetTof()*TAGgeoTrafo::SecToNs(); //s to ns
      }


      // loop over TW MC hits
      int nMCtwHits = twMcNtuHit->GetHitsN();
      for(int imctw=0; imctw<nMCtwHits; imctw++) {
	
	TAMChit * twhit = twMcNtuHit->GetHit(imctw);
	if(!twhit) continue;
        
	int layer = twhit->GetView();
	int bar = twhit->GetBarId();
	int trkId = twhit->GetTrackIdx()-1;
	
	double de = twhit->GetDeltaE()*TAGgeoTrafo::GevToMev();  // Gev to MeV
	double tof = twhit->GetTof()*TAGgeoTrafo::SecToNs(); //s to ns
     
        if(Nsthits>1) {
          cout<<"fragmentation pre SC...drop the event"<<endl;
          continue;
        }
        
	tof-=time_st;
	
	TVector3 initPos = twhit->GetInPosition();
	TVector3 finPos = twhit->GetOutPosition();
	
	TVector3 initP = twhit->GetInMomentum();
	TVector3 finP = twhit->GetOutMomentum();
	
	TAMCpart *mctrk = mcNtuPart->GetTrack(trkId);
	Int_t Z = mctrk->GetCharge();
	Int_t flukaId = mctrk->GetFlukaID();  // neutons == 8
	Int_t mothId = mctrk->GetMotherID();
	Int_t region = mctrk->GetRegion(); // region where track is generated
	TVector3 trkInPos  = mctrk->GetInitPos();
	TVector3 trkFinPos = mctrk->GetFinalPos();
	TVector3 trkInP  = mctrk->GetInitP();
	TVector3 trkFinP = mctrk->GetFinalP();
	
        if(debug) cout<<"trkId::"<<trkId<<" mothId::"<<mothId<<" Z::"<<Z<<endl;

        TAMCpart *mothTrk = nullptr;
	Int_t mothFlukaId = -100;
        if(mothId>(Int_t)kPrimaryID) { // interesting only for mother tracks not primary
          mothTrk = mcNtuPart->GetTrack(mothId);
          mothFlukaId = mothTrk->GetFlukaID();
        }          
        
	if(neutronPosOnTW) {

	  if(layer==(Int_t)LayerY) {
            
	    if(mothId==(Int_t)kPrimaryID) {  // get Primary
              
	      p_vec.push_back(initPos);
	      
	    }
            
            //neutrons coming directly from calo or
            if( flukaId==(Int_t)kNeutronFlukaId || mothFlukaId==(Int_t)kNeutronFlukaId ) {
              // printf("Z::%d, FlId::%d, MothFlId::%d, mothId::%d, reg::%d\n",Z,flukaId,mothFlukaId,mothId,region);
              
              if(flukaId==(Int_t)kNeutronFlukaId) {
                h_posXY->Fill(initPos.X(),initPos.Y());
                n_vec.push_back(initPos);
              }
	      else if(mothFlukaId==(Int_t)kNeutronFlukaId) {
                if(mothTrk) {                  
                  h_posXY->Fill(trkInPos.X(),trkInPos.Y());
                  n_vec.push_back(trkInPos);
                }
	      }
	    }
	  }	       	
	}
        
	if(Z<=0 || Z>GetZbeam()) continue;
	
	if(mothId==(Int_t)kPrimaryID && region==TG_region) {  // get only primary fragmentation coming from the TARGET and not died inside TG (mostly target fragments)
          
	  if(debug) printf("Z::%d  mothId::%d  region::%d\n",Z,mothId,region);
	  
	  dE_vs_tof[layer]->Fill(tof,de);
	  dE_vs_tof_Z[layer][Z-1]->Fill(tof,de);
	  
	}
        
      }
      
      if(neutronPosOnTW) {
	pMap[ev] = p_vec;
	nMap[ev] = n_vec;
      }
    }
    
////////////////////////////////////////////  Study TW reco Hits  //////////////////////////////////

    
    if(StudyTWreco) {

      Int_t nHits = twNtuHit->GetHitN();
      if(debug)
        cout<<" TWhits::"<<nHits<<endl;
      
      for(int ihit=0; ihit<nHits; ihit++) {
      
	TATWhit *hit = twNtuHit->GetHit(ihit);
        if(!hit) continue;
	
	Int_t bar = hit->GetBar();
	Int_t layer = hit->GetLayer();
	Int_t NmcTrk = hit->GetMcTracksN();
	Double_t eloss = hit->GetEnergyLoss();
	Double_t tof = hit->GetToF();
        Int_t Z = hit->GetChargeZ();

        if(debugCMM)
        printf("twhit::%d  %s  bar::%d  Z::%d  eloss::%f  NmcTrk::%d\n",ihit,LayerName[(TLayer)layer].data(),bar,Z,eloss,NmcTrk);
	
	dE_vs_tofRec[layer]->Fill(tof,eloss);
	
	for(int imctr=0; imctr<NmcTrk; imctr++) {
	  
	  Int_t mcId = hit->GetMcIndex(imctr);
	  Int_t mcTrkId = hit->GetMcTrackIdx(imctr);
	  
          if(debugCMM)
            cout<<"mcId::"<<mcId<<"  mcTrkId::"<<mcTrkId<<endl;
	  
	}

        // select only TW hit with a single track associated (no pile-up/multi hit in the same bar)
        if(NmcTrk==1) { // && layer==(Int_t)LayerX) {

          Int_t trkId = hit->GetMcTrackIdx(0);
          
          TAMCpart *mctrk = mcNtuPart->GetTrack(trkId);
          Int_t Zmc = mctrk->GetCharge();
          Int_t mothId = mctrk->GetMotherID();
          Int_t reg = mctrk->GetRegion();

          hCMM_TWhits[layer]->Fill(Z,Zmc);

          if(Z==GetZbeam())
            hyields_TWhits[layer]->Fill(Z);
          else {
            if(mothId==(Int_t)kPrimaryID && reg==TG_region)
              hyields_TWhits[layer]->Fill(Z);
          }

        } 
          
      }
    }

    
//__________________ TW clustering position__________________//

    if(ClusteringPositionResolution) {
      
      // clustering studies for resolution computation
      Int_t nHitsX = twNtuHit->GetHitN(LayerX);
      Int_t nHitsY = twNtuHit->GetHitN(LayerY);
      
      if(debug) {
	cout<<""<<endl;
	cout<<"Hits::  "<<nHitsX<<" "<<nHitsY<<endl;
      }
      
      Int_t nHitsX_good(0); 
      Int_t nHitsY_good(0); 
      Int_t totChargeX(0); 
      Int_t totChargeY(0); 
      Double_t tofX(-1); 
      Double_t tofY(-1); 
      Double_t deX(-1); 
      Double_t deY(-1); 

      for (Int_t i = 0; i < nHitsX; ++i) {  // loop over front TW hits
	
	TATWhit* hitX = twNtuHit->GetHit(i, LayerX);	
	if(!hitX) continue;

	if(debug) cout<<"hitX   Z::"<<hitX->GetChargeZ()<<"  dE::"<<hitX->GetEnergyLoss()<<"  Time::"<<hitX->GetTime()<<"  ToF::"<<hitX->GetToF()<<endl;
	nHitsX_good++;
	totChargeX+=hitX->GetChargeZ();
	
      }
      
      for (Int_t i = 0; i < nHitsY; ++i) {  // loop over front TW hits
	
	TATWhit* hitY = twNtuHit->GetHit(i, LayerY);	
	if(!hitY) continue;

	if(debug)cout<<"hitY   Z::"<<hitY->GetChargeZ()<<"  dE::"<<hitY->GetEnergyLoss()<<"  Time::"<<hitY->GetTime()<<"  ToF::"<<hitY->GetToF()<<endl;
	// cout<<"hitY   Z::"<<hitY->GetChargeZ()<<endl;
	nHitsY_good++;
	totChargeY+=hitY->GetChargeZ();
	
      }
      
      
      // int NtwPoints = twNtuPoint->GetPointN();
      
      if(NtwPoints>=1 && nHitsX>=1 && nHitsY>=1) {
	
	if(nHitsX>=nHitsY) {
	  
	  for(int ipt=0; ipt<NtwPoints; ipt++) {
	    
	    // if(NtwPoints==1 && nHitsX==1 && nHitsY==1) {
	    
	    // cout<<"Nx::"<<nHitsX<<" Ny::"<<nHitsY<<endl;
	    // TATWpoint *twpoint = twNtuPoint->GetPoint(0);
	    TATWpoint *twpoint = twNtuPoint->GetPoint(ipt);
	    TATWhit *hitX = twpoint->GetRowHit();
	    TATWhit *hitY = twpoint->GetColumnHit();
	    
	    Double_t posAlongBar = hitX->GetPosition();
	    Double_t posPerpBar  = twparGeo->GetBarPosition(hitY->GetLayer(),hitY->GetBar())[0];
	    
	    Double_t distX = posAlongBar-posPerpBar;
	    // cout<<"posAlongBar::"<<posAlongBar<<"  posPerpBar::"<<posPerpBar<<"  layer::"<<hitY->GetLayer()<<"  bar::"<<hitY->GetBar()<<"  distX::"<<distX<<endl;
	    // getchar();
	    
	    Int_t Z = twpoint->GetChargeZ();
	    
	    if(Z>0 && Z<GetZbeam()+1) {
	      distAlongX[Z-1]->Fill(posAlongBar-posPerpBar);
	      if(hitX->GetChargeZ()==hitY->GetChargeZ())
		distAlongX_cutZ[Z-1]->Fill(posAlongBar-posPerpBar);
	    }
	    
	  }
	}
	
	else {
	  
	  for(int ipt=0; ipt<NtwPoints; ipt++) {
	    
	    // cout<<"Nx::"<<nHitsX<<" Ny::"<<nHitsY<<endl;
	    // TATWpoint *twpoint = twNtuPoint->GetPoint(0);
	    TATWpoint *twpoint = twNtuPoint->GetPoint(ipt);
	    TATWhit *hitX = twpoint->GetRowHit();
	    TATWhit *hitY = twpoint->GetColumnHit();
	    
	    Double_t posAlongBar = hitY->GetPosition();
	    Double_t posPerpBar  = twparGeo->GetBarPosition(hitX->GetLayer(),hitX->GetBar())[1];
	    
	    Double_t distY = posAlongBar-posPerpBar;
	    // cout<<"posAlongBar::"<<posAlongBar<<"  posPerpBar::"<<posPerpBar<<"  layer::"<<hitX->GetLayer()<<"  bar::"<<hitX->GetBar()<<"  distY::"<<distY<<endl;
	    // getchar();
	    
	    Int_t Z = twpoint->GetChargeZ();
	    
	    if(Z>0 && Z<GetZbeam()+1) {
	      distAlongY[Z-1]->Fill(posAlongBar-posPerpBar);
	      if(hitX->GetChargeZ()==hitY->GetChargeZ())
		distAlongY_cutZ[Z-1]->Fill(posAlongBar-posPerpBar);
	    }
	    
	  }          
	}      
      }
    }  // close ClusteringPositionResolution
	
  } // close for loop over events

  gTAGroot.EndEventLoop();

  if(StudyEffAndXS) {

    // efficiency variables
    double m_num(-1), m_den(-1), m_eff(-1), em_eff(-1);
    
    for(int ichg=0; ichg<GetZbeam(); ichg++) {            
      
      // Fill Efficiencies
      printf("compute angular eff for Z = %d  with Nentries::%.3f\n",ichg+1,hyields_ang[ichg]->GetEntries());
      for(int ibin=1; ibin<hyields_ang_trk[ichg]->GetNbinsX()+1; ibin++) {
	
	m_num = hyields_ang[ichg]->GetBinContent(ibin);
	m_den = hyields_ang_trk[ichg]->GetBinContent(ibin);
	
	// cout<<m_num<<" "<<m_den<<endl;
	
	if(isBinomial)
	  ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
	else	
	  ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
	
	heff_ang[ichg]->SetBinContent(ibin,m_eff);
	heff_ang[ichg]->SetBinError(ibin,em_eff);
      }
      
      printf("compute ekin eff for Z = %d\n",ichg+1);
      for(int ibin=1; ibin<hyields_ekin_trk[ichg]->GetNbinsX()+1; ibin++) {
	
	m_num = hyields_ekin[ichg]->GetBinContent(ibin);
	m_den = hyields_ekin_trk[ichg]->GetBinContent(ibin);
	
	if(isBinomial)
	  ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
	else
	  ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
	
	heff_ekin[ichg]->SetBinContent(ibin,m_eff);
	heff_ekin[ichg]->SetBinError(ibin,em_eff);
      }
      
    }

    gDirectory->cd("EffAndPurity");
    // integrated efficiencies
    printf("compute integral eff\n");
    for(int ibin=1; ibin<hyields_trk->GetNbinsX()+1; ibin++) {
      
      if(ibin<GetZbeam()) {
	m_num = hyields_allCuts->GetBinContent(ibin);
	if(isPUtoBeRemoved)
	  m_num -= hyieldsPU->GetBinContent(ibin);  // summing PU yields
	m_den = hyields_trk->GetBinContent(ibin);
      }
      
      else if(ibin==GetZbeam()) {
	m_num = hyields_rec->GetBinContent(ibin);  // numerator: only request of reconstructed primary ion
	m_den = Nprim_notInteracting;  // denominator: request only a primary ion not interacting in the TG
	
      }
      
      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      heff->SetBinContent(ibin,m_eff);
      heff->SetBinError(ibin,em_eff);
    }
    

    TH1D *heff_Intersections = (TH1D*)heff->Clone("Eff_Intersections");
    heff_Intersections->SetLineColor(kRed+1);
    for(int ibin=1; ibin<hyields_intersections->GetNbinsX()+1; ibin++) {
      
      if(ibin<GetZbeam()) {
	m_num = hyields_allCuts->GetBinContent(ibin);
        m_den = hyields_intersections->GetBinContent(ibin);

        if(isPUtoBeRemoved)
	  m_num -= hyieldsPU->GetBinContent(ibin);  // summing PU yields
      }
      else if(ibin==GetZbeam()) {
	m_num = hyields_rec->GetBinContent(ibin);  // numerator: only request of reconstructed primary ion
	m_den = Nprim_notInteracting;  // denominator: request only a primary ion not int
      }
              
      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      heff_Intersections->SetBinContent(ibin,m_eff);
      heff_Intersections->SetBinError(ibin,em_eff);
    }


    TH1D *heff_fromCrossings = (TH1D*)heff->Clone("Eff_fromCrossings");
    heff_fromCrossings->SetLineColor(kGreen+1);
    for(int ibin=1; ibin<hyields_fromCrossings->GetNbinsX()+1; ibin++) {
      
      m_num = hyields_rec->GetBinContent(ibin);      
      m_den = hyields_fromCrossings->GetBinContent(ibin);
      
      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      heff_fromCrossings->SetBinContent(ibin,m_eff);
      heff_fromCrossings->SetBinError(ibin,em_eff);
    }
    

    TH1D *heff_multiHitReco_inTG = (TH1D*)heff->Clone("Eff_fromMultiHit_inTG");
    heff_multiHitReco_inTG->SetLineColor(kBlue);
    for(int ibin=1; ibin<hyields_multiHitReco_inTG->GetNbinsX()+1; ibin++) {
      
      if(ibin<GetZbeam()) {
        
	m_num = hyields_multiHitReco_inTG->GetBinContent(ibin);
        m_den = hyields_fromCrossings_inTG->GetBinContent(ibin);

      }
      else if(ibin==GetZbeam()) {
        
	m_num = hyields_multiHitReco->GetBinContent(ibin);  // numerator: only request of reconstructed primary ion      
        m_den = hyields_fromCrossings->GetBinContent(ibin);

      }
      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      heff_multiHitReco_inTG->SetBinContent(ibin,m_eff);
      heff_multiHitReco_inTG->SetBinError(ibin,em_eff);
    }

    TH1D *heff_multiHitReco = (TH1D*)heff->Clone("Eff_fromMultiHit");
    heff_multiHitReco->SetLineColor(kBlue-7);
    for(int ibin=1; ibin<hyields_multiHitReco->GetNbinsX()+1; ibin++) {
      
      m_num = hyields_multiHitReco->GetBinContent(ibin);
      m_den = hyields_fromCrossings->GetBinContent(ibin);
      
      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      heff_multiHitReco->SetBinContent(ibin,m_eff);
      heff_multiHitReco->SetBinError(ibin,em_eff);
    }

    TH1D *heff_TWhits[kLayers];
    for (int ilay=0; ilay<kLayers; ilay++) {
      
      heff_TWhits[ilay] = (TH1D*)heff->Clone(Form("Eff_TWhits_%s",LayerName[(TLayer)ilay].data()));
      heff_TWhits[ilay]->SetLineColor(kRed+1*(ilay+1));

      for(int ibin=1; ibin<hyields_TWhits[ilay]->GetNbinsX()+1; ibin++) {
        
	m_num = hyields_TWhits[ilay]->GetBinContent(ibin);

        m_den = hyields_fromCrossings->GetBinContent(ibin);

        if(isBinomial)
          ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
        else
          ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
        
        heff_TWhits[ilay]->SetBinContent(ibin,m_eff);
        heff_TWhits[ilay]->SetBinError(ibin,em_eff);
        
      }
    }


    TH1D *hPurity = (TH1D*)heff->Clone("hPurity");
    hPurity->SetLineColor(kBlue-7);
    for(int ibin=1; ibin<hPurity->GetNbinsX()+1; ibin++) {
      
      m_num = hyields_multiHitReco->GetBinContent(ibin);
      m_den = hyields_multiHitReco->GetBinContent(ibin) + hWrongZ->GetBinContent(ibin);

      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      hPurity->SetBinContent(ibin,m_eff);
      hPurity->SetBinError(ibin,em_eff);
    }

    TH1D *hPurity_inTG = (TH1D*)heff->Clone("hPurity_inTG");
    hPurity_inTG->SetLineColor(kBlue-7);
    for(int ibin=1; ibin<hPurity_inTG->GetNbinsX()+1; ibin++) {
      
      if(ibin<GetZbeam()) {

      	m_num = hyields_multiHitReco_inTG->GetBinContent(ibin);
        m_den = hyields_multiHitReco_inTG->GetBinContent(ibin) + hWrongZ_inTG->GetBinContent(ibin);

      }
      else if(ibin==GetZbeam()) {
        
	m_num = hyields_multiHitReco->GetBinContent(ibin);  // numerator: only request of reconstructed primary ion      
        m_den = hyields_multiHitReco->GetBinContent(ibin) + hWrongZ->GetBinContent(ibin);

      }
      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      hPurity_inTG->SetBinContent(ibin,m_eff);
      hPurity_inTG->SetBinError(ibin,em_eff);
    }

    // produce the CMM matrix
    for(int ibinY=1; ibinY<GetZbeam()+1; ibinY++) {

      Int_t SumYields(0);
      
      for(int ibinX=1; ibinX<GetZbeam()+1; ibinX++) {

        SumYields += hCMM_crossing->GetBinContent(ibinX,ibinY);
        
      }

      Double_t num = hCMM_crossing->GetBinContent(ibinY,ibinY);
      hCMM_crossing->SetBinContent(hCMM_crossing->GetNbinsX(),ibinY,(Double_t)(num/SumYields));

      hCMM_crossing->GetYaxis()->SetBinLabel(ibinY,Form("%s_MC",ElementName[ibinY-1].Data()));

    }

    for(int ibinX=1; ibinX<GetZbeam()+1; ibinX++) {

      hCMM_crossing->SetBinContent(ibinX,hCMM_crossing->GetNbinsY(),hPurity->GetBinContent(ibinX));

      hCMM_crossing->GetXaxis()->SetBinLabel(ibinX,ElementName[ibinX-1].Data());
            
    }
    hCMM_crossing->GetXaxis()->SetBinLabel(hCMM_crossing->GetNbinsX(),"% good rec");
    hCMM_crossing->GetYaxis()->SetBinLabel(hCMM_crossing->GetNbinsY(),"purity");


    // produce the CMM matrix for fragment produced in TG
    for(int ibinY=1; ibinY<GetZbeam()+1; ibinY++) {

      Int_t SumYields(0);
      
      for(int ibinX=1; ibinX<GetZbeam()+1; ibinX++) {

        SumYields += hCMM_crossing_inTG->GetBinContent(ibinX,ibinY);
        
      }

      Double_t num = hCMM_crossing_inTG->GetBinContent(ibinY,ibinY);
      hCMM_crossing_inTG->SetBinContent(hCMM_crossing_inTG->GetNbinsX(),ibinY,(Double_t)(num/SumYields));

      hCMM_crossing_inTG->GetYaxis()->SetBinLabel(ibinY,Form("%s_MC",ElementName[ibinY-1].Data()));

    }

    for(int ibinX=1; ibinX<GetZbeam()+1; ibinX++) {

      hCMM_crossing_inTG->SetBinContent(ibinX,hCMM_crossing_inTG->GetNbinsY(),hPurity_inTG->GetBinContent(ibinX));

      hCMM_crossing_inTG->GetXaxis()->SetBinLabel(ibinX,ElementName[ibinX-1].Data());
            
    }
    hCMM_crossing_inTG->GetXaxis()->SetBinLabel(hCMM_crossing_inTG->GetNbinsX(),"#varepsilon");
    hCMM_crossing_inTG->GetYaxis()->SetBinLabel(hCMM_crossing_inTG->GetNbinsY(),"purity");

    gDirectory->cd("../");

    // efficiencies for fragments produced out of target to correct eventually the yields of no TG runs (why these efficiencies should be different from the nominal ones?)
    for(int ibin=1; ibin<hyields_trk_bkg->GetNbinsX()+1; ibin++) {
      
      m_num = hyields_prim_bkg->GetBinContent(ibin);
      m_den = hyields_trk_bkg->GetBinContent(ibin);
      
      if(isBinomial)
	ComputeBinomialEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      else
	ComputeBayesianEfficiency(ibin,m_num,m_den,m_eff,em_eff);
      
      heff_bkg->SetBinContent(ibin,m_eff);
      heff_bkg->SetBinError(ibin,em_eff);
    }
    
    // compute FLUKA MC cross section
    double Nts = (rho_TG*dx_TG*Na)/A_TG;
    
    for(int ichg=0; ichg<GetZbeam(); ichg++) {

      // differential XS in solid angle
      hXS_dOmega[ichg] = (TH1D*)hangle[ichg]->Clone(Form("YieldsInDomega_Z%d",ichg+1));
      hXS_dOmega[ichg]->SetTitle(Form("Yields_Z%d",ichg+1));
      hXS_dOmega[ichg]->GetXaxis()->SetTitle("#theta [#circ]");
      hXS_dOmega[ichg]->GetYaxis()->SetTitle("dN/d#Omega [sr^{-1}]");
    
      for(int ibin=1; ibin<hXS_dOmega[ichg]->GetNbinsX()+1; ibin++) {
      
	Double_t binlow = hXS_dOmega[ichg]->GetBinLowEdge(ibin)/180.*TMath::Pi();
	Double_t binhigh = binlow + hXS_dOmega[ichg]->GetBinWidth(ibin);
      
	Double_t Domega = 2*TMath::Pi()*(TMath::Cos(binlow)-TMath::Cos(binhigh));
	
	if(ichg==1)
	  cout<<binlow<<" "<<binhigh<<" "<<Domega<<endl;
      
	hXS_dOmega[ichg]->SetBinContent(ibin,hXS_dOmega[ichg]->GetBinContent(ibin)/Domega);
	hXS_dOmega[ichg]->SetBinError(ibin,hangle[ichg]->GetBinError(ibin)/Domega);
	
      }

      hXS_dOmega[ichg]->Scale(1/Nts*1e27/Nprim_impingingOnTG); // cross section in mbarn

      // differential XS in Ekin
      hXS_dEkin[ichg] = (TH1D*)hekin[ichg]->Clone(Form("YieldsInDEkin_Z%d",ichg+1));
      hXS_dEkin[ichg]->SetTitle(Form("Yields_Z%d",ichg+1));
      hXS_dEkin[ichg]->GetXaxis()->SetTitle("E_{kin} [MeV/u]");
      hXS_dEkin[ichg]->GetYaxis()->SetTitle("dN/dE_{kin} [(MeV/u)^{-1}]");
    
      for(int ibin=1; ibin<hXS_dEkin[ichg]->GetNbinsX()+1; ibin++) {
	
	Double_t bin_DE = hXS_dEkin[ichg]->GetBinWidth(ibin);
	
	// cout<<binlow<<" "<<binhigh<<" "<<Domega<<endl;
      
	hXS_dEkin[ichg]->SetBinContent(ibin,hXS_dEkin[ichg]->GetBinContent(ibin)/bin_DE);
	hXS_dEkin[ichg]->SetBinError(ibin,hekin[ichg]->GetBinError(ibin)/bin_DE);
	
      }
      
      hXS_dEkin[ichg]->Scale(1/Nts*1e27/Nprim_impingingOnTG); // cross section in mbarn

    }
    

    // integrated MC true XS
    TH1D *hXS = (TH1D*)hyields_XStrue->Clone("hXS");
    hXS->Scale(1/Nts*1e27/Nprim_impingingOnTG); // cross section in mbarn
    // hXS->Scale(A_TG/rho_TG/dx_TG/Na*1e27/Nprim_impingingOnTG); // cross section in mbarn
    for(Int_t ibin=1; ibin<GetZbeam()+1; ibin++)
      hXS->GetXaxis()->SetBinLabel(ibin,ElementName[ibin-1].Data());

    hXS->GetYaxis()->SetTitle("#sigma [mb]");
    
    //compute reconstructed FLUKA MC cross section  
    TH1D *hXS_rec = (TH1D*)hyields_rec->Clone("hXS_rec");
    for (int Z=1; Z<GetZbeam()+1;Z++) {
      if(isEfftoNoTGused)
	hXS_rec->SetBinContent(Z, (hyields_rec->GetBinContent(Z)/heff->GetBinContent(Z)/Nprim_impingingOnTG-hyields_bkg->GetBinContent(Z)/heff_bkg->GetBinContent(Z)/Nprim_notInteracting));
      else
	hXS_rec->SetBinContent(Z, (hyields_rec->GetBinContent(Z)/Nprim_impingingOnTG-hyields_bkg->GetBinContent(Z)/Nprim_notInteracting)/heff->GetBinContent(Z));
    }
    hXS_rec->Scale(1/Nts*1e27); // cross section in mbarn
    // hXS_rec->Scale(1/Nts*1e27/Nprim_impingingOnTG); // cross section in mbarn
    // hXS_rec->Scale(A_TG/rho_TG/dx_TG/Na*1e27/Nprim_impingingOnTG); // cross section in mbarn
    for(Int_t ibin=1; ibin<GetZbeam()+1; ibin++)
      hXS_rec->GetXaxis()->SetBinLabel(ibin,ElementName[ibin-1].Data());

    hXS_rec->GetYaxis()->SetTitle("#sigma [mb]");
    
    TH1D *hsysXS = (TH1D*)hXS->Clone("hsysXS");
    
    for(int kk=1; kk<hXS->GetNbinsX()+1; kk++) {
      
      hsysXS->SetBinContent(kk,(hXS->GetBinContent(kk)-hXS_rec->GetBinContent(kk))/ hXS->GetBinContent(kk));
      cout<<"Z="<<kk<<endl;
      cout<<"XS::  "<<hXS->GetBinContent(kk)<<" +/- "<<hXS->GetBinError(kk)<<endl;  // neglected the error due to the Nprim: only the error on the yields is dominant
      cout<<"sys:: "<<hsysXS->GetBinContent(kk)<<" +/- "<<hsysXS->GetBinError(kk)<<endl;  // neglected the error due to the Nprim: only the error on the yields is dominant
      cout<<"eff:: "<<heff->GetBinContent(kk)<<" +/- "<<heff->GetBinError(kk)<<endl<<endl;  // neglected the error due to the Nprim: only the error on the yields is dominant
      
    }
    
    double total_cc_XS_trueMC = -1/Nts*TMath::Log((double)Nprim_notInteracting/(double)Nprim_impingingOnTG)*1.e27;
    double total_reaction_XS_trueMC = 1/Nts*((double)Nprim_interacting/Nprim_impingingOnTG)*1.e27;
    
    cout<<"Nprim  notInteracting:: "<<Nprim_notInteracting<<"  ImpingingOnTG:: "<<Nprim_impingingOnTG<<"  interacting::"<<Nprim_interacting<<endl;
    cout<<"total cc_XS:: "<<total_cc_XS_trueMC<<endl;
    cout<<"total reaction XS:: "<<total_reaction_XS_trueMC<<endl;
    
    TH1D *hsys_secFrg = (TH1D*)hXS->Clone("hsys_secFrg");
    
    for(int kk=1; kk<hsys_secFrg->GetNbinsX()+1; kk++) {
      
      hsys_secFrg->SetBinContent(kk,(hyields_trk->GetBinContent(kk)-hyields_trk_inTG->GetBinContent(kk))/ hyields_trk->GetBinContent(kk));
      
      cout<<"Z="<<kk<<"  "<<hsys_secFrg->GetBinContent(kk)<<" +/- "<<hsysXS->GetBinError(kk)<<endl;  // neglected the error due to the Nprim: only the error on the yields is dominant
      
    }
    
  } //close if(StudyEffAndXS)

  if(neutronPosOnTW) {
      
      
    for(auto it_p=pMap.begin(); it_p!=pMap.end(); ++it_p) {
	
      Int_t evt_p = it_p->first;
      vector<TVector3> vec_p = it_p->second;
	
      // cout<<"prim::"<<evt_p<<" size::"<<vec_p.size()<<endl;
	
      for(auto it_n=nMap.begin(); it_n!=nMap.end(); ++it_n) {
	  
	Int_t evt_n = it_n->first;
	  
	if(evt_n==evt_p) {
	
	  vector<TVector3> vec_n = it_n->second;
	  
	  // cout<<"neutr::"<<evt_n<<" size::"<<vec_n.size()<<endl;
	  
	  for (auto it_vp=vec_p.begin();it_vp!=vec_p.end();++it_vp){
	    TVector3 p_pos = *it_vp;
	    double x_p = p_pos.X();
	    double y_p = p_pos.Y();
	    // cout<<p_pos.X()<<" "<<p_pos.Y()<<endl;
	      
	    for (auto it_vn=vec_n.begin();it_vn!=vec_n.end();++it_vn){
	      
	      TVector3 n_pos = *it_vn;
	      
	      double x_n = n_pos.X();
	      double y_n = n_pos.Y();
	      // cout<<n_pos.X()<<" "<<n_pos.Y()<<endl;
		
	      double dist = sqrt(pow((x_p-x_n),2)+pow((y_p-y_n),2));
		
	      // cout<<"dist::"<<dist<<endl;
	      h_dist->Fill(dist);
		
	    }
	  }
	    
	    
	}
      }
	
    }
  }


  cout<<endl<<"Job Done!"<<endl;   


  fout->cd();
  fout->Write();
  fout->Close();

  return;

}

//-----------------------------------------------------------------------------

void BookHistograms() {

  h_reg_vs_z = new TH2D("h_reg_vs_z","h_reg_vs_z",4000,-50,350,500,0.5,500.5);  // x-binnining (mm)

  if(StudyFragmentation) {

    for(int ilay=0; ilay<kLayers; ilay++) {
      
      dE_vs_tof[ilay] = new TH2D(Form("dE_vs_tof_%s",LayerName[(TLayer)ilay].data()),Form("dE_vs_tof_%s",LayerName[(TLayer)ilay].data()),20000,0.,20.,1200,0.,120.);  // 1~ps/bin - 0.1 MeV/bin
      
      for(int ichg=0; ichg<GetZbeam(); ichg++)
	dE_vs_tof_Z[ilay][ichg] = new TH2D(Form("dE_vs_tof_%s_Z%d",LayerName[(TLayer)ilay].data(),ichg+1),Form("dE_vs_tof_%s_Z%d",LayerName[(TLayer)ilay].data(),ichg+1),20000,0.,20.,1200,0.,120.);  // 1~ps/bin - 0.1 MeV/bin
      
    }
  }

  if(StudyTWreco) {
    
    for(int ilay=0; ilay<kLayers; ilay++) {

      dE_vs_tofRec[ilay] = new TH2D(Form("dE_vs_tofRec_%s",LayerName[(TLayer)ilay].data()),Form("dE_vs_tofRec_%s",LayerName[(TLayer)ilay].data()),20000,0.,20.,1200,0.,120.);  // 1~ps/bin - 0.1 MeV/bin
    
      hCMM_TWhits[ilay] = new TH2D(Form("CMM_TWhits_%s",LayerName[(TLayer)ilay].data()),Form("CMM_TWhits_%s",LayerName[(TLayer)ilay].data()),GetZbeam(),0.5,GetZbeam()+.5,GetZbeam(),0.5,GetZbeam()+.5);  
    
      hyields_TWhits[ilay] = new TH1D(Form("IntYields_TWhits_allZ_%s",LayerName[(TLayer)ilay].data()),Form("IntYields_TWhits_allZ_%s",LayerName[(TLayer)ilay].data()),GetZbeam(),0.5,GetZbeam()+.5);

    }
  }

  if(ClusteringPositionResolution) {

    for(int ichg=0; ichg<GetZbeam(); ichg++) {
      
      distAlongX[ichg] = new TH1D(Form("distX_Z%d",ichg+1),Form("distX_Z%d",ichg+1),4000,-40,40);
      distAlongY[ichg] = new TH1D(Form("distY_Z%d",ichg+1),Form("distY_Z%d",ichg+1),4000,-40,40);
      distAlongX_cutZ[ichg] = new TH1D(Form("distX_Z%d_cutZ",ichg+1),Form("distX_Z%d",ichg+1),4000,-40,40);
      distAlongY_cutZ[ichg] = new TH1D(Form("distY_Z%d_cutZ",ichg+1),Form("distY_Z%d",ichg+1),4000,-40,40);
    }
  }
  
  if(StudyEffAndXS) {

    Tof_prim_noTG =  new TH1D("tof_prim_noTG","tof_prim_noTG",1200, 0, 120);
    Z_prim_noTG =  new TH1D("Z_prim_noTG","Z_prim_noTG",4000, -100, 300);
    Z_sec_fragmentation =  new TH1D("Z_sec_fragmentation","Z_sec_fragmentation",4000, -100, 300);
    Reg_prim_noTG =  new TH1D("reg_prim_noTG","reg_prim_noTG",500, 0.5, 500.5);

     TDirectory *EffDir;
     EffDir = gDirectory->mkdir("EffAndPurity");
     EffDir->cd();
      
     hCMM = new TH2D("CMM","CMM",GetZbeam(),0.5,GetZbeam()+.5,GetZbeam(),0.5,GetZbeam()+.5);  
     hCMM_rec = new TH2D("CMM_rec","CMM_rec",GetZbeam(),0.5,GetZbeam()+.5,GetZbeam(),0.5,GetZbeam()+.5);  
     hCMM_crossing = new TH2D("CMM_crossing","CMM_crossing",GetZbeam()+1,0.5,GetZbeam()+1.5,GetZbeam()+1,0.5,GetZbeam()+1.5);  
     hCMM_crossing_inTG = new TH2D("CMM_crossing_inTG","CMM_crossing_inTG",GetZbeam()+1,0.5,GetZbeam()+1.5,GetZbeam()+1,0.5,GetZbeam()+1.5);  

     gDirectory->cd("../");
     
     heff_bkg = new TH1D("EffBkg_allZ","EffBkg_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    heff = new TH1D("Eff_allZ","Eff_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    for(Int_t ibin=1; ibin<GetZbeam()+1; ibin++)
      heff->GetXaxis()->SetBinLabel(ibin,ElementName[ibin-1].Data());
    
    hyields_trk = new TH1D("IntYieldsTrk_allZ","IntYieldsTrk_allZ",GetZbeam(),0.5,GetZbeam()+.5);
    hyields_trk_inTG = new TH1D("IntYieldsTrk_inTG_allZ","IntYieldsTrk_inTG_allZ",GetZbeam(),0.5,GetZbeam()+.5);
    hyields_XStrue = new TH1D("IntYieldsXStrue_allZ","IntYieldsXStrue_allZ",GetZbeam(),0.5,GetZbeam()+.5);
    hyields_bkg = new TH1D("IntYieldsBkg_allZ","IntYieldsBkg_allZ",GetZbeam(),0.5,GetZbeam()+.5);
    
    hyields_trk_bkg = new TH1D("IntYieldsTrkBkg_allZ","IntYieldsTrkBkg_allZ",GetZbeam(),0.5,GetZbeam()+.5);
    hyields_prim_bkg = new TH1D("IntYieldsPrimBkg_allZ","IntYieldsPrimBKg_allZ",GetZbeam(),0.5,GetZbeam()+.5);
    
    EffDir->cd();
    hyields_rec = new TH1D("hyields_rec","IntYieldsRec_allZ",GetZbeam(),0.5,GetZbeam()+.5);
    hyields_crossTW = new TH1D("hyields_crossTW","IntYields_crossTW",GetZbeam(),0.5,GetZbeam()+.5);
    hyields_intersections = new TH1D("hyields_intersections","IntYields_intersections_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hyields_fromCrossings = new TH1D("hyields_fromCrossings","IntYields_fromCrossings_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hyields_fromCrossings_inTG = new TH1D("hyields_fromCrossings_inTG","IntYields_fromCrossings_inTG_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hyields_multiHitReco = new TH1D("hyields_multiHitReco","IntYields_multiHitReco_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hyields_multiHitReco_inTG = new TH1D("hyields_multiHitReco_inTG","IntYields_multiHitReco_inTG_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hWrongZ = new TH1D("hWrongZ","IntYields_WrongZ_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hWrongZ_crossing = new TH1D("hWrongZ_crossing","hWrongZ_crossing",GetZbeam(),0.5,GetZbeam()+.5);  
    hWrongZ_inTG = new TH1D("hWrongZ_inTG","IntYields_WrongZ_inTG_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hWrongZmult = new TH2D("hWrongZmult","HWrongZmult",GetZbeam(),0.5,GetZbeam()+.5,GetZbeam(),0.5,GetZbeam()+.5);  
    gDirectory->cd("../");

    hyields_fromTG = new TH1D("IntYields_fromTG_allZ","IntYields_fromTG_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hyields_allCuts = new TH1D("IntYields_allZ","IntYields_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hyields_allCuts->SetLineColor(2);
    hyields_allCuts->SetMarkerColor(2);
    hyields_allCuts->SetMarkerStyle(21);
    hyieldsPU = new TH1D("IntYieldsPU_allZ","IntYieldsPU_allZ",GetZbeam(),0.5,GetZbeam()+.5);  
    hyieldsPU->SetLineColor(4);
    hyieldsPU->SetMarkerColor(4);
    hyieldsPU->SetMarkerStyle(22);
    
    heloss_all = new TH1D("Eloss_all","Eloss_all",1200,0.,120.);
    heloss_all_frg_noTG  = new TH1D("Eloss_all_frg_noTG","Eloss_all_frg_noTG",1200,0.,120.);
    heloss_all_prim_noTG  = new TH1D("Eloss_all_prim_noTG","Eloss_all_prim_noTG",1200,0.,120.);
    heloss_all_primVT  = new TH1D("Eloss_all_primVT","Eloss_all_primVT",1200,0.,120.);
    heloss_all_cut = new TH1D("Eloss_all_cut","Eloss_all_cut",1200,0.,120.);
    // heloss_sum = new TH1D("Eloss_sum","Eloss_sum",1200,0.,120.);
    heloss_all_cutXS = new TH1D("Eloss_all_cutXS","Eloss_all_cutXS",1200,0.,120.);
    
    
    for(int ichg=0; ichg<GetZbeam(); ichg++) {
      
      hekin_vs_angle[ichg] = new TH2D(Form("Ekin_vs_Angle_Z%d",ichg+1),Form("Ekin_vs_Angle_Z%d",ichg+1),450,0.,45.,10000,0.,1000.);

      hekin[ichg] = new TH1D(Form("Ekin_Z%d",ichg+1),Form("Ekin_Z%d",ichg+1),nEbins-1,ebins);
      hangle[ichg] = new TH1D(Form("Angle_Z%d",ichg+1),Form("Angle_Z%d",ichg+1),30,0.,12.);
      hekin[ichg]->Sumw2(true);
      hangle[ichg]->Sumw2(true);
      
      heloss[ichg] = new TH1D(Form("Eloss_Z%d",ichg+1),Form("Eloss_Z%d",ichg+1),1200,0.,120.);
      heloss_prim_noTG[ichg] = new TH1D(Form("Eloss_prim_noTG_Z%d",ichg+1),Form("Eloss_prim_noTG_Z%d",ichg+1),1200,0.,120.);
      heloss_primVT[ichg] = new TH1D(Form("Eloss_primVT_Z%d",ichg+1),Form("Eloss_primVT_Z%d",ichg+1),1200,0.,120.);
      heloss_cut[ichg] = new TH1D(Form("Eloss_cut_Z%d",ichg+1),Form("Eloss_cut_Z%d",ichg+1),1200,0.,120.);
      heloss_cutXS[ichg] = new TH1D(Form("Eloss_cutXS_Z%d",ichg+1),Form("Eloss_cutXS_Z%d",ichg+1),1200,0.,120.);
      
      heff_ang[ichg]  = new TH1D(Form("AngEff_Z%d",ichg+1),Form("AngEff_Z%d",ichg+1),30,0,12);
      heff_ang[ichg]->GetXaxis()->SetTitle("#theta [#circ]");
      heff_ang[ichg]->GetYaxis()->SetTitle("eff");
      heff_ang[ichg]->SetLineColor(2+ichg);
      heff_ang[ichg]->SetMarkerColor(2+ichg);
      heff_ang[ichg]->SetMarkerStyle(21+ichg);
      
      hyields_ang[ichg]  = new TH1D(Form("AngYields_Z%d",ichg+1),Form("AngYields_Z%d",ichg+1),30,0,12);
      hyields_ang[ichg]->GetXaxis()->SetTitle("#theta [#circ]");
      hyields_ang[ichg]->GetYaxis()->SetTitle("yields");
      hyields_ang[ichg]->SetLineColor(2);
      hyields_ang[ichg]->SetMarkerColor(2);
      hyields_ang[ichg]->SetMarkerStyle(21);
      // hyields_ang[ichg]  = new TH1D(Form("AngYields_Z%d",ichg+1),Form("AngYields_Z%d",ichg+1),nAbins-1,abins);
      
      heff_ekin[ichg] = new TH1D(Form("EkinEff_Z%d",ichg+1),Form("EkinEff_Z%d",ichg+1),nEbins-1,ebins);
      heff_ekin[ichg]->GetXaxis()->SetTitle("(#gamma-1)*10^3");
      heff_ekin[ichg]->GetYaxis()->SetTitle("eff");
      heff_ekin[ichg]->SetLineColor(2+ichg);
      heff_ekin[ichg]->SetMarkerColor(2+ichg);
      heff_ekin[ichg]->SetMarkerStyle(21+ichg);
      
      hyields_ekin[ichg] = new TH1D(Form("EkinYields_Z%d",ichg+1),Form("EkinYields_Z%d",ichg+1),nEbins-1,ebins);
      hyields_ekin[ichg]->GetXaxis()->SetTitle("(#gamma-1)*10^3");
      hyields_ekin[ichg]->GetYaxis()->SetTitle("yields");
      hyields_ekin[ichg]->SetLineColor(2);
      hyields_ekin[ichg]->SetMarkerColor(2);
      hyields_ekin[ichg]->SetMarkerStyle(21);
      
      hyields_ang_trk[ichg]  = new TH1D(Form("AngYieldsTrk_Z%d",ichg+1),Form("AngYields_Z%d",ichg+1),30,0,12);
      hyields_ang_trk[ichg]->GetXaxis()->SetTitle("#theta [#circ]");
      hyields_ang_trk[ichg]->GetYaxis()->SetTitle("yields");
      // hyields_ang_trk[ichg]  = new TH1D(Form("AngYieldsTrk_Z%d",ichg+1),Form("AngYields_Z%d",ichg+1),nAbins-1,abins);
      hyields_ang_trk[ichg]->Sumw2(true);
      
      hyields_ekin_trk[ichg] = new TH1D(Form("EkinYieldsTrk_Z%d",ichg+1),Form("EkinYields_Z%d",ichg+1),nEbins-1,ebins);
      hyields_ekin_trk[ichg]->Sumw2(true);
      
    }
  }

  if(neutronPosOnTW) {

    h_posXY = new TH2D("h_posXY","h_posXY",500,-25,25,500,-25,25);

    h_dist = new TH1D("h_dist","h_dist",500,0.,50.);
    h_dist->GetXaxis()->SetTitle("dist[cm]");
    h_dist->GetYaxis()->SetTitle("Entries/0.1cm");

  }
  
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

        hyields_intersections->Fill(Z);

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
  itNtuClus = new TAITntuCluster();
  TAGdataDsc* itClus    = new TAGdataDsc(itNtuClus);
  actTreeReader->SetupBranch(itClus);

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
  tgNtuEvent= new TAGntuEvent();
  TAGdataDsc* tgEvent    = new TAGdataDsc(tgNtuEvent);
  actTreeReader->SetupBranch(tgEvent);

  
  return;
  
}
