#ifndef _TAMCactNtuTof_HXX
#define _TAMCactNtuTof_HXX
/*!
  \file
  \version $Id: TAMCactNtuTof.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuTof.
*/
/*------------------------------------------+---------------------------------*/

#include "EventStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TAMCactNtuTof : public TAGaction {
public:
    explicit       TAMCactNtuTof(const char* name     = 0,
                                 TAGdataDsc* p_nturaw = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual       ~TAMCactNtuTof();

    virtual Bool_t  Action();

private:
   TAGdataDsc*     fpNtuMC;		    // output data dsc
   EVENT_STRUCT*   fpEvtStr;
   
   ClassDef(TAMCactNtuTof,0)
};

#endif
