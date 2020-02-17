#ifndef _TAMSDactNtuMC_HXX
#define _TAMSDactNtuMC_HXX
/*!
 \file
 \version $Id: TAMSDactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuMC.
 */
/*------------------------------------------+---------------------------------*/
#include <map>

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAMSDdigitizer.hxx"

#include "TAMSDntuHit.hxx"

#include "TAGaction.hxx"

class TAMSDntuRaw;
class TAMSDparGeo;

using namespace  std;
class TAMSDactNtuMC : public TAGaction {
   
public:
   explicit TAMSDactNtuMC(const char* name=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_geomap = 0, EVENT_STRUCT* evtStr=0);
   virtual ~TAMSDactNtuMC() {};
   
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
   TAGdataDsc*     fpNtuRaw;		    // output data dsc
   TAGparaDsc*     fpGeoMap;		    // geometry para dsc
   EVENT_STRUCT*   fpEvtStr;
   
   TAMSDdigitizer* fDigitizer;       // cluster size digitizer
   Int_t           fNoisyStripsN;

   map<int, TAMSDntuHit*> fMap; //! map for pilepup

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
   void            SetMCinfo(TAMSDntuHit* pixel, Int_t hitId);
   void            CreateDigitizer();
   void            FillStrips( Int_t sensorId, Int_t mcId );
   void            ComputeNoiseLevel();

   ClassDef(TAMSDactNtuMC,0)
};

#endif
























