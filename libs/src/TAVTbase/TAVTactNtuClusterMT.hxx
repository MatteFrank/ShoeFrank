#ifndef _TAVTactNtuClusterMT_HXX
#define _TAVTactNtuClusterMT_HXX
/*!
 \file
 \version $Id: TAVTactNtuClusterMT.hxx
 \brief   Declaration of TAVTactNtuClusterMT.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuClusterMT.hxx"

class TAVTntuCluster;
class TAVThit;
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
   
   //! Get list of pixels
   virtual TClonesArray* GetListOfPixels(Int_t sensorId);

private:
   TAGdataDsc*     fpNtuRaw;		  /// input data dsc
   TAGdataDsc*     fpNtuClus;		  /// output data dsc
   Bool_t          fOk;            /// ok flag

private:
   //! Create cluster per thread
   Bool_t  CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);

   ClassDef(TAVTactNtuClusterMT,0)
};

#endif
