#ifndef _TAVTactNtuClusterMT_HXX
#define _TAVTactNtuClusterMT_HXX
/*!
 \file
 \version $Id: TAVTactNtuClusterMT.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactNtuClusterMT.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuClusterMT.hxx"

class TAVTntuCluster;
class TAVTntuHit;
class TAVTparGeo;

class TAVTactNtuClusterMT : public TAVTactBaseNtuClusterMT {
   
public:
   explicit  TAVTactNtuClusterMT(const char* name     = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAVTactNtuClusterMT();
   
   //! Action
   virtual  Bool_t Action();
   

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);
   
private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   
private:
   Bool_t  CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);

   ClassDef(TAVTactNtuClusterMT,0)
};

#endif
