#ifndef _TAMCactNtuItr_HXX
#define _TAMCactNtuItr_HXX
/*!
  \file
  \version $Id: TAMCactNtuItr.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuItr.
*/
/*------------------------------------------+---------------------------------*/

#include "EventStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TAMCactNtuItr : public TAGaction {
public:
    explicit       TAMCactNtuItr(const char* name     = 0,
                                TAGdataDsc* p_nturaw = 0,
                                EVENT_STRUCT* evStr  = 0);
   
    virtual       ~TAMCactNtuItr();

    virtual Bool_t  Action();

private:
   TAGdataDsc*     fpNtuMC;		    // output data dsc
   EVENT_STRUCT*   fpEvtStr;
   
   ClassDef(TAMCactNtuItr,0)
};

#endif
