#ifndef _TAVTactNtuTrackF_HXX
#define _TAVTactNtuTrackF_HXX
/*!
 \file
 \version $Id: TAVTactNtuTrackF.hxx
 \brief   Declaration of TAVTactNtuTrackF.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseNtuTrack.hxx"

using namespace std;
class TAVTactNtuTrackF : public TAVTactBaseNtuTrack {
   
public:
   
   explicit  TAVTactNtuTrackF(const char* name       = 0,
                              TAGdataDsc* p_ntuclus  = 0,
                              TAGdataDsc* p_ntutrack = 0,
                              TAGparaDsc* p_config   = 0,
                              TAGparaDsc* p_geomap   = 0,
                              TAGparaDsc* p_calib    = 0,
                              TAGdataDsc* p_bmtrack  = 0);
   virtual ~TAVTactNtuTrackF();
   
protected:
   //! Find tilted tracks
   Bool_t FindTiltedTracks();
   //! Check track candidate
   virtual Bool_t IsGoodCandidate(TAGbaseTrack* track);
   
protected:
   //! Cluster map
   map<TAGcluster*,  int> fMapClus;

   ClassDef(TAVTactNtuTrackF,0)
};

#endif
