#ifndef _TACAactNtuHitMC_HXX
#define _TACAactNtuHitMC_HXX
/*!
  \file
  \version $Id: TACAactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TACAactNtuHitMC.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGgeoTrafo.hxx"
#include "TRandom3.h"

#include "TH2I.h"
#include "TH1F.h"
#include "TH2F.h"


#define MAX_NCRY 300      //max number of the crystals in the calorimeter
#define MAX_ANUMBER 18     //max atomic number

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

    virtual Bool_t Action();

    void           SmearEnergy();

    void           CreateHistogram();

  private:
   TAGdataDsc*     fpNtuMC;     // input mc hit
   TAGdataDsc*     fpNtuEve;    // input eve track dsc
   TAGdataDsc*     fpNtuRaw;		    // output data dsc
   TAGparaDsc*     fpGeoMap;          // geometry para dsc
   TAGparaDsc*     fpCalMap;          // calib para dsc
   TAGparaDsc*     fpGeoMapG;       // geometry para dsc for beam
   TAGgeoTrafo*    fpGeoTrafo;
   TACAdigitizer*  fDigitizer;       // cluster size digitizer
   EVENT_STRUCT*   fEventStruct;

   TH1F* fpHisDeTot;
   TH1F* fpHisDeTotMc;
   TH1F* fpHisNeutron_dE;
   TH1F* fpHisMass;
   TH1F* fpHisTime;
   TH1F* fpHisRange;
   TH1F* fpHisIon_Ek[MAX_ANUMBER];
   TH1F* fpHisIon_dE[MAX_ANUMBER];
   TH1F* fpHisEnPerCry[MAX_NCRY];
   TH1F* fpHisEnVsPositionPerCry[MAX_NCRY];

   TH2F* fpHisCryHitVsEnDep;
   TH2F* fpHisRangeVsMass;
   TH2F* fpHisCryHitVsZ;
   TH2F* fpHisEnDepVsZ;
   TH2F* fpHisHitMapXY;
   TH2F* fpHisHitMapZYin;
   TH2F* fpHisHitMapZYout;
   TH2I* fpHisParticleVsRegion;

  private:
   void           CreateDigitizer();

   ClassDef(TACAactNtuHitMC,0)

};

#endif
