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
#include "TABMparCon.hxx"
#include "TAGgeoTrafo.hxx"

#include "TVector3.h"
#include "TRandom3.h"

class TABMactNtuMC : public TAGaction {
  public:
    explicit        TABMactNtuMC(const char* name=0,
                                 TAGdataDsc* dscnturaw=0,
                                 TAGparaDsc* dscbmcon=0,
                                 TAGparaDsc* dscbmgeo=0,
                                 EVENT_STRUCT* evStr=0);
    virtual         ~TABMactNtuMC();

    virtual Bool_t  Action();
    virtual  void   CreateHistogram();
    void CreateFakeHits(Int_t nfake, Int_t &nhits);


    ClassDef(TABMactNtuMC,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    TAGparaDsc*     fpParCon;		    // BM config params.
    TAGparaDsc*     fpParGeo;		    // BM geo params.
    EVENT_STRUCT*   fpEvtStr;
    Double_t        rdrift_err;      //default error value of the rdrfit
    
    //histos

};

#endif
