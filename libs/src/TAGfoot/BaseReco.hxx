
#ifndef _BaseReco_HXX_
#define _BaseReco_HXX_

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
#include "TATWntuPoint.hxx"
#include "TACAntuHit.hxx"
#include "TACAntuCluster.hxx"
#include "TAIRntuTrack.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TAGactionFile.hxx"

#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuClusterMT.hxx"
#include "TAITactNtuClusterF.hxx"
#include "TAITactNtuClusterMT.hxx"
#include "TAMSDactNtuCluster.hxx"
#include "TAMSDactNtuPoint.hxx"
#include "TACAactNtuCluster.hxx"
#include "TATWactNtuPoint.hxx"
#include "TATWactCalibTW.hxx"

#include "TABMactNtuTrack.hxx"
#include "TAVTactBaseNtuTrack.hxx"
#include "TAITactNtuTrack.hxx"
#include "TAITactNtuTrackF.hxx"
#include "TAVTactNtuVertex.hxx"

#include "TAIRactNtuTrack.hxx"
#include "TAGactNtuGlbTrack.hxx"

#include "TAGactKFitter.hxx"
#include "UpdatePDG.hxx"

#include "GlobalTrackingStudies.hxx"
#include "TAGntuGlbTrackK.hxx"

class TAMCntuHit;
class TAMCntuTrack;
class TAMCntuRegion;
class TAMCntuEvent;

class BaseReco : public TNamed // using TNamed for the in/out files
{
public:
   //! default constructor
   BaseReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout);
   
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
   
   //! Begin loop
   virtual void BeforeEventLoop();

   //! End loop
   virtual void AfterEventLoop();
   
   //! Open File In
   virtual void OpenFileIn()  { return; }
  
   // ! Global Checks
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
   
   // Set run number
   void SetRunNumber(Int_t run)                { fRunNumber = run; }

   // Enable global
   void EnableTree()      { fFlagTree = true;    }
   void DisableTree()     { fFlagTree = false;   }
   
   void EnableSaveHits()  { fFlagHits = true;    }
   void DisableSaveHits() { fFlagHits = false;   }

   void EnableHisto()     { fFlagHisto = true;   }
   void DisableHisto()    { fFlagHisto = false;  }
   
   void EnableTracking()  { fFlagTrack = true;   }
   void DisableTracking() { fFlagTrack = false;  }
   
   void EnableTWcalibPerBar()  { fFlagTWbarCalib = true;  }
   void DisableTWcalibPerBar() { fFlagTWbarCalib = false; }

   void EnableZfromMCtrue()    { fFlagZtrueMC = true;     }
   void DisableZfromMCtrue()   { fFlagZtrueMC = false;    }

   void EnableZrecWithPUoff()  { fFlagZrecPUoff = true;   }
   void DisableZrecWithPUoff() { fFlagZrecPUoff = false;  }
  
   void EnableTWZmatch()       { fFlagZmatch_TW = true;   }
   void DisableTWZmatch()      { fFlagZmatch_TW = false;  }

   void DisableM28ClusMT()     { fM28ClusMtFlag = false;  }
   void EnableM28lusMT()       { fM28ClusMtFlag = true;   }
   Bool_t IsM28ClusMT()        { return fM28ClusMtFlag;   }

   // Flag for MC data
   Bool_t IsMcData()           { return fFlagMC;          }
   
   //! Set Tracking algorithm
   void SetVtxTrackingAlgo(char c);
  
   // Campaign checks
   void CampaignChecks();

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
   TAITntuTrack*        GetNtuTrackIt()     const { return (TAITntuTrack*)fpNtuTrackIt->Object();   }

   TAMSDntuCluster*     GetNtuClusterMsd()  const { return (TAMSDntuCluster*)fpNtuClusMsd->Object(); }
   
   TATWntuHit*          GetNtuHitTw()       const { return (TATWntuHit*) fpNtuHitTw->Object();       }
   TATWntuPoint*        GetNtuPointTw()     const { return (TATWntuPoint*) fpNtuRecTw->Object();     }
   
   TACAntuHit*          GetNtuHitCa()       const { return (TACAntuHit*) fpNtuHitCa->Object();       }
   TACAntuCluster*      GetNtuClusterCa()   const { return (TACAntuCluster*) fpNtuClusCa->Object();  }

   TAGntuGlbTrack*      GetNtuGlbTrack()    const { return (TAGntuGlbTrack*)fpNtuGlbTrack->Object(); }
   TAIRntuTrack*        GetNtuTrackIr()     const { return (TAIRntuTrack*)fpNtuTrackIr->Object();    }
   TADIgeoField*        GetFootField()      const { return fField;                                   }
   
   //! MC container Getter (virtual)
   virtual TAMCntuRegion* GetNtuMcReg()     const { return 0x0; }
   virtual TAMCntuEvent*  GetNtuMcEvt()     const { return 0x0; }
   virtual TAMCntuTrack*  GetNtuMcTrk()     const { return 0x0; }
   virtual TAMCntuHit*    GetNtuMcSt()      const { return 0x0; }
   virtual TAMCntuHit*    GetNtuMcBm()      const { return 0x0; }
   virtual TAMCntuHit*    GetNtuMcVtx()     const { return 0x0; }
   virtual TAMCntuHit*    GetNtuMcIt()      const { return 0x0; }
   virtual TAMCntuHit*    GetNtuMcMsd()     const { return 0x0; }
   virtual TAMCntuHit*    GetNtuMcTw()      const { return 0x0; }
   virtual TAMCntuHit*    GetNtuMcCa()      const { return 0x0; }
   virtual TTree*         GetTree()               { return 0x0; }

