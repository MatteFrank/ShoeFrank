#ifndef _TAITactNtuTrackC_HXX
#define _TAITactNtuTrackC_HXX
/*!
 \file
 \version $Id: TAITactNtuTrackC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuTrackC.
 */
/*------------------------------------------+---------------------------------*/
class TAVTvertex;
#include "TAITactBaseNtuTrack.hxx"

class TAITactNtuTrackC : public TAITactBaseNtuTrack {
   
public:
   
   explicit  TAITactNtuTrackC(const char* name       = 0,
							 TAGdataDsc* p_ntuclus  = 0, 
							 TAGdataDsc* p_ntutrack = 0, 
							 TAGparaDsc* p_config   = 0,
							 TAGparaDsc* p_geomap   = 0,
							 TAGparaDsc* p_calib    = 0,
							 TAGdataDsc* p_vtvertex = 0);
   virtual ~TAITactNtuTrackC();
   
protected:
   virtual Bool_t FindTiltedTracks();
   virtual Bool_t CheckVtx();

protected:
   TAGdataDsc*  fpVtVertex;        // VT vertex container
   TAVTvertex*  fVtVertex;         // VT vertex pointer

   ClassDef(TAITactNtuTrackC,0)
};

#endif
