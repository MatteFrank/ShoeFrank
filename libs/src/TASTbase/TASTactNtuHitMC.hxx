#ifndef _TASTactNtuHitMC_HXX
#define _TASTactNtuHitMC_HXX
/*!
  \file TASTactNtuHitMC.hxx
  \brief   Declaration of TASTactNtuHitMC.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>

#include "TASTntuHit.hxx"

#include "TAMCflukaStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TASTdigitizer;
class TASTactNtuHitMC : public TAGaction {
  public:
    explicit        TASTactNtuHitMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_ntuEve=0, TAGdataDsc* p_ntuHit=0, EVENT_STRUCT* evStr=0);
   
    virtual         ~TASTactNtuHitMC();

    virtual Bool_t  Action();


  private:
    TAGdataDsc*        fpNtuMC;          // input mc hit
    TAGdataDsc*        fpNtuEve;         // input eve track dsc
    TAGdataDsc*        fpNtuHit;		    // output data dsc

    TASTdigitizer*     fDigitizer;       // cluster size digitizer

    EVENT_STRUCT*      fEventStruct;

    vector<TASThit*>   fVecStHit;     //! vector to store ST Hits

  private:
    void            CreateDigitizer();
    void            FlagUnderEnergyThresholtHits(TASThit *hitst);   

  ClassDef(TASTactNtuHitMC,0)

};

#endif
