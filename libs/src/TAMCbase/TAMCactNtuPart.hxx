#ifndef _TAMCactNtuPart_HXX
#define _TAMCactNtuPart_HXX
/*!
  \file TAMCactNtuPart.hxx
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

   //! Action
    virtual Bool_t  Action();

  private:
    TAGdataDsc*     fpNtuMC;	///< output data dsc
    EVENT_STRUCT*   fpEvtStr; ///< Fluka structure
   
   ClassDef(TAMCactNtuPart,0)
};

#endif
