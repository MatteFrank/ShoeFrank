#ifndef _TABMactNtuMC_HXX
#define _TABMactNtuMC_HXX
/*!
  \file
  \version $Id: TABMactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TABMactNtuMC.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"
#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TAGroot.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"
#include "TABMparConf.hxx"
#include "TABMparCal.hxx"
#include "TABMdigitizer.hxx"
#include "TAGgeoTrafo.hxx"

#include "TVector3.h"
#include "TRandom3.h"

class TABMactNtuMC : public TAGaction {
  public:
    explicit        TABMactNtuMC(const char* name=0,
                                 TAGdataDsc* dscnturaw=0,
                                 TAGparaDsc* dscbmcon=0,
                                 TAGparaDsc* dscbmcal=0,
                                 TAGparaDsc* dscbmgeo=0,
                                 EVENT_STRUCT* evStr=0);
    virtual         ~TABMactNtuMC();

    virtual  void   CreateHistogram();
    void            CreateDigitizer();
    virtual Bool_t  Action();

    void            CreateFakeHits();

    ClassDef(TABMactNtuMC,0)

  private:
    TAGparaDsc*     fpParCon;		    // BM config params.
    TAGparaDsc*     fpParCal;		    // BM calib params.
    TAGparaDsc*     fpParGeo;		    // BM geo params.

    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
    TABMdigitizer*  fDigitizer;     // cluster size digitizer

    //histos
    TH1I*            fpHisCell;       //hits cell
    TH1I*            fpHisView;       //hits view
    TH1I*            fpHisPlane;      //hits plane
    TH1F*            fpHisRdrift;     //charged hits rdrift
    TH1F*            fpDisRdrift;     //discharged hits rdrift
    TH1I*            fpHisHitNum;     //raw hit map
    TH1I*            fpHisFakeIndex;  //hits fake index
};

#endif
