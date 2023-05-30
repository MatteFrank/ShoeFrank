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
#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TAMCntuHit.hxx"

#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGparGeo.hxx"

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

   void SetFlagMC(Bool_t is_MC) { flagMC = is_MC; }
   Bool_t GetFlagMC() { return flagMC; }
   void Finalize(); // function to call after eventloop

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

   //studies on purity of the track
   void           EvaluateTrack();

   
   void PrintEfficiency();
   bool isVTMatched(Int_t Id_part);

       protected : TAGdataDsc *fpBMntuTrack; ///< BM track pointer

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
   TH1F*            fpHisTrackMultiplicity_frag;     ///< multiplicity of clusters of different MC particles in a reconstructed track if fragmentation
   TH1F*            fpHisTrackMultiplicity_primary;     ///< multiplicity of clusters of different MC particles in a reconstructed track if no fragmentation
   TH1F *           fpReconstructedTracks;
   TH1F *           fpTrackEfficiency;
   TH1F *           fpTrackPurityTrack;
   TH1F *           fpTrackEfficiencyFake;
   TH1F *           fpTrackPurity;
   TH1F * fpTrackClones;
   TH1F * fpBadTrackPosZ;
   TH1F *fpBadTrackCharge;
   TH1F *fpBadTrackFlukaID;
   TH1F *fpBadTrackMothID;
   TH1F *fpBadTrackInitP;
   //Int_t primary_charget = ((TAGparGeo *)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object())->GetBeamPar().AtomicNumber;
   TH1F *fpHisThetaRes[8]; ///< theta angle resolution of the track wrt to MC part   ! hard coded
   TH1F *fpTrackClusResX[MaxTrkSens]; ///< Resolution of each tracked clus wrt MC part
   TH1F *fpTrackClusResY[MaxTrkSens]; ///< Resolution of each tracked clus wrt MC part

   Bool_t flagMC; ///< if the dataset is MC

   ClassDef(TAVTactBaseNtuTrack, 0)

       // Efficiency & Purity variables
   map<int, int> m_nRecoTracks;      ///< Map of total number of reconstructed track candidates; the key is the particle charge (1,2,3...)
   map<int, int> m_nMCTracks;            ///< Map of total number of MC particles generated in the geometrical acceptance of the VTX; the key is the particle charge (1,2,3...)
   map<int, int> m_nRecoTracks_matched;
   map<int, int> m_nCorrectClus; ///< Map of total number of MC clusters well associated to a VTX track; the key is the particle charge (1,2,3...)
   map<int, int> m_nTotalClus;   ///< Map of total number of MC clusters  associated to a VTX track; the key is the particle charge (1,2,3...)
   map<int, map<int,int>> m_nClone;   ///< Map of total number of clone id for the same charge in a single event; the key is [particle charge][specific ID]
   map<int, int> n_clones;             ///< Map of total number of clone id for the same charge ; the key is [particle charge]

   private : 
   TAMCntuPart *pNtuEve; ///< Ptr to TAMCntuPart object
   TAMCntuRegion *pNtuReg; ///< Ptr to TAMCntuReg object
   TAMCntuHit *pNtuHit;    ///< Ptr to TAMCntuHit object
};

#endif
