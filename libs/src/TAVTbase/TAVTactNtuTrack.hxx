#ifndef _TAVTactNtuTrack_HXX
#define _TAVTactNtuTrack_HXX
/*!
 \file TAVTactNtuTrack.hxx
 \brief   Declaration of TAVTactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/
#include "TVector3.h"

#include "TAVTactBaseNtuTrack.hxx"

class TAVTactNtuTrack : public TAVTactBaseNtuTrack {
   
public:
   
   explicit  TAVTactNtuTrack(const char* name       = 0,
							 TAGdataDsc* p_ntuclus  = 0, 
							 TAGdataDsc* p_ntutrack = 0, 
							 TAGparaDsc* p_config   = 0,
							 TAGparaDsc* p_geomap   = 0,
							 TAGparaDsc* p_calib    = 0,
							 TAGdataDsc* p_bmtrack  = 0);
   virtual ~TAVTactNtuTrack();
   
protected:
   //! Find tilted tracks
   virtual Bool_t FindTiltedTracks();
 
   ClassDef(TAVTactNtuTrack,0)
};

#endif
