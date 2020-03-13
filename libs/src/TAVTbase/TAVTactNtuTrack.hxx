#ifndef _TAVTactNtuTrack_HXX
#define _TAVTactNtuTrack_HXX
/*!
 \file
 \version $Id: TAVTactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

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
   virtual Bool_t FindTiltedTracks();
      
 
   ClassDef(TAVTactNtuTrack,0)
};

#endif
