/*!
  \file
  \version $Id: TATWactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TATWactNtuMC.
*/
#include "TError.h"

#include "TATWdatRaw.hxx"
#include "TATWparCal.hxx"
#include "TATWactNtuMC.hxx"

#include "TATWdigitizer.hxx"

#include "Parameters.h"

/*!
  \class TATWactNtuMC TATWactNtuMC.hxx "TATWactNtuMC.hxx"
  \brief NTuplizer for BM raw hits. **
*/

ClassImp(TATWactNtuMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWactNtuMC::TATWactNtuMC(const char* name,
                           TAGdataDsc* p_hitraw,
			   TAGparaDsc* p_parcal,
			   TAGparaDsc* p_parGeo,
                           EVENT_STRUCT* evStr,
			   Bool_t isZmc)
  : TAGaction(name, "TATWactNtuMC - NTuplize ToF raw data"),
    fHitContainer(p_hitraw),
    fpCalPar(p_parcal),
    fParGeo(p_parGeo),
    fEventStruct(evStr),
    fCnt(0),
    fCntWrong(0),
    fIsZtrueMC(isZmc)
{
  AddDataOut(p_hitraw, "TATWntuRaw");
  AddPara(p_parcal,"TATWparCal");
  AddPara(p_parGeo,"TAGparGeo");

  CreateDigitizer();

  if( fIsZtrueMC ) {

    fDigitizer->SetMCtrue();
    fDigitizer->SetPileUpOff();

  }

  if(fDigitizer->IsPileUpOff())
    Warning("TATWactNtuMC","Pile Up Off only for gloabal and ZID algorithm debug purposes...if is not the case remove the flag -zmc or check in TATWdigitizer.cxx constructor and set fPileUpOff(false)");
	     

  f_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  f_pargeo = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();
  
  fZbeam = f_pargeo->GetBeamPar().AtomicNumber;
  
  fMapPU.clear();
  fVecPuOff.clear();

  f_parcal = (TATWparCal*)fpCalPar->Object();
    
}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWactNtuMC::~TATWactNtuMC()
{
   delete fDigitizer;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TATWactNtuMC::CreateHistogram()
{
   
   DeleteHistogram();
   
   fpHisHitCol = new TH1F("twHitCol", "ToF Wall - Column hits", 22, 0., 22);
   AddHistogram(fpHisHitCol);
   
   fpHisHitLine = new TH1F("twHitLine", "ToF Wall - Line hits", 22, 0., 22);
   AddHistogram(fpHisHitLine);

   fpHisHitMap = new TH1F("twHitMap", "ToF Wall - Hit Map", 1000,-50,50);
   AddHistogram(fpHisHitMap);

   fpHisRecPos = new TH1F("RecPos", "RecPos", 1000,-50,50);
   AddHistogram(fpHisRecPos);

   fpHisRecPosMc = new TH1F("TruePos", "TruePos", 1000,-50,50);
   AddHistogram(fpHisRecPosMc);
   
   fpHisRecTof = new TH1F("RecTof", "RecTof", 500, 0., 50.);
   AddHistogram(fpHisRecTof);
   
   fpHisRecTofMc = new TH1F("TrueTof", "TrueTof", 500, 0., 50.);
   AddHistogram(fpHisRecTofMc);

   fpHisZID = new TH2I("twZID", "twZID", fZbeam+3,-2.5,(int)fZbeam+0.5, fZbeam+2,-1.5,(int)fZbeam+0.5);
   AddHistogram(fpHisZID);
   
   if(fDigitizer->IsMCtrue()) {  // only for ZID algorithm debug purposes
     fpHisZID_MCtrue = new TH2I("twZID_MCtrue", "twZID_MCtrue", fZbeam+3,-2.5,(int)fZbeam+0.5, fZbeam+2,-1.5,(int)fZbeam+0.5);
     AddHistogram(fpHisZID_MCtrue);

     for(int ilayer=0; ilayer<(int)nLayers; ilayer++) {
       fpHisElossTof_MCtrue[ilayer] = new TH2D(Form("dE_vs_Tof_layer%d_MCtrue",ilayer),Form("dE_vs_Tof_ilayer%d_MCtrue",ilayer),500,0.,50.,480,0.,120.);
       AddHistogram(fpHisElossTof_MCtrue[ilayer]);
     }
     
     for(int iZ=1; iZ < fZbeam+1; iZ++) {
       fpHisElossTof_MC.push_back(new TH2D(Form("dE_vs_Tof_Z%d_MCtrue",iZ),Form("dE_vs_Tof_%d",iZ),500,0.,50.,480,0.,120.));

       AddHistogram(fpHisElossTof_MC[iZ-1]);

       fpHisDistZ_MC.push_back( new TH1F(Form("distMC_Z_%d",iZ),Form("distMC_Z%d",iZ),2000,-20.,80) );
       AddHistogram(fpHisDistZ_MC[iZ-1]);
       
       fpHisResPos.push_back( new TH1D(Form("ResPos_Z_%d",iZ), Form("ResPos_Z_%d",iZ), 1000,-50,50) );
       AddHistogram(fpHisResPos[iZ-1]);
     }
   }

   
   for(int ilayer=0; ilayer<(TWparam)nLayers; ilayer++) {
     fpHisElossTof_MCrec[ilayer] = new TH2D(Form("dE_vs_Tof_layer%d",ilayer),Form("dE_vs_Tof_ilayer%d",ilayer),500,0.,50.,480,0.,120.);
     AddHistogram(fpHisElossTof_MCrec[ilayer]);
   }
   
   for(int iZ=1; iZ < fZbeam+1; iZ++) {

     fpHisElossTof.push_back(new TH2D(Form("dE_vs_Tof_Z%d",iZ),Form("dE_vs_Tof_%d",iZ),500,0.,50.,480,0.,120.));
     AddHistogram(fpHisElossTof[iZ-1]);
     
     fpHisDistZ.push_back( new TH1F(Form("dist_Z%d",iZ),Form("dist_Z%d",iZ),2000,-20.,80) );
     AddHistogram(fpHisDistZ[iZ-1]);

   }

   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TATWactNtuMC::CreateDigitizer()
{
   TATWntuRaw* pNtuRaw = (TATWntuRaw*) fHitContainer->Object();
   
   fDigitizer = new TATWdigitizer(pNtuRaw);
}

//------------------------------------------+-----------------------------------
//! Action.

bool TATWactNtuMC::Action() {

   if ( FootDebugLevel(1)> 0 )
     cout << "TATWactNtuMC::Action() start" << endl;

    //The number of hits inside the TW
   if ( FootDebugLevel(1)> 0 )
     cout << "Processing " << fEventStruct->SCNn << " hits in the TW ..."<<endl;

    // clear maps
    fDigitizer->ClearMap();
    fMapPU.clear();
    //clear vectors
    fVecPuOff.clear();

    // taking the tof for each TW hit with respect to the first hit of ST...for the moment not considered the possibility of multiple Hit in the ST, if any
    Float_t timeST  = fEventStruct->STCtim[0]*TAGgeoTrafo::SecToPs();
    
    // fill the container of hits, divided by layer, i.e. the column at 0 and row at 1
    for (int i=0; i < fEventStruct->SCNn; i++) { 
    
       Int_t layer = fEventStruct->SCNiview[i];    // layers 0 (y-bars) and 1 (x-bars)
       Int_t barId   = fEventStruct->SCNibar[i];
       Int_t trackId = fEventStruct->SCNid[i] - 1;
       Float_t x0    = fEventStruct->SCNxin[i];
       Float_t y0    = fEventStruct->SCNyin[i];
       Float_t z0    = fEventStruct->SCNzin[i];
       Float_t z1    = fEventStruct->SCNzout[i];
       Float_t edep  = fEventStruct->SCNde[i]*TAGgeoTrafo::GevToMev();
       Float_t time  = fEventStruct->SCNtim[i]*TAGgeoTrafo::SecToPs();
       // get true charge
       Int_t Z       = fEventStruct->TRcha[trackId];

       Float_t trueTof = (time - timeST)*TAGgeoTrafo::PsToNs();  //ns
       if(FootDebugLevel(1)>0)
          printf("\n timeTW::%f timeST::%f tof::%f\n",time,timeST,trueTof);

       time -= timeST;  // ToF TW-SC to be digitized in ps

       TVector3 posIn(x0, y0, z0);
       TVector3 posInLoc = f_geoTrafo->FromGlobalToTWLocal(posIn);

       if(FootDebugLevel(1)>0) {
          cout<<layer<<endl;
          cout<<posIn.x()<<" "<<posIn.y()<<" "<<posIn.z()<<" "<<endl;
          cout<<posInLoc.x()<<" "<<posInLoc.y()<<" "<<posInLoc.z()<<" "<<endl;
          cout<<""<<endl;
       }
       
       double truePos = 0;
       if( layer == (Int_t)LayerY )  // layer 0 --> vertical bar
	 truePos = posInLoc.Y(); 
       else if( layer == (Int_t)LayerX )  // layer 1 --> horizontal bar
	 truePos = posInLoc.X();

       if(FootDebugLevel(1)>0)
          cout<<"trueEloss::"<<edep<<" trueTof::"<<trueTof<<" truePos::"<<truePos<<endl;
              
       if(fDigitizer->IsMCtrue()) {  // only for ZID algorithm debug purposes

	 Int_t mothId = (fEventStruct->TRpaid[trackId])-1;
	 
	 Float_t distZ_MC[fZbeam]; //inf
	 
	 Int_t Zrec_MCtrue;	 
	 // test algorithm in MC in a clean situation: only primary fragmentation
	 if( mothId>0) {
	   Zrec_MCtrue=-1.; //set Z to nonsense value
	   if(FootDebugLevel(1) > 0) printf("the energy released is %f MeV (tof %.f ns) so Zraw is set to %d\n",edep,trueTof,Zrec_MCtrue);
	 } else
	   Zrec_MCtrue = f_parcal->GetChargeZ(edep,trueTof,layer);
	 
	 for(int iZ=1; iZ<fZbeam+1; iZ++)
	   distZ_MC[iZ-1]= f_parcal->GetDistBB(iZ);
	 
	 if (ValidHistogram()) {
	   fpHisZID_MCtrue->Fill(Zrec_MCtrue,Z);
	   
	   fpHisElossTof_MCtrue[layer]->Fill(trueTof,edep);
	   
	   if( Zrec_MCtrue>0 && Zrec_MCtrue < fZbeam+1 )
	     fpHisElossTof_MC[Zrec_MCtrue-1]->Fill(trueTof,edep);
	   
	   fpHisRecPosMc->Fill(truePos);
	   fpHisRecTofMc->Fill(trueTof);
	   
	   for(int iZ=1; iZ < fZbeam+1; iZ++) {
	     
	     if(iZ==1) {
	       if(iZ==Zrec_MCtrue) 
             fpHisDistZ_MC[iZ-1]->Fill(distZ_MC[iZ-1]);
	       else
             fpHisDistZ_MC[iZ-1]->Fill(std::numeric_limits<float>::max());
	     }
	     else
	       fpHisDistZ_MC[iZ-1]->Fill(distZ_MC[iZ-1]);
	     
	   }
	   
	 }
	   
	 if(FootDebugLevel(1) > 0) {
	   if(Zrec_MCtrue>0 && Z>0) {	   
	     fCnt++;	   
	     if(Zrec_MCtrue!=Z) {
	       cout<<"Hit MC n::"<<fCnt<<endl;
	       printf("layer::%d bar::%d\n", layer,  fEventStruct->SCNibar[i]);
	       fCntWrong++;
	       cout<<"edep::"<<edep<<"  trueTof::"<<trueTof<<endl;
	       cout<<"Zrec::"<<Zrec_MCtrue<<"  Z_MC::"<<Z<<endl;
	       printf("Z wrong/(Zrec>0) :: %d/%d\n",fCntWrong,fCnt);
	     }
	   }
	 }
       }

       if ( FootDebugLevel(1)> 0 )
          printf("layer::%d bar::%d\n", layer, barId);

       // if bar is dead in data skip it
       if(!f_parcal->IsTWbarActive(layer,barId)) continue;

       barId+=TATWparGeo::GetLayerOffset()*layer;  //offset for the horizontal bars Id
       
       fDigitizer->Process(edep, posInLoc[0], posInLoc[1], z0, z1, time, barId, Z);
       
       TATWntuHit* hit = fDigitizer->GetCurrentHit();
       hit->AddMcTrackIdx(trackId, i);

       fMapPU[barId] = hit;

       fVecPuOff.push_back(hit);

       if (ValidHistogram()) {
          if(hit->GetChargeZ()>0 && hit->GetChargeZ()<fZbeam+1)
             fpHisResPos[hit->GetChargeZ()-1]->Fill(hit->GetPosition()-truePos);
       }
    }


    if( fDigitizer->IsPileUpOff() ) {


      for(auto it=fVecPuOff.begin(); it !=fVecPuOff.end(); ++it) {

	TATWntuHit* hit = *it;
	
	Int_t layer = hit->GetLayer();
	Int_t bar = hit->GetBar();

	// get Eloss threshold from configuration file
	Double_t Ethr = f_parcal->GetElossThreshold(layer,bar);

	Double_t recEloss = hit->GetEnergyLoss(); // MeV
	Double_t recTof   = hit->GetTime();       // ns
	Double_t recPos   = hit->GetPosition();   // cm
	
	Int_t Z = hit->GetChargeZ();  // mc true charge stored in the hit up to now
      
	if(FootDebugLevel(1)>0)
	  cout<<"recEloss::"<<recEloss<<" recTof::"<<recTof<<" recPos::"<<recPos<<endl;
	
	if(!fDigitizer->IsOverEnergyThreshold(Ethr,recEloss)) {
	  if(FootDebugLevel(1) > 0)
	    printf("the energy released (%f MeV) is under the set threshold (%.1f MeV)\n",recEloss,fDigitizer->GetEnergyThreshold());     
	  
	  recEloss=-99.; //set energy to nonsense value
	  hit->SetEnergyLoss(recEloss);	
	}       
	
	Int_t Zrec = f_parcal->GetChargeZ(recEloss,recTof,hit->GetLayer());
	hit->SetChargeZ(Zrec);
	if(fIsZtrueMC)
	  hit->SetChargeZ(Z);

	if(FootDebugLevel(1)>0 && Z!=Zrec)
	  cout<<"set Z::"<<hit->GetChargeZ()<<"  Ztrue::"<<Z<<"  Zrec::"<<Zrec<<endl;
	
 	Float_t distZ[fZbeam];
	for(int iZ=1; iZ<fZbeam+1; iZ++)
	  distZ[iZ-1]= f_parcal->GetDistBB(iZ);
	
	if(FootDebugLevel(1) > 0) {

	  if(!fDigitizer->IsMCtrue()) {  // only for ZID algorithm debug purposes
	    if(Zrec>0 && Z>0) {
	      fCnt++;	     
	      if(Zrec!=Z) {
	    	cout<<"Hit n::"<<fCnt<<endl;
	    	fCntWrong++;
	    	cout<<"edep::"<<recEloss<<"  time::"<<recTof<<endl;
	    	cout<<"Zrec::"<<Zrec<<"  Z_MC::"<<Z<<endl;
	    	printf("Z wrong/(Zrec>0) :: %d/%d\n",fCntWrong,fCnt);
	      }
	    }
	  }
	}

	
	if (ValidHistogram()) {
	  
	  fpHisElossTof_MCrec[hit->GetLayer()]->Fill(recTof,recEloss);
	  
	  if( Zrec>0 && Zrec < fZbeam+1 )
	    fpHisElossTof[Zrec-1]->Fill(recTof,recEloss);
	  
	  for(int iZ=1; iZ < fZbeam+1; iZ++)
	    fpHisDistZ[iZ-1]->Fill(distZ[iZ-1]);
	  
	  fpHisZID->Fill(Zrec,Z);
	  
	  fpHisRecPos->Fill(recPos);
	  fpHisRecTof->Fill(recTof);
	  
	  if (hit->IsColumn())
	    fpHisHitCol->Fill(hit->GetBar());
	  else
	    fpHisHitLine->Fill(hit->GetBar());
	  
	}
      }
      
    }  else {  // full MC reconstructed (pile-up included)
    

      for(auto it=fMapPU.begin(); it !=fMapPU.end(); ++it) {

	TATWntuHit* hit = it->second;
	
	Int_t layer = hit->GetLayer();
	Int_t bar = hit->GetBar();

	// get Eloss threshold from configuration file
	Double_t Ethr = f_parcal->GetElossThreshold(layer,bar);

	Double_t recEloss = hit->GetEnergyLoss(); // MeV
	Double_t recTof   = hit->GetTime();       // ns
	Double_t recPos   = hit->GetPosition();   // cm
	
	Int_t Z = hit->GetChargeZ();  // mc true charge stored in the hit up to now
	
	if(FootDebugLevel(1)>0)
	  cout<<"Ethr::"<<Ethr<<" recEloss::"<<recEloss<<" recTof::"<<recTof<<" tof::"<<hit->GetToF()<<" recPos::"<<recPos<<endl;
	
	if(!fDigitizer->IsOverEnergyThreshold(Ethr,recEloss)) {
	  if(FootDebugLevel(1) > 0)
	    printf("the energy released (%f MeV) is under the set threshold (%.1f MeV)\n",recEloss,fDigitizer->GetEnergyThreshold());     
	  
	  recEloss=-99.; //set energy to nonsense value
	  hit->SetEnergyLoss(recEloss);	
	}       
	
	Int_t Zrec = f_parcal->GetChargeZ(recEloss,recTof,hit->GetLayer());
	hit->SetChargeZ(Zrec);

	if(FootDebugLevel(1)>0) cout<<"rec Z::"<<Zrec<<endl;

	Float_t distZ[fZbeam];
	for(int iZ=1; iZ<fZbeam+1; iZ++)
	  distZ[iZ-1]= f_parcal->GetDistBB(iZ);	  


	if (ValidHistogram()) {
	  
	  fpHisElossTof_MCrec[hit->GetLayer()]->Fill(recTof,recEloss);
	  
	  if( Zrec>0 && Zrec < fZbeam+1 )
	    fpHisElossTof[Zrec-1]->Fill(recTof,recEloss);
	  
	  for(int iZ=1; iZ < fZbeam+1; iZ++)
	    fpHisDistZ[iZ-1]->Fill(distZ[iZ-1]);
	  
	  fpHisZID->Fill(Zrec,Z);
	  
	  fpHisRecPos->Fill(recPos);
	  fpHisRecTof->Fill(recTof);
	  
	  if (hit->IsColumn())
	    fpHisHitCol->Fill(hit->GetBar());
	  else
	    fpHisHitLine->Fill(hit->GetBar());
	  
	}
      }
    }

    
    fHitContainer->SetBit(kValid);

    
    return true;
    
}
