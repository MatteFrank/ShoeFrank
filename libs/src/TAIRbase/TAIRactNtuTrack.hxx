#ifndef _TAIRactNtuTrack_HXX
#define _TAIRactNtuTrack_HXX
/*!
 \file
 \version $Id: TAIRactNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAIRactNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

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
   
   //! Create histograms
   virtual  void    CreateHistogram();
   
   //! Action
   virtual  Bool_t  Action();

public:
   static Bool_t  IsBmMatched()             { return fgBmMatched; }
   static void    SetBmMatched(Bool_t flag) { fgBmMatched = flag; }

private:
    Bool_t    FindTracks();
    Bool_t    CheckVtx();
   TAIRtrack* FillTracks(TAVTtrack* vtTrack);
   void       FillHistogramm(TAVTbaseTrack* track);
   void       FillHistogramm();

private:
   TAGdataDsc*  fpVtVertex;        // VT vertex container
   TAVTvertex*  fVtVertex;         // VT vertex pointer
   TAGparaDsc*  fpVtGeoMap;        // VT par geo

private:
   static Bool_t fgBmMatched;      // vertex matched with BM flag

};

#endif
