#ifndef _TAVTactBaseNtuVertex_HXX
#define _TAVTactBaseNtuVertex_HXX
/*!
 \file
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
class TABMntuTrackTr;
class TAGgeoTrafo;
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
    
    virtual  Bool_t  Action();
    
    virtual  void    CreateHistogram();
   
    virtual  Bool_t  ComputeVertex() = 0;
   
    void     SetBMntuTrack(TAGdataDsc* bmTrack) { fpBMntuTrack = bmTrack; }
      
protected:
    TAGgeoTrafo*    fpFootGeo;       // geo trafo
    TAGdataDsc*     fpNtuTrack;		 // input data dsc
    TAGdataDsc*     fpNtuVertex;		 // output data dsc
    TAGparaDsc*     fpConfig;		    // configuration dsc
    TAGparaDsc*     fpGeoMap;		    // geometry para dsc
    TAGparaDsc*     fpGeoMapG;       // Global geometry para dsc
    TAGdataDsc*     fpBMntuTrack;	 // bm track data dsc

    Float_t         fSearchClusDistance; // distance for pileup
    TVector3        fVtxPos;             // vertex pos

   Double_t fEps; //tolleranza
   Double_t fMinZ; //minimum value of Z
   Double_t fMaxZ; //Maximum value of Z

    TH1F*           fpHisPosZ;
    TH2F*           fpHisPosXY;
    TH1F*           fpHisBmMatchX;
    TH1F*           fpHisBmMatchY;
    
protected:
   static Bool_t    fgCheckBmMatching;
   static Bool_t    fgCheckPileUp;

protected:
   static void    EnableBmMatching()        { fgCheckBmMatching = true;  }
   static void    DisableBmMatching()       { fgCheckBmMatching = false; }
   
   static void    EnablePileUp()            { fgCheckPileUp = true;      }
   static void    DisablePileUp()           { fgCheckPileUp = false;     }
   
protected:
    Bool_t SetNotValidVertex(Int_t idTk);
    void   SetValidVertex();
    Bool_t CheckBmMatching();
    Bool_t CheckPileUp();
    void   ComputePileUp();
    void   ComputeInteractionVertex(TABMntuTrackTr* lbm, TAVTtrack lvtx);

    ClassDef(TAVTactBaseNtuVertex,0)
};

#endif
