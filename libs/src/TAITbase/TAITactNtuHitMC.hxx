#ifndef _TAITactNtuHitMC_HXX
#define _TAITactNtuHitMC_HXX
/*!
 \file TAITactNtuHitMC.hxx
 \brief   Declaration of TAITactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/


#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAMCflukaStruct.hxx"

#include "TAITdigitizerE.hxx"
#include "TAIThit.hxx"
#include "TAVTactBaseNtuHitMC.hxx"

class TAITntuHit;

class TAITactNtuHitMC : public TAVTactBaseNtuHitMC {

public:
   explicit TAITactNtuHitMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_ntuEve=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap=0, EVENT_STRUCT* evStr = 0);

   virtual ~TAITactNtuHitMC() {};
   
   //! Base action 
   bool           Action();
   
   // Fill noise over sensors
   void           FillNoise();

   
private:
   TAGdataDsc*    fpNtuMC;          ///< input data dsc
   TAGdataDsc*    fpNtuEve;         ///< input eve track dsc
   TAGdataDsc*    fpNtuRaw;		   ///< output data dsc
   EVENT_STRUCT*  fEventStruct;
   map<pair<int,int>, TAIThit*> fMap;      //! map for pilepup

private:
   //! Fill noise per sensor
   void           FillNoise(Int_t sensorId);
   //! Set MC info for pixel
   void           SetMCinfo(TAIThit* pixel, Int_t hitId);
   //! Create digitizer
   void           CreateDigitizer();
   //! Fill pixels
   void           FillPixels( Int_t sensorId, Int_t mcId, Int_t trackId);
   //! Digitize hits
   void           DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackId);
   //! Dogitize
   void           Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);
   
   ClassDef(TAITactNtuHitMC,0)
};

#endif
























