#ifndef _TAIRactNtuTrack_HXX
#define _TAIRactNtuTrack_HXX
/*!
 \file TAIRactNtuTrack.hxx
 \brief   Declaration of TAIRactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/
#include "TH1F.h"
#include "TAVTactBaseTrack.hxx"

class TAVTvertex;
class TAVTtrack;
class TAIRtrack;
class TAIRactNtuTrack : public TAVTactBaseTrack {
   
public:
   explicit  TAIRactNtuTrack(const char* name       = 0,
                             TAGdataDsc* p_ntuclus  = 0,
                             TAGdataDsc* p_vtvertex = 0,
                             TAGdataDsc* p_ntutrack = 0,
                             TAGparaDsc* p_config   = 0,
                             TAGparaDsc* p_geomap   = 0,
                             TAGparaDsc* p_vtgeomap = 0,
                             TAGparaDsc* p_calib    = 0);
   virtual ~TAIRactNtuTrack();
   
   // Create histograms
   virtual  void    CreateHistogram();
   
   // Action
   virtual  Bool_t  Action();

public:
   //! Return matching with BM flag
   static Bool_t  IsBmMatched()             { return fgBmMatched; }
   //! Set matching with BM flag
   static void    SetBmMatched(Bool_t flag) { fgBmMatched = flag; }

private:
   // Find track
    Bool_t    FindTracks();
   // Check VTX
    Bool_t    CheckVtx();
   // Fill interaction region track
   TAIRtrack* FillTracks(TAVTtrack* vtTrack);
   // Fill histogram from track
   void       FillHistogramm(TAGbaseTrack* track);
   // Fill histogram
   void       FillHistogramm();

private:
   TAGdataDsc*  fpVtVertex;        ///< VT vertex container
   TAVTvertex*  fVtVertex;         ///< VT vertex pointer
   TAGparaDsc*  fpVtGeoMap;        ///< VT par geo

   TH1F*        fpHisPixelTot;     ///< Total number of pixels per cluster
   TH1F*        fpHisPixel[36];    ///< Total number of pixels per tracked cluster for each sensor

private:
   static Bool_t fgBmMatched;      ///< vertex matched with BM flag

};

#endif
