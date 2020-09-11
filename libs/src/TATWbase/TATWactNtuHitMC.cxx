/*!
 \file
 \version $Id: TATWactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TATWactNtuHitMC.
 */
#include "TAGgeoTrafo.hxx"

#include "TATWdatRaw.hxx"
#include "TATWparCal.hxx"
#include "TATWactNtuHitMC.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TATWdigitizer.hxx"

/*!
 \class TATWactNtuHitMC TATWactNtuHitMC.hxx "TATWactNtuHitMC.hxx"
 \brief NTuplizer for BM raw hits. **
 */

ClassImp(TATWactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWactNtuHitMC::TATWactNtuHitMC(const char* name,
                                 TAGdataDsc* p_ntuMC,
                                 TAGdataDsc* p_ntuStMC,
                                 TAGdataDsc* p_ntuEve,
                                 TAGdataDsc* p_hitraw,
                                 TAGparaDsc* p_parcal,
                                 TAGparaDsc* p_parGeoG,
				 Bool_t isZmc)
 : TAGaction(name, "TATWactNtuHitMC - NTuplize ToF raw data"),
   fpNtuMC(p_ntuMC),
   fpNtuStMC(p_ntuStMC),
   fpNtuEve(p_ntuEve),
   fpNtuRaw(p_hitraw),
   fpCalPar(p_parcal),
   fParGeoG(p_parGeoG),
   fCnt(0),
   fCntWrong(0)
{
   AddDataIn(p_ntuMC, "TAMCntuHit");
   AddDataIn(p_ntuStMC, "TAMCntuHit");
   AddDataIn(p_ntuEve, "TAMCntuEve");
   
   AddDataOut(p_hitraw, "TATWntuRaw");
   AddPara(p_parcal,"TATWparCal");
   AddPara(p_parGeoG,"TAGparGeo");
   
   CreateDigitizer();

   if( fIsZtrueMC ) {
     
     fDigitizer->SetMCtrue();
     fDigitizer->SetPileUpOff();
     
   }

   if(fDigitizer->IsPileUpOff())
     Warning("TATWactNtuMC","Pile Up Off only for gloabal and ZID algorithm debug purposes...if is not the case remove the flag -zmc or check in TATWdigitizer.cxx constructor and set fPileUpOff(false)");
   
   
   f_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   
   f_pargeo = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();
   
   // TAGparGeo* pGeoMap  = (TAGparGeo*) fParGeoG->Object();
   // fZbeam = pGeoMap->GetBeamPar().AtomicNumber;
   fZbeam = f_pargeo->GetBeamPar().AtomicNumber;
   
   fMapPU.clear();
   fVecPuOff.clear();

   f_parcal = (TATWparCal*)fpCalPar->Object();

}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWactNtuHitMC::~TATWactNtuHitMC()
{
   delete fDigitizer;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TATWactNtuHitMC::CreateHistogram()
{
   
   DeleteHistogram();
   
   fpHisHitCol = new TH1F("twHitCol", "ToF Wall - Column hits", 22, 0., 22);
   AddHistogram(fpHisHitCol);
   
   fpHisHitLine = new TH1F("twHitLine", "ToF Wall - Line hits", 22, 0., 22);
   AddHistogram(fpHisHitLine);
   
   fpHisHitMap = new TH1F("twHitMap", "ToF Wall - Hit Map", 1000,-50,50);
   AddHistogram(fpHisHitMap);
   
   fpHisRecPos = new TH1F("twRecPos", "RecPos", 1000,-50,50);
   AddHistogram(fpHisRecPos);
   
   fpHisRecPosMc = new TH1F("twTruePos", "TruePos", 1000,-50,50);
   AddHistogram(fpHisRecPosMc);
   
   fpHisRecTof = new TH1F("twRecTof", "RecTof", 500, 0., 50.);
   AddHistogram(fpHisRecTof);
   
   fpHisRecTofMc = new TH1F("twTrueTof", "TrueTof", 500, 0., 50.);
   AddHistogram(fpHisRecTofMc);
   
   fpHisZID = new TH2I("twZID", "twZID", fZbeam+3,-2.5,(int)fZbeam+0.5, fZbeam+2,-1.5,(int)fZbeam+0.5);
   AddHistogram(fpHisZID);
   
   if(fDigitizer->IsMCtrue()) {  // only for ZID algorithm debug purposes
      fpHisZID_MCtrue = new TH2I("twZID_MCtrue", "twZID_MCtrue", fZbeam+3,-2.5,(int)fZbeam+0.5, fZbeam+2,-1.5,(int)fZbeam+0.5);
      AddHistogram(fpHisZID_MCtrue);
      
      for(int ilayer=0; ilayer<(int)nLayers; ilayer++) {
         fpHisElossTof_MCtrue[ilayer] = new TH2D(Form("twdE_vs_Tof_layer%d_MCtrue",ilayer),Form("dE_vs_Tof_ilayer%d_MCtrue",ilayer),500,0.,50.,480,0.,120.);
         AddHistogram(fpHisElossTof_MCtrue[ilayer]);
      }
      
      for(int iZ=1; iZ < fZbeam+1; iZ++) {
         fpHisElossTof_MC.push_back(new TH2D(Form("twdE_vs_Tof_Z%d_MCtrue",iZ),Form("dE_vs_Tof_%d",iZ),500,0.,50.,480,0.,120.));
         
         AddHistogram(fpHisElossTof_MC[iZ-1]);
         
         fpHisDistZ_MC.push_back( new TH1F(Form("twDistMC_Z_%d",iZ),Form("distMC_Z%d",iZ),2000,-20.,80) );
         AddHistogram(fpHisDistZ_MC[iZ-1]);
         
	 fpHisResPos.push_back( new TH1D(Form("twResPos_Z_%d",iZ), Form("ResPos_Z_%d",iZ), 1000,-50,50) );
	 AddHistogram(fpHisResPos[iZ-1]);
	 
      }
   }
   
   
   for(int ilayer=0; ilayer<nLayers; ilayer++) {
      fpHisElossTof_MCrec[ilayer] = new TH2D(Form("twdE_vs_Tof_layer%d",ilayer),Form("dE_vs_Tof_ilayer%d",ilayer),500,0.,50.,480,0.,120.);
      AddHistogram(fpHisElossTof_MCrec[ilayer]);
   }
   
   for(int iZ=1; iZ < fZbeam+1; iZ++) {
      
      fpHisElossTof.push_back(new TH2D(Form("twdE_vs_Tof_Z%d",iZ),Form("dE_vs_Tof_%d",iZ),500,0.,50.,480,0.,120.));
      AddHistogram(fpHisElossTof[iZ-1]);
      
      fpHisDistZ.push_back( new TH1F(Form("twDist_Z%d",iZ),Form("dist_Z%d",iZ),2000,-20.,80) );
      AddHistogram(fpHisDistZ[iZ-1]);
      
   }
   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TATWactNtuHitMC::CreateDigitizer()
{
   TATWntuRaw* pNtuRaw = (TATWntuRaw*) fpNtuRaw->Object();
   
   fDigitizer = new TATWdigitizer(pNtuRaw);
}

//------------------------------------------+-----------------------------------
//! Action.

bool TATWactNtuHitMC::Action() {
   
   if(FootDebugLevel(1))
     cout << "TATWactNtuHitMC::Action() start" << endl;
   
   TAMCntuHit* pNtuMC   = (TAMCntuHit*) fpNtuMC->Object();
   TAMCntuHit* pNtuStMC = (TAMCntuHit*) fpNtuStMC->Object();

   //The number of hits inside the Start Counter is stn
   if(FootDebugLevel(1))
     cout << "Processing n Scint " << pNtuMC->GetHitsN() << endl;
   
   // clear maps
   fDigitizer->ClearMap();
   fMapPU.clear();
   //clear vectors
   fVecPuOff.clear();
   
   // taking the tof for each TW hit with respect to the first hit of ST...for the moment not considered the possibility of multiple Hit in the ST, if any
   TAMChit* hitStMC = pNtuStMC->GetHit(0);
   Float_t  timeST  = hitStMC->GetTof()*TAGgeoTrafo::SecToPs();
   
   // fill the container of hits, divided by layer, i.e. the column at 0 and row at 1
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hitMC = pNtuMC->GetHit(i);
      
      TVector3 posIn(hitMC->GetInPosition());
      TVector3 posOut(hitMC->GetOutPosition());
      
      Int_t layer   = hitMC->GetLayer();    // layers 0 (y-bars) and 1 (x-bars)
      Int_t barId   = hitMC->GetBarId();
      Int_t trackId = hitMC->GetTrackIdx()-1;
      Float_t z0    = posIn.Z();
      Float_t z1    = posOut.Z();
      Float_t edep  = hitMC->GetDeltaE()*TAGgeoTrafo::GevToMev();
      Float_t time  = hitMC->GetTof()*TAGgeoTrafo::SecToPs();
      // get true charge
      TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();
      TAMCeveTrack*  track = pNtuEve->GetTrack(trackId);
      Int_t  Z = track->GetCharge();
      
      Float_t trueTof = (time - timeST)*TAGgeoTrafo::PsToNs();  //ns
      if(FootDebugLevel(1))
         printf("\n timeTW::%f timeST::%f tof::%f\n",time,timeST,trueTof);
      
      time -= timeST;  // tof TW-SC to be digitized in ps
      
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
      
      if(FootDebugLevel(1))
         cout<<"trueEloss::"<<edep<<" trueTof::"<<trueTof<<" truePos::"<<truePos<<endl;
      
      if(fDigitizer->IsMCtrue()) {  // only for ZID algorithm debug purposes
         
         TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();
         TAMCeveTrack*  track = pNtuEve->GetTrack(trackId);
         Int_t mothId = track->GetMotherID()-1;
         
         Float_t distZ_MC[fZbeam]; //inf
         
         Int_t Zrec_MCtrue;
         // test algorithm in MC in a clean situation: only primary fragmentation
         if( mothId>0) {
            Zrec_MCtrue=-1.; //set Z to nonsense value
            if(FootDebugLevel(1)) printf("the energy released is %f MeV (tof %.f ns) so Zraw is set to %d\n",edep,trueTof,Zrec_MCtrue);
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
         
         if(FootDebugLevel(1)) {
            if(Zrec_MCtrue>0 && Z>0) {
               fCnt++;
               if(Zrec_MCtrue!=Z) {
                  cout<<"Hit MC n::"<<fCnt<<endl;
                  printf("layer::%d bar::%d\n", layer,  hitMC->GetBarId());
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

      if(hit->GetChargeZ()>0 && hit->GetChargeZ()<fZbeam+1)
	fpHisResPos[hit->GetChargeZ()-1]->Fill(hit->GetPosition()-truePos);

      
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
         
         if(FootDebugLevel(1))
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
         
         if(FootDebugLevel(1)) {
            
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
      
   }  else {
      
      
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
         
         if(FootDebugLevel(1))
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
   
   
   fpNtuRaw->SetBit(kValid);
   
   
   return true;
   
}
