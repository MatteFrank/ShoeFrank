#ifndef _TAMCactNtuPart_HXX
#define _TAMCactNtuPart_HXX
/*!
  \file
  \version $Id: TAMCactNtuPart.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TAMCactNtuPart.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaParser.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAGrecoManager.hxx"

class TAMCactNtuPart : public TAGaction {
  public:
    explicit        TAMCactNtuPart(const char* name=0,
                                  TAGdataDsc* p_nturaw=0,
                                  EVENT_STRUCT* evStr=0);
    virtual         ~TAMCactNtuPart();

    virtual Bool_t  Action();

    ClassDef(TAMCactNtuPart,0)

  private:
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    EVENT_STRUCT*   fpEvtStr;
};

#endif
