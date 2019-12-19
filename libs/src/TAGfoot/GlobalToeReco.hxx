
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

   void EnableTree()      { fReco->EnableTree();      }
   void DisableTree()     { fReco->DisableTree();     }
   
   void EnableSaveHits()  { fReco->EnableSaveHits();  }
   void DisableSaveHits() { fReco->DisableSaveHits(); }
   
   
   void EnableHisto()     { fReco->EnableHisto();     }
   void DisableHisto()    { fReco->DisableHisto();    }
   
   void EnableTracking()  { fReco->EnableTracking();  }
   void DisableTracking() { fReco->DisableTracking(); }
   
   
   void BeforeEventLoop()       { fReco->BeforeEventLoop(); }
   void LoopEvent(Int_t nTotEv) { fReco->LoopEvent(nTotEv); }
   void AfterEventLoop()        { fReco->AfterEventLoop();  }
   
private:
   BaseReco*       fReco;    // local reco

   ClassDef(GlobalToeReco, 0); 
};


#endif
