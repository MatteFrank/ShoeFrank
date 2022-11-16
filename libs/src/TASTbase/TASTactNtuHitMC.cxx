/*!
  \file TASTactNtuHitMC.cxx
  \brief   Implementation of TASTactNtuHitMC.
*/

#include "TAGrecoManager.hxx"
#include "TAGroot.hxx"
#include "TASTntuHit.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCflukaParser.hxx"
#include "TASTdigitizer.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

#include "TASTactNtuHitMC.hxx"

/*!
  \class TASTactNtuHitMC
  \brief NTuplizer for ST MC hits. **
*/

//! Class Imp
ClassImp(TASTactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTactNtuHitMC::TASTactNtuHitMC(const char* name, TAGdataDsc* pNtuMC, TAGdataDsc* pNtuEve, TAGdataDsc* pNturaw, EVENT_STRUCT* evStr)
  : TAGaction(name, "TASTactNtuHitMC - NTuplize ToF raw data"),
   fpNtuMC(pNtuMC),
   fpNtuEve(pNtuEve),
   fpNtuRaw(pNturaw),
   fEventStruct(evStr)
{
   if(FootDebugLevel(1))
      Info("Action()"," Creating the Start Counter MC tuplizer action\n");
  
   if (fEventStruct == 0x0) {
     AddDataIn(pNtuMC, "TAMCntuHit");
     AddDataIn(pNtuEve, "TAMCntuPart");
   } 
   AddDataOut(pNturaw, "TASTntuHit");
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TASTactNtuHitMC::CreateDigitizer()
{
   TASTntuHit* p_nturaw = (TASTntuHit*) fpNtuRaw->Object();
   
   fDigitizer = new TASTdigitizer(p_nturaw);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TASTactNtuHitMC::~TASTactNtuHitMC()
{
   delete fDigitizer;
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TASTactNtuHitMC::Action()
{
  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  TASTntuHit* pNturaw   = (TASTntuHit*) fpNtuRaw->Object();
  
  TAMCntuHit* pNtuMC    = 0;

  if (fEventStruct == 0x0)
    pNtuMC    = (TAMCntuHit*) fpNtuMC->Object();
   else
    pNtuMC    = TAMCflukaParser::GetStcHits(fEventStruct, fpNtuMC);
  
  //The number of hits inside the Start Counter is stn
  if(FootDebugLevel(1))
      Info("Action()","Processing n Onion :: %2d hits", pNtuMC->GetHitsN());
   
   Float_t edep, trigtime;
   
   fDigitizer->ClearMap();
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hitMC = pNtuMC->GetHit(i);
      
      TVector3 posIn(hitMC->GetInPosition());
      TVector3 posOut(hitMC->GetOutPosition());

     Int_t id      = hitMC->GetSensorId();
     Int_t trackId = hitMC->GetTrackIdx()-1;
     Float_t z0    = posIn.Z();
     Float_t z1    = posOut.Z();
             edep  = hitMC->GetDeltaE()*TAGgeoTrafo::GevToMev();
     Float_t time  = hitMC->GetTof()*TAGgeoTrafo::SecToPs();
      
     TVector3 posInLoc = geoTrafo->FromGlobalToSTLocal(posIn);
     
     // don't use z for the moment
     fDigitizer->Process(edep, posInLoc[0], posInLoc[1], z0, z1, time, id);


     TASThit* hit = fDigitizer->GetCurrentHit();
     trigtime = hit->GetTime();
     hit->AddMcTrackIdx(trackId, i);
  }
  
  pNturaw->SetCharge(edep);
  pNturaw->SetTriggerTime(trigtime);
  
  if (fEventStruct != 0x0) {
    fpNtuMC->SetBit(kValid);
    fpNtuEve->SetBit(kValid);
  }

  fpNtuRaw->SetBit(kValid);
   
  return kTRUE;
}

