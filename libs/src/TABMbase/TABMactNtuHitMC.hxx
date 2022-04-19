#ifndef _TABMactNtuHitMC_HXX
#define _TABMactNtuHitMC_HXX
/*!
  \file   TABMactNtuHitMC.hxx
  \brief   Declaration of TABMactNtuHitMC, this class converts the FLUKA MC input into a BM hit (TABMntuHit)
*/
/*------------------------------------------+---------------------------------*/
#include <map>
#include "TAMCflukaStruct.hxx"
#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TAGroot.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TABMparConf.hxx"
#include "TABMparCal.hxx"
#include "TABMdigitizer.hxx"
#include "TAGgeoTrafo.hxx"

#include "TVector3.h"
#include "TRandom3.h"

using namespace std;
class TABMactNtuHitMC : public TAGaction {
  public:
    explicit        TABMactNtuHitMC(const char* name      = 0,
                                    TAGdataDsc* dscntuMC  = 0,
                                    TAGdataDsc* dscntuEve = 0,
                                    TAGdataDsc* dscnturaw = 0,
                                    TAGparaDsc* dscbmcon  = 0,
                                    TAGparaDsc* dscbmcal  = 0,
                                    TAGparaDsc* dscbmgeo  = 0,
                                    EVENT_STRUCT* evStr   = 0);

    virtual         ~TABMactNtuHitMC();

    virtual Bool_t  Action();
    virtual  void   CreateHistogram();
    void            CreateDigitizer();
    void            CreateFakeHits();

  private:
    TAGdataDsc*     fpNtuMC;        ///< input mc hit
    TAGdataDsc*     fpNtuEve;       ///< input eve track dsc
    TAGdataDsc*     fpNtuRaw;		    ///< output data dsc
    TAGparaDsc*     fpParCon;		    ///< BM config params.
    TAGparaDsc*     fpParCal;		    ///< BM calibration params.
    TAGparaDsc*     fpParGeo;		    ///< BM geo params.
    TABMdigitizer*  fDigitizer;     ///< cluster size digitizer
    EVENT_STRUCT*   fEventStruct;   ///< old version of input mc hit, useful for back compatibility

    // Histos
    TH1I*            fpHisCell;       ///<hits cell
    TH1I*            fpHisView;       ///<hits view
    TH1I*            fpHisPlane;      ///<hits plane
    TH1F*            fpHisRdrift;     ///<hits drift distance
    TH1F*            fpDisRdrift;     ///<hits real rdrift - smeared rdrift
    TH1I*            fpDisReason;     ///<discharged hits reason
    TH1I*            fpHisHitNum;     ///<hit per event distribution
    TH1I*            fpHisFakeIndex;  ///<hits fake index (0=fluka hit from primary, 1=fluka hit not from primay, 2=Random hit not from fluka)

   ClassDef(TABMactNtuHitMC,0)
};

#endif
