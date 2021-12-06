#ifndef _TAVTactNtuCluster_HXX
#define _TAVTactNtuCluster_HXX
/*!
 \file
 \version $Id: TAVTactNtuCluster.hxx
 \brief   Declaration of TAVTactNtuCluster.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuCluster.hxx"

class TAVTntuCluster;
class TAVThit;
class TAVTparGeo;

class TAVTactNtuCluster : public TAVTactBaseNtuCluster {
   
public:
   explicit  TAVTactNtuCluster(const char* name     = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAVTactNtuCluster();
   
   //! Action
   virtual  Bool_t Action();
   

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor);
   
private:
   TAGdataDsc*     fpNtuRaw;		  ///< input data dsc
   TAGdataDsc*     fpNtuClus;		  ///< output data dsc
   
private:
   //! Create clusters
   Bool_t  CreateClusters(Int_t iSensor);

   ClassDef(TAVTactNtuCluster,0)
};

#endif
