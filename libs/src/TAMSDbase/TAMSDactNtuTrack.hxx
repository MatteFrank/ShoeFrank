#ifndef _TAMSDactNtuTrack_HXX
#define _TAMSDactNtuTrack_HXX
/*!
 \file
 \version $Id: TAMSDactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/
class TAVTvertex;
#include "TAVTactNtuTrack.hxx"

class TAMSDactNtuTrack : public TAVTactNtuTrack {
   
public:
   
   explicit  TAMSDactNtuTrack(const char* name       = 0,
                              TAGdataDsc* p_ntuclus  = 0,
                              TAGdataDsc* p_ntutrack = 0,
                              TAGparaDsc* p_config   = 0,
                              TAGparaDsc* p_geomap   = 0);
   virtual ~TAMSDactNtuTrack();
   
protected:
   Bool_t        IsGoodCandidate(TAGbaseTrack* track);
   
   void          AddNewTrack(TAGbaseTrack* track);
   TAGbaseTrack* NewTrack();
   Int_t         GetTracksN();
   
   TAGcluster*   GetCluster(Int_t iPlane, Int_t iClus);
   Int_t         GetClustersN(Int_t iPlane);
   
   ClassDef(TAMSDactNtuTrack,0)
};

#endif
