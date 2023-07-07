#ifndef _TAMPactNtuCluster_HXX
#define _TAMPactNtuCluster_HXX
/*!
 \file TAMPactNtuCluster.hxx
 \brief   Declaration of TAMPactNtuCluster.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuCluster.hxx"

class TAMPntuCluster;
class TAVThit;
class TAVTbaseParGeo;

class TAMPactNtuCluster : public TAVTactBaseNtuCluster {
   
public:
   explicit  TAMPactNtuCluster(const char* name     = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAMPactNtuCluster();
   
   // Action
   virtual  Bool_t Action();

   // Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor);
   
private:
   TAGdataDsc*     fpNtuRaw;		  ///< input data dsc
   TAGdataDsc*     fpNtuClus;		  ///< output data dsc
   
private:
   // Create cluster
   Bool_t  CreateClusters(Int_t iSensor);

   ClassDef(TAMPactNtuCluster,0)
};

#endif
