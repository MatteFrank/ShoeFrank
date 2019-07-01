#ifndef _TAITactBaseNtuTrack_HXX
#define _TAITactBaseNtuTrack_HXX
/*!
 \file
 \version $Id: TAITactBaseNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactBaseNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"

#include "TAVTactBaseTrack.hxx"

#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"


class TAGgeoTrafo;
class TABMntuTrackTr;
class TGraphErrors;
class TAVTbaseCluster;
class TAVTntuHit;
class TAVTbaseTrack;
class TH2F;
class TH1F;
class TAITactBaseNtuTrack : public TAVTactBaseTrack {
   
public:
   explicit  TAITactBaseNtuTrack(const char* name       = 0,
                                 TAGdataDsc* p_ntuclus  = 0,
                                 TAGdataDsc* p_ntutrack = 0,
                                 TAGparaDsc* p_config   = 0,
                                 TAGparaDsc* p_geomap   = 0,
                                 TAGparaDsc* p_calib    = 0);
   
   virtual ~TAITactBaseNtuTrack();
   
   // Base action
   virtual         Bool_t  Action();
   
protected:
   TAVTbaseTrack* GetTrack(Int_t idx);
   Int_t          GetTracksN() const;
      
   ClassDef(TAITactBaseNtuTrack,0)
};

#endif
