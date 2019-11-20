// Macro to make local reconstruction
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TString.h>
#include <TStopwatch.h>

#include "GlobalPar.hxx"
#include "GlobalRecoMC.h"

#endif


void DecodeGlbToeMC(TString name = "16O_C2H4_200_1.root")
{
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_Out.root");
   
   GlobalRecoMC* glbRec = new GlobalRecoMC(name, nameOut);
   
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


