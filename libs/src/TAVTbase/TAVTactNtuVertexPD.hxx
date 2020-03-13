#ifndef _TAVTactNtuVertexPD_HXX
#define _TAVTactNtuVertexPD_HXX
/*!
 \file
 \brief   Declaration of TAVTactNtuVertexPD
 
 \author R. Rescigno, optimized and cleanup Ch. Finck
 */

#include "TAVTactBaseNtuVertex.hxx"
#include "TVector3.h"
#include "TAVTntuTrack.hxx"
#include <map>
#include "TArrayD.h"
#include "TArrayI.h"

class TAVTtrack;
class TAVTactNtuVertexPD : public TAVTactBaseNtuVertex {
    
public:
    explicit  TAVTactNtuVertexPD(const char* name        =  0,
                                 TAGdataDsc* p_ntutrack  =  0,
                                 TAGdataDsc* p_ntuvertex =  0,
                                 TAGparaDsc* p_config    =  0,
                                 TAGparaDsc* p_geomap    =  0,
                                 TAGparaDsc* p_geomapG   =  0,
                                 TAGdataDsc* p_bmtrack   =  0);
    
    virtual ~TAVTactNtuVertexPD();
    
    void     SetMinimumZ(Double_t min) { fMinZ = min;   }
    Double_t GetMinimumZ()       const { return fMinZ;  }
    void     SetMaximumZ(Double_t max) { fMaxZ = max;   }
    Double_t GetMaximumZ()       const { return fMaxZ;  }
    
private:
    Bool_t   ComputeVertex();
    void     SearchMaxProduct(TAVTtrack* linei, TAVTtrack* linej,Int_t i, Int_t j);
    TVector3 ComputeVertexPoint(TAVTtrack* line0, TAVTtrack* line1, Double_t zVal);
    Double_t ComputeProbabilityForSingleTrack(TAVTtrack* lin0, TVector3 v);
    TVector3 ComputeMinimumPointDistance(TAVTtrack* l, TVector3 vt);
    Double_t ComputeV (TVector3 rpos);
    Bool_t   SetVertex();
    void     ImpactParameterAdjustement();
    TVector3 ComputeMaxVMaxIMaxJ(); 

private:
    Double_t fTracksN;
    Double_t fImpactParameterCut;
    Double_t fProbabilityCut;
   
    TVector3 fErr;    
    
    TArrayD  fProbValuesMax;
    TArrayD  fFlagValidity;
    TArrayD  fVvalues;
    TArrayI  fNotValidTrack;
    map <Int_t, TVector3> fRValuesMax;
        
};

#endif
