#ifndef _TAVTactNtuPostTrack_HXX
#define _TAVTactNtuPostTrack_HXX
/*!
 \file
 \version $Id: TAVTactNtuPostTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactNtuPostTrack.
 */
/*------------------------------------------+---------------------------------*/


#include "TAVTactBaseNtuTrack.hxx"


class TAVTactNtuPostTrack : public TAVTactBaseNtuTrack {
   
public:
   
   explicit  TAVTactNtuPostTrack(const char* name       = 0,
								 TAGdataDsc* p_ntuclus  = 0, 
								 TAGdataDsc* p_ntutrack = 0, 
								 TAGparaDsc* p_config   = 0,
								 TAGparaDsc* p_geomap   = 0,
								 TAGdataDsc* pNtuVertex = 0);
   
   virtual ~TAVTactNtuPostTrack();
   
   //! Action
   Bool_t  Action();
   
   //! Apply cuts
   Bool_t  AppyCuts(TAVTtrack* track);

private:
   TAGdataDsc* fpNtuVertex;		 // input data dsc

private:    
   
   Bool_t FindTiltedTracks();
   
   ClassDef(TAVTactNtuPostTrack,0)
};

#endif
