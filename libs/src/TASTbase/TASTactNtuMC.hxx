#ifndef _TASTactNtuMC_HXX
#define _TASTactNtuMC_HXX
/*!
  \file
  \version $Id: TASTactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TASTactNtuMC.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TASTdigitizer;
class TASTactNtuMC : public TAGaction {
  public:
    explicit        TASTactNtuMC(const char* name     = 0,
                                 TAGdataDsc* p_datraw = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual         ~TASTactNtuMC();

    virtual Bool_t  Action();

    ClassDef(TASTactNtuMC,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
    TASTdigitizer*  fDigitizer;       // cluster size digitizer

  private:
    void            CreateDigitizer();
   

};

#endif
