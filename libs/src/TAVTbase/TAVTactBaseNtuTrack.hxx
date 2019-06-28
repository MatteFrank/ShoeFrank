#ifndef _TAVTactBaseNtuTrack_HXX
#define _TAVTactBaseNtuTrack_HXX
/*!
 \file
 \version $Id: TAVTactBaseNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactBaseNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"


class TAGgeoTrafo;
class TABMntuTrackTr;
class TGraphErrors;
class TAVTcluster;
class TAVTntuHit;
class TAVTtrack;
class TH2F;
class TH1F;
class TAVTactBaseNtuTrack : public TAGaction {
   
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
   //! Set Required Clusters
   void             SetRequiredClusters(Int_t aNumber)         { fRequiredClusters = aNumber; }
   //! Set BM track pointer
   void             SetBMntuTrack(TAGdataDsc* pBMtrack)        { fpBMntuTrack = pBMtrack;     }
   //! Get maximum number of tracks
   Int_t            GetTracksMaximum()                 const   { return  fTracksMaximum;      }
   //! get tracker name
   TString          GetTrackerName()                           { return  TString(GetName());  }
   
   //! Set TAGgeoTrafo
   void             SetGeoTrafo(TString name);
   //! Get nearest track to a given cluster
   TAVTtrack*       NearestTrack(TAVTcluster *aCluster); 
   //! Get nearest cluster for a given track and plane
   TAVTcluster*     NearestCluster(TAVTtrack *aTrack, Int_t aPlaneNumber); 
   
public:
   //! Set refit flag
   static void      SetRefit(Bool_t flag)                      { fgRefit = flag;              }
   //! Get refit flag
   static Bool_t    GetRefit()                                 { return fgRefit;              }
   
protected:    
   
   void   FillHistogramm(TAVTtrack* track);
   void   FillHistogramm();
   void   FillBmHistogramm(TVector3 bmTrackPos);
   void   UpdateParam(TAVTtrack* track);
   Bool_t AppyCuts(TAVTtrack* track) ;
   void   SetChargeProba();
   
   virtual Bool_t FindStraightTracks();
   virtual Bool_t FindTiltedTracks() { return true; }
   virtual Bool_t FindVertices()     { return false; }

protected:
   TAGdataDsc*     fpNtuTrack;		     // input data dsc
   TAGdataDsc*     fpNtuClus;		     // output data dsc
   TAGparaDsc*     fpConfig;		     // configuration dsc
   TAGparaDsc*     fpGeoMap;		     // geometry para dsc
   TAGparaDsc*     fpCalib;		         // calib para dsc
   TAGdataDsc*     fpBMntuTrack;	     // BM track pointer
   TAGgeoTrafo*    fpFootGeo;	         // First geometry transformer
	
   Int_t           fTracksMaximum;       // maximum number of tracks allowed
   Int_t           fRequiredClusters;    //! number of clusters required to make a track
   Double_t        fSearchClusDistance;  //! Max distance to associate a track and a cluster
   Float_t         fPlaneResolution;     // plane resolution
      
   TGraphErrors*    fGraphU;             // pointer to graph for fit in U    
   TGraphErrors*    fGraphV;             // pointer to graph for fit in V  
   
   Bool_t           fBmTrackOk;          // flag for BM track chi2 cut
   TABMntuTrackTr*  fBmTrack;            // BM track pointer 
   TVector3         fBmTrackPos;         // BM track position 
   
   TH1F*            fpHisPixelTot;		 // Total number of pixels per tracked cluster
   TH1F*            fpHisPixel[32];		 // Total number of pixels per tracked cluster for each sensor
   TH2F*            fpHisTrackMap[32];    // track map per sensor
   TH1F*            fpHisResX[32];        // Residual in X
   TH1F*            fpHisResY[32];        // Residual in Y
   TH1F*            fpHisResTotX;        // Total Residual in X
   TH1F*            fpHisResTotY;        // Total Residual in Y
   TH1F*            fpHisChi2TotX;       // Total Chi2 in X direction
   TH1F*            fpHisChi2TotY;       // Total Chi2 in Y direction
   TH1F*            fpHisTrackEvt;       // number of track per event
   TH1F*            fpHisTrackClus;      // number of clusters per track
   TH1F*            fpHisClusSensor;     // number of tracked clusters per sensor
   TH1F*            fpHisClusLeft;       // number of clusters left (not tracked)
   TH2F*            fpHisClusLeftPix;    // number of clusters left (not tracked) vs # pixels
   TH1F*            fpHisTheta;          // Polar angular distribution
   TH1F*            fpHisPhi;            // Azimutal angular distribution
   
   TH2F*            fpHisBeamProf;       // VTX Beam profile extrapolated to target
   TH2F*            fpHisBmBeamProf;     // BM Beam profile extrapolated to target
   TH1F*            fpHisVtxResX;        // residualX between BM position and track intersection at target
   TH1F*            fpHisVtxResY;        // residualY between BM position and track intersection at target
   TH1F*            fpHisBmChi2;         // number of clusters per track
   TH1F*            fpHiVtxTgResX;       // Vertex resolution at Target X
   TH1F*            fpHiVtxTgResY;       // Vertex resolution at Target Y
   
   TString           fPrefix;            // prefix of histogram
   TString           fTitleDev;          // device name for histogram title

protected:
   static Bool_t    fgRefit;             // flag to update parameter during tracking   
      
   ClassDef(TAVTactBaseNtuTrack,1)
};

#endif
