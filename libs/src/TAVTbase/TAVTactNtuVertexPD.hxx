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
   
    //! Set miminum Z for vertices search
    void     SetMinimumZ(Double_t min) { fMinZ = min;   }
    //! Get miminum Z for vertices search
    Double_t GetMinimumZ()       const { return fMinZ;  }
    //! Set maxinum Z for vertices search
    void     SetMaximumZ(Double_t max) { fMaxZ = max;   }
    //! Get maxinum Z for vertices search
    Double_t GetMaximumZ()       const { return fMaxZ;  }
    
private:
    //! Compute vertex
    Bool_t   ComputeVertex();
    //! seach for maximun proba btw two tracks
    void     SearchMaxProduct(TAVTtrack* linei, TAVTtrack* linej,Int_t i, Int_t j);
    //! Compute vertex for two tracks
    TVector3 ComputeVertexPoint(TAVTtrack* line0, TAVTtrack* line1, Double_t zVal);
    //! Compute prob for a given track
    Double_t ComputeProbabilityForSingleTrack(TAVTtrack* lin0, TVector3 v);
    //! Compute minimum distance to track for a given vertex
    TVector3 ComputeMinimumPointDistance(TAVTtrack* l, TVector3 vt);
    //! Compute vertex
    Double_t ComputeV (TVector3 rpos);
    //! Set vertex
    Bool_t   SetVertex();
    //! Adjust parameter impact
    void     ImpactParameterAdjustement();
    //! Compute max values
    TVector3 ComputeMaxVMaxIMaxJ(); 

private:
    Double_t fTracksN;             /// Number of tracks in vertex
    Double_t fImpactParameterCut;  /// Impact cut
    Double_t fProbabilityCut;      /// proba cut
   
    TVector3 fErr;                 /// vertex error
    
    TArrayD  fProbValuesMax;       /// array of proba values
    TArrayD  fFlagValidity;        /// array of flag validity
    TArrayD  fVvalues;             /// array of values
    TArrayI  fNotValidTrack;       /// array of valid tracks
    map <Int_t, TVector3> fRValuesMax; /// map for R max values
        
    ClassDef(TAVTactNtuVertexPD,0)
};

#endif
