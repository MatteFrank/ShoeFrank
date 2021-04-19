#ifndef _TAITactNtuClusterF_HXX
#define _TAITactNtuClusterF_HXX
/*!
 \file
 \version $Id: TAITactNtuClusterF.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuClusterF.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuCluster.hxx"

class TAITntuCluster;
class TAVThit;
class TAVTbaseParGeo;

class TAITactNtuClusterF : public TAVTactBaseNtuCluster {
   
public:
   explicit  TAITactNtuClusterF(const char* name     = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAITactNtuClusterF();
   
   //! Action
   virtual  Bool_t Action();

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor);
   
private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   
private:
   Bool_t  CreateClusters(Int_t iSensor);


   ClassDef(TAITactNtuClusterF,0)
};

#endif
