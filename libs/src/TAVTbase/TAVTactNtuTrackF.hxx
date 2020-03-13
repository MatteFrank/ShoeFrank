#ifndef _TAVTactNtuTrackF_HXX
#define _TAVTactNtuTrackF_HXX
/*!
 \file
 \version $Id: TAVTactNtuTrackF.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactNtuTrackF.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseNtuTrack.hxx"

using namespace std;
class TAVTactNtuTrackF : public TAVTactBaseNtuTrack {
   
public:
   
   explicit  TAVTactNtuTrackF(const char* name      = 0,
							 TAGdataDsc* p_ntuclus  = 0, 
							 TAGdataDsc* p_ntutrack = 0, 
							 TAGparaDsc* p_config   = 0,
							 TAGparaDsc* p_geomap   = 0,
							 TAGparaDsc* p_calib    = 0,
							 TAGdataDsc* p_bmtrack  = 0);
   virtual ~TAVTactNtuTrackF();
   
private:    
   
   Bool_t FindTiltedTracks();
   Bool_t IsGoodCandidate(TAVTtrack* track);
   
private:
   map<TAVTcluster*,  int> fMapClus;

};

#endif
