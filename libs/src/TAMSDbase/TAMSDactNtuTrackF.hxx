#ifndef _TAMSDactNtuTrackF_HXX
#define _TAMSDactNtuTrackF_HXX
/*!
 \file
 \version $Id: TAMSDactNtuTrackF.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuTrackF.
 */
/*------------------------------------------+---------------------------------*/


#include "TAVTactNtuTrackF.hxx"


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
   TAGparaDsc*     fpGeoMapG;       // Global geometry para dsc
   
private:
 //  Bool_t FindTiltedTracks();
   Bool_t        IsGoodCandidate(TAGbaseTrack* track);
   
   void          AddNewTrack(TAGbaseTrack* track);
   TAGbaseTrack* NewTrack();
   Int_t         GetTracksN();
   
   TAGcluster*   GetCluster(Int_t iPlane, Int_t iClus);
   Int_t         GetClustersN(Int_t iPlane);
   ClassDef(TAMSDactNtuTrackF,0)
};

#endif
