/*!
 \file TATWactNtuHitMC.cxx
 \brief   Implementation of TATWactNtuHitMC.
 */

// #include "TAGparTools.hxx"
// #include "TAGgeoTrafo.hxx"

#include "TASTntuHit.hxx"

// #include "TATWntuRaw.hxx"
#include "TATWparCal.hxx"
#include "TATWactNtuHitMC.hxx"
#include "TAMCflukaParser.hxx"


#include <TCanvas.h>
#include "TATWdigitizer.hxx"

/*!
 \class TATWactNtuHitMC
 \brief NTuplizer for TW MC hits. **
 */

ClassImp(TATWactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWactNtuHitMC::TATWactNtuHitMC(const char* name,
                                 TAGdataDsc* p_ntuTwMC,
                                 TAGdataDsc* p_ntuStMC,
                                 TAGdataDsc* p_ntuStHit,
                                 TAGdataDsc* p_ntuEve,
                                 TAGdataDsc* p_hitraw,
                                 TAGparaDsc* p_parconf,
                                 TAGparaDsc* p_parcal,
                                 TAGparaDsc* p_parGeoG,
                                 EVENT_STRUCT* evStr)
 : TAGaction(name, "TATWactNtuHitMC - NTuplize ToF raw data"),
   fpNtuTwMC(p_ntuTwMC),
   fpNtuStMC(p_ntuStMC),
   fpNtuStHit(p_ntuStHit),
   fpNtuEve(p_ntuEve),
   fpNtuRaw(p_hitraw),
   fpParConf(p_parconf),
   fpCalPar(p_parcal),
   fParGeoG(p_parGeoG),
   fCnt(0),
   fCntWrong(0),
   fEventStruct(evStr)
{
   if (fEventStruct == 0x0) {
      AddDataIn(p_ntuTwMC, "TAMCntuHit");
      AddDataIn(p_ntuStMC, "TAMCntuHit");
      AddDataIn(p_ntuEve, "TAMCntuPart");
   }
   AddDataIn(p_ntuStHit, "TASTntuHit");
   AddDataOut(p_hitraw, "TATWntuHit");
   AddPara(p_parconf,"TATWparConf");
   AddPara(p_parcal,"TATWparCal");
   AddPara(p_parGeoG,"TAGparGeo");
   
   
   f_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   f_pargeo = (TAGparGeo*)fParGeoG->Object();
   f_parcal = (TATWparCal*)fpCalPar->Object();
   
   CreateDigitizer();
   
   TATWparConf* parConf = (TATWparConf*) p_parconf->Object();
   fIsZtrueMC   = parConf->IsZmc();
   fIsZrecPUoff = parConf->IsNoPileUp();
   fIsRateSmear = parConf->IsRateSmearMc();

   if( fIsZtrueMC ) {
      fDigitizer->SetMCtrue();
      fDigitizer->SetPileUpOff();
      Warning("TATWactNtuHitMC"," In FootGlobal.par EnableTWZmc: y --> TW with Z from MC truth and Pile Up Off...if is not the case set EnableTWZmc: n");
   }
   if(!fIsZtrueMC && fIsZrecPUoff) {
      fDigitizer->SetPileUpOff();
      Warning("TATWactNtuHitMC"," In FootGlobal.par EnableTWnoPU: y --> TW with Zrec and Pile Up Off...if is not the case set EnableTWnoPU: n");
   }
   if(fIsRateSmear){
      fDigitizer->SetRateSmearing();
   }

   fZbeam = f_pargeo->GetBeamPar().AtomicNumber;
   
   fMapPU.clear();
   fVecPuOff.clear();
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
   
   fpHisHitCol = new TH1I("twHitCol", "ToF Wall - Column hits", 20, 0., 20);
   AddHistogram(fpHisHitCol);
   
   fpHisHitRow = new TH1I("twHitLine", "ToF Wall - Line hits", 20, 0., 20);
   AddHistogram(fpHisHitRow);
   
   for(int ilayer=0; ilayer<(int)nLayers; ilayer++) {

     fpHisElossTof_MCrec[ilayer] = new TH2D(Form("twdE_vs_Tof_%s_MCrec",LayerName[(TLayer)ilayer].data()),Form("dE_vs_Tof_%s_MCrec",LayerName[(TLayer)ilayer].data()),3000,0.,30.,480,0.,120.);
     AddHistogram(fpHisElossTof_MCrec[ilayer]);

     fpHisElossTof_MCtrue[ilayer] = new TH2D(Form("twdE_vs_Tof_%s_MCtrue",LayerName[(TLayer)ilayer].data()),Form("dE_vs_Tof_%s_MCtrue",LayerName[(TLayer)ilayer].data()),3000,0.,30.,480,0.,120.);
     AddHistogram(fpHisElossTof_MCtrue[ilayer]);

     fpHisZID_MCrec[ilayer] = new TH2I(Form("twZID_%s",LayerName[(TLayer)ilayer].data()),Form("twZID_%s",LayerName[(TLayer)ilayer].data()), fZbeam+3,-2.5,(int)fZbeam+0.5, fZbeam+2,-1.5,(int)fZbeam+0.5);
     AddHistogram(fpHisZID_MCrec[ilayer]);
     
     fpHisZID_MCtrue[ilayer] = new TH2I(Form("twZID_MCtrue_%s",LayerName[(TLayer)ilayer].data()),Form("twZID_MCtrue_%s",LayerName[(TLayer)ilayer].data()), fZbeam+3,-2.5,(int)fZbeam+0.5, fZbeam+2,-1.5,(int)fZbeam+0.5);
     AddHistogram(fpHisZID_MCtrue[ilayer]);
          
   }
      
   for(int iZ=1; iZ < fZbeam+1; iZ++) {

     fpHisElossTof_MC.push_back(new TH2D(Form("twdE_vs_Tof_Z%d_MCtrue",iZ),Form("dE_vs_Tof_%d",iZ),3000,0.,30.,480,0.,120.));
     AddHistogram(fpHisElossTof_MC[iZ-1]);
         
     fpHisElossTof.push_back(new TH2D(Form("twdE_vs_Tof_Z%d",iZ),Form("dE_vs_Tof_%d",iZ),3000,0.,30.,480,0.,120.));
     AddHistogram(fpHisElossTof[iZ-1]);
        
     fpHisDistZ.push_back( new TH1F(Form("twDist_Z%d",iZ),Form("dist_Z%d",iZ),2000,-20.,80) );
     AddHistogram(fpHisDistZ[iZ-1]);
        
     fpHisDistZ_MC.push_back( new TH1F(Form("twDistMC_Z_%d",iZ),Form("distMC_Z%d",iZ),2000,-20.,80) );
     AddHistogram(fpHisDistZ_MC[iZ-1]);
         
     fpHisResPos.push_back( new TH1D(Form("twResPos_Z_%d",iZ), Form("ResPos_Z_%d",iZ), 400,-20,20) );
     AddHistogram(fpHisResPos[iZ-1]);
        
     fpHisResTof.push_back( new TH1D(Form("twResTof_Z_%d",iZ), Form("ResTof_Z_%d",iZ), 2000, -1., 1.) );
     AddHistogram(fpHisResTof[iZ-1]);
        
     fpHisResEloss.push_back( new TH1D(Form("twResEloss_Z_%d",iZ), Form("ResEloss_Z_%d",iZ), 2000, -10., 10.) );
     AddHistogram(fpHisResEloss[iZ-1]);
        
   }
   
   if(fDigitizer->GetRate()) {
     fpHisRate = (TH1D*)(f_parcal->GetRate()->Clone("hisRate"));
     AddHistogram(fpHisRate);
   }
      
      
   SetValidHistogram(kTRUE);

   return;
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TATWactNtuHitMC::CreateDigitizer()
{
   TATWntuHit* pNtuRaw = (TATWntuHit*) fpNtuRaw->Object();
   
   fDigitizer = new TATWdigitizer(pNtuRaw, fParGeoG , fpCalPar);
}

//------------------------------------------+-----------------------------------
//! Action.
bool TATWactNtuHitMC::Action() {
   
   if(FootDebugLevel(1))
      cout << "TATWactNtuHitMC::Action() start" << endl;
   
   ClearContainers();
   
   TAMCntuHit*  pNtuTwMC = 0x0;
   TAMCntuHit*  pNtuStMC = 0x0;
   TAMCntuPart* pNtuEve  = 0x0;
   
   if (fEventStruct == 0x0) {
      pNtuTwMC  = (TAMCntuHit*) fpNtuTwMC->Object();
      pNtuStMC  = (TAMCntuHit*) fpNtuStMC->Object();
      pNtuEve   = (TAMCntuPart*) fpNtuEve->Object();
   } else {
      pNtuTwMC   = TAMCflukaParser::GetTwHits(fEventStruct, fpNtuTwMC);
      pNtuStMC   = TAMCflukaParser::GetStcHits(fEventStruct, fpNtuStMC);
      pNtuEve    = TAMCflukaParser::GetTracks(fEventStruct, fpNtuEve);
   }

   // fill the container of hits, divided by layer, i.e. the column at 0 (LayerY) and row at 1 (LayerX)
   if(FootDebugLevel(4))
     Info("Action","Processing %d TW hits", pNtuTwMC->GetHitsN());
   
   for (Int_t idTWhit = 0; idTWhit < pNtuTwMC->GetHitsN(); idTWhit++) {

       TAMChit* hitTwMC = pNtuTwMC->GetHit(idTWhit);
       if(!hitTwMC) continue;
       
       Int_t layer   = hitTwMC->GetView();    // layers 0 (y-bars) and 1 (x-bars)
       Int_t barId   = hitTwMC->GetBarId();
       Int_t trackId = hitTwMC->GetTrackIdx()-1;
       
       Double_t edep  = hitTwMC->GetDeltaE()*TAGgeoTrafo::GevToMev();
       Double_t timeTW  = hitTwMC->GetTof()*TAGgeoTrafo::SecToPs();
       
       Int_t Z = GetZmcTrue(pNtuEve,trackId);
       
       if(FootDebugLevel(4))
         PrintTrueMcTWquantities(hitTwMC,idTWhit);

       // if(fDigitizer->IsMCtrue())
       StudyPerformancesZID(hitTwMC,pNtuStMC,pNtuEve);   // only for ZID algorithm debug purposes       
       
       // if bar is dead in data skip it
       if(!f_parcal->IsTWbarActive(layer,barId)) continue;
       
       // barId+=TATWparGeo::GetLayerOffset()*layer;  //offset for the horizontal bars Id              
       Int_t barIdAll = barId + layer * nBarsPerLayer; // baridAll: TW bar ID from 0 to 39 (0-19 for LayerY and 20-39 for LayerX)
   
       fDigitizer->Process(edep, GetTruePosAlongBar(hitTwMC), (Double_t)layer, (Double_t)barId, (Double_t)idTWhit, timeTW, barIdAll, Z);

     
       TATWhit* hitTW = fDigitizer->GetCurrentHit();
       
       hitTW->AddMcTrackIdx(trackId, idTWhit);
       
       fMapPU[barIdAll] = hitTW;
       
       fVecPuOff.push_back(hitTW);
       
   }

   
   // now assign the charge Z and the Tof to the TWhit and check reconstruction with some plots   
   if( fDigitizer->IsPileUpOff() ) {
     
     
     for(auto it=fVecPuOff.begin(); it !=fVecPuOff.end(); ++it) {
       
       TATWhit* hitTW = *it;

       AssignZchargeAndToF(hitTW,pNtuStMC);

     }

   } else {  // full MC reconstructed (pile-up included)

     for(auto it=fMapPU.begin(); it !=fMapPU.end(); ++it) {
         
       TATWhit* hitTW = it->second;
       
       AssignZchargeAndToF(hitTW,pNtuStMC);
       
     }
   }
   
   if (fEventStruct != 0x0) {
      fpNtuTwMC->SetBit(kValid);
      fpNtuStMC->SetBit(kValid);
      fpNtuEve->SetBit(kValid);
   }
   fpNtuRaw->SetBit(kValid);
   
   return true;
}

//------------------------------------------+-----------------------------------

void TATWactNtuHitMC::AssignZchargeAndToF(TATWhit *hitTW, TAMCntuHit *ntuHitStMC) {

     FlagUnderEnergyThresholtHits(hitTW);

     // set ToF in ns to the TW hit retrieving SC hit time information
     Double_t recTof   = hitTW->GetTime() - GetTimeST(fpNtuStHit,hitTW); // ns
     hitTW->SetToF(recTof);
     
     Int_t Z = hitTW->GetChargeZ();  // mc true charge stored in the hit up to now
     
     Int_t Zrec = -2;
     if(!fIsZtrueMC) {
       Zrec = f_parcal->GetChargeZ(hitTW->GetEnergyLoss(),hitTW->GetToF(),hitTW->GetLayer());
       hitTW->SetChargeZ(Zrec);
     }
     
     PlotRecMcTWquantities(hitTW,ntuHitStMC,Zrec,Z);
     
     if(FootDebugLevel(4))
       PrintRecTWquantities(hitTW,Zrec,Z);

     return;
}
//------------------------------------------+-----------------------------------// retrieve the Start Counter info to assign the ToF to the TW hit
Double_t TATWactNtuHitMC::GetTimeST(TAGdataDsc *g_NtuStHit, TATWhit *hitTW)
{

     Double_t timeST=-99;  // non-sense value
     TASTntuHit* pNtuStHit = (TASTntuHit*) g_NtuStHit->Object();
     // The number of reco hits inside the Start Counter has to be 1 !!
     if(pNtuStHit->GetHitsN()<1) {
       if(FootDebugLevel(4))
         Warning("GetTimeST","N of reco hits in ST is %d...set TW hit NOT Valid",pNtuStHit->GetHitsN());

       hitTW->SetValid(false);
       return timeST;
     }
     
     TASThit* hitST = pNtuStHit->GetHit(0);
     timeST  = hitST->GetTime();  // time stored in the Hit is in ns
   
     if(!hitST->IsValid() || timeST<0)
       hitTW->SetValid(false);

     return timeST;
}
   
//------------------------------------------+-----------------------------------
void TATWactNtuHitMC::StudyPerformancesZID(TAMChit *hitTwMC, TAMCntuHit *ntuHitStMC, TAMCntuPart *ntuPart) {

   // only for ZID algorithm debug purposes: test algorithm in MC in a clean situation: only primary fragmentation inside TG
   Int_t trackId = hitTwMC->GetTrackIdx()-1;
   Int_t layer  =  hitTwMC->GetView();    // layers 0 (y-bars) and 1 (x-bars)
   Float_t edep =  hitTwMC->GetDeltaE()*TAGgeoTrafo::GevToMev(); //MeV

   TAMCpart*  track = ntuPart->GetTrack(trackId);
   if(!track)  return;

   Int_t  Zmc = track->GetCharge();
   Int_t mothId = track->GetMotherID();

   // true Tof between true TW hit and first SC hit (It's ok, only mothId=0 tracks are considered below)
   TAMChit* hitStMC = ntuHitStMC->GetHit(0);
   if(!hitStMC) return;
   
   Double_t trueTof = (hitTwMC->GetTof() - hitStMC->GetTof())*TAGgeoTrafo::SecToNs(); //ns
   
   Int_t Zrec = f_parcal->GetChargeZ(edep,trueTof,layer);   
   Float_t distZ_MC[fZbeam]; //inf
  
   for(int iZ=1; iZ<fZbeam+1; iZ++)
     distZ_MC[iZ-1]= f_parcal->GetDistBB(iZ);
  
   if (ValidHistogram()) {
     
   if(FootDebugLevel(4))
     cout<<"reg_trk::"<<track->GetRegion()<<"  tg_reg::"<<f_pargeo->GetRegTarget()<<endl;

     if(mothId==0 && track->GetRegion()==f_pargeo->GetRegTarget()) {

       fpHisZID_MCtrue[layer]->Fill(Zrec,Zmc);
       
       fpHisElossTof_MCtrue[layer]->Fill(trueTof,edep);
     
       if( Zrec>0 && Zrec < fZbeam+1 )
         fpHisElossTof_MC[Zrec-1]->Fill(trueTof,edep);
    
       for(int iZ=1; iZ < fZbeam+1; iZ++) {
      
         if(iZ==1) {
           if(Zrec==1)
             fpHisDistZ_MC[iZ-1]->Fill(distZ_MC[iZ-1]);
           else
             fpHisDistZ_MC[iZ-1]->Fill(std::numeric_limits<float>::max());
         }
         else
           fpHisDistZ_MC[iZ-1]->Fill(distZ_MC[iZ-1]);
       }
     }
   }
  
   if(FootDebugLevel(4)) {
     if(Zrec>0 && Zmc>0) {
       fCnt++;
       if(Zrec!=Zmc) {
         cout<<"Hit MC n::"<<fCnt<<endl;
         printf("%s bar::%d\n", LayerName[(TLayer)layer].data(),  hitTwMC->GetBarId());
         fCntWrong++;
         cout<<"edep::"<<edep<<"  trueTof::"<<trueTof<<endl;
         cout<<"timeTW::"<<hitTwMC->GetTof()*TAGgeoTrafo::SecToNs()<<" timeST::"<<hitStMC->GetTof()*TAGgeoTrafo::SecToNs()<<endl;
         cout<<"Zrec::"<<Zrec<<"  Z_MC::"<<Zmc<<endl;
         printf("Z wrong/(Zrec>0) :: %d/%d\n",fCntWrong,fCnt);
       }
     }
   }
   
   return;
}
//---------------------------------------------------------------------------
void TATWactNtuHitMC::ClearContainers() {

  // clear maps
   fDigitizer->ClearMap();
   fMapPU.clear();
   
   //clear vectors
   fVecPuOff.clear();

   return;
}
//---------------------------------------------------------------------------
Int_t TATWactNtuHitMC::GetZmcTrue(TAMCntuPart *mcpart, Int_t trkId)
{
     // get true charge
     TAMCpart*  track = mcpart->GetTrack(trkId);
     Int_t  Z = track->GetCharge();

     if(FootDebugLevel(4)) {
         
       Int_t  reg = track->GetRegion();
       Double_t  posz_in = (track->GetInitPos()).Z();
       Double_t  posz_fin = (track->GetFinalPos()).Z();
       TVector3  pin = track->GetInitP();
       Double_t  mass = track->GetMass();
       
       cout<<endl<<"trID::"<<trkId<<" mothId::"<<track->GetMotherID()<<" Z::"<<Z<<" reg::"<<reg<<" posz_in::"<<posz_in<<" posz_fin::"<<posz_fin<<" ekin::"<<sqrt((double)pin.Mag2()+mass*mass)-mass<<endl;
       
     }

     return Z;   
}
//------------------------------------------------------------------------------
void TATWactNtuHitMC::PrintTrueMcTWquantities(TAMChit *hitTwMC, Int_t IdTwHit) {

       
       Int_t layer   = hitTwMC->GetView();    // layers 0 (y-bars) and 1 (x-bars)
       Int_t barId   = hitTwMC->GetBarId();
       Int_t trackId = hitTwMC->GetTrackIdx()-1;

       Double_t edep  = hitTwMC->GetDeltaE()*TAGgeoTrafo::GevToMev();
       Double_t timeTW  = hitTwMC->GetTof()*TAGgeoTrafo::SecToNs();
       
       printf(" trID::%d IdTwHit::%d\n %s barID::%d\n", trackId, IdTwHit, LayerName[(TLayer)layer].data(), barId);      
       cout<<" MCtrue timeTW:: "<<timeTW<<" eloss:: "<<edep<<" pos:: "<<GetTruePosAlongBar(hitTwMC)<<endl<<endl;

       return;       

}
//------------------------------------------------------------------------------
void TATWactNtuHitMC::FlagUnderEnergyThresholtHits(TATWhit *hitTW) {

     Int_t layer = hitTW->GetLayer();
     Int_t bar = hitTW->GetBar();
       
     Double_t recEloss = hitTW->GetEnergyLoss();    // MeV

     // get Eloss threshold in MeV from configuration file: default is 0.1 MeV
     Double_t Ethr = f_parcal->GetElossThreshold(layer,bar);       
     if(!fDigitizer->IsOverEnergyThreshold(Ethr,recEloss)) {
       if(FootDebugLevel(4))
         Info("Action","the energy released (%f MeV) is under the set threshold (%.1f MeV)\n",recEloss,fDigitizer->GetEnergyThreshold());
       
       recEloss=-99.; //set energy to nonsense value
       hitTW->SetEnergyLoss(recEloss);
       hitTW->SetValid(false);
     }
     return;
}
//------------------------------------------------------------------------------
void TATWactNtuHitMC::PlotRecMcTWquantities(TATWhit *hitTW, TAMCntuHit *ntuHitStMC, Int_t Zrec, Int_t Zmc) {

       if(!hitTW->IsValid()) {
         if(FootDebugLevel(4))
           Info("PlotRecMcTWquantities","skip not valid TW hits");
         return;
       }
  
       Int_t layer = hitTW->GetLayer();
       
       Double_t recTof = hitTW->GetToF();
       Double_t recEloss = hitTW->GetEnergyLoss();


       if (ValidHistogram()) {

            if (hitTW->IsColumn())
               fpHisHitCol->Fill(hitTW->GetBar());
            else
               fpHisHitRow->Fill(hitTW->GetBar());
            
            fpHisElossTof_MCrec[layer]->Fill(recTof,recEloss);
            fpHisZID_MCrec[layer]->Fill(Zrec,Zmc);
            
            Float_t distZ[fZbeam];
            for(int iZ=1; iZ<fZbeam+1; iZ++) {
              distZ[iZ-1]= f_parcal->GetDistBB(iZ);         
              fpHisDistZ[iZ-1]->Fill(distZ[iZ-1]);
            }
            
            //selecting the chargeZ as rec or true depending on what is stored in the TW hit
            if(hitTW->GetChargeZ()>0 && hitTW->GetChargeZ()<fZbeam+1) {
              fpHisElossTof[hitTW->GetChargeZ()-1]->Fill(recTof,recEloss);

              Double_t recPosAlongBar = hitTW->GetPosition();
              Double_t truePosAlongBar = hitTW->GetChargeChB(); // in MC in GetChargeB stored the true position along the bar

              fpHisResPos[hitTW->GetChargeZ()-1]->Fill(recPosAlongBar-truePosAlongBar);
              
              Double_t trueEloss = hitTW->GetAmplitudeChA();   // in MC in GetAmplitudeA stored the true Eloss
              fpHisResEloss[hitTW->GetChargeZ()-1]->Fill(recEloss-trueEloss);

              // true Tof between true TW hit and first SC hit (It's wrong only in the case of fragmentation before SC or with delta-rays activation in MC)
              TAMChit* hitStMC = ntuHitStMC->GetHit(0);
              if(!hitStMC) return;
              Double_t trueTWtime = hitTW->GetAmplitudeChB();  // in MC in GetAmplitudeB stored the true TW time
              Double_t trueTof = trueTWtime*TAGgeoTrafo::PsToNs()  - hitStMC->GetTof()*TAGgeoTrafo::SecToNs(); //ns
              fpHisResTof[hitTW->GetChargeZ()-1]->Fill(recTof-trueTof);

            }
       }
       
       return;
}
//------------------------------------------------------------------------------
void TATWactNtuHitMC::PrintRecTWquantities(TATWhit *hit, Int_t Zrec, Int_t Z) {

     cout<<" "<<LayerName[(TLayer)hit->GetLayer()].data()<<" barID::"<<hit->GetBar()<<endl;
     cout<<" Rec timeTW:: "<<hit->GetTime()<<" eloss:: "<<hit->GetEnergyLoss()<<" pos:: "<<hit->GetPosition()<<endl<<endl;


     cout<<"set Z::"<<hit->GetChargeZ()<<"  Ztrue::"<<Z<<"  Zrec::"<<Zrec<<endl;
       
       
     if( fDigitizer->IsPileUpOff() ) {   // only for ZID algorithm debug purposes
       if(Zrec>0 && Z>0) {
         fCnt++;
         if(Zrec!=Z) {
           cout<<"Hit n::"<<fCnt<<endl;
           fCntWrong++;
           cout<<"edep::"<<hit->GetEnergyLoss()<<"  tof::"<<hit->GetToF()<<endl;
           cout<<"Zrec::"<<Zrec<<"  Z_MC::"<<Z<<endl;
           printf("Z wrong/(Zrec>0) :: %d/%d\n",fCntWrong,fCnt);
         }
       }
     }       

     return;       
}
//-----------------------------------------------------------------------------
Double_t TATWactNtuHitMC::GetTruePosAlongBar(TAMChit *hitTwMC) {

       Int_t layer   = hitTwMC->GetView();    // layers 0 (y-bars) and 1 (x-bars)
       TVector3 posIn(hitTwMC->GetInPosition());
       TVector3 posInLoc = f_geoTrafo->FromGlobalToTWLocal(posIn);

       // store the position along the bar
       double posAlongBar = 0;
       if( layer == (Int_t)LayerY )  // layer 0 --> vertical bar
         posAlongBar = posInLoc.Y();
       else if( layer == (Int_t)LayerX )  // layer 1 --> horizontal bar
         posAlongBar = posInLoc.X();      

       return posAlongBar;
}

