
#include "BaseReco.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"
#include "GlobalReco.hxx"

#include "TAGntuEvent.hxx"

ClassImp(GlobalReco)

//__________________________________________________________
GlobalReco::GlobalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
: BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
  TAGrecoManager::GetPar()->EnableLocalReco();
  fFlagMC = TAGrecoManager::GetPar()->IsMC();	
  fgStdAloneFlag = false;
}

//__________________________________________________________
GlobalReco::~GlobalReco()
{
}

//__________________________________________________________
void GlobalReco::OpenFileIn()
{
  if(fFlagMC)	fActEvtReader->Open(GetName());
  else			fActEvtReaderDAQ->Open(GetName());
}

//__________________________________________________________
void GlobalReco::CloseFileIn()
{
  if(fFlagMC)	fActEvtReader->Close();
  else 			fActEvtReaderDAQ->Close();
}

//__________________________________________________________
void GlobalReco::CreateRawAction(){

	if(fFlagMC)	 CreateRawAction_MC();
	else 		 CreateRawAction_Raw();

}

//__________________________________________________________
void GlobalReco::CreateRawAction_Raw() {

	 if (!fgStdAloneFlag) {
      fpDaqEvent = new TAGdataDsc("daqEvt", new TAGdaqEvent());
      fActEvtReaderDAQ = new TAGactDaqReader("daqActReader", fpDaqEvent);

     //  fpNtuEvt = new TAGdataDsc("evtNtu", new TAGntuEvent());
     //  fActNtuEvt = new TAGactNtuEvent("evtActNtu", fpNtuEvt, fpDaqEvent);
     // if (fFlagHisto)
     //   fActNtuEvt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || (TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag) || TAGrecoManager::GetPar()->IncludeCA()) {
      fpDatRawSt   = new TAGdataDsc("stDat", new TASTntuRaw());
      fpDatRawTw   = new TAGdataDsc("twdDat", new TATWntuRaw());
      fpDatRawCa   = new TAGdataDsc("caDat", new TACAntuRaw());

      fActWdRaw  = new TAGactWDreader("wdActRaw", fpDaqEvent, fpDatRawSt, fpDatRawTw, fpDatRawCa, fpParMapWD, fpParTimeWD);
      if (fFlagHisto)
         fActWdRaw->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeST() ||(TAGrecoManager::GetPar()->IncludeBM() && !fgStdAloneFlag)) {
      fpNtuHitSt   = new TAGdataDsc("stNtu", new TASTntuHit());
      fActNtuHitSt = new TASTactNtuHit("stActNtu", fpDatRawSt, fpNtuHitSt, fpParMapSt);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpDatRawBm = new TAGdataDsc("bmDat", new TABMntuRaw());
      fpNtuHitBm = new TAGdataDsc("bmNtu", new TABMntuHit());

      if (fgStdAloneFlag) {
         fActVmeReaderBm  = new TABMactVmeReader("bmActRaw", fpDatRawBm, fpParMapBm, fpParCalBm, fpParGeoBm);
         if (fFlagHisto)
            fActVmeReaderBm->CreateHistogram();
      } else {
         fActDatRawBm = new TABMactNtuRaw("bmActRaw", fpDatRawBm, fpDaqEvent, fpParMapBm, fpParCalBm, fpParGeoBm, fpNtuHitSt);
         if (fFlagHisto)
            fActDatRawBm->CreateHistogram();
      }
      fActNtuHitBm = new TABMactNtuHit("bmActNtu", fpNtuHitBm, fpDatRawBm, fpParGeoBm, fpParConfBm, fpParCalBm);
      if (fFlagHisto)
        fActNtuHitBm->CreateHistogram();

   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpNtuHitVtx   = new TAGdataDsc("vtRaw", new TAVTntuHit());

      if (fgStdAloneFlag) {
         fActVmeReaderVtx  = new TAVTactVmeReader("vtActNtu", fpNtuHitVtx, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
            fActVmeReaderVtx->CreateHistogram();

      } else {
         fActNtuHitVtx = new TAVTactNtuHit("vtActNtu", fpNtuHitVtx, fpDaqEvent, fpParGeoVtx, fpParConfVtx, fpParMapVtx);
         if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
      }
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpNtuHitIt   = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuHitIt = new TAITactNtuHit("itActNtu", fpNtuHitIt, fpDaqEvent, fpParGeoIt, fpParConfIt, fpParMapIt);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpDatRawMsd   = new TAGdataDsc("msdDat", new TAMSDntuRaw());
      fActDatRawMsd = new TAMSDactNtuRaw("msdActRaw", fpDatRawMsd, fpDaqEvent, fpParMapMsd, fpParCalMsd, fpParGeoMsd);
      if (fFlagHisto)
         fActDatRawMsd->CreateHistogram();

      fpNtuHitMsd   = new TAGdataDsc("msdRaw", new TAMSDntuHit());
      fActNtuHitMsd = new TAMSDactNtuHit("msdActNtu", fpDatRawMsd, fpNtuHitMsd, fpParGeoMsd, fpParCalMsd);
      if (fFlagHisto)
         fActNtuHitMsd->CreateHistogram();
   }

   if(TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuHitTw   = new TAGdataDsc("twRaw", new TATWntuHit());

      if(TAGrecoManager::GetPar()->CalibTW()) {
        fActCalibTw = new TATWactCalibTW("twActCalib", fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
        fActCalibTw->CreateHistogram();

      } else {
        fActNtuHitTw = new TATWactNtuHit("twActNtu", fpDatRawTw, fpNtuHitTw, fpNtuHitSt, fpParGeoTw, fpParMapTw, fpParCalTw, fpParGeoG);
        if (fFlagHisto)
          fActNtuHitTw->CreateHistogram();
      }
   }

   if(TAGrecoManager::GetPar()->IncludeCA()) {
     fpNtuHitCa   = new TAGdataDsc("caRaw", new TACAntuHit());
     fActNtuHitCa = new TACAactNtuHit("caActNtu", fpDatRawCa, fpNtuHitCa, fpParMapCa, fpParCalCa);
     if (fFlagHisto){
       fActNtuHitCa->CreateHistogram();
      }
    }

}





//__________________________________________________________
void GlobalReco::CreateRawAction_MC()
{

	if(!fFlagMC)	 return;
   fActEvtReader = new TAGactTreeReader("actEvtReader");

   if ( TAGrecoManager::GetPar()->IsRegionMc()) {
     fpNtuMcReg = new TAGdataDsc(TAMCntuRegion::GetDefParaName(), new TAMCntuRegion());
     if (TAGrecoManager::GetPar()->IsReadRootObj())
       fActEvtReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
     else
       fActNtuMcReg = new TAMCactNtuRegion("regActNtuMc", fpNtuMcReg, fEvtStruct);
   }
  
   fpNtuMcEvt    = new TAGdataDsc("evtMc", new TAMCntuEvent());
   if (TAGrecoManager::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcEvt,TAMCntuEvent::GetBranchName());
   else
     fActNtuMcEvt = new TAMCactNtuEvent("evtActNtuMc", fpNtuMcEvt, fEvtStruct);
  
   fpNtuMcTrk    = new TAGdataDsc("eveMc", new TAMCntuPart());
   if (TAGrecoManager::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcTrk,TAMCntuPart::GetBranchName());
   else
    fActNtuMcTrk = new TAMCactNtuPart("eveActNtuMc", fpNtuMcTrk, fEvtStruct);

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
      
      fpNtuHitSt = new TAGdataDsc("stRaw", new TASTntuHit());
      fActNtuHitSt = new TASTactNtuHitMC("stActNtu", fpNtuMcSt, fpNtuMcTrk, fpNtuHitSt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());

      fpNtuHitBm = new TAGdataDsc("bmRaw", new TABMntuHit());
      fActNtuHitBm = new TABMactNtuHitMC("bmActNtu", fpNtuMcBm, fpNtuMcTrk, fpNtuHitBm, fpParConfBm, fpParCalBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitBm->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
      
      fpNtuHitVtx = new TAGdataDsc("vtRaw", new TAVTntuHit());
      fActNtuHitVtx = new TAVTactNtuHitMC("vtActNtu", fpNtuMcVt, fpNtuMcTrk, fpNtuHitVtx, fpParGeoVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
      
      fpNtuHitIt = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuHitIt = new TAITactNtuHitMC("itActNtu", fpNtuMcIt, fpNtuMcTrk, fpNtuHitIt, fpParGeoIt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
      
      fpNtuHitMsd = new TAGdataDsc("msdRaw", new TAMSDntuHit());
      fActNtuHitMsd = new TAMSDactNtuHitMC("msdActNtu", fpNtuMcMsd, fpNtuMcTrk, fpNtuHitMsd, fpParGeoMsd, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitMsd->CreateHistogram();
   }
   
   if(TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
      
      fpNtuHitTw   = new TAGdataDsc("twRaw", new TATWntuHit());
      fActNtuHitTw = new TATWactNtuHitMC("twActNtu", fpNtuMcTw, fpNtuMcSt, fpNtuMcTrk, fpNtuHitTw,  fpParCalTw, fpParGeoG, fFlagZtrueMC, fFlagZrecPUoff, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitTw->CreateHistogram();
   }
   
   if(TAGrecoManager::GetPar()->IncludeCA()) {
      fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
      if (TAGrecoManager::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
      
      fpNtuHitCa   = new TAGdataDsc("caRaw", new TACAntuHit());
      fActNtuHitCa = new TACAactNtuHitMC("caActNtu", fpNtuMcCa, fpNtuMcTrk, fpNtuHitCa, fpParGeoCa, fpParCalCa, fpParGeoG, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitCa->CreateHistogram();
   }
   
}







//__________________________________________________________
void GlobalReco::SetL0TreeBranches()
{
  BaseReco::SetL0TreeBranches();
  
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsLocalReco()) {
    if (fFlagMC) {
      fpNtuMcTrk = new TAGdataDsc(TAMCntuPart::GetDefDataName(), new TAMCntuPart());
      fActEvtReader->SetupBranch(fpNtuMcTrk,TAMCntuPart::GetBranchName());
      
      fpNtuMcEvt = new TAGdataDsc("evtMc", new TAMCntuEvent());
      fActEvtReader->SetupBranch(fpNtuMcEvt,TAMCntuEvent::GetBranchName());
      
      if (TAGrecoManager::GetPar()->IsRegionMc()) {
        fpNtuMcReg = new TAGdataDsc("regMc", new TAMCntuRegion());
        fActEvtReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
      }
      
      if (TAGrecoManager::GetPar()->IncludeKalman() && TAGrecoManager::GetPar()->IsLocalReco()) {
        if (TAGrecoManager::GetPar()->IncludeST()) {
          fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcSt,TAMCntuHit::GetStcBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeBM()) {
          fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcBm,TAMCntuHit::GetBmBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeVT()) {
          fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcVt,TAMCntuHit::GetVtxBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeIT()) {
          fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcIt,TAMCntuHit::GetItrBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeMSD()) {
          fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcMsd,TAMCntuHit::GetMsdBranchName());
        }
        
        if(TAGrecoManager::GetPar()->IncludeTW()) {
          fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcTw,TAMCntuHit::GetTofBranchName());
        }
        
        if(TAGrecoManager::GetPar()->IncludeCA()) {
          fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcCa,TAMCntuHit::GetCalBranchName());
        }
      }
    }
  }
}







//__________________________________________________________
void GlobalReco::SetTreeBranches()
{
  BaseReco::SetTreeBranches();
  
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsLocalReco()) {
    if (fFlagMC) {
      fActEvtWriter->SetupElementBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTrk, TAMCntuPart::GetBranchName());
      
      if (TAGrecoManager::GetPar()->IsRegionMc() )
        fActEvtWriter->SetupElementBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
    }
  }
}
