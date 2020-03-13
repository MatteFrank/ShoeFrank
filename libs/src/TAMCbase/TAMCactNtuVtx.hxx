#ifndef _TAMCactNtuVtx_HXX
#define _TAMCactNtuVtx_HXX
/*!
  \file
  \version $Id: TAMCactNtuVtx.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuVtx.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TAMCactNtuVtx : public TAGaction {
public:
    explicit       TAMCactNtuVtx(const char* name     = 0,
                                 TAGdataDsc* p_nturaw = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual       ~TAMCactNtuVtx();

    virtual Bool_t  Action();

private:
   TAGdataDsc*     fpNtuMC;		    // output data dsc
   EVENT_STRUCT*   fpEvtStr;
   
   ClassDef(TAMCactNtuVtx,0)
};

#endif
