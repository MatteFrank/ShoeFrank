
#ifndef _ConvertNtuple_HXX_
#define _ConvertNtuple_HXX_

/*!
 \file ConvertNtuple.hxx
 \brief  Base class for reconstruction
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/


#include "TNamed.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

#include "TAGaction.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TAGparGeo.hxx"
#include "TADIparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TACAparGeo.hxx"

#include "TASTntuHit.hxx"

#include "TABMntuHit.hxx"
#include "TABMntuTrack.hxx"

#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAITntuHit.hxx"
#include "TAITntuCluster.hxx"
#include "TAITntuTrack.hxx"

#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDntuTrack.hxx"

#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"

#include "TACAntuHit.hxx"
#include "TACAntuCluster.hxx"

#include "TAGntuGlbTrack.hxx"

#include "TAGactionFile.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TAMCntuEvent.hxx"

using namespace std;

class ConvertNtuple : public TNamed // using TNamed for the in/out files
{
public:
   // default constructor
   ConvertNtuple(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC = false, TString fileNameMcIn = "", TString treeNameMc = "EventTree");
   
   // default destructor
   virtual ~ConvertNtuple();
   
   // Read parameters
   void ReadParFiles();
   
   // Create raw action
   virtual void CreateRecAction();
   
   //! Create rec action
   virtual void CreateRawAction()      { return; }
   

   // Add required items
   virtual void AddRecRequiredItem();
   
   // Loop events
   virtual void LoopEvent(Int_t nEvents);
   
   //! Goto Event
   virtual Bool_t GoEvent(Int_t /*iEvent*/) { return true; }
   
   // Begin loop
   virtual void BeforeEventLoop();

   // End loop
   virtual void AfterEventLoop();
   
   //! Open File In
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();
      
   // Open File Out
   virtual void OpenFileOut();
   
   // Close File Out
   virtual void CloseFileOut();
   
   // Global sets
   virtual void GlobalSettings();
   
   // Create branch in tree
   virtual void SetTreeBranches();
  
  // Create L0 branch in tree
  virtual void SetL0TreeBranches();
   
   //! Set experiment name
   virtual void SetExpName(const Char_t* name) { fExpName = name;  }
   
   //! Set run number
   void SetRunNumber(Int_t run)                { fRunNumber = run; }

   // Fill tree out
   void FillTreeOut();
   
   //! Flag for MC data
   Bool_t IsMcData()           { return fFlagMC;          }
   
   // Campaign checks
   void CampaignChecks();
   
   // Add friend tree
   void AddFriendTree(TString fileName, TString treeName);
   
   //! Enable tracking
   void EnableTracking()       { fFlagTrack = true;       }
   //! Disable tracking
   void DisableTracking()      { fFlagTrack = false;      }
   
   //! Enable MSD tracking
   void EnableMsdTracking()    { fFlagMsdTrack = true;    }
   //! Disable MSD tracking
   void DisableMsdTracking()   { fFlagMsdTrack = false;   }
   
   //! Enable ITR tracking
   void EnableItrTracking()    { fFlagItrTrack = true;    }
   //! Disable ITR tracking
   void DisableItrTracking()   { fFlagItrTrack = false;   }
   
   // Par geo getters
   //! Get parameters geo transformations
   TAGgeoTrafo*         GetGeoTrafo()       const { return fpFootGeo;                                }
   //! Get STC geometry parameters
   TASTparGeo*          GetParGeoSt()       const { return (TASTparGeo*)fpParGeoSt->Object();        }
   //! Get target geometry parameters
   TAGparGeo*           GetParGeoG()        const { return (TAGparGeo*)fpParGeoG->Object();          }
   //! Get BM geometry parameters
   TABMparGeo*          GetParGeoBm()       const { return (TABMparGeo*)fpParGeoBm->Object();        }
   //! Get VTX geometry parameters
   TAVTparGeo*          GetParGeoVtx()      const { return (TAVTparGeo*)fpParGeoVtx->Object();       }
   //! Get ITR geometry parameters
   TAITparGeo*          GetParGeoIt()       const { return (TAITparGeo*)fpParGeoIt->Object();        }
   //! Get MSD geometry parameters
   TAMSDparGeo*         GetParGeoMsd()      const { return (TAMSDparGeo*)fpParGeoMsd->Object();      }
   //! Get TW geometry parameters
   TATWparGeo*          GetParGeoTw()       const { return (TATWparGeo*)fpParGeoTw->Object();        }
   //! Get CAL geometry parameters
   TACAparGeo*          GetParGeoCa()       const { return (TACAparGeo*)fpParGeoCa->Object();        }
   
   // Containers getters
   //! Get STC hits containers
   TASTntuHit*          GetNtuHitSt()       const { return (TASTntuHit*) fpNtuHitSt->Object();       }
   //! Get BM hits containers
   TABMntuHit*          GetNtuHitBm()       const { return (TABMntuHit*)fpNtuHitBm->Object();        }
   //! Get BM tracks containers
   TABMntuTrack*        GetNtuTrackBm()     const { return (TABMntuTrack*)fpNtuTrackBm->Object();    }
   
   //! Get VTX clusters containers
   TAVTntuCluster*      GetNtuClusterVtx()  const { return (TAVTntuCluster*)fpNtuClusVtx->Object();  }
   //! Get VTX Tracks containers
   TAVTntuTrack*        GetNtuTrackVtx()    const { return (TAVTntuTrack*)fpNtuTrackVtx->Object();   }
   //! Get VTX vertex containers
   TAVTntuVertex*       GetNtuVertexVtx()   const { return (TAVTntuVertex*)fpNtuVtx->Object();       }
   //! Get VTX vertex desciptor
   TAGdataDsc*          GetDscVertexVtx()   const { return fpNtuVtx;                                 }
   
   //! Get ITR clusters containers
   TAITntuCluster*      GetNtuClusterIt()   const { return (TAITntuCluster*)fpNtuClusIt->Object();   }
   //! Get ITR tracks containers
   TAITntuTrack*        GetNtuTrackIt()     const { return (TAITntuTrack*)fpNtuTrackIt->Object();    }

   //! Get MSD clusters containers
   TAMSDntuCluster*     GetNtuClusterMsd()  const { return (TAMSDntuCluster*)fpNtuClusMsd->Object(); }
   //! Get MSD points containers
   TAMSDntuPoint*       GetNtuPointMsd()    const { return (TAMSDntuPoint*)fpNtuRecMsd->Object();    }
   //! Get MSD tracks containers
   TAMSDntuTrack*       GetNtuTrackMsd()    const { return (TAMSDntuTrack*)fpNtuTrackMsd->Object();  }

   //! Get TW hits containers
   TATWntuHit*          GetNtuHitTw()       const { return (TATWntuHit*) fpNtuHitTw->Object();       }
   //! Get TW points containers
   TATWntuPoint*        GetNtuPointTw()     const { return (TATWntuPoint*) fpNtuRecTw->Object();     }
   
   //! Get CAL hits containers
   TACAntuHit*          GetNtuHitCa()       const { return (TACAntuHit*) fpNtuHitCa->Object();       }
   //! Get CAL clusters containers
   TACAntuCluster*      GetNtuClusterCa()   const { return (TACAntuCluster*) fpNtuClusCa->Object();  }

   //! Get global tracks containers
   TAGntuGlbTrack*      GetNtuGlbTrack()    const { return (TAGntuGlbTrack*)fpNtuGlbTrack->Object(); }
   
   // MC container Getter
   //! MC event container Getter
   TAMCntuEvent*        GetNtuMcEvt()       const { return (TAMCntuEvent*)fpNtuMcEvt->Object();      }
   //! MC particle container Getter
   TAMCntuPart*         GetNtuMcTrk()       const { return (TAMCntuPart*)fpNtuMcTrk->Object();       }
   //! MC region container Getter
   TAMCntuRegion*       GetNtuMcReg()       const { return (TAMCntuRegion*)fpNtuMcReg->Object();     }
   //! MC STC hit container Getter
   TAMCntuHit*          GetNtuMcSt()        const { return (TAMCntuHit*)fpNtuMcSt->Object();         }
   //! MC BM hit container Getter
   TAMCntuHit*          GetNtuMcBm()        const { return (TAMCntuHit*)fpNtuMcBm->Object();         }
   //! MC VTX hit container Getter
   TAMCntuHit*          GetNtuMcVtx()       const { return (TAMCntuHit*)fpNtuMcVt->Object();         }
   //! MC ITR hit container Getter
   TAMCntuHit*          GetNtuMcIt()        const { return (TAMCntuHit*)fpNtuMcIt->Object();         }
   //! MC MSD hit container Getter
   TAMCntuHit*          GetNtuMcMsd()       const { return (TAMCntuHit*)fpNtuMcMsd->Object();        }
   //! MC TW hit container Getter
   TAMCntuHit*          GetNtuMcTw()        const { return (TAMCntuHit*)fpNtuMcTw->Object();         }
   //! MC CAL hit container Getter
   TAMCntuHit*          GetNtuMcCa()        const { return (TAMCntuHit*)fpNtuMcCa->Object();         }
  

