// Macro to extrapole BM track to target
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

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGcampaignManager.hxx"

#include "TABMtrack.hxx"
#include "TABMntuTrack.hxx"
#endif

// main
void ExtrapoleBMtoTG(TString nameFile = "12C_C_200_L0Out.root", TString expName = "12C_200", Int_t runNumber = 1)
{
  //Read global parameters
  TAGrecoManager::Instance(expName);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();
  
  TAGroot tagr;
  
  TAGcampaignManager* campManager = new TAGcampaignManager(expName);
  campManager->FromFile();
  
  //Initialise global tranformation
  TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
  TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
  geoTrafo->FromFile(parFileName);
  
  //Read BM geometry
  TAGparaDsc* parGeoBm = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
  TABMparGeo* pGeoMap = (TABMparGeo*)parGeoBm->Object();
  parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
  pGeoMap->FromFile(parFileName);
  
  //Set BM track branch
  TABMntuTrack *vtTrack = new TABMntuTrack();
  TAGdataDsc* vtTrck    = new TAGdataDsc("vtTrck", vtTrack);
  TAGactTreeReader* vtActReader  = new TAGactTreeReader("vtActEvtReader");
  vtActReader->SetupBranch(vtTrck, TABMntuTrack::GetBranchName());
  
  vtActReader->Open(nameFile);
  tagr.AddRequiredItem(vtActReader);
  
  tagr.BeginEventLoop();
  
  Int_t ev = 0;
  
   while (tagr.NextEvent() ){
   
     //Retrieve first BM track
     TABMtrack* track = vtTrack->GetTrack(0);
     if (!track) continue;

     //Track position at the output of BM
     TVector3 A0 = track->Intersection(pGeoMap->GetMylar2().Z());
     A0.Print();
     
     //Target position in global  framework
     Float_t posZtg = geoTrafo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
     
     //Target position in BM framework
     posZtg = geoTrafo->FromGlobalToBMLocal(TVector3(0, 0, posZtg)).Z();
     
     //Track position at target in BM framework
     TVector3 A1 = track->Intersection(posZtg);
     A1.Print();
     
     //Track position at target in global framework 
     TVector3 A2 = geoTrafo->FromBMLocalToGlobal(A1);
     A2.Print();
     
     ev++;
     
     if (ev == 1)
       break;
  }
  tagr.EndEventLoop();

}



