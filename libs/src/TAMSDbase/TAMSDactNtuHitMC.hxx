#ifndef _TAMSDactNtuHitMC_HXX
#define _TAMSDactNtuHitMC_HXX
/*!
 \file TAMSDactNtuHitMC.hxx
 \brief   Declaration of TAMSDactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/
#include <map>

#include "TH1F.h"

#include "TAMCflukaStruct.hxx"
#include "TAMSDparameters.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAMSDdigitizer.hxx"

#include "TAMSDhit.hxx"

#include "TAGaction.hxx"

class TAMSDntuHit;
class TAMSDparGeo;

using namespace  std;
class TAMSDactNtuHitMC : public TAGaction {
   
public:
   explicit        TAMSDactNtuHitMC(const char* name     = 0,
                                    TAGdataDsc* p_ntuMC  = 0,
                                    TAGdataDsc* p_ntuEve = 0,
                                    TAGdataDsc* p_nturaw = 0,
                                    TAGparaDsc* p_geomap = 0,
                                    TAGparaDsc* p_calmap = 0,
                                    EVENT_STRUCT* evStr  = 0);
   virtual        ~TAMSDactNtuHitMC() {};
   
   //! Base action 
   virtual bool    Action();
   
   // Fill noise over sensors
   void            FillNoise();

   //! Base creation of histogram
   void            CreateHistogram();

public:   
   //! Get MC noisy hit id
   static Int_t    GetMcNoiseId()                    { return fgMcNoiseId;        }
   //! Set MC noisy hit id
   static void     SetMcNoiseId(Int_t id)            { fgMcNoiseId = id;          }
   
private:
   TAGdataDsc*     fpNtuMC;            ///< input mc hit
   TAGdataDsc*     fpNtuEve;           ///< input eve track dsc
   TAGdataDsc*     fpNtuRaw;		      ///< output data dsc
   TAGparaDsc*     fpGeoMap;           ///< geometry para dsc
   TAGparaDsc*     fpCalMap;           ///< calibration para dsc
   TAGgeoTrafo*    fpGeoTrafo;         ///< gobal transformation
   TAMSDdigitizer* fDigitizer;         ///< cluster size digitizer
   Int_t           fNoisyStripsN[32];  ///< noisy strip
   Bool_t          fNoiseLevel;        ///< sigma noise level flag
   vector<double>  fSigmaNoiseLevel;   ///< sigma noise level array
   EVENT_STRUCT*   fEventStruct;       ///< old fluka structure

   map<pair<int, int>, TAMSDhit*> fMap; //! map for pilepup

   TH1F*           fpHisStripMap[MaxPlane];  ///< strip map per sensor
   TH1F*           fpHisPosMap[MaxPlane];    ///< strip map per sensor
   TH1F*           fpHisStrip[MaxPlane];     ///< number strips per cluster MC
   TH1F*           fpHisStripTot;            ///< total number strips per cluster MC
   TH1F*           fpHisDeTot;               ///< Total energy loss
   TH1F*           fpHisDeSensor[MaxPlane];  ///< Energy loss per sensor
   TH1F*           fpHisAdc[MaxPlane];       ///< charge per strip
   
private:
   static Int_t    fgMcNoiseId;        ///< default noisy hit id

private:
   void            FillNoise(Int_t sensorId) ;
   void            SetMCinfo(TAMSDhit* strip, Int_t hitId);
   void            CreateDigitizer();
   void            FillStrips( Int_t sensorId, Int_t mcId, Int_t trackIdx);
   void            ComputeNoiseLevel();

   ClassDef(TAMSDactNtuHitMC,0)
};

#endif
