public:
   //! Disable MC info saving in output tree
   static void DisableSaveMc() { fSaveMcFlag = false; }
   //! Enable MC info saving in output tree
   static void EnableSaveMc()  { fSaveMcFlag = true;  }
   //! Check MC info saving in output tree
   static Bool_t IsSaveMc()    { return fSaveMcFlag;  }
   
protected:
   TString               fExpName;        ///< Experiment name
   TAGcampaignManager*   fCampManager;    ///< Campaign manager
   Int_t                 fRunNumber;      ///< Run number
   Int_t                 fSkipEventsN;    ///< Skipped events
   TAGroot*              fTAGroot;        ///< pointer to TAGroot
   TAGgeoTrafo*          fpFootGeo;       ///< trafo prointer
   TFile*                fActEvtWriter;   ///< File writer
   TTree*                fTreeOut;        ///< Flay Ntuple out
   TString               fFriendFileName; ///< Friend file name
   TString               fFriendTreeName; ///< Friend tree name

   TAGparaDsc*           fpParGeoSt;      ///< STC geometry parameter
   TAGparaDsc*           fpParGeoG;       ///< Beam/target geometry parameter
   TAGparaDsc*           fpParGeoBm;      ///< BM geometry parameter
   TAGparaDsc*           fpParGeoVtx;     ///< VTX geometry parameter
   TAGparaDsc*           fpParGeoIt;      ///< ITR geometry parameter
   TAGparaDsc*           fpParGeoMsd;     ///< MSD geometry parameter
   TAGparaDsc*           fpParGeoTw;      ///< TW geometry parameter
   TAGparaDsc*           fpParGeoCa;      ///< CAL geometry parameter
   
   TAGdataDsc*           fpNtuHitSt;     ///< Hit input dsc for STC
   TAGdataDsc*           fpNtuHitBm;     ///< Hit input dsc for STC
   TAGdataDsc*           fpNtuHitVtx;    ///< Hit input dsc for VTX
   TAGdataDsc*           fpNtuHitIt;     ///< Hit input dsc for ITR
   TAGdataDsc*           fpNtuHitMsd;    ///< Hit input dsc for MSD
   TAGdataDsc*           fpNtuHitTw;     ///< Hit input dsc for TW
   TAGdataDsc*           fpNtuHitCa;     ///< Hit input dsc for CAL

   TAGdataDsc*           fpNtuClusVtx;	  ///< input cluster data dsc for VTX
   TAGdataDsc*           fpNtuClusIt;	  ///< input cluster data dsc for ITR
   TAGdataDsc*           fpNtuClusMsd;   ///< input cluster data dsc for MSD
   TAGdataDsc*           fpNtuRecMsd;    ///< input point data dsc for MSD
   TAGdataDsc*           fpNtuRecTw;     ///< input point data dsc for TW
   TAGdataDsc*           fpNtuClusCa;    ///< input cluster data dsc for CAL
  
   TAGdataDsc*           fpNtuMcEvt;     ///< input MC event dsc
   TAGdataDsc*           fpNtuMcTrk;     ///< input MC track dsc
   TAGdataDsc*           fpNtuMcReg;     ///< input MC region dsc
   TAGdataDsc*           fpNtuMcSt;      ///< input MC dsc for STC
   TAGdataDsc*           fpNtuMcBm;      ///< input MC dsc for BM
   TAGdataDsc*           fpNtuMcVt;      ///< input MC dsc for VTX
   TAGdataDsc*           fpNtuMcIt;      ///< input MC dsc for ITR
   TAGdataDsc*           fpNtuMcMsd;     ///< input MC dsc for MSD
   TAGdataDsc*           fpNtuMcTw;      ///< input MC dsc for TW
   TAGdataDsc*           fpNtuMcCa;      ///< input MC dsc for CAL
  
   TAGdataDsc*           fpNtuTrackBm;   ///< input track data dsc for BM
   TAGdataDsc*           fpNtuTrackVtx;  ///< input track data dsc for VTX
   TAGdataDsc*           fpNtuTrackIt;   ///< input track data dsc for ITR
   TAGdataDsc*           fpNtuTrackMsd;  ///< input track data dsc for MSD
   TAGdataDsc*           fpNtuVtx;       ///< input Vtx data dsc for VTX
   
   TAGdataDsc*           fpNtuGlbTrack;  ///< input data dsc global track TOE/GenFit
   
   TAGactionFile*        fActEvtReader;  ///< Tree/event reader
   
   
   Bool_t                fFlagOut;       ///< flag for output file
   Bool_t                fFlagTrack;     ///< flag for tracking
   Bool_t                fFlagMsdTrack;  ///< flag for MSD tracking
   Bool_t                fFlagItrTrack;  ///< flag for ITR tracking
   Bool_t                fFlagMC;        ///< MC flag
   Bool_t                fReadL0Hits;    ///< read back hits

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
   // Create reconstruction action for BM
   void CreateRecActionBm();
   // Create reconstruction action for VTX
   void CreateRecActionVtx();
   // Create reconstruction action for ITR
   void CreateRecActionIt();
   // Create reconstruction action for MSD
   void CreateRecActionMsd();
   // Create reconstruction action for TW
   void CreateRecActionTw();
   // Create reconstruction action for CAL
   void CreateRecActionCa();
   // Create reconstruction action for global tracks with TOE
   void CreateRecActionGlb();
   
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
   
protected:
   static Bool_t fgItrTrackFlag; ///< ITR tracking flag
   static Bool_t fgMsdTrackFlag; ///< MSD tracking flag
   static Bool_t fSaveMcFlag;    ///< MC saving flag

   ClassDef(ConvertNtuple, 1);        ///< Base class for reconstruction
};


#endif
