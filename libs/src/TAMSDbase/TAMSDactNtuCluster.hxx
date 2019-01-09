#ifndef _TAMSDactNtuCluster_HXX
#define _TAMSDactNtuCluster_HXX
/*!
 \file
 \version $Id: TAMSDactNtuCluster.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuCluster.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"

class TAMSDntuCluster;
class TAMSDcluster;
class TAVTntuHit;
class TAVTbaseParGeo;

class TAMSDactNtuCluster : public TAGaction {
   
public:
   explicit  TAMSDactNtuCluster(const char* name     = 0,
								       TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0,
								       TAGparaDsc* p_config  = 0,
							          TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAMSDactNtuCluster();
   
   //! Action
   virtual  Bool_t Action();

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor);
   
   //! Apply basic cuts
   virtual Bool_t  ApplyCuts(TAMSDcluster* /*cluster*/) { return true; }// no config for the moment
   
   //! Base creation of histogram
   virtual  void   CreateHistogram();

   //! Compute position
   void    ComputePosition();

   //! Get list of pixels for a given plane
   TClonesArray*   GetListOfStrips()   const { return fListOfStrips;    }
   
   //! Get position of current cluster
   Float_t       GetCurrentPosition()  const { return fCurrentPosition; }
   
   //! Get position error of current cluster
   Float_t       GetCurrentPosError()  const { return fCurrentPosError; }
   
   //! Set list of pixels
   void SetListOfStrips(TClonesArray* list)  { fListOfStrips = list;    }
   
   //! Set position of current cluster
   void SetCurrentPosition(Float_t u)        { fCurrentPosition = u;    }

private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   
   TAGparaDsc*     fpConfig;		  // config para dsc
   TAGparaDsc*     fpGeoMap;		  // geometry para dsc
   Float_t         fCurrentPosition;   // pointer to current position
   Float_t         fCurrentPosError ;  // pointer to current position error
   TClonesArray*   fListOfStrips;      // list of pixels
   TClonesArray*   fCurListOfStrips;   // list of pixels in current cluster
   
   map<Int_t, Int_t> fStripMap; // pixel map;
   map<Int_t, Int_t> fIndexMap; // index map of the pixel;
   TArrayI fFlagMap;
   
   Int_t          fClustersN;     // number of cluster
   
   TH1F*          fpHisStripTot;	     // Total number of pixels per cluster
   TH1F*          fpHisStrip[32];	  // number of pixels per cluster per sensor
   TH1F*          fpHisClusMap[32];   // cluster map per sensor

private:
   void  SearchCluster(TAVTbaseParGeo* pGeoMap);
   void  FillMaps(TAVTbaseParGeo* pGeoMap);
   Bool_t  ShapeCluster(Int_t noClus, Int_t Ind, TAVTbaseParGeo* pGeoMap);
   Bool_t  FindClusters(Int_t iSensor, TAMSDntuCluster* pNtuClus, TAVTbaseParGeo* pGeoMap);
   Bool_t  CreateClusters(Int_t iSensor, TAMSDntuCluster* pNtuClus, TAVTbaseParGeo* pGeoMap);


   ClassDef(TAMSDactNtuCluster,0)
};

#endif
