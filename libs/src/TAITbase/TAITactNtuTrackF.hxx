#ifndef _TAITactNtuTrackF_HXX
#define _TAITactNtuTrackF_HXX
/*!
 \file TAITactNtuTrackF.hxx
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
   TAGparaDsc*     fpGeoMapG;       /// Global geometry para dsc
   
private:
   //! Check candidate
   Bool_t                   IsGoodCandidate(TAGbaseTrack* track);
   
   //! Add new track
   void                     AddNewTrack(TAGbaseTrack* track);
   //! Create new track
   TAGbaseTrack*            NewTrack();
   //! Get number of tracks
   Int_t                    GetTracksN();
   
   //! Get cluster per plane
   TAGcluster*              GetCluster(Int_t iPlane, Int_t iClus);
   //! Get number of clusters per plane
   Int_t                    GetClustersN(Int_t iPlane);
   //! Set beam position
   void                     SetBeamPosition(TVector3 pos);
   
   //! Get par geo pointer
   virtual TAVTbaseParGeo*  GetParGeo();
   //! Get conf par pointer
   virtual TAVTbaseParConf* GetParConf();
   
   ClassDef(TAITactNtuTrackF,0)
};

#endif
