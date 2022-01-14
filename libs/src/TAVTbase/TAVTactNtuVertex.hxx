#ifndef _TAVTactNtuVertex_HXX
#define _TAVTactNtuVertex_HXX
/*!
 \file TAVTactNtuVertex.hxx
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
   
    //! Set tolerance
    void SetEps(Double_t q)           { fEps = q;     }
    //! Get tolerance
    Double_t GetEps()           const { return fEps;  }
    //! Set miminum Z for vertices search
    void SetMinimumZ(Double_t min)    { fMinZ = min;  }
    //! Get miminum Z for vertices search
    Double_t GetMinimumZ()      const { return fMinZ; }
    //! Get maxinum Z for vertices search
    void SetMaximumZ(Double_t max)    { fMaxZ = max;  }
    //! Set maxinum Z for vertices search
    Double_t GetMaximumZ()      const { return fMaxZ; }

private:
    // Compute vertex
    Bool_t   ComputeVertex();
    // Comoute distance to tracks for a given Z
    Double_t ComputeDistance(Double_t zVal);
    // Compute final position of vertex
    Double_t ComputeFinalCoordinate(Int_t a, Double_t zVal);
    // Set vertex position
    void     SetVertex(TVector3& xyz, Double_t mind);
   
    ClassDef(TAVTactNtuVertex,0)
};

#endif
