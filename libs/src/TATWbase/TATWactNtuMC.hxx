#ifndef _TATWactNtuMC_HXX
#define _TATWactNtuMC_HXX
/*!
  \file
  \version $Id: TATWactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TATWactNtuMC.
*/
/*------------------------------------------+---------------------------------*/
 #include <vector>

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

#include "Evento.hxx"

#include "TATWntuRaw.hxx"
#include "TATWparGeo.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TATWdigitizer;

class TATWactNtuMC : public TAGaction {
public:
  explicit TATWactNtuMC(const char* name=0, TAGdataDsc* p_hitraw=0, TAGparaDsc* p_parcal=0, TAGparaDsc *p_pargeo=0, EVENT_STRUCT* evStr=0);
    virtual  ~TATWactNtuMC();

    virtual bool  Action();
   
    void          CreateHistogram();

private:
    TAGdataDsc*     m_hitContainer;	// output data dsc
    TAGparaDsc*     fpCalPar;           // calibration parameter dsc
    TAGparaDsc*     fParGeo;            // beam parameter dsc
    EVENT_STRUCT*   m_eventStruct;
    TATWdigitizer*  m_Digitizer;        // digitizer
  
    int cnt = 0;
    int cntWrong = 0;
   
    TH1F* fpHisHitCol;
    TH1F* fpHisHitLine;
    TH2F* fpHisHitMap;
    TH1F* fpHisDeTot;
    TH1F* fpHisDeTotMc;
    TH1F* fpHisTimeTot;
    TH1F* fpHisTimeTotMc;
    TH2I* fpHisZID;
    TH2I* fpHisZID_MCtrue;
    TH2D* fpHisElossTof_MCrec[TATWparCal::kLayers];
    TH2D* fpHisElossTof_MCtrue[TATWparCal::kLayers];
    // vector of histo with the same size of the ion beam atomic number
    vector<TH2D*> fpHisElossTof_MC;
    vector<TH2D*> fpHisElossTof;
    vector<TH1F*> fpHisDistZ;
   
private:
    void          CreateDigitizer();

   ClassDef(TATWactNtuMC,0)
};

#endif
