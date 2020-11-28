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
  explicit       TAMCflukaParser(EVENT_STRUCT* evStr   = 0,
                                 TAGdataDsc* p_ntutrck = 0,
                                 TAGdataDsc* p_ntuhit  = 0);
  
  virtual       ~TAMCflukaParser();
  
  TAMCntuHit*    GetStcHits();
  TAMCntuHit*    GetBmHits();
  TAMCntuHit*    GetVtxHits();
  TAMCntuHit*    GetItrHits();
  TAMCntuHit*    GetMsdHits();
  TAMCntuHit*    GetTofHits();
  TAMCntuHit*    GetCalHits();
  TAMCntuEve*    GetTracks();
  
private:
  EVENT_STRUCT*   fpEvtStr;
  TAGdataDsc*     fpNtuMCtrack;
  TAGdataDsc*     fpNtuMChit;
};

#endif
