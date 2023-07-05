#ifndef _TAGactNtuGlbTrackF_HXX
#define _TAGactNtuGlbTrackF_HXX
/*!
 \file TAGactNtuGlbTrackF.hxx
 \brief   Declaration of TAGactNtuGlbTrackF.
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"

class TAVTvertex;
class TAVTtrack;
class TAGtrack;
class TADItrackEmProperties;
class TADItrackPropagator;
class TADIgeoField;

using namespace  std;

class TAGactNtuGlbTrackF : public TAGaction {
   
public:
   explicit  TAGactNtuGlbTrackF(const char* name         = 0,
                                TAGdataDsc* p_vtvertex   = 0,
                                TAGdataDsc* p_itntuclus  = 0,
                                TAGdataDsc* p_msntuclus  = 0,
                                TAGdataDsc* p_twntuclus  = 0,
                                TAGdataDsc* p_cantuclus  = 0,

                                TAGdataDsc* p_irntutrack = 0,
                                TAGparaDsc* p_vtgeomap   = 0,
                                TAGparaDsc* p_itgeomap   = 0,
                                TAGparaDsc* p_msgeomap   = 0,
                                TAGparaDsc* p_twgeomap   = 0,
                                TAGparaDsc* p_geomap     = 0,
                                TADIgeoField* field      = 0);


   virtual ~TAGactNtuGlbTrackF();
   
   // Create histograms
   virtual  void    CreateHistogram();
   
   // Action
   virtual  Bool_t  Action();

public:
   //! BM match
   static Bool_t  IsBmMatched()              { return fgBmMatched;  }
   //! Set BM match
   static void    SetBmMatched(Bool_t flag)  { fgBmMatched = flag;  }
   //! Get update track parameter for TW & CA points flag
   static Bool_t  IsCaTwUpdate()             { return fgCaTwUpdate; }
   //! Set update track parameter for TW & CA points flag
   static void    SetCaTwUpdate(Bool_t flag) { fgCaTwUpdate = flag; }


private:
   // Find tracks
   Bool_t     FindTracks();
   // Check VTX
   Bool_t     CheckVtx();
   // Fill VTX tracks
   TAGtrack*  FillVtxTracks(TAVTtrack* vtTrack);
   // Find ITR cluster for a given track
   void       FindItrCluster(TAGtrack* track);
   // Find MSD cluster for a given track
   void       FindMsdCluster(TAGtrack* track);
   // Find TW cluster for a given track
   void       FindTwCluster(TAGtrack* track, Bool_t update = true);
   // Find CAL cluster for a given track
   void       FindCaCluster(TAGtrack* track);

   // Fill track histograms
   void       FillHistogramm(TAGtrack* track);
   // Fill histograms
   void       FillHistogramm();
   // Update track parameters
   void       UpdateParam(TAGtrack* track);
   
   // Compute mass
   void       ComputeMass(TAGtrack* track);
   // Fill MC info
   void       FillMcTrackId(TAGcluster* cluster, TAGpoint* point);

   // Get linear fit parameter
   vector<double> GetLinearFit(const vector<double>& z, const vector<double>& x, const vector<double>& dx);

private:
   TAGdataDsc*     fpVertexVtx;         ///< VT vertex container
   TAGdataDsc*     fpNtuClusItr;        ///< IT cluster container
   TAGdataDsc*     fpNtuClusPoint;      ///< MSD point container
   TAGdataDsc*     fpNtuRecTw;          ///< TW rec point container
   TAGdataDsc*     fpNtuClusCa;         ///< CA cluster container

   TAGdataDsc*     fpNtuTrack;          ///< output tracks

   TAGparaDsc*     fpGeoMapVtx;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapItr;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapMsd;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapTof;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapG;           ///< geometry para dsc

   TAGgeoTrafo*    fpFootGeo;           ///< First geometry transformer
   TAVTvertex*     fVtVertex;           ///< VT vertex pointer

   Int_t           fRequiredClusters;   ///< number of clusters required to make a track
   Double_t        fSearchClusDistItr;  ///< Max distance to associate a track and a cluster
   Double_t        fSearchClusDistMsd;  ///< Max distance to associate a track and a cluster
   Double_t        fSearchClusDistTof;  ///< Max distance to associate a track and a cluster
   Float_t         fSensorThickVtx;     ///< Sensor thickness for VTX
   Float_t         fSensorThickItr;     ///< Sensor thickness for ITR
   Float_t         fSensorThickMsd;     ///< Sensor thickness for MSD
   Float_t         fWallThickTw;        ///< Sensor thickness for TW

   Float_t         fLastPlaneVtx;       ///< Last plane id for VTX
   Float_t         fLastPlaneItr;       ///< Last plane id for ITR
   Float_t         fLastPlaneMsd;       ///< Last plane id for MSD

   Float_t         fPartSigmaTheta;     ///< Scattering angle
   Float_t         fPartEnergy;         ///< Particle energy
   Float_t         fPartA;              ///< Particle A
   Float_t         fPartZ;              ///< Particle Z
   Float_t         fBeamEnergyTarget;   ///< Beam energy at target

   TADItrackEmProperties* fEmProperties; ///< EM properties
   TADItrackPropagator*   fpTrackProp;   ///< track propagator in mag field

   Int_t           fOffsetVtx;          ///< offset VTX
   Int_t           fOffsetItr;          ///< offset ITR
   Int_t           fOffsetMsd;          ///< offset MSD
   Int_t           fOffsetTof;          ///< offset TOF

   TH1F*           fpHisResX[43];       ///< Residual in X
   TH1F*           fpHisResY[43];       ///< Residual in Y
   TH1F*           fpHisResTotX;        ///< Total Residual in X
   TH1F*           fpHisResTotY;        ///< Total Residual in Y
   TH1F*           fpHisResTrkTotX;     ///< Total Residual in X
   TH1F*           fpHisResTrkTotY;     ///< Total Residual in Y
   TH1F*           fpHisTrackEvt;       ///< number of track per event
   TH1F*           fpHisTrackClus;      ///< number of clusters per track
   TH1F*           fpHisClusSensor;     ///< number of tracked clusters per sensor
   TH1F*           fpHisClusLeft;       ///< number of clusters left (not tracked)
   TH2F*           fpHisClusLeftPix;    ///< number of clusters left (not tracked) vs # pixels
   TH1F*           fpHisTheta;          ///< Polar angular distribution
   TH1F*           fpHisPhi;            ///< Azimutal angular distribution
   TH1F*           fpHisMass;           ///< Mass distribution
   TH1F*           fpHisZ;              ///< Atomic number distribution
   TH1F*           fpHisMeanPixel;      ///< Mean number of pixels per tracked cluster
   TH1F*           fpHisMeanCharge;     ///< Mwean Charge per tracked cluster
   TH2F*           fpHisTwDeCaE;        ///< TW-deltaE vs CA-E
   TH2F*           fpHisMsdDeCaE;       ///< MSD-deltaE vs CA-E

private:
   static Bool_t fgBmMatched;           ///< vertex matched with BM flag
   static Bool_t fgCaTwUpdate;          ///< flag to update the track with TW and CA points

   ClassDef(TAGactNtuGlbTrackF, 0)

};

#endif
