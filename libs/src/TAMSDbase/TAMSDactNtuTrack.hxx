#ifndef _TAMSDactNtuTrack_HXX
#define _TAMSDactNtuTrack_HXX
/*!
 \file
 \version $Id: TAMSDactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/
class TAVTvertex;
#include "TAMSDactBaseNtuTrack.hxx"

class TAMSDactNtuTrack : public TAMSDactBaseNtuTrack {
   
public:
   
   explicit  TAMSDactNtuTrack(const char* name       = 0,
							 TAGdataDsc* p_ntuclus  = 0, 
							 TAGdataDsc* p_ntutrack = 0, 
							 TAGparaDsc* p_config   = 0,
							 TAGparaDsc* p_geomap   = 0,
							 TAGparaDsc* p_calib    = 0,
							 TAGdataDsc* p_bmtrack  = 0);
   virtual ~TAMSDactNtuTrack();
   
protected:
   virtual Bool_t FindTiltedTracks();
      
 
   ClassDef(TAMSDactNtuTrack,0)
};

#endif
