
#ifndef _GlobalToeReco_HXX_
#define _GlobalToeReco_HXX_

#include "TString.h"
#include "TAGobject.hxx"
#include "BaseReco.hxx"


class GlobalToeReco : public TAGobject
{
public:
   //! default constructor
   GlobalToeReco(TString expName, TString fileNameIn = "", TString fileNameout = "", Bool_t isMC = false);
   
   virtual ~GlobalToeReco();

   void EnableTree()                 { fReco->EnableTree();         }
   void DisableTree()                { fReco->DisableTree();        }
   
   void EnableSaveHits()             { fReco->EnableSaveHits();     }
   void DisableSaveHits()            { fReco->DisableSaveHits();    }
   
   void EnableHisto()                { fReco->EnableHisto();        }
   void DisableHisto()               { fReco->DisableHisto();       }
   
   void EnableTracking()             { fReco->EnableTracking();     }
   void DisableTracking()            { fReco->DisableTracking();    }
   
   //! Read parameters
   void ReadParFiles()               { fReco->ReadParFiles();       }
   
   //! Create raw action
   virtual void CreateRecAction()    { fReco->CreateRecAction();    }
   
   //! Create rec action
   virtual void CreateRawAction()    { fReco->CreateRawAction();    }
   
   //! Add required items
   virtual void AddRawRequiredItem() { fReco->AddRawRequiredItem(); }
   
   //! Add required items
   virtual void AddRecRequiredItem() { fReco->AddRecRequiredItem(); }
   
   //! Set raw histogram directory
   virtual void SetRawHistogramDir() { fReco->SetRawHistogramDir(); }
   
   //! Set rec histogram directory
   virtual void SetRecHistogramDir() { fReco->SetRecHistogramDir(); }
   
   //! Loop events
   void LoopEvent(Int_t nTotEv)      { fReco->LoopEvent(nTotEv);    }

   //! Begin loop
   void BeforeEventLoop()            { fReco->BeforeEventLoop();    }

   //! End loop
   void AfterEventLoop()             { fReco->AfterEventLoop();     }

   //! Open File In
   virtual void OpenFileIn()         { fReco->OpenFileIn();         }
   
   //! Close File in
   virtual void CloseFileIn()        { fReco->CloseFileIn();        }
   
   //! Set Run number
   virtual void SetRunNumber()       { fReco->SetRunNumber();       }
   
   //! Open File Out
   virtual void OpenFileOut()        { fReco->OpenFileOut();        }
   
   //! Close File Out
   virtual void CloseFileOut()       { fReco->CloseFileOut();       }
   
   //! Create branch in tree
   virtual void SetTreeBranches()    { fReco->SetTreeBranches();    }
   
   //! Set Tracking algorithm
   void SetTrackingAlgo(char c)      { fReco->SetTrackingAlgo(c);   }

   //! Set name
   void SetName(const Char_t* name)  { fReco->SetName(name);        }
   
   //! Set experiment name
   virtual void SetExpName(const Char_t* name) { fReco->SetExpName(name); }

   //! Par geo getters
   TAGgeoTrafo*         GetGeoTrafo()       const { return fReco->GetGeoTrafo();      }
   TADIparGeo*          GetParGeoDi()       const { return fReco->GetParGeoDi();      }
   TASTparGeo*          GetParGeoSt()       const { return fReco->GetParGeoSt();      }
   TAGparGeo*           GetParGeoG()        const { return fReco->GetParGeoG();       }
   TABMparGeo*          GetParGeoBm()       const { return fReco->GetParGeoBm();      }
   TAVTparGeo*          GetParGeoVtx()      const { return fReco->GetParGeoVtx();     }
   TAITparGeo*          GetParGeoIt()       const { return fReco->GetParGeoIt();      }
   TAMSDparGeo*         GetParGeoMsd()      const { return fReco->GetParGeoMsd();     }
   TATWparGeo*          GetParGeoTw()       const { return fReco->GetParGeoTw();      }
   TACAparGeo*          GetParGeoCa()       const { return fReco->GetParGeoCa();      }
   
   //! Containers getters
   TASTntuRaw*          GetNtuHitSt()       const { return fReco->GetNtuHitSt();      }
   TABMntuRaw*          GetNtuRawBm()       const { return fReco->GetNtuRawBm();      }
   TABMntuTrack*        GetNtuTrackBm()     const { return fReco->GetNtuTrackBm();    }
   
   TAVTntuCluster*      GetNtuClusterVtx()  const { return fReco->GetNtuClusterVtx(); }
   TAVTntuTrack*        GetNtuTrackVtx()    const { return fReco->GetNtuTrackVtx();   }
   TAVTntuVertex*       GetNtuVertexVtx()   const { return fReco->GetNtuVertexVtx();  }
   TAGdataDsc*          GetDscVertexVtx()   const { return fReco->GetDscVertexVtx();  }
   
   TAITntuCluster*      GetNtuClusterIt()   const { return fReco->GetNtuClusterIt();  }
   TAMSDntuCluster*     GetNtuClusterMsd()  const { return fReco->GetNtuClusterMsd(); }
   
   TATWntuRaw*          GetNtuHitTw()       const { return fReco->GetNtuHitTw();      }
   TATWntuPoint*        GetNtuPointTw()     const { return fReco->GetNtuPointTw();    }
   
   TACAntuRaw*          GetNtuHitCa()       const { return fReco->GetNtuHitCa();      }
   
   TAGactNtuGlbTrack*   GetNtuGlbTrack()    const { return fReco->GetNtuGlbTrack();   }
   TAIRntuTrack*        GetNtuTrackIr()     const { return fReco->GetNtuTrackIr();    }
   
   
   //! MC container Getter (virtual)
   virtual TAMCntuEve*  GetNtuMcEve()       const { return fReco->GetNtuMcEve();      }
   virtual TAMCntuHit*  GetNtuMcSt()        const { return fReco->GetNtuMcSt();       }
   virtual TAMCntuHit*  GetNtuMcBm()        const { return fReco->GetNtuMcBm();       }
   virtual TAMCntuHit*  GetNtuMcVtx()       const { return fReco->GetNtuMcVtx();      }
   virtual TAMCntuHit*  GetNtuMcIt()        const { return fReco->GetNtuMcIt();       }
   virtual TAMCntuHit*  GetNtuMcMsd()       const { return fReco->GetNtuMcMsd();      }
   virtual TAMCntuHit*  GetNtuMcTw()        const { return fReco->GetNtuMcTw();       }
   virtual TAMCntuHit*  GetNtuMcCa()        const { return fReco->GetNtuMcCa();       }
   virtual TTree*       GetTree()                 { return fReco->GetTree();          }
   
private:
   BaseReco*       fReco;    // local reco

   ClassDef(GlobalToeReco, 0); 
};


#endif
