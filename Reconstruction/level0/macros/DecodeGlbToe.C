// Macro to make local reconstruction
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TString.h>
#include <TStopwatch.h>

#include "TAGrecoManager.hxx"
#include "GlobalToeReco.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"

#endif


void DecodeGlbToe(TString in = "dataRaw/data_built.2211.physics_foot.daq.VTX.1.dat", TString exp = "GSI", Int_t runNb = 2211, Bool_t mc = false)
{
  Int_t pos = in.Last('.');
  TString out = in(0, pos);
  out.Append("_Out.root");
  
  TAGrecoManager::Instance(exp);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();
  
  Bool_t lrc = TAGrecoManager::GetPar()->IsLocalReco();
  Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
  Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
  Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();
  Bool_t trk = TAGrecoManager::GetPar()->IsTracking();
  Bool_t zmc = TAGrecoManager::GetPar()->IsTofZmc();
  Bool_t tbc = TAGrecoManager::GetPar()->IsTofCalBar();
  
  TAGrecoManager::GetPar()->IncludeTOE(true);
  TAGrecoManager::GetPar()->IncludeKalman(false);
  
  BaseReco* glbRec = 0x0;
  
  if (lrc)
    glbRec = new GlobalToeReco(exp, runNb, in, out, mc);
  else if (mc) {
      glbRec = new LocalRecoMC(exp, runNb, in, out);
    if(zmc)
      glbRec->EnableZfromMCtrue();
  } else {
    glbRec = new LocalReco(exp, runNb, in, out);
    if (tbc)
      glbRec->EnableTWcalibPerBar();
  }
  
  
  // global setting
  if (ntu)
    glbRec->EnableTree();
  if(his)
    glbRec->EnableHisto();
  if(hit) {
    glbRec->EnableTree();
    glbRec->EnableSaveHits();
  }
  if (trk)
    glbRec->EnableTracking();


   TStopwatch watch;
   watch.Start();
   
   glbRec->BeforeEventLoop();
   glbRec->LoopEvent(500);
   glbRec->AfterEventLoop();

   watch.Print();
}


