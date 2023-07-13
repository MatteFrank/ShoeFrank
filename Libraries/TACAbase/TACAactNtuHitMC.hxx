#ifndef _TACAactNtuHitMC_HXX
#define _TACAactNtuHitMC_HXX
/*!
  \file TACAactNtuHitMC.hxx
  \brief   Declaration of TACAactNtuHitMC.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaStruct.hxx"

#include "TACAparameters.hxx"
#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGgeoTrafo.hxx"
#include "TRandom3.h"

#include "TH2I.h"
#include "TH1F.h"
#include "TH2F.h"

class TACAdigitizer;
class TACAactNtuHitMC : public TAGaction {
  
  public:
    explicit       TACAactNtuHitMC(const char* name       = 0,
                                    TAGdataDsc* p_ntuMC   = 0,
                                    TAGdataDsc* p_ntuEve  = 0,
                                    TAGdataDsc* p_nturaw  = 0,
                                    TAGparaDsc* p_geomap  = 0,
                                    TAGparaDsc* p_calmap  = 0,
                                    TAGparaDsc* p_geomapG = 0,
                                    EVENT_STRUCT* evStr   = 0);

    virtual        ~TACAactNtuHitMC();

    virtual        Bool_t Action();

    void           CreateHistogram();

  private:
   TAGdataDsc*     fpNtuMC;      ///< input mc hit
   TAGdataDsc*     fpNtuEve;     ///< input eve track dsc
   TAGdataDsc*     fpNtuRaw;	   ///<  output data dsc
   TAGparaDsc*     fpGeoMap;     ///<  geometry para dsc
   TAGparaDsc*     fpCalMap;     ///<  calib para dsc
   TAGparaDsc*     fpGeoMapG;    ///<  geometry para dsc for beam
   TAGgeoTrafo*    fpGeoTrafo;   ///<  global transformation
   TACAdigitizer*  fDigitizer;   ///<  cluster size digitizer
   EVENT_STRUCT*   fEventStruct; ///<  old Fluka structure

   TH1F*           fpHisDeTot;                         ///< Total energy loss histogram
   TH1F*           fpHisDeTotMc;                       ///< Total MC energy loss histogram
   TH1F*           fpHisNeutrondE;                     ///< Neutron energy loss histogram
   TH1F*           fpHisMass;                          ///< MC mass histogram
   TH1F*           fpHisTime;                          ///< MC time histogram
   TH1F*           fpHisRange;                         ///< MC range histogram

   TH2F*           fpHisRangeVsMass;                   ///< range vs mass histogram
   TH2F*           fpHisHitMapXY;                      ///< MC Hit map histogram
   TH2F*           fpHisHitMap;                        ///< Hit map histogram
   TH2F*           fpHisHitMapZYin;                    ///< MC Hit ZY in map histogram
   TH2F*           fpHisHitMapZYout;                   ///< MC Hit ZY out map histogram
   TH2I*           fpHisParticleVsRegion;              ///< particle vs region histogram

  private:
   void           CreateDigitizer();

   ClassDef(TACAactNtuHitMC,0)
};

#endif
