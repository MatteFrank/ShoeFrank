#ifndef _TAITactNtuTrack_HXX
#define _TAITactNtuTrack_HXX
/*!
 \file
 \version $Id: TAITactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/
class TAVTvertex;
#include "TAITactBaseNtuTrack.hxx"

class TAITactNtuTrack : public TAITactBaseNtuTrack {
   
public:
   
   explicit  TAITactNtuTrack(const char* name       = 0,
							 TAGdataDsc* p_ntuclus  = 0, 
							 TAGdataDsc* p_ntutrack = 0, 
							 TAGparaDsc* p_config   = 0,
							 TAGparaDsc* p_geomap   = 0,
							 TAGparaDsc* p_calib    = 0,
							 TAGdataDsc* p_vtvertex = 0);
   virtual ~TAITactNtuTrack();
   
protected:
   virtual Bool_t FindTiltedTracks();
   void    CheckVtx();

protected:
   TAGdataDsc*  fpVtVertex;        // VT vertex container
   TAVTvertex*  fVtVertex;         // VT vertex pointer
   TVector3     fVtVertexPos;      // VT vertex position
   Bool_t       fVtVertexOk;       // VT vertex bool

   ClassDef(TAITactNtuTrack,0)
};

#endif
