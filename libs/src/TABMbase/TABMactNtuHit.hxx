#ifndef _TABMactNtuHit_HXX
#define _TABMactNtuHit_HXX
/*!
  \file
  \version $Id: TABMactNtuHit.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TABMactNtuHit.
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
    TAGdataDsc*       fpNtuRaw;		    // output data dsc
    TAGdataDsc*       fpDatRaw;		    // input data dsc
    TAGparaDsc*       fpGeoMap;		    // geometry para dsc
    TAGparaDsc*       fpParCon;		    // config para dsc
    TAGparaDsc*       fpParCal;		    // calibration para dsc

    map<Int_t,Int_t>  fDrawMap;       //map to draw fpHisMapX and fpHisMapY <cellid,bin number>

    //histos
    TH2I*              fpHisMapX;                 //raw hit map X view
    TH2I*              fpHisMapY;                 //raw hit map Y view
    TH1I*              fpHisPivot_paoloni;        //Pivots of the Paoloni's method
    TH1I*              fpHisProbe_paoloni;        //Probes of the Paoloni's method
    TH1F*              fpHisEval_paoloni;         //efficiency evaluation of the Paoloni's method for all the different configuration
    TH1F*              fpHisEval_paoloni_Xview;   //efficiency evaluation of the Paoloni's method only for the x view
    TH1F*              fpHisEval_paoloni_Yview;   //efficiency evaluation of the Paoloni's method only for the y view
    TH1I*              fpHisHitXCell;             //number of raw hits x event
    TH1I*              fpHisCell;                 //hits cell
    TH1I*              fpHisView;                 //hits view
    TH1I*              fpHisPlane;                //hits plane
    TH1I*              fpHisHitTotDist;           //total hits distribution
    TH1I*              fpHisHitXzviewDist;        //hits distribution XZ view
    TH1I*              fpHisHitYzviewDist;        //hits distribution YZ view
    TH1F*              fpHisRdrift;               //hits rdrift
    TH1F*              fpHisTdrift;               //hits tdrift
    TH1I*              fpHisDiscAccept;           //discharged vs accepted number of hits
    TH1F*              fpHisDiscTime;             //raw discharged hit time difference
   
   ClassDef(TABMactNtuHit,0)
};

#endif
