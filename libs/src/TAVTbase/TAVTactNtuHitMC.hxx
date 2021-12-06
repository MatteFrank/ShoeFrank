#ifndef _TAVTactNtuHitMC_HXX
#define _TAVTactNtuHitMC_HXX
/*!
 \file
 \version $Id: TAVTactNtuHitMC.hxx
 \brief   Declaration of TAVTactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/
#include <map>
#include "TVector3.h"

#include "TAMCflukaStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAVTbaseDigitizer.hxx"
#include "TAVTdigitizerE.hxx"
#include "TAVTdigitizerG.hxx"

#include "TAVThit.hxx"

#include "TAVTactBaseNtuHitMC.hxx"

class TAVTntuHit;
class TAVTparGeo;

using namespace std;

class TAVTactNtuHitMC : public TAVTactBaseNtuHitMC {

public:
   explicit TAVTactNtuHitMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_ntuEve=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap=0, EVENT_STRUCT* evStr=0);
   virtual ~TAVTactNtuHitMC() {};
   
   //! Base action 
   virtual bool   Action();
   
   //! Fill noise over sensors
   void           FillNoise();

private:
   TAGdataDsc*    fpNtuMC;          ///< input mc hit
   TAGdataDsc*    fpNtuEve;         ///< input eve track dsc
   TAGdataDsc*    fpNtuRaw;         ///< output data dsc
   EVENT_STRUCT*  fEventStruct;     ///< Fluka structure

   map<pair<int, int>, TAVThit*> fMap;     //!< map for pilepup

private:
   //! Fill noise per sensor
   void           FillNoise(Int_t sensorId) ;
   //! Set MC info for a given pixel hit
   void           SetMCinfo(TAVThit* pixel, Int_t hitId);
   //! Create the digitizer
   void           CreateDigitizer();
   //! Fill pixels
   void           FillPixels( Int_t sensorId, Int_t mcId, Int_t trackId);
   //! Digitize hit
   void           DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackId);
   //! Digitize all hits
   void           Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents);

   ClassDef(TAVTactNtuHitMC,0)
};

#endif
























