#ifndef _TAIRactNtuTrack_HXX
#define _TAIRactNtuTrack_HXX
/*!
 \file
 \version $Id: TAIRactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAIRactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseTrack.hxx"

class TAVTvertex;
class TAVTtrack;
class TAIRtrack;
class TAIRactNtuTrack : public TAVTactBaseTrack {
   
public:
   explicit  TAIRactNtuTrack(const char* name       = 0,
                             TAGdataDsc* p_ntuclus  = 0,
                             TAGdataDsc* p_ntutrack = 0,
                             TAGparaDsc* p_config   = 0,
                             TAGparaDsc* p_geomap   = 0,
                             TAGparaDsc* p_calib    = 0,
                             TAGdataDsc* p_vtvertex = 0);
   virtual ~TAIRactNtuTrack();
   
   virtual  Bool_t  Action();

protected:
    Bool_t FindTracks();
    Bool_t CheckVtx();
   TAIRtrack* FillTracks(TAVTtrack* vtTrack);
   
protected:
   TAGdataDsc*  fpVtVertex;        // VT vertex container
   TAVTvertex*  fVtVertex;         // VT vertex pointer

   ClassDef(TAIRactNtuTrack,0)
};

#endif
