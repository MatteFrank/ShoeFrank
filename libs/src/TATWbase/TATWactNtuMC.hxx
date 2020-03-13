#ifndef _TATWactNtuMC_HXX
#define _TATWactNtuMC_HXX
/*!
  \file
  \version $Id: TATWactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TATWactNtuMC.
*/
/*------------------------------------------+---------------------------------*/

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
    explicit TATWactNtuMC(const char* name=0, TAGdataDsc* p_hitraw=0, EVENT_STRUCT* evStr=0);
    virtual  ~TATWactNtuMC();

    virtual bool  Action();
   
    void          CreateHistogram();

private:
    TAGdataDsc*     m_hitContainer;		// output data dsc
    EVENT_STRUCT*   m_eventStruct;
    TATWdigitizer*  m_Digitizer;       // digitizer

   
    TH1F* fpHisHitCol;
    TH1F* fpHisHitLine;
    TH2F* fpHisHitMap;
    TH1F* fpHisDeTot;
    TH1F* fpHisDeTotMc;
    TH1F* fpHisTimeTot;
    TH1F* fpHisTimeTotMc;
   
private:
    void          CreateDigitizer();

   ClassDef(TATWactNtuMC,0)
};

#endif
