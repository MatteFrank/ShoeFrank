#ifndef _TAVTactBaseNtuTrack_HXX
#define _TAVTactBaseNtuTrack_HXX
/*!
 \file
 \version $Id: TAVTactBaseNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactBaseNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"

#include "TAVTactBaseTrack.hxx"

#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"


class TAGgeoTrafo;
class TABMntuTrackTr;
class TGraphErrors;
class TAVTbaseCluster;
class TAVTntuHit;
class TAVTbaseTrack;
class TH2F;
class TH1F;
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
   //! Base creation of histogram
   virtual  void   CreateHistogram();
   
   //! Set BM track pointer
   void             SetBMntuTrack(TAGdataDsc* pBMtrack)        { fpBMntuTrack = pBMtrack;     }
   
protected:
   void   FillBmHistogramm(TVector3 bmTrackPos);
   
   virtual Bool_t FindStraightTracks();
   virtual Bool_t FindTiltedTracks() { return true; }
   virtual Bool_t FindVertices()     { return false; }
   
   TAVTbaseTrack* GetTrack(Int_t idx);
   Int_t          GetTracksN() const;
   
protected:
   TAGdataDsc*     fpBMntuTrack;	     // BM track pointer
	
   
   Bool_t           fBmTrackOk;          // flag for BM track chi2 cut
   TABMntuTrackTr*  fBmTrack;            // BM track pointer 
   TVector3         fBmTrackPos;         // BM track position 
   
   TH2F*            fpHisBmBeamProf;     // BM Beam profile extrapolated to target
   TH1F*            fpHisVtxResX;        // residualX between BM position and track intersection at target
   TH1F*            fpHisVtxResY;        // residualY between BM position and track intersection at target
   TH1F*            fpHisBmChi2;         // number of clusters per track
   TH1F*            fpHiVtxTgResX;       // Vertex resolution at Target X
   TH1F*            fpHiVtxTgResY;       // Vertex resolution at Target Y
   
};

#endif
