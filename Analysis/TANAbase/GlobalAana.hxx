
#ifndef _GlobalAana_HXX_
#define _GlobalAana_HXX_

/*!
 \file GlobalAana.hxx
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
#include "TAGrunManager.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAWDparTime.hxx"
#include "TAWDparMap.hxx"

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

#include "TADIgeoField.hxx"

#include "TABMparCal.hxx"
#include "TAVTparCal.hxx"
#include "TAMSDparCal.hxx"
#include "TATWparCal.hxx"
#include "TACAparCal.hxx"

#include "TASTparConf.hxx"
#include "TABMparConf.hxx"
#include "TAVTparConf.hxx"
#include "TAITparConf.hxx"
#include "TAMSDparConf.hxx"
#include "TATWparConf.hxx"
#include "TACAparConf.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"

#include "TAMSDntuTrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAMCntuEvent.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCntuHit.hxx"
#include "TASTntuHit.hxx"
#include "TAITntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDntuTrack.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuCluster.hxx"
#include "TAIRntuTrack.hxx"
#include "TABMntuTrack.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TAGactionFile.hxx"

#include "TAGactFlatTreeWriter.hxx"
#include "TAGactDscTreeWriter.hxx"

#include "TANAactBaseNtu.hxx"

class TAMCntuHit;
class TAMCntuPart;
class TAMCntuRegion;
class TAMCntuEvent;

class GlobalAana : public TNamed // using TNamed for the in/out files
{
public:
   // default constructor
   GlobalAana(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC = false);
   
   // default destructor
   virtual ~GlobalAana();
   
   // Read parameters
   void ReadParFiles();

   // Create raw action
   virtual void CreateAnaAction();
   
      
   // Add required items
   virtual void AddRequiredItem();
   
   // Set histogram directory
   virtual void SetHistogramDir();
   
   // Loop events
   virtual void LoopEvent(Int_t nEvents);
      
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
     
   // Create L0 branch in tree
   virtual void SetL0TreeBranches();

   //! Set experiment name
   virtual void SetExpName(const Char_t* name) { fExpName = name;  }
   
   //! Set run number
   void SetRunNumber(Int_t run)                { fRunNumber = run; }

   //! Enable Histo
   void EnableHisto()          { fFlagHisto = true;       }
   //! Disable Histo
   void DisableHisto()         { fFlagHisto = false;      }
   
   //! Flag for MC data
   Bool_t IsMcData()           { return fFlagMC;          }
     
   // Par geo getters
   //! Get parameters geo transformations
   TAGgeoTrafo*         GetGeoTrafo()       const { return fpFootGeo;                                }
   //! Get dipole geometry parameters
   TADIparGeo*          GetParGeoDi()       const { return (TADIparGeo*)fpParGeoDi->Object();        }
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

   //! Get TW points containers
   TATWntuPoint*        GetNtuPointTw()     const { return (TATWntuPoint*) fpNtuRecTw->Object();     }
   
   //! Get CAL clusters containers
   TACAntuCluster*      GetNtuClusterCa()   const { return (TACAntuCluster*) fpNtuClusCa->Object();  }

   //! Get global tracks containers
   TAGntuGlbTrack*      GetNtuGlbTrack()    const { return (TAGntuGlbTrack*)fpNtuGlbTrack->Object(); }
   //! Get field
   TADIgeoField*        GetFootField()      const { return fField;                                   }
   
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
  
   //! Get run number
   Int_t               GetRunNumber()       const { return fRunNumber;                               }
   
   
protected:

   TString               fExpName;        ///< Experiment name
   TAGcampaignManager*   fCampManager;    ///< Campaign manager
   TAGrunManager*        fRunManager;     ///< Run manager
   Int_t                 fRunNumber;      ///< Run number
   TAGroot*              fTAGroot;        ///< pointer to TAGroot
   TAGgeoTrafo*          fpFootGeo;       ///< trafo prointer

   TAGparaDsc*           fpParMapIt;      ///< ITR mapping parameter

   TAGparaDsc*           fpParGeoSt;      ///< STC geometry parameter
   TAGparaDsc*           fpParGeoG;       ///< Beam/target geometry parameter
   TAGparaDsc*           fpParGeoDi;      ///< Dipole geometry parameter
   TAGparaDsc*           fpParGeoBm;      ///< BM geometry parameter
   TAGparaDsc*           fpParGeoVtx;     ///< VTX geometry parameter
   TAGparaDsc*           fpParGeoIt;      ///< ITR geometry parameter
   TAGparaDsc*           fpParGeoMsd;     ///< MSD geometry parameter
   TAGparaDsc*           fpParGeoTw;      ///< TW geometry parameter
   TAGparaDsc*           fpParGeoCa;      ///< CAL geometry parameter
   
   TAGparaDsc*           fpParConfSt;     ///< ST configuration parameter
   TAGparaDsc*           fpParConfBm;     ///< BM configuration parameter
   TAGparaDsc*           fpParConfVtx;    ///< VTX configuration parameter
   TAGparaDsc*           fpParConfIt;     ///< ITR configuration parameter
   TAGparaDsc*           fpParConfMsd;    ///< MSD configuration parameter
   TAGparaDsc*           fpParConfTw;     ///< TW configuration parameter
   TAGparaDsc*           fpParConfCa;     ///< CA configuration parameter

   TAGdataDsc*           fpNtuHitSt;     ///< Hit input dsc for STC

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
   TANAactBaseNtu*       fActGlbAna;     ///< analysis action

   Bool_t                fFlagOut;          ///< flag for output file
   Bool_t                fFlagHisto;        ///< flag for histo generatiom

   Bool_t                fFlagMC;           ///< MC flag
   Int_t                 fSkipEventsN;      ///< number of events to skip
   
   ClassDef(GlobalAana, 1);        ///< Base class for reconstruction
};


#endif
