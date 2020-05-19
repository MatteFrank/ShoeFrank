#ifndef _TAITactNtuClusterMT_HXX
#define _TAITactNtuClusterMT_HXX
/*!
 \file
 \version $Id: TAITactNtuClusterMT.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAITactNtuClusterMT.
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTactBaseNtuClusterMT.hxx"

class TClonesArray;
class TAITactNtuClusterMT : public TAVTactBaseNtuClusterMT {
   
public:
   explicit  TAITactNtuClusterMT(const char* name    = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAITactNtuClusterMT();
   
   //! Action
   virtual  Bool_t Action();

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);
   
   //! Get list of pixels
   virtual TClonesArray* GetListOfPixels(Int_t sensorId);

private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   Bool_t          fOk;

   pthread_t*      fThread[4];

private:
   Bool_t  CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr);

   ClassDef(TAITactNtuClusterMT,0)
};

#endif
