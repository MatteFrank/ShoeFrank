
#ifndef _BaseReco_HXX_
#define _BaseReco_HXX_

/*!
 \file BaseReco.hxx
 \brief  Base class for reconstruction
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/


#include "TNamed.h"
#include "TString.h"
#include "TFile.h"

#include "TAGaction.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGbaseWDparTime.hxx"
#include "TAGbaseWDparMap.hxx"

#include "TASTparMap.hxx"
#include "TABMparMap.hxx"
#include "TAVTparMap.hxx"
#include "TAITparMap.hxx"
#include "TAMSDparMap.hxx"
#include "TATWparMap.hxx"
#include "TACAparMap.hxx"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TAGparGeo.hxx"
#include "TADIparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TACAparGeo.hxx"

#include "TADIgenField.hxx"
#include "TADIgeoField.hxx"

#include "TABMparCal.hxx"
#include "TAMSDparCal.hxx"
#include "TATWparCal.hxx"
#include "TACAparCal.hxx"

#include "TABMparConf.hxx"
#include "TAVTparConf.hxx"
#include "TAITparConf.hxx"
#include "TAMSDparConf.hxx"

#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAITntuTrack.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDntuTrack.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuHit.hxx"
#include "TACAactNtuHit.hxx"
#include "TACAntuCluster.hxx"
#include "TAIRntuTrack.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TAGactionFile.hxx"

#include "TAVTactNtuCluster.hxx"
#include "TAVTactNtuClusterMT.hxx"
#include "TAITactNtuCluster.hxx"
#include "TAITactNtuClusterMT.hxx"
#include "TAMSDactNtuCluster.hxx"
#include "TAMSDactNtuPoint.hxx"
#include "TACAactNtuCluster.hxx"
#include "TATWactNtuPoint.hxx"
#include "TATWactCalibTW.hxx"

#include "TABMactNtuTrack.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAITactNtuTrack.hxx"
#include "TAITactNtuTrackF.hxx"
#include "TAMSDactNtuTrack.hxx"
#include "TAMSDactNtuTrackF.hxx"

#include "TAVTactNtuVertex.hxx"

#include "TAGactNtuGlbTrackS.hxx"
#include "TAGactNtuGlbTrack.hxx"

#include "TAGactKFitter.hxx"
#include "UpdatePDG.hxx"

#include "TAGFtrackingStudies.hxx"

class TAMCntuHit;
class TAMCntuPart;
class TAMCntuRegion;
class TAMCntuEvent;

class BaseReco : public TNamed // using TNamed for the in/out files
{
public:
   //! default constructor
   BaseReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout);
   
   //! default destructor
   virtual ~BaseReco();
   
   //! Read parameters
   void ReadParFiles();

   //! Create raw action
   virtual void CreateRecAction();
   
   //! Create rec action
   virtual void CreateRawAction()      { return; }
   
   //! Add required items
   virtual void AddRawRequiredItem()   { return; }
   
   //! Add required items
   virtual void AddRecRequiredItem();
   
   //! Set raw histogram directory
   virtual void SetRawHistogramDir()   { return; }
   
   //! Set rec histogram directory
   virtual void SetRecHistogramDir();
   
   //! Loop events
   virtual void LoopEvent(Int_t nEvents);
   
   //! Goto Event
   virtual Bool_t GoEvent(Int_t /*iEvent*/) { return true; }
   
   //! Begin loop
   virtual void BeforeEventLoop();

   //! End loop
   virtual void AfterEventLoop();
   
   //! Open File In
   virtual void OpenFileIn()  { return; }
  
   ///< ! Global Checks
   virtual void GlobalChecks();
   
   //! Close File in
   virtual void CloseFileIn() { return; }
      
   //! Open File Out
   virtual void OpenFileOut();
   
   //! Close File Out
   virtual void CloseFileOut();
   
   //! Create branch in tree
   virtual void SetTreeBranches();
  
  //! Create L0 branch in tree
  virtual void SetL0TreeBranches();
   
   //! Set experiment name
   virtual void SetExpName(const Char_t* name) { fExpName = name;  }
   
   //! Set run number
   void SetRunNumber(Int_t run)                { fRunNumber = run; }

   //! Enable tree
   void EnableTree()           { fFlagTree = true;        }
   //! Disable tree
   void DisableTree()          { fFlagTree = false;       }
   
   //! Enable save hits
   void EnableSaveHits()       { fFlagHits = true;        }
   //! Disable save hits
   void DisableSaveHits()      { fFlagHits = false;       }

   //! Enable Histo
   void EnableHisto()          { fFlagHisto = true;       }
   //! Disable Histo
   void DisableHisto()         { fFlagHisto = false;      }
   
   //! Enable tracking
   void EnableTracking()       { fFlagTrack = true;       }
   //! Disable tracking
   void DisableTracking()      { fFlagTrack = false;      }
   
   //! Enable TW calibration per bar
   void EnableTWcalibPerBar()  { fFlagTWbarCalib = true;  }
   //! Disable TW calibration per bar
   void DisableTWcalibPerBar() { fFlagTWbarCalib = false; }

   //! Enable using true MC charge for TW
   void EnableZfromMCtrue()    { fFlagZtrueMC = true;     }
   //! Disable using true MC charge for TW
   void DisableZfromMCtrue()   { fFlagZtrueMC = false;    }

   //! Enable Z reconstruction with pileup off for TW
   void EnableZrecWithPUoff()  { fFlagZrecPUoff = true;   }
   //! Disable Z reconstruction with pileup off for TW
   void DisableZrecWithPUoff() { fFlagZrecPUoff = false;  }
  
   //! Enable Z reconstruction matching for TW
   void EnableTWZmatch()       { fFlagZmatch_TW = true;   }
   //! Disable Z reconstruction matching for TW
   void DisableTWZmatch()      { fFlagZmatch_TW = false;  }
   
   //! Enable Reconstruction cutter for TOE
   void EnableRecCutter()      { fFlagRecCutter = true;   }
   //! Disable Reconstruction cutter for TOE
   void DisableRecCutter()     { fFlagRecCutter = false;  }

   //! Enable multi-threading for M28 clustering
   void DisableM28ClusMT()     { fM28ClusMtFlag = false;  }
   //! Disable multi-threading for M28 clustering
   void EnableM28lusMT()       { fM28ClusMtFlag = true;   }
   //! Check multi-threading for M28 clustering
   Bool_t IsM28ClusMT()        { return fM28ClusMtFlag;   }
   
   //! Enable read L0 hits
   void EnableReadL0Hits()     { fReadL0Hits = true;      }
   //! Disable read L0 hits
   void DisableReadL0Hits()    { fReadL0Hits = false;     }
   //! Check read L0 hits
   Bool_t IsReadL0Hits()       { return fReadL0Hits;      }

   //! Flag for MC data
   Bool_t IsMcData()           { return fFlagMC;          }
   
   //! Set Vtx Tracking algorithm
   void SetVtxTrackingAlgo(char c);
   
   //! Set Itr Tracking algorithm
   void SetItrTrackingAlgo(char c);
   
   //! Set Msd Tracking algorithm
   void SetMsdTrackingAlgo(char c);
  
   ///< Campaign checks
   void CampaignChecks();
   
   ///< Add friend tree
   void AddFriendTree(TString fileName, TString treeName);

   //! Par geo getters
   TAGgeoTrafo*         GetGeoTrafo()       const { return fpFootGeo;                                }
   TADIparGeo*          GetParGeoDi()       const { return (TADIparGeo*)fpParGeoDi->Object();        }
   TASTparGeo*          GetParGeoSt()       const { return (TASTparGeo*)fpParGeoSt->Object();        }
   TAGparGeo*           GetParGeoG()        const { return (TAGparGeo*)fpParGeoG->Object();          }
   TABMparGeo*          GetParGeoBm()       const { return (TABMparGeo*)fpParGeoBm->Object();        }
   TAVTparGeo*          GetParGeoVtx()      const { return (TAVTparGeo*)fpParGeoVtx->Object();       }
   TAITparGeo*          GetParGeoIt()       const { return (TAITparGeo*)fpParGeoIt->Object();        }
   TAMSDparGeo*         GetParGeoMsd()      const { return (TAMSDparGeo*)fpParGeoMsd->Object();      }
   TATWparGeo*          GetParGeoTw()       const { return (TATWparGeo*)fpParGeoTw->Object();        }
   TACAparGeo*          GetParGeoCa()       const { return (TACAparGeo*)fpParGeoCa->Object();        }
   
   //! Containers getters
   TASTntuHit*          GetNtuHitSt()       const { return (TASTntuHit*) fpNtuHitSt->Object();       }
   TABMntuHit*          GetNtuHitBm()       const { return (TABMntuHit*)fpNtuHitBm->Object();        }
   TABMntuTrack*        GetNtuTrackBm()     const { return (TABMntuTrack*)fpNtuTrackBm->Object();    }
   
   TAVTntuCluster*      GetNtuClusterVtx()  const { return (TAVTntuCluster*)fpNtuClusVtx->Object();  }
   TAVTntuTrack*        GetNtuTrackVtx()    const { return (TAVTntuTrack*)fpNtuTrackVtx->Object();   }
   TAVTntuVertex*       GetNtuVertexVtx()   const { return (TAVTntuVertex*)fpNtuVtx->Object();       }
   TAGdataDsc*          GetDscVertexVtx()   const { return fpNtuVtx;                                 }
   
   TAITntuCluster*      GetNtuClusterIt()   const { return (TAITntuCluster*)fpNtuClusIt->Object();   }
   TAITntuTrack*        GetNtuTrackIt()     const { return (TAITntuTrack*)fpNtuTrackIt->Object();    }

   TAMSDntuCluster*     GetNtuClusterMsd()  const { return (TAMSDntuCluster*)fpNtuClusMsd->Object(); }
   TAMSDntuPoint*       GetNtuPointMsd()    const { return (TAMSDntuPoint*)fpNtuRecMsd->Object();    }
   TAMSDntuTrack*       GetNtuTrackMsd()    const { return (TAMSDntuTrack*)fpNtuTrackMsd->Object();  }

   TATWntuHit*          GetNtuHitTw()       const { return (TATWntuHit*) fpNtuHitTw->Object();       }
   TATWntuPoint*        GetNtuPointTw()     const { return (TATWntuPoint*) fpNtuRecTw->Object();     }
   
   TACAntuHit*          GetNtuHitCa()       const { return (TACAntuHit*) fpNtuHitCa->Object();       }
   TACAntuCluster*      GetNtuClusterCa()   const { return (TACAntuCluster*) fpNtuClusCa->Object();  }

   TAGntuGlbTrack*      GetNtuGlbTrack()    const { return (TAGntuGlbTrack*)fpNtuGlbTrack->Object(); }
   TADIgeoField*        GetFootField()      const { return fField;                                   }
   
   //! MC container Getter
   TAMCntuEvent*        GetNtuMcEvt()       const { return (TAMCntuEvent*)fpNtuMcEvt->Object();      }
   TAMCntuPart*         GetNtuMcTrk()       const { return (TAMCntuPart*)fpNtuMcTrk->Object();       }
   TAMCntuRegion*       GetNtuMcReg()       const { return (TAMCntuRegion*)fpNtuMcReg->Object();     }
   TAMCntuHit*          GetNtuMcSt()        const { return (TAMCntuHit*)fpNtuMcSt->Object();         }
   TAMCntuHit*          GetNtuMcBm()        const { return (TAMCntuHit*)fpNtuMcBm->Object();         }
   TAMCntuHit*          GetNtuMcVtx()       const { return (TAMCntuHit*)fpNtuMcVt->Object();         }
   TAMCntuHit*          GetNtuMcIt()        const { return (TAMCntuHit*)fpNtuMcIt->Object();         }
   TAMCntuHit*          GetNtuMcMsd()       const { return (TAMCntuHit*)fpNtuMcMsd->Object();        }
   TAMCntuHit*          GetNtuMcTw()        const { return (TAMCntuHit*)fpNtuMcTw->Object();         }
   TAMCntuHit*          GetNtuMcCa()        const { return (TAMCntuHit*)fpNtuMcCa->Object();         }
  

