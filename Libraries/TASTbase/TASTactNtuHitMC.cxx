/*!
  \file TASTactNtuHitMC.cxx
  \brief   Implementation of TASTactNtuHitMC.
*/

#include "TAMCflukaParser.hxx"

#include "TASTdigitizer.hxx"

#include "TASTactNtuHitMC.hxx"

/*!
  \class TASTactNtuHitMC
  \brief NTuplizer for ST MC hits. **
*/

//! Class Imp
ClassImp(TASTactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTactNtuHitMC::TASTactNtuHitMC(const char* name,
                                 TAGdataDsc* pNtuMC,
                                 TAGdataDsc* pNtuEve,
                                 TAGdataDsc* pNtuHit,
                                 TAGparaDsc* p_parconf,
                                 EVENT_STRUCT* evStr)  
 : TAGaction(name, "TASTactNtuHitMC - NTuplize ST raw data"),
   fpNtuMC(pNtuMC),
   fpNtuEve(pNtuEve),
   fpNtuHit(pNtuHit),
   fpParConf(p_parconf),
   fEventStruct(evStr)
{
   if(FootDebugLevel(1))
      Info("Action()"," Creating the Start Counter MC tuplizer action\n");
  
   if(fEventStruct == 0x0) {
     AddDataIn(pNtuMC, "TAMCntuHit");
     AddDataIn(pNtuEve, "TAMCntuPart");
   } 
   AddDataOut(pNtuHit, "TASTntuHit");
   AddPara(p_parconf,"TATWparConf");

   fparTwConf  = (TATWparConf*) fpParConf->Object();
   fIsEnergyThrEnabled = fparTwConf->IsEnergyThrEnabled();
   if(FootDebugLevel(4))
     Info("TASTactNtuHitMC()","IsEnergyThresholdEnable::%d",fIsEnergyThrEnabled);

   CreateDigitizer();

   fVecStHit.clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TASTactNtuHitMC::~TASTactNtuHitMC()
{
   delete fDigitizer;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TASTactNtuHitMC::CreateHistogram()
{
   DeleteHistogram();

   fpHisElossTime_MCrec = new TH2D("stdE_vs_Time_MCrec","dE_vs_Time_MCrec",10000,-1.,9.,100,0.,10.);
   AddHistogram(fpHisElossTime_MCrec);

   fpHisElossTime_MCtrue = new TH2D("stdE_vs_Time_MCtrue","dE_vs_Time_MCtrue",10000,-1.,9.,100,0.,10.);
   AddHistogram(fpHisElossTime_MCtrue);
   
   fpHisResTime = new TH1D("stResTime","ResTime", 2000, -1., 1.);
   AddHistogram(fpHisResTime);

   SetValidHistogram(kTRUE);
      
   return;        
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TASTactNtuHitMC::CreateDigitizer()
{
   TASTntuHit* p_ntuHit = (TASTntuHit*) fpNtuHit->Object();
   
   fDigitizer = new TASTdigitizer(p_ntuHit);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TASTactNtuHitMC::Action()
{

  fVecStHit.clear();

  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  TASTntuHit* pNtuHit   = (TASTntuHit*) fpNtuHit->Object();
  
  TAMCntuHit* pNtuMC    = 0;
  
  if (fEventStruct == 0x0)
    pNtuMC    = (TAMCntuHit*) fpNtuMC->Object();
   else
    pNtuMC    = TAMCflukaParser::GetStcHits(fEventStruct, fpNtuMC);
  
  //The number of hits inside the Start Counter is stn
  if(FootDebugLevel(1))
      Info("Action()","Processing n :: %d hits", pNtuMC->GetHitsN());
    
  Double_t edep, trigtime;
   
   fDigitizer->ClearMap();
   for (Int_t idSThit = 0; idSThit < pNtuMC->GetHitsN(); idSThit++) {

      TAMChit* hitMC = pNtuMC->GetHit(idSThit);
      if(!hitMC) continue;
      
      TVector3 posIn(hitMC->GetInPosition());
      TVector3 posInLoc = geoTrafo->FromGlobalToSTLocal(posIn);

      TVector3 posOut(hitMC->GetOutPosition());

      Int_t id      = hitMC->GetSensorId();
      Int_t trackId = hitMC->GetTrackIdx()-1;
      Double_t z0    = posIn.Z();
      Double_t z1    = posOut.Z();
      edep  = hitMC->GetDeltaE()*TAGgeoTrafo::GevToMev();
      Double_t time  = hitMC->GetTof()*TAGgeoTrafo::SecToPs();      

     // don't use z for the moment
     fDigitizer->Process(edep, posInLoc[0], posInLoc[1], z0, z1, time, id);


     TASThit* hitST = fDigitizer->GetCurrentHit();

     trigtime = hitST->GetTime();

     hitST->AddMcTrackIdx(trackId, idSThit);

     fVecStHit.push_back(hitST);

   }

     for(auto it=fVecStHit.begin(); it !=fVecStHit.end(); ++it) {
       
       TASThit* hitST = *it;

       if(fIsEnergyThrEnabled)
         FlagUnderEnergyThresholdHits(hitST);

       PlotSCquantities(hitST);
       
     }
   
   pNtuHit->SetTriggerTime(trigtime);
   
   if (fEventStruct != 0x0) {
     fpNtuMC->SetBit(kValid);
     fpNtuEve->SetBit(kValid);
   }
   
   fpNtuHit->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------------------------------------------
void TASTactNtuHitMC::FlagUnderEnergyThresholdHits(TASThit *hitST) {

     Double_t edep = hitST->GetDe();    // MeV

     if(!fDigitizer->IsOverEnergyThreshold(fDigitizer->GetEnergyThreshold(),edep)) {
       if(FootDebugLevel(4))
         Info("Action","the energy released (%f MeV) is under the set threshold (%.1f MeV)\n",edep,fDigitizer->GetEnergyThreshold());
       
       edep=-99.; //set energy to nonsense value
       hitST->SetDe(edep);
       hitST->SetValid(false);
     }
     
     return;
}
//------------------------------------------------------------------------------
void TASTactNtuHitMC::PlotSCquantities(TASThit *hitSt) {

  if (ValidHistogram()) {

    Double_t recTime  = hitSt->GetTime();
    Double_t trueTime  = hitSt->GetCharge(); // in GetCharge stored the true time (only in MC)
    Double_t edep  = hitSt->GetDe();  // get true MC eloss
    
    fpHisElossTime_MCtrue->Fill(trueTime,edep);
    fpHisElossTime_MCrec->Fill(recTime,edep);
    fpHisResTime->Fill(recTime-trueTime);

  }
  
  return;
}
