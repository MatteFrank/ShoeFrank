#ifndef _TAMCactNtuRegion_HXX
#define _TAMCactNtuRegion_HXX
/*!
  \file
  \version $Id: TAMCactNtuRegion.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuRegion.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaParser.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "GlobalPar.hxx"

class TAMCactNtuRegion : public TAGaction {
  public:
    explicit        TAMCactNtuRegion(const char* name=0,
                                  TAGdataDsc* p_nturaw=0,
                                  EVENT_STRUCT* evStr=0);
    virtual         ~TAMCactNtuRegion();

    virtual Bool_t  Action();

    ClassDef(TAMCactNtuRegion,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
};

#endif
