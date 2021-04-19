#ifndef _TAVTactBaseNtuClusterMT_HXX
#define _TAVTactBaseNtuClusterMT_HXX
/*!
 \file
 \version $Id: TAVTactBaseNtuClusterMT.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
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
   Bool_t          fOk;
   pthread_t*      fThread[8];

protected:
   static void* Thread0(void* arg);
   static void* Thread1(void* arg);
   static void* Thread2(void* arg);
   static void* Thread3(void* arg);
   static void* Thread4(void* arg);
   static void* Thread5(void* arg);
   static void* Thread6(void* arg);
   static void* Thread7(void* arg);
   static Int_t fgSensorOff;

   ClassDef(TAVTactBaseNtuClusterMT,0)
};

#endif
