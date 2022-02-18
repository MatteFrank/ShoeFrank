#ifndef _TAVTbaseParConf_HXX
#define _TAVTbaseParConf_HXX
/*!
  \file TAVTbaseParConf.hxx
  \brief   Declaration of TAVTbaseParConf.
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TArrayC.h"

#include "TAGparTools.hxx"

using namespace std;

//##############################################################################

class TAVTbaseParConf : public TAGparTools {
      
protected:
   Int_t      fSensorsN;         ///< Number of sensors
   TString    fkDefaultConfName; ///< default detector configuration file

   /*!
    \struct SensorParameter_t
    \brief  Sensors parameters
    */
   struct SensorParameter_t : public  TObject {
	  Int_t     SensorIdx;              ///< sensor index
     Int_t     Type;                   ///< sensor type
	  Int_t     Status;                 ///< Status flag: Primary Reference = 1., Secondary Reference = 2. DeviceUunderTest = 3. Out = -1.
	  Int_t     MinNofPixelsInCluster;  ///< minimum pixels in a cluster
	  Int_t     MaxNofPixelsInCluster;  ///< maximum pixels in a cluster
     map< pair<int, int>, int > DeadPixelMap;    ///< dead pixel map
   };
   
   /*!
    \struct AnalysisParameter_t
    \brief  Analysis parameters
    */
   struct AnalysisParameter_t : public  TObject {
	  Int_t      TracksMaximum;          ///< maximum number of tracks to be allowed
	  Int_t      PlanesForTrackMinimum;  ///< min # planes to build a track in an event
	  Float_t    SearchHitDistance;      ///< max distance hit-track to add hit to track
	  Float_t    TrackChi2Limit;         ///< chi2 limit on the track
	  Float_t    BmTrackChi2Limit;       ///< chi2 limit on the BM track
   };
   
   SensorParameter_t fSensorParameter[32]; ///< sensor parameters
   AnalysisParameter_t fAnalysisParameter; ///< analysis parameters

public:
   TAVTbaseParConf();
    virtual ~TAVTbaseParConf();

   // Reading from file
   Bool_t          FromFile(const TString& name = "");
   
   // Clear
   virtual void    Clear(Option_t* opt="");
   
   // To stream
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
   // Dead pixel map check
   Bool_t IsDeadPixel(Int_t sensorId, Int_t aLine, Int_t aColumn);
   
   // Get status for a sensor
   virtual Int_t   GetStatus(Int_t idx)      const;
   // Get minor status for a sensor
   virtual Int_t   GetStatusMinor(Int_t idx) const;
   //! Get tyoe for a sensor
   virtual Int_t   GetType(Int_t idx)        const { return fSensorParameter[idx].Type; }
   //! Get number of sensors
   Int_t           GetSensorsN()             const { return fSensorsN;                  }
   //! Get default configuration name
   const Char_t*   GetDefaultConfName()      const { return fkDefaultConfName.Data();   }
   
   //! Get sensor parameters
   SensorParameter_t& GetSensorPar(Int_t idx)      { return fSensorParameter[idx];      }
   //! Get analysis parameters
   AnalysisParameter_t& GetAnalysisPar()           { return fAnalysisParameter;         }

   ClassDef(TAVTbaseParConf,3)
};

#endif
