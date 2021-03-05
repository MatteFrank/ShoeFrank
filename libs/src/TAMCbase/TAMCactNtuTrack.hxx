#ifndef _TAMCactNtuTrack_HXX
#define _TAMCactNtuTrack_HXX
/*!
  \file
  \version $Id: TAMCactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuTrack.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaParser.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "GlobalPar.hxx"

class TAMCactNtuTrack : public TAGaction {
  public:
    explicit        TAMCactNtuTrack(const char* name=0,
                                  TAGdataDsc* p_nturaw=0,
                                  EVENT_STRUCT* evStr=0);
    virtual         ~TAMCactNtuTrack();

    virtual Bool_t  Action();

    ClassDef(TAMCactNtuTrack,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
};

#endif
