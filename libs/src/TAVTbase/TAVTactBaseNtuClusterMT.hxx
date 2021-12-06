#ifndef _TAVTactBaseNtuClusterMT_HXX
#define _TAVTactBaseNtuClusterMT_HXX
/*!
 \file
 \version $Id: TAVTactBaseNtuClusterMT.hxx
 \brief   Declaration of TAVTactBaseNtuClusterMT.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseClusterMT.hxx"

class TAVTntuCluster;
class TAVThit;
class TAVTparGeo;
class TClonesArray;

class TAVTactBaseNtuClusterMT : public TAVTactBaseClusterMT {
   
public:
   explicit  TAVTactBaseNtuClusterMT(const char* name      = 0,
                                     TAGparaDsc* p_config  = 0,
                                     TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAVTactBaseNtuClusterMT();
   

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t /*iSensor*/, TClonesArray* /*listOfPixels*/, Int_t /*thr*/) { return false; }
   
   //! Get list of pixels
   virtual TClonesArray* GetListOfPixels(Int_t /*sensorId*/) { return 0x0; }
   
   //! Thread start
   Bool_t   ThreadStart();
   
   //! Thread join
   Bool_t   ThreadJoin();
   
   //! Thread stop
   Bool_t   ThreadStop();
   
protected:
   Bool_t          fOk;          /// ok flag
   pthread_t*      fThread[8];   /// thread array

protected:
   //! Thread function 0
   static void* Thread0(void* arg);
   //! Thread function 1
   static void* Thread1(void* arg);
   //! Thread function 2
   static void* Thread2(void* arg);
   //! Thread function 3
   static void* Thread3(void* arg);
   //! Thread function 4
   static void* Thread4(void* arg);
   //! Thread function 5
   static void* Thread5(void* arg);
   //! Thread function 6
   static void* Thread6(void* arg);
   //! Thread function 7
   static void* Thread7(void* arg);
   //! Flag for offline sensor
   static Int_t fgSensorOff;

   ClassDef(TAVTactBaseNtuClusterMT,0)
};

#endif
