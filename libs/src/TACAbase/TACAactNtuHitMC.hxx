#ifndef _TACAactNtuHitMC_HXX
#define _TACAactNtuHitMC_HXX
/*!
  \file
  \version $Id: TACAactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TACAactNtuHitMC.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TH2I.h"
#include "TH1F.h"
#include "TH2F.h"

class TACAdigitizer;



class TACAactNtuHitMC : public TAGaction {
   
private:
   // Helper class to sum MC hit of the same particle
   struct EnergyDep_t : public TObject {
      EnergyDep_t( int iEvn, int icry, int idpart, float ti, double de ) :
      index(iEvn), fn(1), fCryid(icry), fid(idpart), fTimeFirstHit(ti), fDE(de) {}
      int index;             // index in EvnStr
      int fn;                // number of Edep
      int fCryid;            // crystal index hit
      int fid;               // index in the particle block
      float fTimeFirstHit;   // dep. time at FIRST hit
      double fDE;            // sum Edep
      
   };
   
  public:
    explicit       TACAactNtuHitMC(const char* name     = 0,
                                    TAGdataDsc* p_ntuMC  = 0,
                                    TAGdataDsc* p_ntuEve = 0,
                                    TAGdataDsc* p_nturaw = 0,
                                    TAGparaDsc* p_geomap = 0);
   
    virtual        ~TACAactNtuHitMC();

    virtual Bool_t Action();

    void           CreateHistogram();

    ClassDef(TACAactNtuHitMC,0)

  private:
   TAGdataDsc*     fpNtuMC;     // input mc hit
   TAGdataDsc*     fpNtuEve;    // input eve track dsc
   TAGdataDsc*     fpNtuRaw;		    // output data dsc
   TAGparaDsc*     fpGeoMap;          // geometry para dsc

   TACAdigitizer*  fDigitizer;       // cluster size digitizer
   
   TH1F*           fpHisMass;
   TH1F*           fpHisEnergy;
   TH2F*           fpHisEnergyReleasePosXY;
   TH2F*           fpHisEnergyReleasePosZY_in;
   TH2F*           fpHisEnergyReleasePosZY_out;
   TH2F*           fpHisFinalPositionVsMass;
   TH2F*           fpHisChargeVsMass;
   TH2I*           fpHistypeParticleVsRegion;
   TH1F*           fpHisEnergyNeutron;
   TH1F*           fpHistimeFirstHit;
   TH1F*           fpHisEnergyIon[8];
   TH1F*           fpHisEnergyIonSpect[8];
   TH1F*           fpHisEnergyDep[8];
   TH2F*           fpHisP_vs_EDepIon[8];

private:
   void           CreateDigitizer();

};

#endif
