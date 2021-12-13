#ifndef _TAVTactBaseTrack_HXX
#define _TAVTactBaseTrack_HXX
/*!
 \file
 \version $Id: TAVTactBaseTrack.hxx
 \brief   Declaration of TAVTactBaseTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"


class TAGgeoTrafo;
class TABMtrack;
class TGraphErrors;
class TAVTbaseCluster;
class TAGbaseTrack;
class TH2F;
class TH1F;
class TAVTbaseParGeo;
class TAVTbaseParConf;

class TAVTactBaseTrack : public TAGaction {
   
public:
   explicit  TAVTactBaseTrack(const char* name       = 0,
                                 TAGdataDsc* p_ntuclus  = 0,
                                 TAGdataDsc* p_ntutrack = 0,
                                 TAGparaDsc* p_config   = 0,
                                 TAGparaDsc* p_geomap   = 0,
                                 TAGparaDsc* p_calib    = 0);
   
   virtual ~TAVTactBaseTrack();
   
   //! Create histograms
   virtual  void   CreateHistogram();
   
   //! Set Required Clusters
   void             SetRequiredClusters(Int_t aNumber)         { fRequiredClusters = aNumber; }
   
   //! Get maximum number of tracks
   Int_t            GetTracksMaximum()                 const   { return  fTracksMaximum;      }
   
   //! get tracker name
   TString          GetTrackerName()                           { return  TString(GetName());  }
   
   //! Set TAGgeoTrafo
   void             SetGeoTrafo(TString name);
   
   
public:
   //! Set refit flag
   static void      SetRefit(Bool_t flag)                      { fgRefit = flag;              }
   //! Get refit flag
   static Bool_t    GetRefit()                                 { return fgRefit;              }
   
protected:
   //! Fill track histogram
   void   FillHistogramm(TAGbaseTrack* track);
   //! Update track parameters
   void   UpdateParam(TAGbaseTrack* track);
   //! Apply cuts on track
   Bool_t AppyCuts(TAGbaseTrack* track) ;
   
   //! Get track
   virtual TAGbaseTrack*    GetTrack(Int_t idx);
   //! Number of tracks (const)
   virtual Int_t            GetTracksN() const;
   //! Number of tracks
   virtual Int_t            GetTracksN();

   //! Add new track
   virtual void             AddNewTrack(TAGbaseTrack* track);
   //! Make new track
   virtual TAGbaseTrack*    NewTrack();
   //! Get cluster from plane
   virtual TAGcluster*      GetCluster(Int_t iPlane, Int_t iClus);
   //! Get number of cluster for a given plane
   virtual Int_t            GetClustersN(Int_t iPlane);
   //! Set beam position
   virtual void             SetBeamPosition(TVector3 pos);
   
   //! Get par geo pointer
   virtual TAVTbaseParGeo*  GetParGeo();
   //! Get par conf pointer
   virtual TAVTbaseParConf* GetParConf();
   
protected:
   TAGdataDsc*     fpNtuTrack;		    ///< input data dsc
   TAGdataDsc*     fpNtuClus;		       ///< output data dsc
   TAGparaDsc*     fpConfig;		       ///< configuration dsc
   TAGparaDsc*     fpGeoMap;		       ///< geometry para dsc
   TAGparaDsc*     fpCalib;		       ///< calib para dsc
   TAGgeoTrafo*    fpFootGeo;	          ///< First geometry transformer
	
   Int_t           fTracksMaximum;      ///< maximum number of tracks allowed
   Int_t           fRequiredClusters;   //! number of clusters required to make a track
   Double_t        fSearchClusDistance; //! Max distance to associate a track and a cluster
   Float_t         fPlaneResolution;    ///< plane resolution
      
   TGraphErrors*   fGraphU;             ///< pointer to graph for fit in U
   TGraphErrors*   fGraphV;             ///< pointer to graph for fit in V
      
   TH1F*           fpHisResX[36];       ///< Residual in X histogram
   TH1F*           fpHisResY[36];       ///< Residual in Y histogram
   TH1F*           fpHisResTotX;        ///< Total Residual in X histogram
   TH1F*           fpHisResTotY;        ///< Total Residual in Y histogram
   TH2F*           fpHisTrackMap[36];   ///< track map per sensor histogram
   TH1F*           fpHisMeanCharge;     ///< Mwean Charge per tracked cluster histogram
   TH1F*           fpHisMeanPixel;      ///< Mean number of pixels per tracked cluster histogram
   TH2F*           fpHisBeamProf;       ///< VTX Beam profile extrapolated to target histogram
   TH1F*           fpHisChi2TotX;       ///< Total Chi2 in X direction histogram
   TH1F*           fpHisChi2TotY;       ///< Total Chi2 in Y direction histogram
   TH1F*           fpHisTrackEvt;       ///< number of track per event histogram
   TH1F*           fpHisTrackClus;      ///< number of clusters per track histogram
   TH1F*           fpHisClusSensor;     ///< number of tracked clusters per sensor histogram
   TH1F*           fpHisTheta;          ///< Polar angular distribution histogram
   TH1F*           fpHisPhi;            ///< Azimutal angular distribution histogram
  
   TString         fPrefix;             ///< prefix of histogram
   TString         fTitleDev;           ///< device name for histogram title

protected:
   static Bool_t    fgRefit;            ///< flag to update parameter during tracking
      
   ClassDef(TAVTactBaseTrack,1)
};

#endif
