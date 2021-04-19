// Macro to make local reconstruction
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TString.h>
#include <TStopwatch.h>

#include "TAGrecoManager.hxx"
#include "LocalRecoMC.hxx"

#endif


//void ReadVtxRawMC(TString name = "16O_C2H4_200_1.root")
//void ReadVtxRawMC(TString name = "p_80_vtx.root")
//void ReadVtxRawMC(TString name = "12C_80_vtx.root")
void DecodeMC(TString name = "12C_C_200_1.root", TString exp = "12C_200", Int_t runNumber = 1)
{
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_Out.root");
  
   Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
   Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
   Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();
   Bool_t trk = TAGrecoManager::GetPar()->IsTracking();
   Bool_t zmc = TAGrecoManager::GetPar()->IsTofZmc();
  
   LocalRecoNMtuC* locRec = new LocalRecoMC(exp, runNumber, name, nameOut);
   
   // global setting
   if (ntu)
     locRec->EnableTree();
  
   if (trk)
     locRec->EnableTracking();

   if (his)
     locRec->EnableHisto();
  
   if(hit) {
     locRec->EnableTree();
     locRec->EnableSaveHits();
   }
  
   if(zmc)
     locRec->EnableZfromMCtrue();
  
   TStopwatch watch;
   watch.Start();
   
   locRec->BeforeEventLoop();
   locRec->LoopEvent(500);
   locRec->AfterEventLoop();

   watch.Print();
}


