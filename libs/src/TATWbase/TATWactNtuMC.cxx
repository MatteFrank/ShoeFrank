/*!
  \file
  \version $Id: TATWactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TATWactNtuMC.
*/
#include "TAGgeoTrafo.hxx"

#include "TATWdatRaw.hxx"
#include "TATWparCal.hxx"
#include "TATWactNtuMC.hxx"

#include "TATWdigitizer.hxx"

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
                           EVENT_STRUCT* evStr)
  : TAGaction(name, "TATWactNtuMC - NTuplize ToF raw data"),
    m_hitContainer(p_hitraw),
    fpCalPar(p_parcal),
    fParGeo(p_parGeo),
    m_eventStruct(evStr),
    fCnt(0),
    fCntWrong(0)
{
  AddDataOut(p_hitraw, "TATWntuRaw");
  AddPara(p_parcal,"TATWparCal");
  AddPara(p_parGeo,"TAGparGeo");

  CreateDigitizer();

  f_pargeo = (TAGparGeo*)(TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();
  
  fZbeam = f_pargeo->GetBeamPar().AtomicNumber;

  fMapPU.clear();
  fVecPuOff.clear();

}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWactNtuMC::~TATWactNtuMC()
{
   delete m_Digitizer;
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
   
   if(m_Digitizer->SetMCtrue()) {  // only for ZID algorithm debug purposes
     fpHisZID_MCtrue = new TH2I("twZID_MCtrue", "twZID_MCtrue", fZbeam+3,-2.5,(int)fZbeam+0.5, fZbeam+2,-1.5,(int)fZbeam+0.5);
     AddHistogram(fpHisZID_MCtrue);

     for(int ilayer=0; ilayer<TATWparCal::kLayers; ilayer++) {
       fpHisElossTof_MCtrue[ilayer] = new TH2D(Form("dE_vs_Tof_layer%d_MCtrue",ilayer),Form("dE_vs_Tof_ilayer%d_MCtrue",ilayer),500,0.,50.,480,0.,120.);
       AddHistogram(fpHisElossTof_MCtrue[ilayer]);
     }
     
     for(int iZ=1; iZ < fZbeam+1; iZ++) {
       fpHisElossTof_MC.push_back(new TH2D(Form("dE_vs_Tof_Z%d_MCtrue",iZ),Form("dE_vs_Tof_%d",iZ),500,0.,50.,480,0.,120.));

       AddHistogram(fpHisElossTof_MC[iZ-1]);

       fpHisDistZ_MC.push_back( new TH1F(Form("distMC_Z_%d",iZ),Form("distMC_Z%d",iZ),2000,-20.,80) );
       AddHistogram(fpHisDistZ_MC[iZ-1]);
       
     }
   }

   
   for(int ilayer=0; ilayer<TATWparCal::kLayers; ilayer++) {
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
   TATWntuRaw* pNtuRaw = (TATWntuRaw*) m_hitContainer->Object();
   
   m_Digitizer = new TATWdigitizer(pNtuRaw);
}

//------------------------------------------+-----------------------------------
//! Action.

bool TATWactNtuMC::Action() {

   if ( fDebugLevel> 0 )     cout << "TATWactNtuMC::Action() start" << endl;

   TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   TATWntuRaw* containerHit = (TATWntuRaw*) m_hitContainer->Object();
   TATWparCal* m_parcal = (TATWparCal*) fpCalPar->Object();
   TAGparGeo*  m_parGeo = (TAGparGeo*) fParGeo->Object();
   TATWparGeo* geoMap = (TATWparGeo*) gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object();

    //The number of hits inside the Start Counter is stn
    if ( fDebugLevel> 0 )     cout << "Processing n Scint " << m_eventStruct->SCNn << endl;

    // clear maps
    m_Digitizer->ClearMap();
    fMapPU.clear();
    //clear vectors
    fVecPuOff.clear();

    if(m_Digitizer->SetPileUpOff())
      cout<<" WARNING!!! Pile Up Off only for ZID algorithm debug purposes...in TATWdigitizer.cxx constructor set fPileUpOff(false)"<<endl;
	     
    // taking the tof for each TW hit with respect to the first hit of ST...for the moment not considered the possibility of multiple Hit in the ST, if any
    Float_t timeST  = m_eventStruct->STCtim[0]*TAGgeoTrafo::SecToPs();
    
    // fill the container of hits, divided by layer, i.e. the column at 0 and row at 1
    for (int i=0; i < m_eventStruct->SCNn; i++) { 
    
       Int_t id      = m_eventStruct->SCNibar[i];
       Int_t trackId = m_eventStruct->SCNid[i] - 1;
       Float_t x0    = m_eventStruct->SCNxin[i];
       Float_t y0    = m_eventStruct->SCNyin[i];
       Float_t z0    = m_eventStruct->SCNzin[i];
       Float_t z1    = m_eventStruct->SCNzout[i];
       Float_t edep  = m_eventStruct->SCNde[i]*TAGgeoTrafo::GevToMev();
       Float_t time  = m_eventStruct->SCNtim[i]*TAGgeoTrafo::SecToPs();
       // get true charge
       Int_t Z       = m_eventStruct->TRcha[trackId];

       Float_t trueTof = (time - timeST)*TAGgeoTrafo::PsToNs();  //ns
       if(fDebugLevel>0)
	 printf("\n timeTW::%f timeST::%f tof::%f\n",time,timeST,trueTof);

       time -= timeST;  // tof TW-SC to be digitized in ps

       // layer, bar, de, time, ntupID, parentID
       int view = m_eventStruct->SCNiview[i];    // in ntuple now layers are 0 and 1
       if ( fDebugLevel> 0 )
          printf("%d %d\n", view,  m_eventStruct->SCNibar[i]);

       TVector3 posIn(x0, y0, z0);
       TVector3 posInLoc = geoTrafo->FromGlobalToTWLocal(posIn);
       
       double truePos = 0;
       if(view==0) //layer 0 (rear) --> vertical bar
	 truePos = posInLoc.Y(); 
       else if(view==1)  //layer 1 (front) --> horizontal bar
	 truePos = posInLoc.X();

       if(fDebugLevel>0)
	 cout<<"trueEloss::"<<edep<<" trueTof::"<<trueTof<<" truePos::"<<truePos<<endl;
              
       if(m_Digitizer->SetMCtrue()) {  // only for ZID algorithm debug purposes

	 Int_t mothId = (m_eventStruct->TRpaid[trackId])-1;
	 
	 Float_t distZ_MC[fZbeam]; //inf
	 
	 Int_t Zrec_MCtrue;	 
	 // test algorithm in MC in a clean situation: only primary fragmentation
	 if( mothId>0) {
	   Zrec_MCtrue=-1.; //set Z to nonsense value
	   if(fDebugLevel > 0) printf("the energy released is %f MeV (tof %.f ns) so Zraw is set to %d\n",edep,trueTof,Zrec_MCtrue);
	 } else
	   Zrec_MCtrue = m_parcal->GetChargeZ(edep,trueTof,view);
	 
	 for(int iZ=1; iZ<fZbeam+1; iZ++)
	   distZ_MC[iZ-1]= m_parcal->GetDistBB(iZ);
	 
	 if (ValidHistogram()) {
	   fpHisZID_MCtrue->Fill(Zrec_MCtrue,Z);
	   
	   fpHisElossTof_MCtrue[view]->Fill(trueTof,edep);
	   
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
	   
	 if(fDebugLevel > 0) {
	   if(Zrec_MCtrue>0 && Z>0) {	   
	     fCnt++;	   
	     if(Zrec_MCtrue!=Z) {
	       cout<<"Hit MC n::"<<fCnt<<endl;
	       printf("layer::%d bar::%d\n", view,  m_eventStruct->SCNibar[i]);
	       fCntWrong++;
	       cout<<"edep::"<<edep<<"  trueTof::"<<trueTof<<endl;
	       cout<<"Zrec::"<<Zrec_MCtrue<<"  Z_MC::"<<Z<<endl;
	       printf("Z wrong/(Zrec>0) :: %d/%d\n",fCntWrong,fCnt);
	     }
	   }
	 }
       }

       
       id+=TATWparGeo::GetLayerOffset()*view;
       
       m_Digitizer->Process(edep, posInLoc[0], posInLoc[1], z0, z1, time, id, Z);
       
       TATWntuHit* hit = m_Digitizer->GetCurrentHit();
       hit->AddMcTrackIdx(trackId, i);

       fMapPU[id] = hit;

       fVecPuOff.push_back(hit);
       
    }


    if( m_Digitizer->SetPileUpOff() ) {


      for(auto it=fVecPuOff.begin(); it !=fVecPuOff.end(); ++it) {

	TATWntuHit* hit = *it;
	
	Double_t recEloss = hit->GetEnergyLoss(); // MeV
	Double_t recTof   = hit->GetTime();       // ns
	Double_t recPos   = hit->GetPosition();   // cm
	
	Int_t Z = hit->GetChargeZ();  // mc true charge stored in the hit up to now
      
	if(fDebugLevel>0)
	  cout<<"recEloss::"<<recEloss<<" recTof::"<<recTof<<" recPos::"<<recPos<<endl;
	
	Int_t Zrec = m_parcal->GetChargeZ(recEloss,recTof,hit->GetLayer());
	hit->SetChargeZ(Zrec);
	
 	Float_t distZ[fZbeam];
	for(int iZ=1; iZ<fZbeam+1; iZ++)
	  distZ[iZ-1]= m_parcal->GetDistBB(iZ);
	
	if(fDebugLevel > 0) {

	  if(!m_Digitizer->SetMCtrue()) {  // only for ZID algorithm debug purposes
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
      
    }  else {
    

      for(auto it=fMapPU.begin(); it !=fMapPU.end(); ++it) {

	TATWntuHit* hit = it->second;
	
	Double_t recEloss = hit->GetEnergyLoss(); // MeV
	Double_t recTof   = hit->GetTime();       // ns
	Double_t recPos   = hit->GetPosition();   // cm
	
	Int_t Z = hit->GetChargeZ();  // mc true charge stored in the hit up to now
	
	if(fDebugLevel>0)
	  cout<<"recEloss::"<<recEloss<<" recTof::"<<recTof<<" recPos::"<<recPos<<endl;
	
	// set an energy threshold --> TODO: to be tuned on data. On GSI data 50mV over all the bars but the central ones. To be set for different energy campaigns
	if(!m_Digitizer->IsOverEnergyThreshold(recEloss)) {
	  if(fDebugLevel > 0)
	    printf("the energy released (%f MeV) is under the set threshold (%.1f MeV)\n",recEloss,m_Digitizer->GetEnergyThreshold());     
	  
	  recEloss=-99.; //set energy to nonsense value
	  hit->SetEnergyLoss(recEloss);	
	}       
	
	Int_t Zrec = m_parcal->GetChargeZ(recEloss,recTof,hit->GetLayer());
	hit->SetChargeZ(Zrec);

	Float_t distZ[fZbeam];
	for(int iZ=1; iZ<fZbeam+1; iZ++)
	  distZ[iZ-1]= m_parcal->GetDistBB(iZ);	  


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

    
    m_hitContainer->SetBit(kValid);

    
    return true;
    
}
