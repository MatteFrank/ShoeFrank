#ifndef _TAVTactBaseNtuVertex_HXX
#define _TAVTactBaseNtuVertex_HXX
/*!
 \file TAVTactBaseNtuVertex.hxx
 \brief   Declaration of TAVTactBaseNtuVertex.
 
 \author R. Rescigno
 */
/*------------------------------------------+---------------------------------*/


#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTntuTrack.hxx"


class TH2F;
class TH1F;
class TABMtrack;
class TAGgeoTrafo;
class TADItrackEmProperties;
class TAVTactBaseNtuVertex : public TAGaction {
    
public:
    explicit  TAVTactBaseNtuVertex(const char* name       = 0,
                                  TAGdataDsc* p_ntutrack  = 0, 
                                  TAGdataDsc* p_ntuvertex = 0, 
                                  TAGparaDsc* p_config    = 0,
                                  TAGparaDsc* p_geomap    = 0,
                                  TAGparaDsc* p_geomapG   = 0,
								          TAGdataDsc* p_bmtrack   = 0);
    
    virtual ~TAVTactBaseNtuVertex();
   
    //! Action
    virtual  Bool_t  Action();
    //! Create histogram
    virtual  void    CreateHistogram();
    //! virtual method to compute vertex
    virtual  Bool_t  ComputeVertex() = 0;
    //! Set BM track container
    void     SetBMntuTrack(TAGdataDsc* bmTrack) { fpBMntuTrack = bmTrack; }
      
protected:
    TAGgeoTrafo*    fpFootGeo;       ///< geo trafo
    TAGdataDsc*     fpNtuTrack;		 ///< input data dsc
    TAGdataDsc*     fpNtuVertex;		 ///< output data dsc
    TAGparaDsc*     fpConfig;		    ///< configuration dsc
    TAGparaDsc*     fpGeoMap;		    ///< geometry para dsc
    TAGparaDsc*     fpGeoMapG;       ///< Global geometry para dsc
    TAGdataDsc*     fpBMntuTrack;	 ///< bm track data dsc
   
    TADItrackEmProperties* fEmProp;   ///< EM properties

    Float_t         fSearchClusDistance; ///< distance for pileup
    TVector3        fVtxPos;             ///< vertex pos

    Double_t        fEps;             ///< Tolerance
    Double_t        fMinZ;            ///< Minimum value of Z
    Double_t        fMaxZ;            ///< Maximum value of Z
    Double_t        fScatterAng;      ///< Scattering angle

    TH1F*           fpHisPosZ;        ///< Vertex postion Z histogram
    TH2F*           fpHisPosXY;       ///< Vertex postion X-Y histogram
    TH1F*           fpHisBmMatchX;    ///< Vertex BM matching in postion X histogram
    TH1F*           fpHisBmMatchY;    ///< Vertex BM matching in postion Y histogram
    
protected:
   static Bool_t    fgCheckBmMatching; ///< Flag to enable checking with BM track matching

protected:
   //! Enable BM matching
   static void    EnableBmMatching()        { fgCheckBmMatching = true;  }
   //! Disable BM matching
   static void    DisableBmMatching()       { fgCheckBmMatching = false; }
   
protected:
    //! Set invalid vertex
    Bool_t   SetNotValidVertex(Int_t idTk);
    //!Set valid vertex
    void     SetValidVertex();
    //! Check BM matching
    Bool_t   CheckBmMatching();
    //! Compute minimal distance VTX track to BM track
    void     ComputeInteractionVertex(TABMtrack* lbm, TAVTtrack lvtx);
    //! Copmpute scatter angle
    Double_t ComputeScatterAngle();
    //! Search for Nuclear reaction
    Int_t    SearchNucReac(TAVTtrack* track0);

    ClassDef(TAVTactBaseNtuVertex,0)
};

#endif
