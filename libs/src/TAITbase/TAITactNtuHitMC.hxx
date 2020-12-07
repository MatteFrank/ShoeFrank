#ifndef _TAITactNtuHitMC_HXX
#define _TAITactNtuHitMC_HXX
/*!
 \file
 \version $Id: TAITactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/


#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "Evento.hxx"

#include "TAITdigitizerE.hxx"

#include "TAITntuHit.hxx"

#include "TAVTactBaseNtuMC.hxx"

class TAITntuRaw;

class TAITactNtuHitMC : public TAVTactBaseNtuMC {

public:
   explicit TAITactNtuHitMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_ntuEve=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap=0, EVENT_STRUCT* evStr = 0);

   virtual ~TAITactNtuHitMC() {};
   
   //! Base action 
   bool           Action();
   
   // Fill noise over sensors
   void           FillNoise();

   
private:
   TAGdataDsc*    fpNtuMC;          // input data dsc
   TAGdataDsc*    fpNtuEve;         // input eve track dsc
   TAGdataDsc*    fpNtuRaw;		   // output data dsc
   EVENT_STRUCT*  fEventStruct;
   map<pair<int,int>, TAITntuHit*> fMap;      //! map for pilepup

private:
   void           FillNoise(Int_t sensorId) ;
   void           SetMCinfo(TAITntuHit* pixel, Int_t hitId);
   void           CreateDigitizer();
   void           FillPixels( Int_t sensorId, Int_t mcId, Int_t trackId);
   void           DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackId);
   void           Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);
   
   ClassDef(TAITactNtuHitMC,0)
};

#endif
























