#ifndef _TAITactNtuMC_HXX
#define _TAITactNtuMC_HXX
/*!
 \file
 \version $Id: TAITactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuMC.
 */
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAITdigitizerE.hxx"

#include "TAITntuHit.hxx"

#include "TAVTactBaseNtuMC.hxx"

class TAITntuRaw;

class TAITactNtuMC : public TAVTactBaseNtuMC {

public:
   explicit TAITactNtuMC(const char* name=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap = 0, EVENT_STRUCT* evtStr=0);
   explicit TAITactNtuMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap=0);

   virtual ~TAITactNtuMC() {};
   
   //! Base action 
   bool           Action();
   
   // Fill noise over sensors
   void           FillNoise();

   
private:
   TAGdataDsc*    fpNtuMC;          // input data dsc
   TAGdataDsc*    fpNtuRaw;		   // output data dsc
   map<pair<int,int>, TAITntuHit*> fMap;      //! map for pilepup

private:
   void           FillNoise(Int_t sensorId) ;
   void           SetMCinfo(TAITntuHit* pixel, Int_t hitId);
   void           CreateDigitizer();
   void           FillPixels( Int_t sensorId, Int_t mcId, Int_t trackId);
   void           DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackId);
   void           DigitizeOld(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);
   void           Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);

   
   ClassDef(TAITactNtuMC,0)
};

#endif
