public:
   //! Disable/Enable ITR tracking
   static void DisableItrTracking() { fgItrTrackFlag = false; }
   static void EnableItrTracking()  { fgItrTrackFlag = true;  }
   static Bool_t IsItrTracking()    { return fgItrTrackFlag;  }
   
protected:
   TString               fExpName;
   TAGcampaignManager*   fCampManager;
   Int_t                 fRunNumber;
   TAGroot*              fTAGroot;             // pointer to TAGroot
   TAGgeoTrafo*          fpFootGeo;           // trafo prointer

   TAGparaDsc*           fpParTimeWD;

   TAGparaDsc*           fpParMapWD;
   TAGparaDsc*           fpParMapSt;
   TAGparaDsc*           fpParMapBm;
   TAGparaDsc*           fpParMapVtx;
   TAGparaDsc*           fpParMapIt;
   TAGparaDsc*           fpParMapTw;
   TAGparaDsc*           fpParMapCa;

   TAGparaDsc*           fpParGeoSt;
   TAGparaDsc*           fpParGeoG;
   TAGparaDsc*           fpParGeoDi;
   TAGparaDsc*           fpParGeoBm;
   TAGparaDsc*           fpParGeoVtx;
   TAGparaDsc*           fpParGeoIt;
   TAGparaDsc*           fpParGeoMsd;
   TAGparaDsc*           fpParGeoTw;
   TAGparaDsc*           fpParGeoCa;
   
   TAGparaDsc*           fpParCalBm;
   TAGparaDsc*           fpParCalTw;
   TAGparaDsc*           fpParCalCa;

   TAGparaDsc*           fpParConfBm;
   TAGparaDsc*           fpParConfVtx;
   TAGparaDsc*           fpParConfIt;
   TAGparaDsc*           fpParConfMsd;
   
   TAGdataDsc*           fpDatRawSt;    // input data dsc
   TAGdataDsc*           fpNtuHitSt;    // input data dsc
   TAGdataDsc*           fpDatRawBm;    // input data dsc
   TAGdataDsc*           fpNtuHitBm;    // input data dsc
   TAGdataDsc*           fpNtuHitVtx;     // input ntu data dsc
   TAGdataDsc*           fpNtuHitIt;     // input ntu data dsc
   TAGdataDsc*           fpDatRawMsd;    // input data dsc
   TAGdataDsc*           fpNtuHitMsd;     // input ntu data dsc
   TAGdataDsc*           fpDatRawTw;     // input data dsc
   TAGdataDsc*           fpNtuHitTw;     // input data dsc
   TAGdataDsc*           fpDatRawCa;     // input data dsc
   TAGdataDsc*           fpNtuHitCa;     // input data dsc

   TAGdataDsc*           fpNtuClusVtx;	  // input cluster data dsc
   TAGdataDsc*           fpNtuClusIt;	  // input cluster data dsc
   TAGdataDsc*           fpNtuClusMsd;     // input cluster data dsc
   TAGdataDsc*           fpNtuRecMsd;
   TAGdataDsc*           fpNtuRecTw;     // input data dsc
   TAGdataDsc*           fpNtuClusCa;     // input cluster data dsc
  
   TAGdataDsc*           fpNtuMcTrk;
   TAGdataDsc*           fpNtuMcSt;    // input data dsc
   TAGdataDsc*           fpNtuMcBm;    // input data dsc
   TAGdataDsc*           fpNtuMcVt;    // input data dsc
   TAGdataDsc*           fpNtuMcIt;    // input data dsc
   TAGdataDsc*           fpNtuMcMsd;    // input data dsc
   TAGdataDsc*           fpNtuMcTw;    // input data dsc
   TAGdataDsc*           fpNtuMcCa;    // input data dsc
  
   TADIgeoField*         fField;       // magnetic field

   TAGdataDsc*           fpNtuTrackBm;  // input track data dsc
   TAGdataDsc*           fpNtuTrackVtx;  // input track data dsc
   TAGdataDsc*           fpNtuTrackIt;  // input track data dsc
   TAGdataDsc*           fpNtuVtx;        // input Vtx data dsc

   TAGdataDsc*           fpNtuGlbTrack;     // input data dsc
   TAGdataDsc*           fpNtuGlbTrackK;      // input data dsc
   TAGdataDsc*           fpNtuTrackIr;     // input data dsc

   TAGactionFile*        fActEvtReader;
   TAGactTreeWriter*     fActEvtWriter;  // write histo and tree

   TABMactNtuTrack*      fActTrackBm;    // action for tracks
   
   TAGaction*            fActClusVtx;    // action for clusters
   TAVTactBaseNtuTrack*  fActTrackVtx;   // action for tracks
   TAVTactBaseNtuVertex* fActVtx;        // action for vertex
   
   TAGaction*            fActClusIt;     // action for clusters
   TAITactBaseNtuTrack*  fActTrackIt;   // action for tracks

   TAMSDactNtuCluster*   fActClusMsd;    // action for clusters
   TAMSDactNtuPoint*     fActPointMsd;   // action for point in MSD

   TATWactNtuPoint*      fActPointTw;    // action for clusters
   TATWactCalibTW*       fActCalibTw;

   TACAactNtuCluster*    fActClusCa;    // action for clusters

   TAGactNtuGlbTrack*    fActGlbTrack;    // Global tracking action
   TAIRactNtuTrack*      fActTrackIr;     // action for IR tracks
  
   GlobalTrackingStudies* fActGlbTrackStudies;    // Global tracking studies with GenFit
   TAGactKFitter*         fActGlbkFitter;    // Global tracking kalman Fitter

   
   Bool_t                fFlagOut;       // flag for output file
   Bool_t                fFlagTree;      // flag to save in tree
   Bool_t                fFlagHits;      // flag to save hits in tree
   Bool_t                fFlagHisto;     // flag for histo generatiom
   Bool_t                fFlagTrack;     // flag for tracking
   Bool_t                fFlagTWbarCalib; // flag for TW calibration per Bar
   TString               fgTrackingAlgo; // tracking algorithm ("std" with BM, "Full" combinatory)
   Bool_t                fFlagZtrueMC;      // Z true MC flag
   Bool_t                fFlagZrecPUoff;    // Z rec TW PU off flag
   Bool_t                fFlagZmatch_TW;    // TW Z match

   Bool_t                fFlagMC;        // MC flag
   Bool_t                fM28ClusMtFlag; // flag for multi-threading clustering

 protected:
   void CreateRecActionBm();
   void CreateRecActionVtx();
   void CreateRecActionIt();
   void CreateRecActionMsd();
   void CreateRecActionTw();
   void CreateRecActionCa();
   void CreateRecActionGlb() ;
   void CreateRecActionGlbGF() ;
   void CreateRecActionIr();

protected:
   static Bool_t fgItrTrackFlag;

   ClassDef(BaseReco, 1); // Base class for event display
};


#endif
