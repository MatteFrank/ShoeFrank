#ifndef _TAMCactNtuMsd_HXX
#define _TAMCactNtuMsd_HXX
/*!
  \file
  \version $Id: TAMCactNtuMsd.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuMsd.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TAMCactNtuMsd : public TAGaction {
public:
    explicit       TAMCactNtuMsd(const char* name     = 0,
                                 TAGdataDsc* p_nturaw = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual       ~TAMCactNtuMsd();

    virtual Bool_t  Action();

private:
   TAGdataDsc*     fpNtuMC;		    // output data dsc
   EVENT_STRUCT*   fpEvtStr;
   
   ClassDef(TAMCactNtuMsd,0)
};

#endif
