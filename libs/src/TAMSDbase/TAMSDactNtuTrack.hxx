#ifndef _TAMSDactNtuTrack_HXX
#define _TAMSDactNtuTrack_HXX
/*!
 \file TAMSDactNtuTrack.hxx
 \brief   Declaration of TAMSDactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactNtuTrack.hxx"

class TAVTbaseParGeo;
class TAVTbaseParConf;
class TAMSDactNtuTrack : public TAVTactNtuTrack {
   
public:
   
   explicit  TAMSDactNtuTrack(const char* name       = 0,
                              TAGdataDsc* p_ntuclus  = 0,
                              TAGdataDsc* p_ntutrack = 0,
                              TAGparaDsc* p_config   = 0,
                              TAGparaDsc* p_geomap   = 0);
   virtual ~TAMSDactNtuTrack();
   
protected:
   void                     AddNewTrack(TAGbaseTrack* track);
   TAGbaseTrack*            NewTrack();
   Int_t                    GetTracksN();
   
   TAGcluster*              GetCluster(Int_t iPlane, Int_t iClus);
   Int_t                    GetClustersN(Int_t iPlane);
   
   void                     SetBeamPosition(TVector3 pos);
   
   virtual TAVTbaseParGeo*  GetParGeo();
   virtual TAVTbaseParConf* GetParConf();

   ClassDef(TAMSDactNtuTrack,0)
};

#endif
