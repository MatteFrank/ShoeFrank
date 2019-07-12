

#include "GlobalRecoMC.hxx"

#include "TAGroot.hxx"
#include "TAGntuGlbTrack.hxx"
#include "GlobalPar.hxx"

ClassImp(GlobalRecoMC)

//__________________________________________________________
GlobalRecoMC::GlobalRecoMC(TString expName, TString fileNameIn, TString fileNameout)
 : LocalRecoMC(expName, fileNameIn, fileNameout),
   fpNtuGlbTrack(0x0),
   fActGlbTrack(0x0)
{
}

//__________________________________________________________
GlobalRecoMC::~GlobalRecoMC()
{
}

//__________________________________________________________
void GlobalRecoMC::CreateRecAction()
{
   LocalRecoMC::CreateRecAction();
   
   if (GlobalPar::GetPar()->IncludeKalman()) {
      fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
      fActGlbTrack  = new TAGactNtuGlbTrack("glbActTrack", fpNtuVtx, fpNtuClusIt, fpNtuClusMsd, fpNtuRecTw, fpNtuGlbTrack, fpParGeoDi,
                                            fpParGeoVtx, fpParGeoIt, fpParGeoMsd, fpParGeoTw);
      if (fFlagHisto)
         fActGlbTrack->CreateHistogram();
   }
}

//__________________________________________________________
void GlobalRecoMC::SetHistogramDir()
{
   LocalRecoMC::SetRecHistogramDir();
   if (GlobalPar::GetPar()->IncludeKalman())
	  fActGlbTrack->SetHistogramDir((TDirectory*)fActEvtWriter->File());
}

//__________________________________________________________
void GlobalRecoMC::AddRecRequiredItem()
{
   LocalRecoMC::AddRecRequiredItem();
   if (GlobalPar::GetPar()->IncludeKalman())
     gTAGroot->AddRequiredItem("glbActTrack");
}
