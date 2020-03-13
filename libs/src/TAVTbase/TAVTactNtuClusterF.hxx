#ifndef _TAVTactNtuClusterF_HXX
#define _TAVTactNtuClusterF_HXX
/*!
 \file
 \version $Id: TAVTactNtuClusterF.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactNtuClusterF.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuCluster.hxx"

class TAVTntuCluster;
class TAVTntuHit;
class TAVTparGeo;

class TAVTactNtuClusterF : public TAVTactBaseNtuCluster {
   
public:
   explicit  TAVTactNtuClusterF(const char* name     = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAVTactNtuClusterF();
   
   //! Action
   virtual  Bool_t Action();
   

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor);
   
private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   
private:
   Bool_t  CreateClusters(Int_t iSensor);

};

#endif
