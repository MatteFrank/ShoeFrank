/*!
  \file
  \version $Id: TASTactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TASTactNtuMC.
*/

#include "GlobalPar.hxx"
#include "TAGroot.hxx"
#include "TASTntuRaw.hxx"
#include "TAGgeoTrafo.hxx"
#include "TASTdigitizer.hxx"

#include "TASTactNtuMC.hxx"

/*!
  \class TASTactNtuMC TASTactNtuMC.hxx "TASTactNtuMC.hxx"
  \brief NTuplizer for BM raw hits. **
*/

ClassImp(TASTactNtuMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTactNtuMC::TASTactNtuMC(const char* name,
			 TAGdataDsc* p_datraw, 
			 EVENT_STRUCT* evStr)
  : TAGaction(name, "TASTactNtuMC - NTuplize ToF raw data"),
    fpNtuMC(p_datraw),
    fpEvtStr(evStr)
{
   if(FootDebugLevel(1))
      Info("Action()"," Creating the Start Counter MC tuplizer action\n");
   AddDataOut(p_datraw, "TASTntuRaw");
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TASTactNtuMC::CreateDigitizer()
{
   TASTntuRaw* p_nturaw = (TASTntuRaw*) fpNtuMC->Object();
   
   fDigitizer = new TASTdigitizer(p_nturaw);
}

//------------------------------------------+-----------------------------------
//! Destructor.

TASTactNtuMC::~TASTactNtuMC()
{
   delete fDigitizer;
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TASTactNtuMC::Action()
{
  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  TASTntuRaw* p_nturaw = (TASTntuRaw*) fpNtuMC->Object();

  //The number of hits inside the Start Counter is stn
   if(FootDebugLevel(1))
      Info("Action()","Processing n Onion :: %2d hits", fpEvtStr->STCn);
   Float_t edep, trigtime;
   
   fDigitizer->ClearMap();
   for (Int_t i = 0; i < fpEvtStr->STCn; i++) {
     Int_t id      = fpEvtStr->STCid[i];
     Int_t trackId = fpEvtStr->STCid[i] - 1;
     Float_t x0    = fpEvtStr->STCxin[i];
     Float_t y0    = fpEvtStr->STCyin[i];
     Float_t z0    = fpEvtStr->STCzin[i];
     Float_t z1    = fpEvtStr->STCzout[i];
     edep  = fpEvtStr->STCde[i]*TAGgeoTrafo::GevToMev();
     Float_t time  = fpEvtStr->STCtim[i]*TAGgeoTrafo::SecToPs();
     
     
     TVector3 posIn(x0, y0, z0);
     TVector3 posInLoc = geoTrafo->FromGlobalToSTLocal(posIn);
     
     // don't use z for the moment
     fDigitizer->Process(edep, posInLoc[0], posInLoc[1], z0, z1, time, id);


     TASTntuHit* hit = fDigitizer->GetCurrentHit();
     trigtime = hit->GetTime();
     hit->AddMcTrackIdx(trackId, i);
  }
  
  p_nturaw->SetCharge(edep);
  p_nturaw->SetTriggerTime(trigtime);
  fpNtuMC->SetBit(kValid);
   
  return kTRUE;
}

