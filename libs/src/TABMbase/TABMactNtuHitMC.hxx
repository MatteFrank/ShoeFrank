#ifndef _TABMactNtuHitMC_HXX
#define _TABMactNtuHitMC_HXX
/*!
  \file
  \version $Id: TABMactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TABMactNtuHitMC.
*/
/*------------------------------------------+---------------------------------*/
#include <map>
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

using namespace std;
class TABMactNtuHitMC : public TAGaction {
  public:
    explicit        TABMactNtuHitMC(const char* name      = 0,
                                    TAGdataDsc* dscntuMC  = 0,
                                    TAGdataDsc* dscntuEve = 0,
                                    TAGdataDsc* dscnturaw = 0,
                                    TAGparaDsc* dscbmcon  = 0,
                                    TAGparaDsc* dscbmgeo  = 0);
   
    virtual         ~TABMactNtuHitMC();

    virtual Bool_t  Action();
    virtual  void   CreateHistogram();
    void CreateFakeHits(Int_t nfake, Int_t &nhits);
    void SmearRdrift(Int_t smear_type, Double_t &tobesmeared, Double_t sigma); //to smear rdrift with resolution
   
    void ClearMap() { fMap.clear(); }

    ClassDef(TABMactNtuHitMC,0)

  private:
    TAGdataDsc*     fpNtuMC;     // input mc hit
    TAGdataDsc*     fpNtuEve;    // input eve track dsc
    TAGdataDsc*     fpNtuRaw;		    // output data dsc
    TAGparaDsc*     fpParCon;		    // BM config params.
    TAGparaDsc*     fpParGeo;		    // BM geo params.
    Double_t        fRdriftErr;      //default error value of the rdrfit
    map<pair<int, int>, TABMntuHit*> fMap; //! map for pilepup

    //histos

    TH1I*            fpHisCell;    //hits cell
    TH1I*            fpHisView;    //hits view
    TH1I*            fpHisPlane;   //hits plane
    TH1F*            fpHisRdrift;  //hits rdrift
    TH1F*            fpHisSmearDiff;  //hits real rdrift - smeared rdrift
    TH1I*            fpHisHitNum;  //raw hit map    
    TH1I*            fpHisFakeIndex;  //hits fake index
};

#endif
