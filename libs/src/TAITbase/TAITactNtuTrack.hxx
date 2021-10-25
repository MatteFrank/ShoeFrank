#ifndef _TAITactNtuTrack_HXX
#define _TAITactNtuTrack_HXX
/*!
 \file
 \version $Id: TAITactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/
class TAVTvertex;
#include "TAVTactNtuTrack.hxx"

class TAITactNtuTrack : public TAVTactNtuTrack {
   
public:
   
   explicit  TAITactNtuTrack(const char* name       = 0,
                             TAGdataDsc* p_ntuclus  = 0,
                             TAGdataDsc* p_ntutrack = 0,
                             TAGparaDsc* p_config   = 0,
                             TAGparaDsc* p_geomap   = 0,
                             TAGparaDsc* p_calib    = 0,
                             TAGdataDsc* p_bmtrack  = 0);
   virtual ~TAITactNtuTrack();
   
protected:   
   void          AddNewTrack(TAGbaseTrack* track);
   TAGbaseTrack* NewTrack();
   Int_t         GetTracksN();
   
   TAGcluster*   GetCluster(Int_t iPlane, Int_t iClus);
   Int_t         GetClustersN(Int_t iPlane);
 
   ClassDef(TAITactNtuTrack,0)
};

#endif
