#ifndef _TAMCactNtuEvent_HXX
#define _TAMCactNtuEvent_HXX
/*!
  \file
  \version $Id: TAMCactNtuEvent.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuEvent.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaParser.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "GlobalPar.hxx"

class TAMCactNtuEvent : public TAGaction {
  public:
    explicit        TAMCactNtuEvent(const char* name=0,
                                  TAGdataDsc* p_nturaw=0,
                                  EVENT_STRUCT* evStr=0);
    virtual         ~TAMCactNtuEvent();

    virtual Bool_t  Action();

    ClassDef(TAMCactNtuEvent,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
};

#endif
