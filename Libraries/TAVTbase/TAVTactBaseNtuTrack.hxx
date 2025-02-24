#ifndef _TAVTactBaseNtuTrack_HXX
#define _TAVTactBaseNtuTrack_HXX
/*!
 \file TAVTactBaseNtuTrack.hxx
 \brief   Declaration of TAVTactBaseNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"

#include "TAVTactBaseTrack.hxx"
#include "TAVTparameters.hxx"

#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"


class TAGgeoTrafo;
class TABMtrack;
class TGraphErrors;
class TAVTbaseCluster;
class TAVTcluster;
class TAVThit;
class TAGbaseTrack;
class TH2F;
class TH1F;
class TAVTbaseParGeo;
class TAVTbaseParConf;
class TAVTactBaseNtuTrack : public TAVTactBaseTrack {
   
public:
   explicit  TAVTactBaseNtuTrack(const char* name       = 0,
                                 TAGdataDsc* p_ntuclus  = 0,
                                 TAGdataDsc* p_ntutrack = 0,
                                 TAGparaDsc* p_config   = 0,
                                 TAGparaDsc* p_geomap   = 0,
                                 TAGparaDsc* p_calib    = 0,
                                 TAGdataDsc* p_bmtrack  = 0);
   
   virtual ~TAVTactBaseNtuTrack();
   
   // Base action
   virtual         Bool_t  Action();
   
   // Check BM info
   void            CheckBM();
   
   // Base creation of histogram
   virtual  void   CreateHistogram();
   
   //! Set BM track pointer
   void             SetBMntuTrack(TAGdataDsc* pBMtrack) { fpBMntuTrack = pBMtrack;     }
   
protected:
   // Fill BM histogram from BM track
   void   FillBmHistogramm(TVector3 bmTrackPos);
   // Fill VTX histogram from VTX track
   void   FillHistogramm(TAGbaseTrack* track);
   // Fill VTX histogram
   void   FillHistogramm();
   // Compute straight tracks
   virtual Bool_t FindStraightTracks();
   //! Compute tilted tracks (virtual)
   virtual Bool_t FindTiltedTracks() { return true; }
   //! Compute vertices (virtual)
   virtual Bool_t FindVertices()     { return false; }
   // Set charge probability
   void           SetChargeProba();
   
protected:
   TAGdataDsc*      fpBMntuTrack;	     ///< BM track pointer
	
   Bool_t           fBmTrackOk;          ///< flag for BM track chi2 cut
   TABMtrack*       fBmTrack;            ///< BM track pointer
   TVector3         fBmTrackPos;         ///< BM track position
   
   TH1F*            fpHisPixelTot;       ///< Total number of pixels per tracked cluster
   TH1F*            fpHisPixel[MaxTrkSens]; ///< Total number of pixels per tracked cluster for each sensor
   TH2F*            fpHisClusLeftPix;    ///< number of clusters left (not tracked) vs # pixels
   TH1F*            fpHisClusLeft;       ///< number of clusters left (not tracked)
   TH2F*            fpHisBmBeamProf;     ///< BM Beam profile extrapolated to target
   TH2F*            fpHisVtxResX;        ///< residualX between BM position and track intersection at target
   TH2F*            fpHisVtxResY;        ///< residualY between BM position and track intersection at target
   TH1F*            fpHisBmChi2;         ///< number of clusters per track
   TH1F*            fpHiVtxTgResX;       ///< Vertex resolution at Target X
   TH1F*            fpHiVtxTgResY;       ///< Vertex resolution at Target Y
   
   ClassDef(TAVTactBaseNtuTrack,0)
};

#endif
