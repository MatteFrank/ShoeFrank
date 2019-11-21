// Macro to make local reconstruction
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TString.h>
#include <TStopwatch.h>

#include "GlobalPar.hxx"
#include "GlobalReco.h"

#endif


void DecodeGlbToe(TString name = "data/data_built.2211.physics_foot.daq.VTX.1.dat")
{
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_Out.root");
   
   GlobalReco* glbRec = new GlobalReco(name, nameOut);
   
   // global setting
   glbRec->EnableTree();
   glbRec->EnableHisto();
   glbRec->EnableTracking();

   
   TStopwatch watch;
   watch.Start();
   
   glbRec->BeforeEventLoop();
   glbRec->LoopEvent(500);
   glbRec->AfterEventLoop();

   watch.Print();
}


