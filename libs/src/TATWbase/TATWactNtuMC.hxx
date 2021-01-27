#ifndef _TATWactNtuMC_HXX
#define _TATWactNtuMC_HXX
/*!
  \file
  \version $Id: TATWactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TATWactNtuMC.
*/
/*------------------------------------------+---------------------------------*/
 #include <vector>
 #include <map>

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

#include "Evento.hxx"

#include "TATWntuRaw.hxx"
#include "TATWparGeo.hxx"

#include "TAGgeoTrafo.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "Parameters.h"

class TATWdigitizer;

class TATWactNtuMC : public TAGaction {
public:
  explicit TATWactNtuMC(const char* name=0, TAGdataDsc* p_hitraw=0, TAGparaDsc* p_parcal=0, TAGparaDsc *p_pargeo=0, EVENT_STRUCT* evStr=0, Bool_t isZmc = false);
    virtual  ~TATWactNtuMC();

    virtual bool  Action();
   
    void          CreateHistogram();

private:
    TAGdataDsc*     fHitContainer;	// output data dsc
    TAGparaDsc*     fpCalPar;           // calibration parameter dsc
    TAGparaDsc*     fParGeo;            // beam parameter dsc
    EVENT_STRUCT*   fEventStruct;
    TATWdigitizer*  fDigitizer;        // digitizer

    TATWparCal*     f_parcal;

    TAGparGeo*      f_pargeo;
    TAGgeoTrafo*    f_geoTrafo;
  
    Int_t fZbeam;  
    Int_t fCnt;
    Int_t fCntWrong;

    Bool_t fIsZtrueMC;
  
    TH1F* fpHisHitCol;
    TH1F* fpHisHitLine;
    TH1F* fpHisHitMap;
    TH1F* fpHisRecPos;
    TH1F* fpHisRecPosMc;
    TH1F* fpHisRecTof;
    TH1F* fpHisRecTofMc;
    TH2I* fpHisZID_f;
    TH2I* fpHisZID_r;
    TH2I* fpHisZID_MCtrue_f;
    TH2I* fpHisZID_MCtrue_r;
    TH2D* fpHisElossTof_MCrec[nLayers];
    TH2D* fpHisElossTof_MCtrue[nLayers];
    // vector of histo with the same size of the ion beam atomic number
    vector<TH2D*> fpHisElossTof_MC;
    vector<TH2D*> fpHisElossTof;
    vector<TH1F*> fpHisDistZ_MC;
    vector<TH1F*> fpHisDistZ;
    vector<TH1D*> fpHisResPos;
   
    map<int, TATWntuHit*> fMapPU; //! map for pilepup
    vector<TATWntuHit*> fVecPuOff; //! vector for pilepup Off

private:
    void          CreateDigitizer();

   ClassDef(TATWactNtuMC,0)
};

#endif
