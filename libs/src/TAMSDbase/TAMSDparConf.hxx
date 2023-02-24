#ifndef _TAMSDparConf_HXX
#define _TAMSDparConf_HXX
/*!
  \file TAMSDparConf.hxx
  \brief   Declaration of TAMSDparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"

#include "TAVTbaseParConf.hxx"

//##############################################################################

class TAMSDparConf : public TAVTbaseParConf {
   
public:
   TAMSDparConf();
   virtual ~TAMSDparConf();
   
private:
   struct SensorParameter_t : public  TObject {
      Int_t     SensorIdx;              ///< sensor index
      Int_t     Type;                   ///< sensor type
      Int_t     Status;                 ///< Status flag: Primary Reference = 1., Secondary Reference = 2. DeviceUunderTest = 3. Out = -1.
      Int_t     MinNofStripsInCluster;  ///< minimum strips in a cluster
      Int_t     MaxNofStripsInCluster;  ///< maximum strips in a cluster
      TArrayC   DeadStripMap;           ///< dead strip map
   };
   /*!
    \struct AnalysisParameter_t
    \brief  Analysis parameters
    */
   struct AnalysisParameter_t : public  TObject {
      Bool_t     PedestalFlag;           ///< Pedestal run flag
      Bool_t     TrackingFlag;           ///< Tracking run flag
      Int_t      PlanesForTrackMinimum;  ///< min # planes to build a track in an event
      Float_t    SearchHitDistance;      ///< max distance hit-track to add hit to track
   };
   
   AnalysisParameter_t fAnalysisParameter;   ///< analysis parameters
   SensorParameter_t   fSensorParameter[16]; ///< sensor paramater

public:
   Bool_t               FromFile(const TString& name = "");
      
   //! Get type for a given sen sor
   Int_t                GetType(Int_t idx)     const { return fSensorParameter[idx].Type;   }
   //! Get status for a given sensor
   Int_t                GetStatus(Int_t idx)   const { return fSensorParameter[idx].Status; }

   //! Get sensor parameter
   SensorParameter_t&   GetSensorPar(Int_t idx)      { return fSensorParameter[idx];        }
   //! Get analysis parameters
   AnalysisParameter_t& GetAnalysisPar()             { return fAnalysisParameter;           }

   ClassDef(TAMSDparConf,2)
};

#endif
