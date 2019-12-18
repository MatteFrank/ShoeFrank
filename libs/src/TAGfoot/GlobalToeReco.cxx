
/*!
 \class GlobalToeReco
 \brief Global reconstruction from local reconstruction Tree **
 author: Ch. Finck
 */

#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TADIparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"

#include "TAVTntuVertex.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"

#include "TAGntuGlbTrack.hxx"
#include "TAGactNtuGlbTrack.hxx"

#include "GlobalToeReco.hxx"


ClassImp(GlobalToeReco)

//__________________________________________________________
GlobalToeReco::GlobalToeReco(TString expName, TString fileNameIn, TString fileNameout)
 : TNamed(fileNameIn.Data(), fileNameout.Data()),
   fExpName(expName),
   fTAGroot(new TAGroot()),
   fpFootGeo(new TAGgeoTrafo()),
   fActEvtWriter(new TAGactTreeWriter("locRecFile"))
{
   TAGactNtuGlbTrack::EnableStdAlone();
   
   // Read Trafo file
   TString parFileName = Form("./geomaps/%sFOOT_geo.map", fExpName.Data());
   fpFootGeo->FromFile(parFileName);
}

//__________________________________________________________
GlobalToeReco::~GlobalToeReco()
{
}

//__________________________________________________________
void GlobalToeReco::LoopEvent(Int_t nEvents)
{
   
   for (Int_t ientry = 0; ientry < nEvents; ientry++) {
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
      if (!fTAGroot->NextEvent()) break;;
   }
}

//__________________________________________________________
void GlobalToeReco::BeforeEventLoop()
{
   ReadParFiles();
   CreateRecAction();
   
   OpenFileIn();
   
   AddRecRequiredItem();
   
   OpenFileOut();
   
   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
}

//__________________________________________________________
void GlobalToeReco::AfterEventLoop()
{
   fTAGroot->EndEventLoop();
   CloseFileOut();
   CloseFileIn();
}

//__________________________________________________________
void GlobalToeReco::ReadParFiles()
{
   fpParGeoDi = new TAGparaDsc(TADIparGeo::GetDefParaName(), new TADIparGeo());
   TADIparGeo* parGeoDi = (TADIparGeo*)fpParGeoDi->Object();
   TString parFileName = "./geomaps/TADIdetector.map";
   parGeoDi->FromFile(parFileName.Data());
   
   fpParGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
   TAVTparGeo* parGeoVtx = (TAVTparGeo*)fpParGeoVtx->Object();
   TString parVtxFileName = Form("./geomaps/%sTAVTdetector.map", fExpName.Data());
   parGeoVtx->FromFile(parVtxFileName.Data());
   
   fpParGeoIt = new TAGparaDsc(TAITparGeo::GetItDefParaName(), new TAITparGeo());
   TAITparGeo* parGeoIt = (TAITparGeo*)fpParGeoIt->Object();
   TString parItFileName = Form("./geomaps/%sTAITdetector.map", fExpName.Data());
   parGeoIt->FromFile(parItFileName.Data());
   
   fpParGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
   TAMSDparGeo* parGeoMsd = (TAMSDparGeo*)fpParGeoMsd->Object();
   TString parMsdFileName = Form("./geomaps/%sTAMSDdetector.map", fExpName.Data());
   parGeoMsd->FromFile(parMsdFileName.Data());
   
   fpParGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* parGeoTw = (TATWparGeo*)fpParGeoTw->Object();
   TString parTwFileName = "./geomaps/TATWdetector.map";
   parGeoTw->FromFile(parTwFileName);
}

//__________________________________________________________
void GlobalToeReco::CreateRecAction()
{
   fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
   fActGlbTrack  = new TAGactNtuGlbTrack("glbActTrack", fpNtuVtx, fpNtuClusIt, fpNtuClusMsd, fpNtuRecTw, fpNtuGlbTrack, fpParGeoDi);
   fActGlbTrack->CreateHistogram();
}

//__________________________________________________________
void GlobalToeReco::AddRecRequiredItem()
{
   if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman())
      gTAGroot->AddRequiredItem("glbActTrack");
}

//__________________________________________________________
void GlobalToeReco::SetRecHistogramDir()
{
   //Global track
   fActGlbTrack->SetHistogramDir((TDirectory*)fActEvtWriter->File());
}


//__________________________________________________________
void GlobalToeReco::OpenFileIn()
{
   fActGlbTrack->Open(GetName());
}

//__________________________________________________________
void GlobalToeReco::CloseFileIn()
{
   fActGlbTrack->Close();
}

//__________________________________________________________
void GlobalToeReco::OpenFileOut()
{
   fActEvtWriter->Open(GetTitle(), "RECREATE");
   SetRecHistogramDir();
}

//__________________________________________________________
void GlobalToeReco::CloseFileOut()
{
   fActEvtWriter->Print();
   fActEvtWriter->Close();
}

