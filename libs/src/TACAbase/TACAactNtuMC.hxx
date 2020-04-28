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

// Helper class to sum MC hit of the same particle
class energyDep : public TObject {
public:
   energyDep( int iEvn, int icry, int idpart, float ti, double de ) :
   index(iEvn), fn(1), fCryid(icry), fid(idpart), fTimeFirstHit(ti), fDE(de) {}
   int index;             // index in EvnStr
   int fn;                // number of Edep
   int fCryid;            // crystal index hit
   int fid;               // index in the particle block
   float fTimeFirstHit;   // dep. time at FIRST hit
   double fDE;            // sum Edep
   
   ClassDef(energyDep,0)
   
};

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
