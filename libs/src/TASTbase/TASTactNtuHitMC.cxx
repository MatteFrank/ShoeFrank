/*!
  \file
  \version $Id: TASTactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TASTactNtuHitMC.
*/

#include "GlobalPar.hxx"
#include "TAGroot.hxx"
#include "TASTntuRaw.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCflukaParser.hxx"
#include "TASTdigitizer.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TASTactNtuHitMC.hxx"

/*!
  \class TASTactNtuHitMC TASTactNtuHitMC.hxx "TASTactNtuHitMC.hxx"
  \brief NTuplizer for BM raw hits. **
*/

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
     AddDataIn(pNtuEve, "TAMCntuEve");
   } 
   AddDataOut(pNturaw, "TASTntuRaw");
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TASTactNtuHitMC::CreateDigitizer()
{
   TASTntuRaw* p_nturaw = (TASTntuRaw*) fpNtuRaw->Object();
   
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
  TASTntuRaw* pNturaw   = (TASTntuRaw*) fpNtuRaw->Object();
  
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


     TASTntuHit* hit = fDigitizer->GetCurrentHit();
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

