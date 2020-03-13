#ifndef _TAMCactNtuBm_HXX
#define _TAMCactNtuBm_HXX
/*!
  \file
  \version $Id: TAMCactNtuBm.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuBm.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TAMCactNtuBm : public TAGaction {
public:
    explicit       TAMCactNtuBm(const char* name     = 0,
                                 TAGdataDsc* p_nturaw = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual       ~TAMCactNtuBm();

    virtual Bool_t  Action();

private:
   TAGdataDsc*     fpNtuMC;		    // output data dsc
   EVENT_STRUCT*   fpEvtStr;
   
   ClassDef(TAMCactNtuBm,0)
};

#endif
