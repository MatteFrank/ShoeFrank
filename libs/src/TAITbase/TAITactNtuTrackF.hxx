#ifndef _TAITactNtuTrackF_HXX
#define _TAITactNtuTrackF_HXX
/*!
 \file
 \version $Id: TAITactNtuTrackF.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuTrackF.
 */
/*------------------------------------------+---------------------------------*/


#include "TAITactBaseNtuTrack.hxx"


class TAITactNtuTrackF : public TAITactBaseNtuTrack {
   
public:
   explicit  TAITactNtuTrackF(const char* name      = 0,
                              TAGdataDsc* p_ntuclus  = 0,
                              TAGdataDsc* p_ntutrack = 0,
                              TAGparaDsc* p_config   = 0,
                              TAGparaDsc* p_geomap   = 0,
                              TAGparaDsc* p_calib    = 0,
                              TAGparaDsc* p_geo_g    = 0);
   
   virtual ~TAITactNtuTrackF();
   
private:
   TAGparaDsc*     fpGeoMapG;       // Global geometry para dsc
   
private:
   Bool_t FindTiltedTracks();
   Bool_t IsGoodCandidate(TAITtrack* track);
   
   ClassDef(TAITactNtuTrackF,0)
};

#endif
