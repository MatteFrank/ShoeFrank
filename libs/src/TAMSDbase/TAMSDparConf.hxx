#ifndef _TAMSDparConf_HXX
#define _TAMSDparConf_HXX
/*!
  \file
  \version $Id: TAMSDparConf.hxx
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
   Int_t      fSensorsN;         // Number of sensors
   TString    fkDefaultConfName; // default detector configuration file
   
   struct SensorParameter_t : public  TObject {
      Int_t     SensorIdx;              // sensor index
      Int_t     Type;                   // sensor type
      Int_t     Status;                 // Status flag: Primary Reference = 1., Secondary Reference = 2. DeviceUunderTest = 3. Out = -1.
      Int_t     MinNofStripsInCluster;  // minimum strips in a cluster
      Int_t     MaxNofStripsInCluster;  // maximum strips in a cluster
      TArrayC   DeadStripMap;           // dead strip map
   };
   
   struct AnalysisParameter_t : public  TObject {
   };
   
   SensorParameter_t   fSensorParameter[6];
   AnalysisParameter_t fAnalysisParameter;

public:
   Int_t         GetStatus(Int_t idx) const;
   Int_t         GetStatusMinor(Int_t idx) const ;
   Bool_t        FromFile(const TString& name = "");
   
   Int_t         GetSensorsN()          const  { return fSensorsN;                }
   const Char_t* GetDefaultConfName()   const  { return fkDefaultConfName.Data(); }
   
   SensorParameter_t& GetSensorPar(Int_t idx)  { return fSensorParameter[idx];    }
   AnalysisParameter_t& GetAnalysisPar()       { return fAnalysisParameter;       }

   
   ClassDef(TAMSDparConf,1)
};

#endif
