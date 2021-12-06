#ifndef _TAITactNtuTrack_HXX
#define _TAITactNtuTrack_HXX
/*!
 \file TAITactNtuTrack.hxx
 \brief   Declaration of TAITactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/
#include "TAVTactNtuTrack.hxx"

class TAVTbaseParGeo;
class TAVTbaseParConf;

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

   ClassDef(TAITactNtuTrack,0)
};

#endif
