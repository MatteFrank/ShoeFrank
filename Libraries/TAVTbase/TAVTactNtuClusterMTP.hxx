#ifndef _TAVTactNtuClusterMTP_HXX
#define _TAVTactNtuClusterMTP_HXX
/*!
 \file TAVTactNtuClusterMTP.hxx
 \brief   Declaration of TAVTactNtuClusterMTP.
 */
/*------------------------------------------+---------------------------------*/

#include <thread>
#include <vector>
#include "TAVTtaskClusterMT.hxx"

#include "TAVTactBaseNtuCluster.hxx"

using namespace std;


class TAVTactNtuClusterMTP : public TAVTactBaseNtuCluster {
   
public:
   explicit  TAVTactNtuClusterMTP(const char* name   = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAVTactNtuClusterMTP();
   
   // Action
   virtual  Bool_t Action();
   
   
   // Get list of pixels
   virtual TClonesArray* GetListOfPixels(Int_t sensorId);

   //! Get Cluster number
   Int_t               GetClusterNumber(Int_t sensorId, Int_t line, Int_t col) const { return fFlagMapVect[sensorId][line*fDimX+col]; }
   
public:
   //! Set maximum number of threads
   static void  SetMaxThread(Int_t thr) { fgMaxThreadsN = thr;  }
   //! Get maximum number of threads
   static Int_t GetMaxThread()          { return fgMaxThreadsN; }
   
private:
   TAGdataDsc*     fpNtuRaw;		  ///< input data dsc
   TAGdataDsc*     fpNtuClus;		  ///< output data dsc
   Int_t*          fClusterVect;   ///< number of cluster
   vector<Int_t*>  fFlagMapVect;   ///< Array of flag map
   Int_t           fThreadsN;      ///< number of thread

   TThreadPool<TAVTtaskClusterMT, EProc>* fThreadPool;    ///< thread array
   vector<TAVTtaskClusterMT>              fThreads;       ///< thread array

   
private:
   static Int_t fgMaxThreadsN;  ///< maximum number of thread
   static Int_t fgkLimThreadsN; ///< upper limit of thread (fix to 8)

private:
   // Create cluster per thread
   Bool_t  CreateClusters(Int_t iSensor, TClonesArray* listOfPixels);

   // Set up size map
   void   SetupMaps(Int_t size);
   
   ClassDef(TAVTactNtuClusterMTP,0)
};

#endif
