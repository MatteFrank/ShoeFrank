#ifndef _TAMCactNtuRegion_HXX
#define _TAMCactNtuRegion_HXX
/*!
  \file TAMCactNtuRegion.hxx
  \brief   Declaration of TAMCactNtuRegion.
*/
/*------------------------------------------+---------------------------------*/

#include "TAMCflukaParser.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAGrecoManager.hxx"

class TAMCactNtuRegion : public TAGaction {
  public:
    explicit        TAMCactNtuRegion(const char* name=0,
                                  TAGdataDsc* p_nturaw=0,
                                  EVENT_STRUCT* evStr=0);
    virtual         ~TAMCactNtuRegion();

   //! Action
    virtual Bool_t  Action();

    ClassDef(TAMCactNtuRegion,0)

  private:
    TAGdataDsc*     fpNtuMC;	 ///< output data dsc
    EVENT_STRUCT*   fpEvtStr; ///< Fluka structure
};

#endif
