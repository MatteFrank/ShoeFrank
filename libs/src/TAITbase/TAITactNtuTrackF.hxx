#ifndef _TAITactNtuTrackF_HXX
#define _TAITactNtuTrackF_HXX
/*!
 \file
 \version $Id: TAITactNtuTrackF.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuTrackF.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactNtuTrackF.hxx"

class TAGbaseTrack;
class TAVTbaseParGeo;
class TAVTbaseParConf;
class TAITactNtuTrackF : public TAVTactNtuTrackF {
   
public:
   explicit  TAITactNtuTrackF(const char* name       = 0,
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
   Bool_t                   IsGoodCandidate(TAGbaseTrack* track);
   
   void                     AddNewTrack(TAGbaseTrack* track);
   TAGbaseTrack*            NewTrack();
   Int_t                    GetTracksN();
   
   TAGcluster*              GetCluster(Int_t iPlane, Int_t iClus);
   Int_t                    GetClustersN(Int_t iPlane);
   
   void                     SetBeamPosition(TVector3 pos);
   
   virtual TAVTbaseParGeo*  GetParGeo();
   virtual TAVTbaseParConf* GetParConf();
   
   ClassDef(TAITactNtuTrackF,0)
};

#endif
