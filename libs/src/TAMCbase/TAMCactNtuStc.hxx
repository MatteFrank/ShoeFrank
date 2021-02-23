#ifndef _TAMCactNtuStc_HXX
#define _TAMCactNtuStc_HXX
/*!
  \file
  \version $Id: TAMCactNtuStc.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuStc.
*/
/*------------------------------------------+---------------------------------*/

#include "EventStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TAMCactNtuStc : public TAGaction {
public:
    explicit       TAMCactNtuStc(const char* name     = 0,
                                 TAGdataDsc* p_nturaw = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual       ~TAMCactNtuStc();

    virtual Bool_t  Action();

private:
   TAGdataDsc*     fpNtuMC;		    // output data dsc
   EVENT_STRUCT*   fpEvtStr;
   
   ClassDef(TAMCactNtuStc,0)
};

#endif
