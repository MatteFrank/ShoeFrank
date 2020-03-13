#ifndef _TAVTactNtuTrackH_HXX
#define _TAVTactNtuTrackH_HXX


#include "TAVTactBaseNtuTrack.hxx"

class TH2F;
class TList;
class TAVTactNtuTrackH : public TAVTactBaseNtuTrack {
   
public:
   
   explicit  TAVTactNtuTrackH(const char* name      = 0,
							 TAGdataDsc* p_ntuclus  = 0, 
							 TAGdataDsc* p_ntutrack = 0, 
							 TAGparaDsc* p_config   = 0,
							 TAGparaDsc* p_geomap   = 0,
							 TAGparaDsc* p_calib    = 0,
							 TAGdataDsc* p_bmtrack  = 0);
   virtual ~TAVTactNtuTrackH();
	   
   //! Get list of clusters
   TList*  GetListOfVertices() const { return   fListOfVertices; }

private:
   TH2F*   fpHisHoughXZ;		// XZ histogram for Hough transform
   TH2F*   fpHisHoughYZ;		// YZ histogram for Hough transform
   TList*  fListOfVertices;     // list of tracks' intersection on target

private:    
   
   Bool_t FindTiltedTracks();
   Bool_t IsGoodCandidate(TAVTtrack* track);
   Bool_t FindVertices();

   ClassDef(TAVTactNtuTrackH,0)
};

#endif
