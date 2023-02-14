#ifndef _TAMSDactNtuTrack_HXX
#define _TAMSDactNtuTrack_HXX
/*!
 \file TAMSDactNtuTrack.hxx
 \brief   Declaration of TAMSDactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactNtuTrack.hxx"
#include "TAGrecoManager.hxx"

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
   Bool_t   Action() override;
protected:
   void                     AddNewTrack(TAGbaseTrack* track) override;
   TAGbaseTrack*            NewTrack() override;
   Int_t                    GetTracksN() override;
   
   TAGcluster*              GetCluster(Int_t iPlane, Int_t iClus) override;
   Int_t                    GetClustersN(Int_t iPlane) override;
   
   void                     SetBeamPosition(TVector3 pos) override;
   
   virtual TAVTbaseParGeo*  GetParGeo() override;
   virtual TAVTbaseParConf* GetParConf() override;

   ClassDef(TAMSDactNtuTrack,0)
};

#endif
