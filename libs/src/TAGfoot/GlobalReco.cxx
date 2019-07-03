

#include "GlobalReco.hxx"

#include "TAGroot.hxx"
#include "TAGntuGlbTrack.hxx"
#include "GlobalPar.hxx"

ClassImp(GlobalReco)

//__________________________________________________________
GlobalReco::GlobalReco(TString expName, TString fileNameIn, TString fileNameout)
 : LocalReco(fileNameIn.Data(), fileNameout.Data()),
   fpNtuGlbTrack(0x0),
   fActGlbTrack(0x0)
{
}

//__________________________________________________________
GlobalReco::~GlobalReco()
{
}

//__________________________________________________________
void GlobalReco::CreateRecAction()
{
   LocalReco::CreateRecAction();
   
   fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
   fActGlbTrack  = new TAGactNtuGlbTrack("glbTrack", fpNtuVtx, fpNtuClusIt, fpNtuClusMsd, fpNtuRecTw, fpNtuGlbTrack, fpParGeoDi);
}

//__________________________________________________________
void GlobalReco::SetHistogramDir()
{
   LocalReco::SetRecHistogramDir();
   fActGlbTrack->SetHistogramDir((TDirectory*)fActEvtWriter->File());
}

//__________________________________________________________
void GlobalReco::AddRequiredItem()
{
   LocalReco::AddRecRequiredItem();
   
   gTAGroot->AddRequiredItem("glbTrack");
}
