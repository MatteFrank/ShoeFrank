#ifndef _TAGactNtuGlbTrackS_HXX
#define _TAGactNtuGlbTrackS_HXX
/*!
 \file
 \version $Id: TAGactNtuGlbTrackS.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAGactNtuGlbTrackS.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"

class TAVTvertex;
class TAVTtrack;
class TAGtrack;
class TADItrackEmProperties;
using namespace  std;

class TAGactNtuGlbTrackS : public TAGaction {
   
public:
   explicit  TAGactNtuGlbTrackS(const char* name         = 0,
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
                                TAGparaDsc* p_geomap     = 0);


   virtual ~TAGactNtuGlbTrackS();
   
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
   TAGtrack*  FillVtxTracks(TAVTtrack* vtTrack);
   void       FindItrCluster(TAGtrack* track);
   void       FindMsdCluster(TAGtrack* track);
   void       FindTwCluster(TAGtrack* track, Bool_t update = true);
   void       FindCaCluster(TAGtrack* track);

   void       FillHistogramm(TAGtrack* track);
   void       FillHistogramm();
   void       UpdateParam(TAGtrack* track, Int_t viewX = -1);
   
   void       ComputeMass(TAGtrack* track);

   vector<double> GetLinearFit(const vector<double>& z, const vector<double>& x, const vector<double>& dx);

private:
   TAGdataDsc*     fpVertexVtx;         // VT vertex container
   TAGdataDsc*     fpNtuClusItr;        // IT cluster container
   TAGdataDsc*     fpNtuClusPoint;      // MSD point container
   TAGdataDsc*     fpNtuRecTw;          // TW rec point container
   TAGdataDsc*     fpNtuClusCa;         // CA cluster container

   TAGdataDsc*     fpNtuTrack;          // output tracks

   TAGparaDsc*     fpGeoMapVtx;         // geometry para dsc
   TAGparaDsc*     fpGeoMapItr;         // geometry para dsc
   TAGparaDsc*     fpGeoMapMsd;         // geometry para dsc
   TAGparaDsc*     fpGeoMapTof;         // geometry para dsc
   TAGparaDsc*     fpGeoMapG;           // geometry para dsc

   TAGgeoTrafo*    fpFootGeo;           // First geometry transformer
   TAVTvertex*     fVtVertex;           // VT vertex pointer

   Int_t           fRequiredClusters;   //! number of clusters required to make a track
   Double_t        fSearchClusDistItr;  //! Max distance to associate a track and a cluster
   Double_t        fSearchClusDistMsd;  //! Max distance to associate a track and a cluster
   Double_t        fSearchClusDistTof;  //! Max distance to associate a track and a cluster
   Float_t         fSensorThickVtx;
   Float_t         fSensorThickItr;
   Float_t         fSensorThickMsd;
   Float_t         fWallThickTw;

   Float_t         fLastPlaneVtx;
   Float_t         fLastPlaneItr;
   Float_t         fLastPlaneMsd;

   Float_t         fPartSigmaTheta;
   Float_t         fPartEnergy;
   Float_t         fPartA;
   Float_t         fPartZ;
   Float_t         fBeamEnergyTarget;

   TADItrackEmProperties* fEmProperties; // EM properties
   
   Int_t           fOffsetVtx;          // offset VTX
   Int_t           fOffsetItr;          // offset ITR
   Int_t           fOffsetMsd;          // offset MSD
   Int_t           fOffsetTof;          // offset TOF

   TH1F*           fpHisResX[43];       // Residual in X
   TH1F*           fpHisResY[43];       // Residual in Y
   TH1F*           fpHisResTotX;        // Total Residual in X
   TH1F*           fpHisResTotY;        // Total Residual in Y
   TH1F*           fpHisResTrkTotX;        // Total Residual in X
   TH1F*           fpHisResTrkTotY;        // Total Residual in Y
   TH1F*           fpHisTrackEvt;       // number of track per event
   TH1F*           fpHisTrackClus;      // number of clusters per track
   TH1F*           fpHisClusSensor;     // number of tracked clusters per sensor
   TH1F*           fpHisClusLeft;       // number of clusters left (not tracked)
   TH2F*           fpHisClusLeftPix;    // number of clusters left (not tracked) vs # pixels
   TH1F*           fpHisTheta;          // Polar angular distribution
   TH1F*           fpHisPhi;            // Azimutal angular distribution
   TH1F*           fpHisMeanPixel;      // Mean number of pixels per tracked cluster
   TH1F*           fpHisMeanCharge;     // Mwean Charge per tracked cluster
   TH2F*           fpHisTwDeCaE;        // TW-deltaE vs CA-E
   TH2F*           fpHisMsdDeCaE;       // MSD-deltaE vs CA-E

private:
   static Bool_t fgBmMatched;      // vertex matched with BM flag

   ClassDef(TAGactNtuGlbTrackS, 0)

};

#endif
