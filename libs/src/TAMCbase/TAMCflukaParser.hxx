#ifndef _TAMCflukaParser_HXX
#define _TAMCflukaParser_HXX
/*!
 \file TAMCflukaParser.hxx
 \brief   Declaration of TAMCflukaParser.
 */
/*------------------------------------------+---------------------------------*/
#include "TTree.h"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCflukaStruct.hxx"

#include "TAGobject.hxx"
#include "TAGdataDsc.hxx"

class TAMCflukaParser : public TAGobject {
public:
  explicit       TAMCflukaParser();
  virtual       ~TAMCflukaParser();
  
public:
  //! Get MC Stc hits
  static TAMCntuHit*    GetStcHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  //! Get MC BM hits
  static TAMCntuHit*    GetBmHits( EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  //! Get MC VTX hits
  static TAMCntuHit*    GetVtxHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  //! Get MC ITR hits
  static TAMCntuHit*    GetItrHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  //! Get MC MSD hits
  static TAMCntuHit*    GetMsdHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  //! Get MC TW hits
  static TAMCntuHit*    GetTwHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  //! Get MC CAL hits
  static TAMCntuHit*    GetCalHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  //! Get MC tracks
  static TAMCntuPart*  GetTracks( EVENT_STRUCT* evStr, TAGdataDsc* p_ntutrck);
  //! Get MC regions
  static TAMCntuRegion* GetRegions(EVENT_STRUCT* evStr, TAGdataDsc* p_ntureg);
  //! Get MC events
  static TAMCntuEvent*  GetEvents(EVENT_STRUCT* evStr,  TAGdataDsc* p_ntuevt);

  //! Find branches in tree
  static void           FindBranches(TTree* rootTree, EVENT_STRUCT* evStr);
  //! Create branches in tree
  static void           CreateBranches(TTree* rootTree, EVENT_STRUCT* evStr, Bool_t reg = false);
  //! Reset
  static Bool_t         ResetEvent(EVENT_STRUCT* evStr);

};

#endif
