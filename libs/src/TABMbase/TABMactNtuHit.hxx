#ifndef _TABMactNtuHit_HXX
#define _TABMactNtuHit_HXX
/*!
  \file    TABMactNtuHit.hxx
  \brief   Declaration of TABMactNtuHit, the class that converts the BM raw hits (TABMrawHit) into a BM hit (TABMhit)
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TABMntuRaw.hxx"
#include "TABMrawHit.hxx"
#include "TASTntuHit.hxx"
#include "TABMparGeo.hxx"
#include "TABMparConf.hxx"
#include "TABMparCal.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"

#include "TH2.h"
#include "TRandom3.h"

class TABMactNtuHit : public TAGaction {
  public:
    explicit         TABMactNtuHit(const char* name=0,
				 TAGdataDsc* dscnturaw=0,
				 TAGdataDsc* dscdatraw=0,
				 TAGparaDsc* dscgeomap=0,
				 TAGparaDsc* dscparcon=0,
				 TAGparaDsc* dscparcal=0);
    virtual          ~TABMactNtuHit();

    virtual  void   CreateHistogram();
    virtual Bool_t  Action();

  private:
    TAGdataDsc*       fpNtuRaw;		    ///< output data dsc (TABMntuHit)
    TAGdataDsc*       fpDatRaw;		    ///< input data dsc (TABMntuRaw)
    TAGparaDsc*       fpGeoMap;		    ///< geometry para dsc (TABMparGeo)
    TAGparaDsc*       fpParCon;		    ///< config para dsc (TABMparConf)
    TAGparaDsc*       fpParCal;		    ///< calibration para dsc (TABMparCal)

    map<Int_t,Int_t>  fDrawMap;       ///<map adopted to draw fpHisMapX and fpHisMapY histograms, it is filled with: <cellid,bin number>

    // histograms
    TH2I*              fpHisMapX;                 ///< raw hit map X view
    TH2I*              fpHisMapY;                 ///< raw hit map Y view
    TH1I*              fpHisPivot_paoloni;        ///< Pivots of the Paoloni efficiency method
    TH1I*              fpHisProbe_paoloni;        ///< Probes of the Paoloni efficiency method
    TH1F*              fpHisEval_paoloni;         ///< efficiency evaluation of the Paoloni's method for all the different configuration
    TH1F*              fpHisEval_paoloni_Xview;   ///< efficiency evaluation of the Paoloni's method only for the x view hits
    TH1F*              fpHisEval_paoloni_Yview;   ///< efficiency evaluation of the Paoloni's method only for the y view hits
    TH1I*              fpHisHitXCell;             ///< number of raw hits x event
    TH1I*              fpHisCell;                 ///< hits cell
    TH1I*              fpHisView;                 ///< hits view
    TH1I*              fpHisPlane;                ///< hits plane
    TH1I*              fpHisHitTotDist;           ///< total hits distribution
    TH1I*              fpHisHitXzviewDist;        ///< number of hits on the XZ view
    TH1I*              fpHisHitYzviewDist;        ///< number of hits on the YZ view
    TH1F*              fpHisRdrift;               ///< hits drift distance
    TH1F*              fpHisTdrift;               ///< hits drift time
    TH1I*              fpHisDiscAccept;           ///< discharged vs accepted number of hits
    TH1F*              fpHisDiscTime;             ///< discharged hit time

   ClassDef(TABMactNtuHit,0)
};

#endif
