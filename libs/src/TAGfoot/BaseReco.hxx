
#ifndef _BaseReco_HXX_
#define _BaseReco_HXX_

#include "TNamed.h"
#include "TString.h"

#include "TAGaction.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

#include "TASTparTime.hxx"

#include "TASTparMap.hxx"
#include "TABMparMap.hxx"
#include "TAVTparMap.hxx"
#include "TAITparMap.hxx"
#include "TATWparMap.hxx"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TAGparGeo.hxx"
#include "TADIparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TACAparGeo.hxx"

#include "TATWparCal.hxx"
#include "TATWparMap.hxx"
#include "TATWparTime.hxx"

#include "TABMparCon.hxx"
#include "TAVTparConf.hxx"
#include "TAITparConf.hxx"
#include "TAMSDparConf.hxx"

#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAVTntuRaw.hxx"
#include "TAITntuRaw.hxx"
#include "TAMSDntuRaw.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuRaw.hxx"
#include "TAIRntuTrack.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TAGactionFile.hxx"

#include "TAVTactNtuClusterF.hxx"
#include "TAITactNtuClusterF.hxx"
#include "TAMSDactNtuCluster.hxx"
#include "TATWactNtuPoint.hxx"

#include "TABMactNtuTrack.hxx"
#include "TAVTactBaseNtuTrack.hxx"
#include "TAVTactNtuVertex.hxx"

#include "TAIRactNtuTrack.hxx"
#include "TAGactNtuGlbTrack.hxx"

class TAMCntuHit;
class TAMCntuEve;
class BaseReco : public TNamed // using TNamed for the in/out files
{
public:
   //! default constructor
   BaseReco(TString expName, TString fileNameIn, TString fileNameout);
   
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
   virtual void LoopEvent(Int_t /*nEvents*/) { return; }
   
   //! Begin loop
   virtual void BeforeEventLoop();

   //! End loop
   virtual void AfterEventLoop();
   
   //! Open File In
   virtual void OpenFileIn()  { return; }
   
   //! Close File in
   virtual void CloseFileIn() { return; }
   
   //! Set Run number
   virtual void SetRunNumber() { return; }

   //! Open File Out
   virtual void OpenFileOut();
   
   //! Close File Out
   virtual void CloseFileOut();
   
   //! Create branch in tree
   virtual void SetTreeBranches();
   
   //! Set experiment name
   virtual void SetExpName(const Char_t* name) { fExpName = name; }
   
   // Enable global
   void EnableTree()      { fFlagTree = true;    }
   void DisableTree()     { fFlagTree = false;   }
   
   void EnableSaveHits()  { fFlagHits = true;    }
   void DisableSaveHits() { fFlagHits = false;   }

   
   void EnableHisto()     { fFlagHisto = true;   }
   void DisableHisto()    { fFlagHisto = false;  }
   
   void EnableTracking()  { fFlagTrack = true;  }
   void DisableTracking() { fFlagTrack = false; }

   //! Set Tracking algorithm
   void SetTrackingAlgo(char c);
  
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
   TASTntuRaw*          GetNtuHitSt()       const { return (TASTntuRaw*) fpNtuRawSt->Object();       }
   TABMntuRaw*          GetNtuRawBm()       const { return (TABMntuRaw*)fpNtuRawBm->Object();        }
   TABMntuTrack*        GetNtuTrackBm()     const { return (TABMntuTrack*)fpNtuTrackBm->Object();    }
   
   TAVTntuCluster*      GetNtuClusterVtx()  const { return (TAVTntuCluster*)fpNtuClusVtx->Object();  }
   TAVTntuTrack*        GetNtuTrackVtx()    const { return (TAVTntuTrack*)fpNtuTrackVtx->Object();   }
   TAVTntuVertex*       GetNtuVertexVtx()   const { return (TAVTntuVertex*)fpNtuVtx->Object();       }
   TAGdataDsc*          GetDscVertexVtx()   const { return fpNtuVtx;                                 }
   
   TAITntuCluster*      GetNtuClusterIt()   const { return (TAITntuCluster*)fpNtuClusIt->Object();   }
   TAMSDntuCluster*     GetNtuClusterMsd()  const { return (TAMSDntuCluster*)fpNtuClusMsd->Object(); }
   
   TATWntuRaw*          GetNtuHitTw()       const { return (TATWntuRaw*) fpNtuRawTw->Object();       }
   TATWntuPoint*        GetNtuPointTw()     const { return (TATWntuPoint*) fpNtuRecTw->Object();     }
   
   TACAntuRaw*          GetNtuHitCa()       const { return (TACAntuRaw*) fpNtuRawCa->Object();       }

   TAIRntuTrack*        GetNtuTrackIr()    const { return (TAIRntuTrack*)fpNtuTrackIr->Object();     }

