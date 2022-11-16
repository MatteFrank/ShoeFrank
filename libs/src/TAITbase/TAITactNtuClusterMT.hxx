#ifndef _TAITactNtuClusterMT_HXX
#define _TAITactNtuClusterMT_HXX
/*!
 \file TAITactNtuClusterMT.hxx
 \brief   Declaration of TAITactNtuClusterMT.
 */
/*------------------------------------------+---------------------------------*/

#include <thread>
#include <vector>

#include "TAVTactBaseClusterMT.hxx"

class TClonesArray;
class TAITactNtuClusterMT : public TAVTactBaseClusterMT {
   
public:
   explicit  TAITactNtuClusterMT(const char* name    = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAITactNtuClusterMT();
   
   // Action
   virtual  Bool_t Action();

   // Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);
   
   // Get list of pixels
   virtual TClonesArray* GetListOfPixels(Int_t sensorId);

   // Thread start
   Bool_t   ThreadStart(Int_t iSensor);

   // Thread join
   Bool_t   ThreadJoin();
   
   // Thread stop
   Bool_t   ThreadStop();
   
private:
   TAGdataDsc*     fpNtuRaw;		  ///< input data dsc
   TAGdataDsc*     fpNtuClus;		  ///< output data dsc
   Bool_t          fOk;            ///< ok flag

   vector<thread>  fThreads;       ///< thread array

private:
   // Create cluster
   Bool_t  CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);

   ClassDef(TAITactNtuClusterMT,0)
};

#endif
