#ifndef _TAMCactNtuEvent_HXX
#define _TAMCactNtuEvent_HXX
/*!
  \file TAMCactNtuEvent.hxx
  \brief   Declaration of TAMCactNtuEvent.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaParser.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAGrecoManager.hxx"

class TAMCactNtuEvent : public TAGaction {
  public:
    explicit        TAMCactNtuEvent(const char* name=0,
                                  TAGdataDsc* p_nturaw=0,
                                  EVENT_STRUCT* evStr=0);
    virtual         ~TAMCactNtuEvent();

    //! Action
    virtual Bool_t  Action();

  private:
    TAGdataDsc*     fpNtuMC;   ///< output data dsc
    EVENT_STRUCT*   fpEvtStr;  ///< Fluka structure
   
   ClassDef(TAMCactNtuEvent,0)
};

#endif
