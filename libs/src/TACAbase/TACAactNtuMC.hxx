#ifndef _TACAactNtuMC_HXX
#define _TACAactNtuMC_HXX
/*!
  \file
  \version $Id: TACAactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TACAactNtuMC.
*/
/*------------------------------------------+---------------------------------*/

#include "EventStruct.hxx"

#include "TAGaction.hxx"
#include "TAGparGeo.hxx"
#include "TAGdataDsc.hxx"
#include "TACAdigitizer.hxx"
#include "TACAparGeo.hxx"
#include "TAGgeoTrafo.hxx"

#include "TH1F.h"
#include "TH2F.h"


#define MAX_NCRY 300      //max number of the crystals in the calorimeter
#define MAX_ANUMBER 18     //max atomic number


class TACAactNtuMC : public TAGaction {

  public:
    explicit        TACAactNtuMC(const char* name     = 0,
                                 TAGdataDsc* p_datraw = 0,
                                 TAGparaDsc* p_geomap = 0,
                                 TAGparaDsc* g_geomap = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual         ~TACAactNtuMC();

    virtual         Bool_t  Action();

    void           CreateHistogram();

    ClassDef(TACAactNtuMC,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    TAGparaDsc*     fpGeoMap;          // geometry para dsc
    TAGparaDsc*     fpGeoMapG;       // geometry para dsc for beam
    TAGgeoTrafo*    fpGeoTrafo;
    TACAdigitizer*  fDigitizer;     // cluster size digitizer
    EVENT_STRUCT*   fpEvtStr;

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

};

#endif