   //! MC container Getter (virtual)
   virtual TAMCntuEve*  GetNtuMcEve()       const { return 0x0; }
   virtual TAMCntuHit*  GetNtuMcSt()        const { return 0x0; }
   virtual TAMCntuHit*  GetNtuMcBm()        const { return 0x0; }
   virtual TAMCntuHit*  GetNtuMcVtx()       const { return 0x0; }
   virtual TAMCntuHit*  GetNtuMcIt()        const { return 0x0; }
   virtual TAMCntuHit*  GetNtuMcMsd()       const { return 0x0; }
   virtual TAMCntuHit*  GetNtuMcTw()        const { return 0x0; }
   virtual TAMCntuHit*  GetNtuMcCa()        const { return 0x0; }
   virtual TTree*       GetTree()                 { return 0x0; }

protected:
   TString               fExpName;
   TAGroot*              fTAGroot;             // pointer to TAGroot
   TAGgeoTrafo*          fpFootGeo;           // trafo prointer

   TAGparaDsc*           fpParTimeSt;
   TAGparaDsc*           fpParTimeTw;

   TAGparaDsc*           fpParMapSt;
   TAGparaDsc*           fpParMapBm;
   TAGparaDsc*           fpParMapVtx;
   TAGparaDsc*           fpParMapIt;
   TAGparaDsc*           fpParMapTw;

   TAGparaDsc*           fpParGeoSt;
   TAGparaDsc*           fpParGeoG;
   TAGparaDsc*           fpParGeoDi;
   TAGparaDsc*           fpParGeoBm;
   
   TAGparaDsc*           fpParGeoIt;
   TAGparaDsc*           fpParGeoVtx;
   TAGparaDsc*           fpParGeoMsd;
   TAGparaDsc*           fpParGeoTw;
   TAGparaDsc*           fpParGeoCa;
   
   TAGparaDsc*           fpParCalTw;
   
   TAGparaDsc*           fpParConfBm;
   TAGparaDsc*           fpParConfIt;
   TAGparaDsc*           fpParConfVtx;
   TAGparaDsc*           fpParConfMsd;
   
   TAGdataDsc*           fpDatRawSt;    // input data dsc
   TAGdataDsc*           fpNtuRawSt;    // input data dsc
   TAGdataDsc*           fpDatRawBm;    // input data dsc
   TAGdataDsc*           fpNtuRawBm;    // input data dsc
   TAGdataDsc*           fpNtuTrackBm;  // input track data dsc
   
   TAGdataDsc*           fpDatRawVtx;    // input data dsc
   TAGdataDsc*           fpNtuRawVtx;	  // input ntu data dsc
   TAGdataDsc*           fpNtuClusVtx;	  // input cluster data dsc
   TAGdataDsc*           fpNtuTrackVtx;  // input track data dsc
   TAGdataDsc*           fpNtuVtx;		  // input Vtx data dsc
   
   TAGdataDsc*           fpDatRawIt;     // input data dsc
   TAGdataDsc*           fpNtuRawIt;	  // input ntu data dsc
   TAGdataDsc*           fpNtuClusIt;	  // input cluster data dsc
   
   TAGdataDsc*           fpDatRawMsd;    // input data dsc
   TAGdataDsc*           fpNtuRawMsd;	  // input ntu data dsc
   TAGdataDsc*           fpNtuClusMsd;	  // input cluster data dsc
   
  //   TAGdataDsc*           fpWdRawTw;     // input data dsc
   TAGdataDsc*           fpDatRawTw;     // input data dsc
   TAGdataDsc*           fpNtuRawTw;     // input data dsc
   TAGdataDsc*           fpNtuRecTw;     // input data dsc
   TAGdataDsc*           fpNtuRawCa;     // input data dsc
   TAGdataDsc*           fpNtuGlbTrack;     // input data dsc
   TAGdataDsc*           fpNtuTrackIr;     // input data dsc

   TAGactionFile*        fActEvtReader;
   TAGactTreeWriter*     fActEvtWriter;  // write histo and tree

   TABMactNtuTrack*      fActTrackBm;    // action for tracks
   
   TAVTactNtuClusterF*   fActClusVtx;    // action for clusters
   TAVTactBaseNtuTrack*  fActTrackVtx;   // action for tracks
   TAVTactBaseNtuVertex* fActVtx;        // action for vertex
   
   TAITactNtuClusterF*   fActClusIt;     // action for clusters
   
   TAMSDactNtuCluster*   fActClusMsd;    // action for clusters
   
   // TATWactNtuRaw*        fActNtuRawTw;  // action for ntu data
   TATWactNtuPoint*      fActPointTw;    // action for clusters
   
   TAGactNtuGlbTrack*    fActGlbTrack;    // Global tracking action
   TAIRactNtuTrack*      fActTrackIr;     // action for IR tracks

   Bool_t                fFlagOut;       // flag for output file
   Bool_t                fFlagTree;      // flag to save in tree
   Bool_t                fFlagHits;      // flag to save hits in tree
   Bool_t                fFlagHisto;     // flag for histo generatiom
   Bool_t                fFlagTrack;     // flag for tracking
   TString               fgTrackingAlgo; // tracking algorithm ("std" with BM, "Full" combinatory)
   Bool_t                fFlagMC;        // MC flag

 protected:
   void CreateRecActionBm();
   void CreateRecActionVtx();
   void CreateRecActionIt();
   void CreateRecActionMsd();
   void CreateRecActionTw();
   void CreateRecActionGlb();
   void CreateRecActionIr();

   ClassDef(BaseReco, 1); // Base class for event display
};


#endif
