// Macro to make local reconstruction


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TString.h>
#include <TStopwatch.h>

#include "GlobalPar.hxx"
#include "GlobalRecoMC.hxx"

#endif

//void ReadVtxRawMC(TString name = "16O_C2H4_200_1.root")
//void ReadVtxRawMC(TString name = "p_80_vtx.root")
//void ReadVtxRawMC(TString name = "12C_80_vtx.root")
void TestBO(TString name = "12C_400_vtx.root")
{
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();
   
   //Int_t pos = name.Last('.');
   //TString nameOut = name(0, pos);
   TString nameOut = "./prova";

   nameOut.Append("_Out.root");
   
   GlobalRecoMC* globRec = new GlobalRecoMC("", name, nameOut);
   
   // global setting
   //locRec->EnableTree();
   globRec->EnableHisto();
   
   
   TStopwatch watch;
   watch.Start();

   globRec->BeforeEventLoop();
   globRec->LoopEvent(1000);
   globRec->AfterEventLoop();

   watch.Print();
}



