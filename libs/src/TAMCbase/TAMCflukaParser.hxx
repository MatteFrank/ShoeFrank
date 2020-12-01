#ifndef _TAMCflukaParser_HXX
#define _TAMCflukaParser_HXX
/*!
 \file
 \version $Id: TAMCflukaParser.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMCflukaParser.
 */
/*------------------------------------------+---------------------------------*/

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"
#include "Evento.hxx"

#include "TAGobject.hxx"
#include "TAGdataDsc.hxx"

class TAMCflukaParser : public TAGobject {
public:
  explicit       TAMCflukaParser();
  
  virtual       ~TAMCflukaParser();
  
public:
  static TAMCntuHit*  GetStcHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  static TAMCntuHit*  GetBmHits( EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  static TAMCntuHit*  GetVtxHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  static TAMCntuHit*  GetItrHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  static TAMCntuHit*  GetMsdHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  static TAMCntuHit*  GetTofHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  static TAMCntuHit*  GetCalHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit);
  static TAMCntuEve*  GetTracks( EVENT_STRUCT* evStr, TAGdataDsc* p_ntutrck);
};

#endif
