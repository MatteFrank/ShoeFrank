#ifndef _TAMSDactNtuHitMC_HXX
#define _TAMSDactNtuHitMC_HXX
/*!
 \file
 \version $Id: TAMSDactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/
#include <map>

#include "TH1F.h"

#include "EventStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAMSDdigitizer.hxx"

#include "TAMSDhit.hxx"

#include "TAGaction.hxx"

class TAMSDntuHit;
class TAMSDparGeo;

using namespace  std;
class TAMSDactNtuHitMC : public TAGaction {
   
public:
   explicit TAMSDactNtuHitMC(const char* name=0,
                             TAGdataDsc* p_ntuMC=0,
                             TAGdataDsc* p_ntuEve=0,
                             TAGdataDsc* p_nturaw=0,
                             TAGparaDsc* p_geomap = 0,
                             EVENT_STRUCT* evStr = 0);
   virtual ~TAMSDactNtuHitMC() {};
   
   //! Base action 
   virtual bool   Action();
   
   // Fill noise over sensors
   void           FillNoise();

   //! Base creation of histogram
   void          CreateHistogram();

public:
   static Float_t GetSigmaNoiseLevel()              { return fgSigmaNoiseLevel;  }
   static void    SetSigmaNoiseLevel(Float_t level) { fgSigmaNoiseLevel = level; }
   
   static Int_t   GetMcNoiseId()                    { return fgMcNoiseId;        }
   static void    SetMcNoiseId(Int_t id)            { fgMcNoiseId = id;          }
   
private:
   TAGdataDsc*    fpNtuMC;          // input mc hit
   TAGdataDsc*    fpNtuEve;         // input eve track dsc
   TAGdataDsc*    fpNtuRaw;		    // output data dsc
   TAGparaDsc*    fpGeoMap;		    // geometry para dsc
   
   TAMSDdigitizer* fDigitizer;       // cluster size digitizer
   Int_t           fNoisyStripsN;
   EVENT_STRUCT*   fEventStruct;

   map<pair<int, int>, TAMSDhit*> fMap; //! map for pilepup

   TH1F*           fpHisStripMap[32];  // pixel map per sensor
   TH1F*           fpHisPosMap[32];    // pixel map per sensor
   TH1F*           fpHisStrip[32];     // number pixels per cluster MC
   TH1F*           fpHisStripTot;     // total number pixels per cluster MC
   TH1F*           fpHisDeTot;        // Total energy loss
   TH1F*           fpHisDeSensor[32];  // Energy loss per sensor
   TH1F*           fpHisAdc[32];     // charge per strip
   
private:
   static Float_t  fgSigmaNoiseLevel;
   static Int_t    fgMcNoiseId;

private:
   void            FillNoise(Int_t sensorId) ;
   void            SetMCinfo(TAMSDhit* pixel, Int_t hitId);
   void            CreateDigitizer();
   void            FillStrips( Int_t sensorId, Int_t mcId, Int_t trackIdx);
   void            ComputeNoiseLevel();

   ClassDef(TAMSDactNtuHitMC,0)
};

#endif
























