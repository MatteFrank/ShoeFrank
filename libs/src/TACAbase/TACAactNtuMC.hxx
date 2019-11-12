#ifndef _TACAactNtuMC_HXX
#define _TACAactNtuMC_HXX
/*!
  \file
  \version $Id: TACAactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TACAactNtuMC.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TH1F.h"
#include "TH2F.h"

class TACAdigitizer;
class TACAactNtuMC : public TAGaction {
  public:
    explicit        TACAactNtuMC(const char* name     = 0,
                                 TAGdataDsc* p_datraw = 0,
                                 TAGparaDsc* p_geomap = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual         ~TACAactNtuMC();

    virtual Bool_t  Action();

    void           CreateHistogram();

    ClassDef(TACAactNtuMC,0)

  private:
    TAGparaDsc*     fpGeoMap;		    // geometry para dsc
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
    TACAdigitizer*   fDigitizer;       // cluster size digitizer

   TH1F* fpHisDeTot;
   TH1F* fpHisMass;
   TH1F* fpHisTimeTot;
   TH1F* fpHisDeTotMc;
   TH1F* fpHisTimeTotMc;
   TH2F* fpHisHitMapXY;
   TH2F* fpHisHitMapZY;
   
private:
   void           CreateDigitizer();

};

#endif
