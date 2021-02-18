#ifndef _TAMCactNtuEve_HXX
#define _TAMCactNtuEve_HXX
/*!
  \file
  \version $Id: TAMCactNtuEve.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuEve.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaParser.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "GlobalPar.hxx"

class TAMCactNtuEve : public TAGaction {
  public:
    explicit        TAMCactNtuEve(const char* name=0,
                                  TAGdataDsc* p_nturaw=0,
                                  EVENT_STRUCT* evStr=0);
    virtual         ~TAMCactNtuEve();

    virtual Bool_t  Action();

    ClassDef(TAMCactNtuEve,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
};

#endif
