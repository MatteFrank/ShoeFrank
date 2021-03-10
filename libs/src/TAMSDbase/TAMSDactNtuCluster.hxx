#ifndef _TAMSDactNtuCluster_HXX
#define _TAMSDactNtuCluster_HXX
/*!
 \file
 \version $Id: TAMSDactNtuCluster.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuCluster.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGactNtuCluster1D.hxx"

class TAMSDntuCluster;
class TAMSDcluster;
class TAMSDhit;
class TAMSDparGeo;

class TAMSDactNtuCluster : public TAGactNtuCluster1D {
   
public:
   explicit  TAMSDactNtuCluster(const char* name     = 0, TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0, TAGparaDsc* p_config  = 0,
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

   // Get object in list
   TAGobject*  GetHitObject(Int_t idx) const;

private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   
   TAGparaDsc*     fpConfig;		  // config para dsc
   TAGparaDsc*     fpGeoMap;		  // geometry para dsc
   Float_t         fCurrentPosition;   // pointer to current position
   Float_t         fCurrentPosError ;  // pointer to current position error
   TClonesArray*   fListOfStrips;      // list of pixels
   TClonesArray*   fCurListOfStrips;   // list of pixels in current cluster
   
   Int_t          fClustersN;     // number of cluster
   
   TH1F*          fpHisStripTot;	     // Total number of pixels per cluster
   TH1F*          fpHisStrip[6];	  // number of pixels per cluster per sensor
   TH1F*          fpHisClusMap[6];   // cluster map per sensor

private:
   void    SearchCluster();
   void    FillMaps();
   Bool_t  CreateClusters(Int_t iSensor);


   ClassDef(TAMSDactNtuCluster,0)
};

#endif
