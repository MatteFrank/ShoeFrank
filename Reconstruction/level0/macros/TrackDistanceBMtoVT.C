// Macro to compute distance between BM track and vertex
// Ch. Finck, Jan 21.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TString.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TStyle.h>

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGcampaignManager.hxx"

#include "TABMtrack.hxx"
#include "TABMntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#endif

// main
void TrackDistanceBMtoVT(TString nameFile = "12C_C_200_L0Out.root", TString expName = "12C_200", Int_t runNumber = 1)
{
  GlobalPar::Instance(expName);
  GlobalPar::GetPar()->FromFile();
  GlobalPar::GetPar()->Print();
  
  TAGroot tagr;
  
  TAGcampaignManager* campManager = new TAGcampaignManager(expName);
  campManager->FromFile();
  
  TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
  TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
  geoTrafo->FromFile(parFileName);
  
  TAGparaDsc* parGeoBm = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
  TABMparGeo* pGeoMap = (TABMparGeo*)parGeoBm->Object();
  parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
  pGeoMap->FromFile(parFileName);
  
  
  TABMntuTrack *bmTrack = new TABMntuTrack();
  TAGdataDsc* bmTrck    = new TAGdataDsc("bmTrck", bmTrack);
  TAGactTreeReader* vtActReader  = new TAGactTreeReader("vtActEvtReader");
  vtActReader->SetupBranch(bmTrck, TABMntuTrack::GetBranchName());
  
  TAVTntuVertex* vtVtx = new TAVTntuVertex();
  TAGdataDsc* vtVt    = new TAGdataDsc("vtVtx", vtVtx);
  vtActReader->SetupBranch(vtVt, TAVTntuVertex::GetBranchName());

  vtActReader->Open(nameFile);
  tagr.AddRequiredItem(vtActReader);
  
  tagr.BeginEventLoop();
  
  TH1F* fpHisDistX = new TH1F("vtDistX", "Distance btw BM and VTX in X direction", 100, -1, 1);
  TH1F* fpHisDistY = new TH1F("vtDistY", "Distance btw BM and VTX in Y direction", 100, -1, 1);

  
  Int_t ev = 0;
  
   while (tagr.NextEvent() ){
   
     //Retrieve first BM track
     TABMtrack* track = bmTrack->GetTrack(0);
     if (!track) continue;

     //Retrieve first VTX vertex
     TAVTvertex* vertex = vtVtx->GetVertex(0);
     if (!vertex) continue;

     //Target position in global  framework
     Float_t posZtg = geoTrafo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
     
     //Target position in BM framework
     posZtg = geoTrafo->FromGlobalToBMLocal(TVector3(0, 0, posZtg)).Z();
     
     //Track position at target in BM framework
     TVector3 bmLoc = track->Intersection(posZtg);
     
     //Track position at target in global framework
     TVector3 bmGlo = geoTrafo->FromBMLocalToGlobal(bmLoc);
     
     //Vertex position in VTX framework
     TVector3 vtLoc = vertex->GetPosition();
     
     //Vertex position in global framework
     TVector3 vtGlo = geoTrafo->FromVTLocalToGlobal(vtLoc);

     //Position difference
     TVector3 diff = vtGlo-bmGlo;
     
     fpHisDistX->Fill(diff[0]);
     fpHisDistY->Fill(diff[1]);

     ev++;
  }
  
  tagr.EndEventLoop();
  
  TCanvas* c1 = new TCanvas();
  c1->Divide(1,2);
  c1->cd(1);
  fpHisDistX->Draw();
  c1->cd(2);
  fpHisDistY->Draw();
}



