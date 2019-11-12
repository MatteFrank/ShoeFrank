#ifndef _TABMactNtuRaw_HXX
#define _TABMactNtuRaw_HXX
/*!
  \file
  \version $Id: TABMactNtuRaw.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TABMactNtuRaw.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TABMdatRaw.hxx"
#include "TABMrawHit.hxx"
#include "TASTntuRaw.hxx"
#include "TABMparGeo.hxx"
#include "TABMparCon.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"

#include "TRandom3.h"

class TABMactNtuRaw : public TAGaction {
  public:
    explicit        TABMactNtuRaw(const char* name=0,
				 TAGdataDsc* p_nturaw=0, 
				 TAGdataDsc* p_datraw=0, 
				 TAGparaDsc* p_geomap=0, 
				 TAGparaDsc* p_parcon=0);
    virtual         ~TABMactNtuRaw();

    virtual  void   CreateHistogram();
    virtual Bool_t  Action();

    ClassDef(TABMactNtuRaw,0)

  private:
    TAGdataDsc*     fpNtuRaw;		    // output data dsc
    TAGdataDsc*     fpDatRaw;		    // input data dsc
    TAGparaDsc*     fpGeoMap;		    // geometry para dsc
    TAGparaDsc*     fpParCon;		    // BM config params.
    
    
    //histos
    TH1I*            fpHisPivot_paoloni;  //Pivots of the Paoloni's method
    TH1I*            fpHisProbe_paoloni;  //Probes of the Paoloni's method
    TH1F*            fpHisEval_paoloni;   //efficiency evaluation of the Paoloni's method for all the different configuration
    TH1F*            fpHisEval_paoloni_Xview;   //efficiency evaluation of the Paoloni's method only for the x view
    TH1F*            fpHisEval_paoloni_Yview;   //efficiency evaluation of the Paoloni's method only for the y view
    TH1I*            fpHisCell;    //hits cell
    TH1I*            fpHisView;    //hits view
    TH1I*            fpHisPlane;   //hits plane
    TH1F*            fpHisRdrift;  //hits rdrift
    TH1F*            fpHisTdrift;  //hits tdrift
};

#endif
