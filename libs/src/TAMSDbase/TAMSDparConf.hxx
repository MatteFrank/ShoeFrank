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
   struct SensorParameter_t : public  TObject {
      Int_t     SensorIdx;              // sensor index
      Int_t     Type;                   // sensor type
      Int_t     Status;                 // Status flag: Primary Reference = 1., Secondary Reference = 2. DeviceUunderTest = 3. Out = -1.
      Int_t     MinNofStripsInCluster;  // minimum strips in a cluster
      Int_t     MaxNofStripsInCluster;  // maximum strips in a cluster
      TArrayC   DeadStripMap;           // dead strip map
   };
   
   SensorParameter_t   fSensorParameter[6];

public:
   Bool_t               FromFile(const TString& name = "");
   
   Int_t GetType(Int_t idx)     const { return fSensorParameter[idx].Type;}
   Int_t GetStatus(Int_t idx)   const { return fSensorParameter[idx].Status;}

   SensorParameter_t&   GetSensorPar(Int_t idx) { return fSensorParameter[idx];    }

   ClassDef(TAMSDparConf,1)
};

#endif
