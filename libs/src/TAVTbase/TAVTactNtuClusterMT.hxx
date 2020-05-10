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
class TThread;

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
   
   //! Thread start
   Bool_t   ThreadStart();
   
   //! Thread stop
   Bool_t   ThreadStop();
   
   
private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   Bool_t          fOk;

   TThread*        fThread[4];

private:
   Bool_t  CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);

private:
   static void Thread0(void* arg);
   static void Thread1(void* arg);
   static void Thread2(void* arg);
   static void Thread3(void* arg);

   ClassDef(TAVTactNtuClusterMT,0)
};

#endif
