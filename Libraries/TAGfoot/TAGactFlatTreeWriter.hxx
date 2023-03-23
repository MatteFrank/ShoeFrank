
#ifndef _TAGactFlatTreeWriter_HXX_
#define _TAGactFlatTreeWriter_HXX_

/*!
 \file TAGactFlatTreeWriter.hxx
 \brief  Base class for reconstruction
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/


#include "TNamed.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

#include "TAGactTreeReader.hxx"
#include "TAGaction.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"


#include "TAGactTreeWriter.hxx"

using namespace std;

class TAGactFlatTreeWriter : public TAGactTreeWriter
{
public:
   // default constructor
   TAGactFlatTreeWriter(const char* name = 0,
                    Bool_t isMC      = false);
   
   // default destructor
   virtual ~TAGactFlatTreeWriter();
   
   // Loop events
   Bool_t Process();
   
   //! Open File In
   Int_t  Open(const TString& name, Option_t* option="RECREATE", const TString treeName="tree", Bool_t dscBranch = true);

   // virtual close file
   void   Close();
      
   // Create branch in tree
   void   SetTreeBranches();
     
   // Fill tree out
   void   FillTreeOut();
   
protected:
   TFile*          fpFile;   ///< File writer
   TTree*          fpTree;        ///< Flay Ntuple out

   TAGdataDsc*     fpNtuHitSt;           ///< VT vertex container
   TAGdataDsc*     fpNtuTrackBm;           ///< VT vertex container
   TAGdataDsc*     fpNtuTrackVtx;           ///< VT vertex container
   TAGdataDsc*     fpNtuVtx;           ///< VT vertex container
   TAGdataDsc*     fpNtuClusIt;        ///< IT cluster container
   TAGdataDsc*     fpNtuTrackIt;        ///< IT cluster container
   TAGdataDsc*     fpNtuRecMsd;      ///< MSD point container
   TAGdataDsc*     fpNtuTrackMsd;      ///< MSD point container
   TAGdataDsc*     fpNtuRecTw;          ///< TW rec point container
   TAGdataDsc*     fpNtuClusCa;         ///< CA cluster container
   TAGdataDsc*     fpNtuGlbTrack;          ///< output tracks
   TAGdataDsc*     fpNtuMcTrk;          ///< output tracks
   
   TAGparaDsc*     fpGeoMapVtx;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapItr;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapMsd;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapTof;         ///< geometry para dsc
   TAGparaDsc*     fpGeoMapG;           ///< geometry para dsc
   Bool_t          fFlagMC;             ///< MC flag
   Bool_t          fFlagTrack;          ///< tracking flag
   Bool_t          fFlagItrTrack;       ///< flag for ITR tracking
   Bool_t          fFlagMsdTrack;       ///< flag for MSD tracking

   TAGgeoTrafo*    fpFootGeo;           ///< First geometry transformer
   ///<
   //Output fields
   //ST
   Int_t                 fHitsNst;       ///< ST hit number
   vector<float>         fChargeSt;      ///< ST charge
   vector<float>         fTimeSt;        ///< ST time
   vector<TVector3>      fPosSt;         ///< ST position
   
   //BM
   Int_t                 fTracksNbm;     ///< BM track number
   vector<int>           fTrackIdBm;     ///< BM track id
   vector<TVector3>      fPversBm;       ///< BM track slope
   vector<TVector3>      fR0Bm;          ///< BM track origin
   vector<float>         fTrackChi2Bm;   ///< BM track chi2

   //VTX
   Int_t                fTracksNvt;              ///< VTX track number
   vector<int>          fTrackIdVt;              ///< VTX track id
   vector<TVector3>     fTrackClusPosVt;         ///< VTX track position
   vector<TVector3>     fTrackSlopezVt;          ///< VTX track slope
   vector<TVector3>     fTrackOriginVt;          ///< VTX track origin
   vector<vector<TVector3> > fTrackClusPosVecVt; ///< VTX cluster track position
   vector<int>          fTrackClustersNvt;       ///< VTX cluster track number
   vector<int>          fTrackClusHitsNvt;       ///< VTX hit cluster track number
   vector<float>        fTrackChi2Vt;            ///< VTX track Chi2
   
   Int_t                fVextexNvt;              ///< VTX vertice number
   vector<TVector3>     fVertexPosVt;            ///< VTX vertice position
   
   //ITR
   vector<int>          fSensorIdIt;             ///< ITR sensor id
   vector<int>          fClustersNit;            ///< ITR cluster number
   vector<TVector3>     fClusPosIt;              ///< ITR cluster position
   vector<vector<TVector3> > fClusPosVecIt;      ///< ITR cluster position vector
   
   Int_t                fTracksNit;              ///< ITR track number
   vector<int>          fTrackIdIt;              ///< ITR track id
   vector<TVector3>     fTrackClusPosIt;         ///< ITR track position
   vector<TVector3>     fTrackSlopezIt;          ///< ITR track slope
   vector<TVector3>     fTrackOriginIt;          ///< ITR track origin
   vector<vector<TVector3> > fTrackClusPosVecIt; ///< ITR cluster track posi
   vector<int>          fTrackClustersNit;       ///< ITR cluster track numb
   vector<int>          fTrackClusHitsNit;       ///< ITR hit cluster track
   vector<float>        fTrackChi2It;            ///< ITR track Chi2
   
   //MSD
   vector<int>          fStationIdMsd;           ///< MSD station id
   vector<int>          fPointsNmsd;             ///< MSD point number
   vector<double>       fEnergyLoss1Msd;         ///< MSD point energy loss position
   vector<double>       fEnergyLoss2Msd;         ///< MSD point energy loss position
   vector<double>       fTimeMsd;                ///< MSD point time
   vector<TVector3>     fPosMsd;                 ///< MSD point position
   
   Int_t                fTracksNmsd;             ///< MSD track number
   vector<int>          fTrackIdMsd;             ///< MSD track id
   vector<TVector3>     fTrackClusPosMsd;        ///< MSD track position
   vector<TVector3>     fTrackSlopezMsd;         ///< MSD track slope
   vector<TVector3>     fTrackOriginMsd;         ///< MSD track origin
   vector<vector<TVector3> > fTrackClusPosVecMsd;///< MSD cluster track p
   vector<int>          fTrackClustersNmsd;      ///< MSD cluster track n
   vector<int>          fTrackClusHitsNmsd;      ///< MSD hit cluster tra
   vector<float>        fTrackChi2Msd;           ///< MSD track Chi2
   
   //TW
   Int_t                fPointsNtw;              ///< TW point number
   vector<double>       fEnergyLossTw;           ///< TW point energy loss
   vector<double>       fTimeTw;                 ///< TW point time
   vector<TVector3>     fPosTw;                  ///< TW point position
   vector<double>       fChargeZTw;              ///< TW point charge Z
   
   //CA
   Int_t                fClustersNca;            ///< CAL cluster number
   vector<double>       fEnergyCa;               ///< CAL cluster energy
   vector<TVector3>     fPosCa;                  ///< CAL cluster position

   //Glb track
   vector<Long64_t>     fEvtNumberGlb;      ///< event number
   vector<int>          fPdgIdGlb;          ///< PDG Id used in the fit
   vector<double>       fLengthGlb;         ///< track length from target to TW
   vector<double>       fChi2Glb;           ///< Chi2 of the fitted track
   vector<Int_t>        fNdofGlb;           ///< number of freedom of the fitted track
   vector<double>       fPvalGlb;           ///< p-Value of the fitted track
   vector<double>       fQualityGlb;        ///< quality factor of the fitted track
   
   vector<double>       fMassGlb;           ///< Initial mass
   vector<double>       fMomModuleGlb;      ///< Momentum module
   vector<int>          fTwChargeZGlb;      ///< TW atomic charge Z
   vector<double>       fTwTofGlb;          ///< TW time of flight
   vector<double>       fCalEnergyGlb;      ///< CAL energy (loss)
   vector<int>          fTrkIdGlb;          ///< track absolute Id
   
   vector<double>       fFitMassGlb;         ///< fitted mass
   vector<int>          fFitChargeZGlb;      ///< fitted charge Z
   vector<double>       fFitTofGlb;          ///< fitted time of flight
   vector<double>       fFitEnergyLossGlb;   ///< fitted energy loss
   vector<double>       fFitEnergyGlb;       ///< fitted energy
   
   //Particle momentum and positions computed on target middle
   vector<TVector3>     fTgtDirGlb;          ///< Direction of particle at target
   vector<TVector3>     fTgtPosGlb;          ///< Position of particle at target
   vector<TVector3>     fTgtPosErrorGlb;     ///< Position error of particle at target
   vector<TVector3>     fTgtMomGlb;          ///< Momentum of particle at target
   vector<TVector3>     fTgtMomErrorGlb;     ///< Momentum error of particle at target
   
   //Particle momentum and positions computed on TW Wall
   vector<TVector3>     fTwPosGlb;           ///< Position of particle at TW
   vector<TVector3>     fTwPosErrorGlb;      ///< Position error of particle at TW
   vector<TVector3>     fTwMomGlb;           ///< Momentum of particle at TW
   vector<TVector3>     fTwMomErrorGlb;      ///< Momentum error of particle at TW

   //MC
   Int_t                fPartsNmc;           ///< particle number
   vector<double>       fPartChargeMc;       ///< particle charge
   vector<double>       fPartMassMc;         ///< particle mass
   vector<double>       fPartTofMc;          ///< particle tof
   vector<double>       fPartLengthMc;       ///< particle length
   vector<float>        fPartInPosxMc;       ///< particle input position x
   vector<float>        fPartInPosyMc;       ///< particle input position y
   vector<float>        fPartInPoszMc;       ///< particle input position z
   vector<float>        fPartOutPosxMc;      ///< particle output position x
   vector<float>        fPartOutPosyMc;      ///< particle output position y
   vector<float>        fPartOutPoszMc;      ///< particle output position z

 protected:
   // Fill tree Out ST
   void FillTreeOutSt();
   // Fill tree Out BM
   void FillTreeOutBm();
   // Fill tree Out VT
   void FillTreeOutVt();
   // Fill tree Out IT
   void FillTreeOutIt();
   // Fill tree Out MSD
   void FillTreeOutMsd();
   // Fill tree Out TW
   void FillTreeOutTw();
   // Fill tree Out CA
   void FillTreeOutCa();
   // Fill tree Out Glb
   void FillTreeOutGlb();
   // Fill tree Out MC
   void FillTreeOutMc();
   
   // Reset items in tree out
   void ResetTreeOut();
   
   // Set descriptors
   void SetDescriptors();

   ClassDef(TAGactFlatTreeWriter, 1);        ///< Base class for reconstruction
};

#endif
