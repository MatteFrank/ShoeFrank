#ifndef _TAVTactNtuVertex_HXX
#define _TAVTactNtuVertex_HXX
/*!
 \file
 \brief   Declaration of TAVTactNtuVertex
 
 \author R. Rescigno
 */

#include "TAVTntuTrack.hxx"

#include "TAVTactBaseNtuVertex.hxx"
#include "TVector3.h"

class TAVTactNtuVertex : public TAVTactBaseNtuVertex {
    
public:
  explicit  TAVTactNtuVertex(const char* name        = 0,
                             TAGdataDsc* p_ntutrack  = 0,
                             TAGdataDsc* p_ntuvertex = 0,
                             TAGparaDsc* p_config    = 0,
                             TAGparaDsc* p_geomap    = 0,
                             TAGparaDsc* p_geomapG   = 0,
                             TAGdataDsc* p_bmtrack   = 0);
  
    virtual ~TAVTactNtuVertex();
    
    void SetEps(Double_t q)           { fEps = q;     }
    Double_t GetEps()           const { return fEps;  }
    void SetMinimumZ(Double_t min)    { fMinZ = min;  }
    Double_t GetMinimumZ()      const { return fMinZ; }
    void SetMaximumZ(Double_t max)    { fMaxZ = max;  }
    Double_t GetMaximumZ()      const { return fMaxZ; }

private:
    Bool_t   ComputeVertex();
    Double_t ComputeDistance(Double_t zVal);
    Double_t ComputeFinalCoordinate(Int_t a, Double_t zVal);
    void     SetVertex(TVector3& xyz, Double_t mind);
   
};

#endif
