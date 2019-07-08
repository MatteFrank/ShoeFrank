

#include "GlobalReco.hxx"

#include "TAGroot.hxx"
#include "TAGntuGlbTrack.hxx"
#include "GlobalPar.hxx"

ClassImp(GlobalReco)

//__________________________________________________________
GlobalReco::GlobalReco(TString expName, TString fileNameIn, TString fileNameout)
 : LocalReco(expName, fileNameIn, fileNameout),
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
   fActGlbTrack  = new TAGactNtuGlbTrack("glbActTrack", fpNtuVtx, fpNtuClusIt, fpNtuClusMsd, fpNtuRecTw, fpNtuGlbTrack, fpParGeoDi,
                                         fpParGeoVtx, fpParGeoIt, fpParGeoMsd, fpParGeoTw);
   if (fFlagHisto)
      fActGlbTrack->CreateHistogram();
}

//__________________________________________________________
void GlobalReco::SetHistogramDir()
{
   LocalReco::SetRecHistogramDir();
   fActGlbTrack->SetHistogramDir((TDirectory*)fActEvtWriter->File());
}

//__________________________________________________________
void GlobalReco::AddRecRequiredItem()
{
   LocalReco::AddRecRequiredItem();
   gTAGroot->AddRequiredItem("glbActTrack");
}
