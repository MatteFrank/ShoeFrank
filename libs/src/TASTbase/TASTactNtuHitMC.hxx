#ifndef _TASTactNtuHitMC_HXX
#define _TASTactNtuHitMC_HXX
/*!
  \file
  \version $Id: TASTactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TASTactNtuHitMC.
*/
/*------------------------------------------+---------------------------------*/

#include "EventStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TASTdigitizer;
class TASTactNtuHitMC : public TAGaction {
  public:
    explicit        TASTactNtuHitMC(const char* name=0, TAGdataDsc* p_ntuMC=0, TAGdataDsc* p_ntuEve=0, TAGdataDsc* p_nturaw=0, EVENT_STRUCT* evStr=0);
   
    virtual         ~TASTactNtuHitMC();

    virtual Bool_t  Action();


  private:
    TAGdataDsc*     fpNtuMC;          // input mc hit
    TAGdataDsc*     fpNtuEve;         // input eve track dsc
    TAGdataDsc*     fpNtuRaw;		    // output data dsc
    TASTdigitizer*  fDigitizer;       // cluster size digitizer
    EVENT_STRUCT*   fEventStruct;

  private:
    void            CreateDigitizer();
   
   ClassDef(TASTactNtuHitMC,0)

};

#endif