public:
   //! Disable ITR tracking
   static void DisableItrTracking() { fgItrTrackFlag = false; }
   //! Enable ITR tracking
   static void EnableItrTracking()  { fgItrTrackFlag = true;  }
   //! Check ITR tracking
   static Bool_t IsItrTracking()    { return fgItrTrackFlag;  }
   
   //! Disable MSD tracking
   static void DisableMsdTracking() { fgMsdTrackFlag = false; }
   //! Enable MSD tracking
   static void EnableMsdTracking()  { fgMsdTrackFlag = true;  }
   //! Check MSD tracking
   static Bool_t IsMsdTracking()    { return fgMsdTrackFlag;  }
   
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
   TAGroot*              fTAGroot;        ///< pointer to TAGroot
   TAGgeoTrafo*          fpFootGeo;       ///< trafo prointer
   TString               fFriendFileName; ///< Friend file name
   TString               fFriendTreeName; ///< Friend tree name

   TAGparaDsc*           fpParTimeWD;     ///< WD time parameter

   TAGparaDsc*           fpParMapWD;      ///< WD mapping parameter
   TAGparaDsc*           fpParMapSt;      ///< STC mapping parameter
   TAGparaDsc*           fpParMapBm;      ///< BM mapping parameter
   TAGparaDsc*           fpParMapVtx;     ///< VTX mapping parameter
   TAGparaDsc*           fpParMapIt;      ///< ITR mapping parameter
   TAGparaDsc*           fpParMapMsd;     ///< MSD mapping parameter
   TAGparaDsc*           fpParMapTw;      ///< TW mapping parameter
   TAGparaDsc*           fpParMapCa;      ///< CAL mapping parameter

   TAGparaDsc*           fpParGeoSt;      ///< STC geometry parameter
   TAGparaDsc*           fpParGeoG;       ///< Beam/target geometry parameter
   TAGparaDsc*           fpParGeoDi;      ///< Dipole geometry parameter
   TAGparaDsc*           fpParGeoBm;      ///< BM geometry parameter
   TAGparaDsc*           fpParGeoVtx;     ///< VTX geometry parameter
   TAGparaDsc*           fpParGeoIt;      ///< ITR geometry parameter
   TAGparaDsc*           fpParGeoMsd;     ///< MSD geometry parameter
   TAGparaDsc*           fpParGeoTw;      ///< TW geometry parameter
   TAGparaDsc*           fpParGeoCa;      ///< CAL geometry parameter
   
   TAGparaDsc*           fpParCalBm;      ///< BM calibration parameter
   TAGparaDsc*           fpParCalMsd;     ///< MSD calibration parameter
   TAGparaDsc*           fpParCalTw;      ///< TW calibration parameter
   TAGparaDsc*           fpParCalCa;      ///< CAL calibration parameter

   TAGparaDsc*           fpParConfBm;     ///< BM configuration parameter
   TAGparaDsc*           fpParConfVtx;    ///< VTX configuration parameter
   TAGparaDsc*           fpParConfIt;     ///< ITR configuration parameter
   TAGparaDsc*           fpParConfMsd;    ///< MSD configuration parameter
   
   TAGdataDsc*           fpDatRawSt;     ///< Raw hit input dsc for STC
   TAGdataDsc*           fpNtuHitSt;     ///< Hit input dsc for STC
   TAGdataDsc*           fpDatRawBm;     ///< Raw hit input dsc for BM
   TAGdataDsc*           fpNtuHitBm;     ///< Hit input dsc for STC
   TAGdataDsc*           fpNtuHitVtx;    ///< Hit input dsc for VTX
   TAGdataDsc*           fpNtuHitIt;     ///< Hit input dsc for ITR
   TAGdataDsc*           fpDatRawMsd;    ///< Raw hit input dsc for MSD
   TAGdataDsc*           fpNtuHitMsd;    ///< Hit input dsc for MSD
   TAGdataDsc*           fpDatRawTw;     ///< Raw hit input dsc for TW
   TAGdataDsc*           fpNtuHitTw;     ///< Hit input dsc for TW
   TAGdataDsc*           fpDatRawCa;     ///< Raw hit input dsc for CAL
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
  
   TADIgeoField*         fField;         ///< magnetic field

   TAGdataDsc*           fpNtuTrackBm;   ///< input track data dsc for BM
   TAGdataDsc*           fpNtuTrackVtx;  ///< input track data dsc for VTX
   TAGdataDsc*           fpNtuTrackIt;   ///< input track data dsc for ITR
   TAGdataDsc*           fpNtuTrackMsd;  ///< input track data dsc for MSD
   TAGdataDsc*           fpNtuVtx;       ///< input Vtx data dsc for VTX

   TAGdataDsc*           fpNtuGlbTrack;  ///< input data dsc global track TOE/GenFit
   
   TAGactionFile*        fActEvtReader;  ///< Tree/event reader
   TAGactTreeWriter*     fActEvtWriter;  ///< write histo and tree

   TABMactNtuTrack*      fActTrackBm;    ///< action for tracks
   
   TAGaction*            fActClusVtx;    ///< action for VTX clusters
   TAVTactBaseNtuTrack*  fActTrackVtx;   ///< action for VTX tracks
   TAVTactBaseNtuVertex* fActVtx;        ///< action for VTX vertex
   
   TAGaction*            fActClusIt;     ///< action for ITR clusters
   TAVTactBaseNtuTrack*  fActTrackIt;    ///< action for ITR tracks

   TAMSDactNtuCluster*   fActClusMsd;    ///< action for MSD clusters
   TAMSDactNtuPoint*     fActPointMsd;   ///< action for MSD points
   TAVTactBaseNtuTrack*  fActTrackMsd;   ///< action for MSD tracsk

   TATWactNtuPoint*      fActPointTw;    ///< action for TW points
   TATWactCalibTW*       fActCalibTw;    ///< action for TW calibration

   TACAactNtuCluster*    fActClusCa;    // action for clusters
   //TACAactNtuHit*        fActNtuHitCa;    // action for hit

   TAGactNtuGlbTrack*    fActGlbTrack;   ///< Global tracking action
    
   TAGactNtuGlbTrackS*   fActGlbTrackS;  ///< action for straight tracks
  
   TAGactKFitter*        fActGlbkFitter; ///< Global tracking kalman Fitter
   TAGFtrackingStudies*  fActGlbTrackStudies;    // Global tracking studies with GenFit

   Bool_t                fFlagOut;          ///< flag for output file
   Bool_t                fFlagTree;         ///< flag to save in tree
   Bool_t                fFlagHits;         ///< flag to save hits in tree
   Bool_t                fFlagHisto;        ///< flag for histo generatiom
   Bool_t                fFlagTrack;        ///< flag for tracking
   Bool_t                fFlagTWbarCalib;   ///< flag for TW calibration per Bar
   TString               fgVtxTrackingAlgo; ///< vtx tracking algorithm ("std" with BM, "Full" combinatory)
   TString               fgItrTrackingAlgo; ///< itr tracking algorithm ("std" with BM, "Full" combinatory)
   TString               fgMsdTrackingAlgo; ///< msd tracking algorithm ("std" with BM, "Full" combinatory)
   Bool_t                fFlagZtrueMC;      ///< Z true MC flag
   Bool_t                fFlagZrecPUoff;    ///< Z rec TW PU off flag
   Bool_t                fFlagZmatch_TW;    ///< TW Z match

   Bool_t                fFlagMC;           ///< MC flag
   Bool_t                fReadL0Hits;       ///< read back hits
   Bool_t                fM28ClusMtFlag;    ///< flag for multi-threading clustering
   Bool_t                fFlagRecCutter;    ///< cutter flag for TOE Glb reco
   Int_t                 fSkipEventsN;      ///< number of events to skip
   
 protected:
   //! Create reconstruction action for BM
   void CreateRecActionBm();
   //! Create reconstruction action for VTX
   void CreateRecActionVtx();
   //! Create reconstruction action for ITR
   void CreateRecActionIt();
   //! Create reconstruction action for MSD
   void CreateRecActionMsd();
   //! Create reconstruction action for TW
   void CreateRecActionTw();
   //! Create reconstruction action for CAL
   void CreateRecActionCa();
   //! Create reconstruction action for global tracks with TOE
   void CreateRecActionGlb();
   //! Create reconstruction action for global tracks with GenFit
   void CreateRecActionGlbGF();
   //! Create reconstruction action for global straight tracks
   void CreateRecActionGlbS();

protected:
   static Bool_t fgItrTrackFlag; ////< ITR tracking flag
   static Bool_t fgMsdTrackFlag; ////< MSD tracking flag
   static Bool_t fSaveMcFlag;    ////< MC saving flag

   ClassDef(BaseReco, 1);        ////< Base class for reconstruction
};


#endif
