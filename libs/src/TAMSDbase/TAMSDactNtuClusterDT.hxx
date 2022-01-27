#ifndef _TAMSDactNtuClusterDT_HXX
#define _TAMSDactNtuClusterDT_HXX
/*!
 \file
 \version $Id: TAMSDactNtuClusterDT.hxx
 \brief   Declaration of TAMSDactNtuClusterDT.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"

class TAMSDntuCluster;
class TAMSDcluster;
class TAMSDhit;
class TAMSDparGeo;

class TAMSDactNtuClusterDT : public TAGaction {
   
public:
   explicit  TAMSDactNtuClusterDT(const char* name     = 0, TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0, TAGparaDsc* p_config  = 0,
			       TAGparaDsc* p_geomap  = 0);
   
   virtual ~TAMSDactNtuClusterDT();
   
   //! Action
   virtual  Bool_t Action();

   //! Find cluster charge, noise & position
   virtual Bool_t  FindClusters(Int_t iSensor);
   
   //! Apply basic cuts
   virtual Bool_t  ApplyCuts(TAMSDcluster* /*cluster*/);
   
   //! Base creation of histogram
   virtual  void   CreateHistogram();

   //! Compute position
   void    ComputePosition(TAMSDcluster* cluster);
   //! Compute center of gravity of cluster
   void    ComputeCog(TAMSDcluster* cluster);

   //! Get list of strips for a given plane
   TClonesArray*   GetListOfStrips()   const { return fListOfStrips;    }
   
   //! Get position of current cluster
   Float_t       GetCurrentPosition()  const { return fCurrentPosition; }
   
   //! Get position error of current cluster
   Float_t       GetCurrentPosError()  const { return fCurrentPosError; }
   //! Get center of gravity of current cluster 
   Float_t       GetCurrentCog()  const { return fCurrentCog; }

   
   //! Set list of strips
   void SetListOfStrips(TClonesArray* list)  { fListOfStrips = list;    }
   
   //! Set position of current cluster
   void SetCurrentPosition(Float_t u)        { fCurrentPosition = u;    }
   //! Set center of gravity of current cluster 
   void SetCurrentCog(Float_t u)             { fCurrentCog = u;         }

   // Get object in list
   TAGobject*  GetHitObject(Int_t idx) const;

private:
   TAGdataDsc*     fpNtuRaw;		  // input data dsc
   TAGdataDsc*     fpNtuClus;		  // output data dsc
   
   TAGparaDsc*     fpConfig;		  // config para dsc
   TAGparaDsc*     fpGeoMap;		  // geometry para dsc
   Float_t         fCurrentPosition;   // pointer to current position
   Float_t         fCurrentPosError ;  // pointer to current position error
   Float_t         fCurrentCog;        // pointer to current center of gravity
   TClonesArray*   fListOfStrips;      // list of strips
   TClonesArray*   fCurListOfStrips;   // list of strips in current cluster
   
   Int_t          fClustersN;     // number of cluster
   
   TH1F*          fpHisStripTot;	     // Total number of strips per cluster
   TH1F*          fpHisStrip[16];	     // number of strips per cluster per sensor
   TH1F*          fpHisClusMap[16];    // cluster map per sensor
   TH1F*          fpHisClusCharge[16]; // cluster charge per sensor
   TH1F*          fpHisClusChargeTot; // Total cluster charge for all sensors

   ClassDef(TAMSDactNtuClusterDT,1)
};

#endif
