#ifndef _TAVTactNtuMC_HXX
#define _TAVTactNtuMC_HXX
/*!
 \file
 \version $Id: TAVTactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactNtuMC.
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

class TAVTactNtuMC : public TAVTactBaseNtuMC {

public:
   explicit TAVTactNtuMC(const char* name=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap = 0, EVENT_STRUCT* evtStr=0);
   explicit TAVTactNtuMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap=0);
   virtual ~TAVTactNtuMC() {};
   
   //! Base action 
   virtual bool   Action();
   
   // Fill noise over sensors
   void           FillNoise();

private:
   TAGdataDsc*    fpNtuMC;          // input data dsc
   TAGdataDsc*    fpNtuRaw;         // output data dsc
   map<pair<int, int>, TAVTntuHit*> fMap;     //! map for pilepup

private:
   void           FillNoise(Int_t sensorId) ;
   void           SetMCinfo(TAVTntuHit* pixel, Int_t hitId);
   void           CreateDigitizer();
   void           FillPixels( Int_t sensorId, Int_t mcId, Int_t trackId);
   void           DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackId);
   void           DigitizeOld(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);
   void           Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);

};

#endif
























