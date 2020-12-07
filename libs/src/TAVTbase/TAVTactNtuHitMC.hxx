#ifndef _TAVTactNtuHitMC_HXX
#define _TAVTactNtuHitMC_HXX
/*!
 \file
 \version $Id: TAVTactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/
#include <map>
#include "TVector3.h"

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAVTbaseDigitizer.hxx"
#include "TAVTdigitizerE.hxx"
#include "TAVTdigitizerG.hxx"

#include "TAVTntuHit.hxx"

#include "TAVTactBaseNtuMC.hxx"

class TAVTntuRaw;
class TAVTparGeo;

using namespace std;

class TAVTactNtuHitMC : public TAVTactBaseNtuMC {

public:
   explicit TAVTactNtuHitMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_ntuEve=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap=0, EVENT_STRUCT* evStr=0);
   virtual ~TAVTactNtuHitMC() {};
   
   //! Base action 
   virtual bool   Action();
   
   // Fill noise over sensors
   void           FillNoise();

private:
   TAGdataDsc*    fpNtuMC;          // input mc hit
   TAGdataDsc*    fpNtuEve;         // input eve track dsc
   TAGdataDsc*    fpNtuRaw;         // output data dsc
   EVENT_STRUCT*  fEventStruct;

   map<pair<int, int>, TAVTntuHit*> fMap;     //! map for pilepup

private:
   void           FillNoise(Int_t sensorId) ;
   void           SetMCinfo(TAVTntuHit* pixel, Int_t hitId);
   void           CreateDigitizer();
   void           FillPixels( Int_t sensorId, Int_t mcId, Int_t trackId);
   void           DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackId);
   void           Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);

   ClassDef(TAVTactNtuHitMC,0)
};

#endif
























