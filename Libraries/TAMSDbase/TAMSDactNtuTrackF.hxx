#ifndef _TAMSDactNtuTrackF_HXX
#define _TAMSDactNtuTrackF_HXX
/*!
 \file TAMSDactNtuTrackF.hxx
 \brief   Declaration of TAMSDactNtuTrackF.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactNtuTrackF.hxx"

class TAVTbaseParGeo;
class TAVTbaseParConf;
class TAMSDactNtuTrackF : public TAVTactNtuTrackF {
   
public:
   explicit  TAMSDactNtuTrackF(const char* name      = 0,
                              TAGdataDsc* p_ntuclus  = 0,
                              TAGdataDsc* p_ntutrack = 0,
                              TAGparaDsc* p_config   = 0,
                              TAGparaDsc* p_geomap   = 0,
                              TAGparaDsc* p_geo_g    = 0);
   
   virtual ~TAMSDactNtuTrackF();
   
private:
   TAGparaDsc*              fpGeoMapG;       ///< target geometry para dsc
   
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

   ClassDef(TAMSDactNtuTrackF,0)
};

#endif
