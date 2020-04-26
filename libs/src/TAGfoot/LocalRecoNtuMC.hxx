
#ifndef _LocalRecoNtuMC_HXX_
#define _LocalRecoNtuMC_HXX_

#include "BaseReco.hxx"

#include "TFile.h"

#include "TAGdataDsc.hxx"
#include "TAGactTreeReader.hxx"
#include "TASTactNtuHitMC.hxx"
#include "TABMactNtuHitMC.hxx"
#include "TAVTactNtuHitMC.hxx"
#include "TAITactNtuHitMC.hxx"
#include "TAMSDactNtuHitMC.hxx"
#include "TATWactNtuHitMC.hxx"
#include "TACAactNtuHitMC.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"
#include "TAMCactNtuStc.hxx"
#include "TAMCactNtuBm.hxx"
#include "TAMCactNtuVtx.hxx"
#include "TAMCactNtuItr.hxx"
#include "TAMCactNtuMsd.hxx"
#include "TAMCactNtuTof.hxx"
#include "TAMCactNtuCal.hxx"
#include "TAMCactNtuEve.hxx"

#include "TATWntuRaw.hxx"
#include "TATWntuPoint.hxx"

#include "Evento.hxx"

class TTree;

class LocalRecoNtuMC : public BaseReco
{
public:
   //! default constructor
   LocalRecoNtuMC(TString expName = "", TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~LocalRecoNtuMC();
   
   //! Add required items
   virtual void AddRawRequiredItem();

   //! Create raw data action
   virtual void CreateRawAction();
   
   //! Set raw histogram directory
   virtual void SetRawHistogramDir();
   
   //! Create branch in tree
   virtual void SetTreeBranches();
   
   //! Open File
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();

   //! Loop events
   virtual void LoopEvent(Int_t nEvents);
   
   //! MC container Getter
   TAMCntuEve*  GetNtuMcEve() const { return (TAMCntuEve*)fpNtuMcEve->Object();}
   TAMCntuHit*  GetNtuMcSt()  const { return (TAMCntuHit*)fpNtuMcSt->Object(); }
   TAMCntuHit*  GetNtuMcBm()  const { return (TAMCntuHit*)fpNtuMcBm->Object(); }
   TAMCntuHit*  GetNtuMcVtx() const { return (TAMCntuHit*)fpNtuMcVt->Object(); }
   TAMCntuHit*  GetNtuMcIt()  const { return (TAMCntuHit*)fpNtuMcIt->Object(); }
   TAMCntuHit*  GetNtuMcMsd() const { return (TAMCntuHit*)fpNtuMcMsd->Object();}
   TAMCntuHit*  GetNtuMcTw()  const { return (TAMCntuHit*)fpNtuMcTw->Object(); }
   TAMCntuHit*  GetNtuMcCa()  const { return (TAMCntuHit*)fpNtuMcCa->Object(); }

protected:
   EVENT_STRUCT*         fEvtStruct;
   
   TASTactNtuHitMC*      fActNtuRawSt;  // action for ntu data
   TABMactNtuHitMC*      fActNtuRawBm;  // action for ntu data
   TAVTactNtuHitMC*      fActNtuRawVtx;  // action for ntu data
   TAITactNtuHitMC*      fActNtuRawIt;  // action for ntu data
   TAMSDactNtuHitMC*     fActNtuRawMsd;  // action for ntu data
   TATWactNtuHitMC*      fActNtuRawTw;  // action for ntu data
   TACAactNtuHitMC*      fActNtuRawCa;  // action for ntu data
   
   TAGdataDsc*           fpNtuMcEve;    // input data dsc
   TAGdataDsc*           fpNtuMcSt;    // input data dsc
   TAGdataDsc*           fpNtuMcBm;    // input data dsc
   TAGdataDsc*           fpNtuMcVt;    // input data dsc
   TAGdataDsc*           fpNtuMcIt;    // input data dsc
   TAGdataDsc*           fpNtuMcMsd;    // input data dsc
   TAGdataDsc*           fpNtuMcTw;    // input data dsc
   TAGdataDsc*           fpNtuMcCa;    // input data dsc
   
   TAGactTreeReader*     fActEvtReader; // file for MC
   
   ClassDef(LocalRecoNtuMC, 1); // Base class for event display
};


#endif
