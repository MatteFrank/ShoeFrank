#ifndef _TAITactNtuCluster_HXX
#define _TAITactNtuCluster_HXX
/*!
 \file TAITactNtuCluster.hxx
 \version $Id: TAITactNtuCluster.hxx
 \brief   Declaration of TAITactNtuCluster.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuCluster.hxx"

class TAITntuCluster;
class TAVThit;
class TAVTbaseParGeo;

class TAITactNtuCluster : public TAVTactBaseNtuCluster {
   
public:
   explicit  TAITactNtuCluster(const char* name     = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAITactNtuCluster();
   
   //! Action
   virtual  Bool_t Action();

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor);
   
private:
   TAGdataDsc*     fpNtuRaw;		  /// input data dsc
   TAGdataDsc*     fpNtuClus;		  /// output data dsc
   
private:
   //! Create cluster
   Bool_t  CreateClusters(Int_t iSensor);

   ClassDef(TAITactNtuCluster,0)
};

#endif
