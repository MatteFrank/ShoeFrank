/*!
 \file TAMPalignC.cxx
 \brief   Implementation of TAMPalignC.
 \author C.A. Reidel & Ch. Finck
 */
#include "TMath.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TF1.h"
#include "TList.h"
#include "TStyle.h"
#include "TArrayD.h"
#include "TGraphErrors.h"
#include "TObjArray.h"
#include "TCanvas.h"

#include "TAGparGeo.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGdataDsc.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAVTntuCluster.hxx"

#include "TAMPparGeo.hxx"
#include "TAMPparConf.hxx"

#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGnameManager.hxx"

#include "TAMPalignC.hxx"

#include "TAIRntuAlignC.hxx"


/*!
 \class TAMPalignC
 \brief Alignment class for vertex tracks. **
 */

//! Class Imp
ClassImp(TAMPalignC);

TAMPalignC* TAMPalignC::fgInstance   = 0x0;
      Int_t TAMPalignC::fgkPreciseIt = 5;
//__________________________________________________________
//! Instance
//!
//! \param[in] name action name
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] weight weight flag alignment
TAMPalignC* TAMPalignC::Instance(const TString name, const TString expName, Int_t runNumber, Int_t weight)
{
   if (fgInstance == 0x0)
      fgInstance = new TAMPalignC(name, expName, runNumber, weight);
   
   return fgInstance;
}

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] weight weight flag alignment
TAMPalignC::TAMPalignC(const TString name, const TString expName, Int_t runNumber, Int_t weight)
 : TObject(),
   fFileName(name),
   fCampManager(0x0),
   fRunNumber(runNumber),
   fEbeamInit(0),
   fpcInit(0),
   fBetaInit(0),
   fAbeam(0),
   fZbeam(0),
   fEbeam(0),
   fpc(0),
   fBeta(0),
   fWeighted(weight),
   fPreciseIt(1),
   fCut1(200),
   fCut2(200),
   fCutFactor(0),
   fHitPlanes(-1),
   fEvents1(0),
   fSumWeightQ(0),
   fFixPlaneRef1(false),
   fFixPlaneRef2(false),
   fPlaneRef1(-1),
   fPlaneRef2(-1),
   fResidualX(new TGraphErrors()),
   fResidualY(new TGraphErrors())
{
   
   Int_t devsNtot = 0;
   
   fAGRoot        = new TAGroot();
   fGeoTrafo      = new TAGgeoTrafo();
   fGeoTrafo->FromFile();
   
   // load campaign file
   fCampManager = new TAGcampaignManager(expName);
   fCampManager->FromFile();
   
   fClusterArray  = new TObjArray();
   fClusterArray->SetOwner(false);
   
   fInfile = new TAGactTreeReader("inFile");
   
   fpGeoMapG = new TAGparaDsc(new TAGparGeo());
   TAGparGeo* geomapG   = (TAGparGeo*) fpGeoMapG->Object();
   TString parFile = fCampManager->GetCurGeoFile(TAGparGeo::GetBaseName(), fRunNumber);
   geomapG->FromFile(parFile.Data());
   fpDiff = new TADItrackEmProperties();

   
   // MP
   fpGeoMapMp = new TAGparaDsc(new TAMPparGeo());
   TAMPparGeo* geomapMp = (TAMPparGeo*) fpGeoMapMp->Object();
   parFile = fCampManager->GetCurGeoFile(TAMPparGeo::GetBaseName(), fRunNumber);
   geomapMp->FromFile(parFile.Data());
   
   fpConfigMp = new TAGparaDsc(new TAMPparConf());
   parFile = fCampManager->GetCurConfFile(TAMPparGeo::GetBaseName(), fRunNumber);
   TAMPparConf* parConfMp = (TAMPparConf*) fpConfigMp->Object();
   parConfMp->FromFile(parFile.Data());
   
   devsNtot += parConfMp->GetSensorsN();
   
   Int_t sensorsN = geomapMp->GetSensorsN();
   fpNtuClusVtx   = new TAGdataDsc("vtClus", new TAVTntuCluster(sensorsN));
   fInfile->SetupBranch(fpNtuClusVtx);

   if (devsNtot <= 2) {
      Error("TAMPalignC", "No enough sensors to align");
      exit(0);
   }

   fDevStatus = new Int_t[devsNtot];

   FillStatus();
   
   InitParameters();
   
   FillPosition();
   
   fAlign = new TAIRntuAlignC(&fSecArray, fDevStatus);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMPalignC::~TAMPalignC()
{
   delete[] fWeightQ;
   delete[] fZposition;
   delete[] fThickDect;
   delete[] fSigmaAlfaDist;
   delete[] fSigmaMeasQfinal;
   delete[] fPosUClusters;
   delete[] fPosVClusters;
   delete[] fErrUClusters;
   delete[] fErrVClusters;
   delete[] fTiltW;
   delete[] fAlignmentU;
   delete[] fAlignmentV;
   delete[] fStatus;
   delete[] fDevStatus;
   
   delete fResidualX;
   delete fResidualY;
   delete fClusterArray;
   delete fGeoTrafo;
}

//______________________________________________________________________________
//! Fill array of clusters
void TAMPalignC::FillClusterArray()
{
   fClusterArray->Clear();
   
   TAVTntuCluster* pNtuClus  = (TAVTntuCluster*) fpNtuClusVtx->Object();
   TAMPparConf* pConfigMp = (TAMPparConf*) fpConfigMp->Object();
   
   for (Int_t iPlane = 0; iPlane < pConfigMp->GetSensorsN(); ++iPlane) {
      if (pConfigMp->GetStatus(iPlane) == -1) continue;
      TClonesArray* list = pNtuClus->GetListOfClusters(iPlane);
      fClusterArray->AddLast(list);
   }
}

//______________________________________________________________________________
//! Create all the residuals histograms
void TAMPalignC::InitParameters()
{
   fWeightQ         = new Double_t[fSecArray.GetSize()];
   fZposition       = new Double_t[fSecArray.GetSize()];
   fThickDect       = new Double_t[fSecArray.GetSize()];
   fSigmaAlfaDist   = new Double_t[fSecArray.GetSize()];
   fSigmaMeasQfinal = new Double_t[fSecArray.GetSize()];
   fPosUClusters    = new Double_t[fSecArray.GetSize()];
   fPosVClusters    = new Double_t[fSecArray.GetSize()];
   fErrUClusters    = new Double_t[fSecArray.GetSize()];
   fErrVClusters    = new Double_t[fSecArray.GetSize()];
   fTiltW           = new Float_t[fSecArray.GetSize()];
   fAlignmentU      = new Float_t[fSecArray.GetSize()];
   fAlignmentV      = new Float_t[fSecArray.GetSize()];
   fStatus          = new Bool_t[fSecArray.GetSize()];
   
   for(Int_t i = 0; i < fSecArray.GetSize(); i++){
      fWeightQ[i]         = 0;
      fZposition[i]       = 0;
      fThickDect[i]       = 0;
      fSigmaAlfaDist[i]   = 0;
      fSigmaMeasQfinal[i] = 0;
      fPosUClusters[i]    = 0;
      fPosVClusters[i]    = 0;
      fErrUClusters[i]    = 0;
      fErrVClusters[i]    = 0;
      fTiltW[i]           = 0;
      fAlignmentU[i]      = 0;
      fAlignmentV[i]      = 0;
      fStatus[i]          = false;
   }
}

//______________________________________________________________________________
//! Create all the residuals histograms
void TAMPalignC::FillStatus()
{
   TAVTbaseParConf* parConf = 0x0;
   parConf = (TAMPparConf*) fpConfigMp->Object();
   FillStatus(parConf);
}

//______________________________________________________________________________
//! Fill Status
//!
//! \param[in] parConf base configuration parameters
void TAMPalignC::FillStatus(TAVTbaseParConf* parConf)
{
   for (Int_t i = 0; i < parConf->GetSensorsN(); i++) {
      if (parConf->GetStatus(i) != -1) {
         fSecArray.Set(fSecArray.GetSize()+1);
         fSecArray.AddAt(i, fSecArray.GetSize()-1);
      }
      if (parConf->GetStatus(i) == 0) {
         fFixPlaneRef1 = true;
         fPlaneRef1 = fSecArray.GetSize()-1;
      }
      
      if (parConf->GetStatus(i) == 1) {
         fFixPlaneRef2 = true;
         fPlaneRef2 = fSecArray.GetSize()-1;
      }
      
      Int_t iSensor = fSecArray.GetSize()-1;
      fDevStatus[fSecArray.GetSize()-1] = parConf->GetStatus(iSensor);
   }
   
}

//______________________________________________________________________________
//! Fill Position
void TAMPalignC::FillPosition()
{
   TAVTbaseParGeo*  parGeo = 0x0;
   parGeo  = (TAMPparGeo*)  fpGeoMapMp->Object();
   FillPosition(parGeo);
}

//______________________________________________________________________________
//! Fill position of devices
//!
//! \param[in] parGeo base geometry parameters
void TAMPalignC::FillPosition(TAVTbaseParGeo* parGeo)
{
   TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   Int_t sensorsN = parGeo->GetSensorsN();
   
   for (Int_t i = 0; i < sensorsN; i++) {
      TVector3 posSens = parGeo->GetSensorPosition(i);
      posSens = geoTrafo->FromVTLocalToGlobal(posSens);
      
      fZposition[i] = posSens.Z()*TAGgeoTrafo::CmToMm();
      fThickDect[i] = parGeo->GetTotalSize()[2]*TAGgeoTrafo::CmToMm();
   }
}

//______________________________________________________________________________
//! Create all the residuals histograms
void TAMPalignC::CreateHistogram()
{   
   Int_t iPlane = 0;
   for (Int_t i = 0; i < fSecArray.GetSize(); i++) {
      iPlane = fSecArray[i];
      
      fpResXC[i] = new TH1F(Form("ResX%dC", i+1), Form("ResidualX of sensor %d", iPlane+1), 600, -600, 600);
      fpResYC[i] = new TH1F(Form("ResY%dC", i+1), Form("ResidualY of sensor %d", iPlane+1), 600, -600, 600);
   }
   
   fpResTotXC = new TH1F("TotResXC", "Total residualX", 600, -600, 600);
   fpResTotYC = new TH1F("TotResYC", "Total residualY", 600, -600, 600);
   
   return;
}

//_____________________________________________________________________________
//! Loop event
//!
//! \param[in] nEvts number of events de process
void TAMPalignC::LoopEvent(Int_t nEvts)
{
   if ((fFixPlaneRef1 == false)||(fFixPlaneRef2 == false)) {
      Error("LoopEvent()", "In config:: Wrong references planes in the configuration file: 1 plane must be set to 0 (reference 0),  1 plane must be set to 1 (reference 1) and the others to 2");
      return;
   }
   
   fAGRoot->AddRequiredItem(fInfile);
   if (fInfile->Open(fFileName.Data())) return;
   
   CreateHistogram();
   
   fAGRoot->BeginEventLoop();
   fAGRoot->Print();
   
   if (nEvts > fInfile->NEvents() || nEvts == 1) nEvts = fInfile->NEvents();
   printf("Number of total entries: %d \n", fInfile->NEvents());
   printf("Number of events: %d \n", nEvts);
   
   if(!DefineWeights()) return;
   
   fAlign->DefineConstant(fWeightQ, fZposition);
   fEvents1 = 0;
   fInfile->Reset();
   
   Float_t limitShift = 5;
   Float_t limitTilt  = 0.1*TMath::DegToRad();
   Bool_t roughAlign = true;
   
   for (Int_t i = 0; i < nEvts; ++i ) {
      if(i % 10000 == 0) {
         std::cout << "Loaded Event:: " << i << std::endl;
      }
      if (!fAGRoot->NextEvent()) break;
      
      FillClusterArray();
      if(!Align(roughAlign)) continue;
   }
   
   fAlign->Constraint(fPlaneRef1, fPlaneRef2);
   fAlign->Minimize();
   UpdateAlignmentParams();
   fAlign->Reset();
   printf("Number of events for iteration 1: %d / %d \n", fEvents1, nEvts);
   
   roughAlign = false;
   fCutFactor = fCut1;
   for (Int_t j = 0; j < fPreciseIt; j++){
      fInfile->Reset();
      fEvents1 = 0;
      for (Int_t i = 0; i < nEvts; ++i ) {
         if(i % 10000 == 0) {
            std::cout << "Loaded Event:: " << i << std::endl;
         }
         if (!fAGRoot->NextEvent()) break;
         FillClusterArray();
         if(!Align(roughAlign)) continue;
      }
      
      fAlign->Constraint(fPlaneRef1, fPlaneRef2);
      fAlign->Minimize();
      UpdateAlignmentParams();
      fAlign->Reset();
      printf("Number of events for iteration %d: %d / %d \n", j+2, fEvents1, nEvts);
      fCutFactor = fCut2;
      Bool_t status = true;
      for(Int_t i = 0; i < fSecArray.GetSize(); i++)
         status &= ((fAlign->GetOffsetU()[i]*TAGgeoTrafo::MmToMu() < limitShift) && (fAlign->GetOffsetV()[i]*TAGgeoTrafo::MmToMu() < limitShift)) && (fAlign->GetTiltW()[i]<limitTilt);
      if (!status)
         fPreciseIt++;
      if (fPreciseIt >= fgkPreciseIt)
         break;
   }
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++ ) {
      UpdateTransfo(i);
   }
   
   UpdateGeoMaps();
   
  
   
   // Now display the filled histos
   printf("------------------------------------------------------------\n");
   printf("--------------------- Alignment RESULT --------------------\n");
   printf("------------------------------------------------------------\n");
   
   for(Int_t i = 0; i < fSecArray.GetSize(); i++) {
      Int_t iPlane = fSecArray[i];
      fStatus[i] = ((fAlign->GetOffsetU()[i]*TAGgeoTrafo::MmToMu() < limitShift) && (fAlign->GetOffsetV()[i]*TAGgeoTrafo::MmToMu() < limitShift)) && (fAlign->GetTiltW()[i]<limitTilt);
      printf("----------------------------------------------\n");
      printf("Sensor: %d AlignmentU: %5.1f AlignmentV: %5.1f TiltW: %5.2f status %d\n", iPlane+1,
             fAlignmentU[i]*TAGgeoTrafo::MmToMu(),
             fAlignmentV[i]*TAGgeoTrafo::MmToMu(),
             fTiltW[i]*TMath::RadToDeg(),
             fStatus[i]);
   }
   
   printf("\n----------------------------------------------\n");
   printf("Restart to produce residual plots with %d evts\n", nEvts);
   
   fInfile->Reset();
   for (Int_t i = 0; i < nEvts; ++i ) {
      if(i % 10000 == 0)
         std::cout << " Loaded Event:: " << i << std::endl;
      if (!fAGRoot->NextEvent()) break;
      FillClusterArray();
      if(!FillHistograms()) continue;
   }
   
   TCanvas* canvas1 = new TCanvas("canvas1", "ResidualX");
   canvas1->Divide(fSecArray.GetSize()/2,3);
   gStyle->SetOptStat(1111);
   
   TCanvas* canvas2 = new TCanvas("canvas2", "ResidualY");
   canvas2->Divide(fSecArray.GetSize()/2,3);
   gStyle->SetOptStat(1111);
   
   TCanvas* canvas3 = new TCanvas("canvas3", "ResidualTotX");
   gStyle->SetOptStat(1111);
   
   TCanvas* canvas4 = new TCanvas("canvas4", "ResidualTotY");
   gStyle->SetOptStat(1111);
   
   
   for(Int_t i = 0; i < fSecArray.GetSize(); i++) {
      canvas1->cd(i+1);
      fpResXC[i]->Draw();
      
      canvas2->cd(i+1);
      fpResYC[i]->Draw();
   }
   
   canvas3->cd();
   fpResTotXC->Draw();
   
   canvas4->cd();
   fpResTotYC->Draw();
   
}

//______________________________________________________________________________
//!  Alignment with all the events which fired all the planes
//!
//! \param[in] rough flag for rough or fine cuts
Bool_t TAMPalignC::Align(Bool_t rough)
{
   fSlopeU = 0;
   fSlopeV = 0;
   fNewSlopeU = 0;
   fNewSlopeV = 0;
   
   Int_t iPlane = 0;
   Int_t nCluster = 0;
   fHitPlanes = 0;
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      fPosUClusters[i] = 999999;
      fPosVClusters[i] = 999999;
      Int_t nValidCluster = 0;
      Int_t aCluster = 0;
      TClonesArray* list = (TClonesArray*)fClusterArray->At(i);
      nCluster = list->GetEntriesFast();

      if (nCluster < 1) return false;
      for (Int_t j = 0; j < nCluster; j++){
         TAGcluster* cluster = (TAGcluster*)list->At(j);
         if (cluster->IsValid() != true) continue;
         nValidCluster++;
         if (nValidCluster > 1) return false;
         aCluster = j;
      }
      if (nValidCluster < 1) return false;
      
      fHitPlanes ++;
      TAGcluster* cluster = (TAGcluster*)list->At(aCluster);
      if (rough) {
         FillClusPosRough(i, cluster);
      } else {
         if (!FillClusPosPrecise(i, cluster)) return false;
      }
   }
   
   if (fHitPlanes < fSecArray.GetSize()) return false;
   if (!(fAlign->Accumulate(fPosUClusters, fPosVClusters))) return false;
   fEvents1++;
   fAlign->Sum();
   
   return true;
}

//______________________________________________________________________________
//! Fill rough position of cluster
//!
//! \param[in] i position index
//! \param[in] cluster a given cluster
Bool_t TAMPalignC::FillClusPosRough(Int_t i, TAGcluster* cluster)
{
   fPosUClusters[i] = cluster->GetPositionG()[0]*TAGgeoTrafo::CmToMm();
   fPosVClusters[i] = cluster->GetPositionG()[1]*TAGgeoTrafo::CmToMm();
      
   return true;
}

//______________________________________________________________________________
//! Fill precise position of cluster, rejecting the ones which are too much scattered for
//! a precise alignment
//!
//! \param[in] i position index
//! \param[in] cluster a given cluster
Bool_t TAMPalignC::FillClusPosPrecise(Int_t i, TAGcluster* cluster)
{
   TAVTbaseParGeo* pGeoMap   =  0x0;
   
   pGeoMap = (TAMPparGeo*) fpGeoMapMp->Object();
 

   fPosUClusters[i] = cluster->GetPositionG()[0]*TAGgeoTrafo::CmToMm() + (cluster->GetPositionG()[1]*TAGgeoTrafo::CmToMm() * (-fTiltW[i])) - fAlignmentU[i];
   fPosVClusters[i] = cluster->GetPositionG()[1]*TAGgeoTrafo::CmToMm() - (cluster->GetPositionG()[0]*TAGgeoTrafo::CmToMm() * (-fTiltW[i])) - fAlignmentV[i];
   
   if (i != 0){
      fNewSlopeU = (fPosUClusters[i]-fPosUClusters[i-1])/(fZposition[i]-fZposition[i-1]);
      fNewSlopeV = (fPosVClusters[i]-fPosVClusters[i-1])/(fZposition[i]-fZposition[i-1]);
   }
   
   if ((i != 0) && (i !=1)){
      if (fCutFactor*fSigmaAlfaDist[i]*((fZposition[i]-fZposition[i-1])/TMath::Sqrt(12)*TAGgeoTrafo::MmToMu()) < pGeoMap->GetPitchX()/TMath::Sqrt(12)*TAGgeoTrafo::CmToMu()){
         if ((TMath::Abs(fNewSlopeU - fSlopeU) > pGeoMap->GetPitchX()/TMath::Sqrt(12)*fCutFactor*TAGgeoTrafo::CmToMu()) ||
             (TMath::Abs(fNewSlopeV - fSlopeV) > pGeoMap->GetPitchX()/TMath::Sqrt(12)*fCutFactor*TAGgeoTrafo::CmToMu())) return false;
      } else {
         if ((TMath::Abs(fNewSlopeU - fSlopeU) > fSigmaAlfaDist[i]*fCutFactor) || (TMath::Abs(fNewSlopeV - fSlopeV) > fSigmaAlfaDist[i]*fCutFactor)) return false;
      }
   }
   
   fSlopeU = fNewSlopeU;
   fSlopeV = fNewSlopeV;
   
   return true;
}

//______________________________________________________________________________
//! Filling the histograms with all the events when all the planes are fired
Bool_t TAMPalignC::FillHistograms()
{
   Int_t iPlane = 0;
   Int_t nCluster = 0;
   Double_t intersectionU = 0;
   Double_t intersectionV = 0;
   fHitPlanes = 0;
   Int_t aCluster[50] = {0};
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      fPosUClusters[i] = 999999;
      fPosVClusters[i] = 999999;
      fErrUClusters[i] = 0;
      fErrVClusters[i] = 0;
      Int_t nValidCluster = 0;
      
      iPlane = fSecArray[i];
      TClonesArray* list = (TClonesArray*)fClusterArray->At(iPlane);
      if (list == 0x0) continue;
      
      nCluster = list->GetEntriesFast();
      
      if (nCluster < 1) return false;
      for (Int_t j = 0; j < nCluster; j++){
         TAGcluster* cluster = (TAGcluster*)list->At(j);
         if (cluster->IsValid() != true) continue;
         nValidCluster++;
         if (nValidCluster > 1) return false;
         aCluster[i] = j;
      }
      if (nValidCluster < 1) return false;
      fHitPlanes ++;
      TAGcluster* cluster = (TAGcluster*)list->At(aCluster[i]);
      
      fPosUClusters[i] = cluster->GetPositionG()[0]*TAGgeoTrafo::CmToMm() + (cluster->GetPositionG()[1]*TAGgeoTrafo::CmToMm() * (-fTiltW[i])) - fAlignmentU[i];
      fPosVClusters[i] = cluster->GetPositionG()[1]*TAGgeoTrafo::CmToMm() - (cluster->GetPositionG()[0]*TAGgeoTrafo::CmToMm() * (-fTiltW[i])) - fAlignmentV[i];
      TVector3 posG (fPosUClusters[i]*TAGgeoTrafo::MmToCm(), fPosVClusters[i]*TAGgeoTrafo::MmToCm(), fZposition[i]*TAGgeoTrafo::MmToCm());
      TVector3 pos (fPosUClusters[i]*TAGgeoTrafo::MmToCm(), fPosVClusters[i]*TAGgeoTrafo::MmToCm(), 0.);
      cluster->SetPositionG(posG);
      cluster->SetPosition(pos);
      fErrUClusters[i] = cluster->GetPosError()(0)*TAGgeoTrafo::CmToMm();
      fErrVClusters[i] = cluster->GetPosError()(1)*TAGgeoTrafo::CmToMm();
   }
   
   if (fHitPlanes < fSecArray.GetSize()) return false;
   
   fResidualX->Set(fHitPlanes);
   fResidualY->Set(fHitPlanes);
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      if (fPosUClusters[i] == 999999 || fPosVClusters[i] == 999999) return false;
      fResidualX->SetPoint(i, fZposition[i]*TAGgeoTrafo::MmToMu(), fPosUClusters[i]*TAGgeoTrafo::MmToMu());
      fResidualX->SetPointError(i, 1, fErrUClusters[i]*TAGgeoTrafo::MmToMu()*(1/TMath::Sqrt(fWeightQ[i])));
      fResidualY->SetPoint(i, fZposition[i]*TAGgeoTrafo::MmToMu(), fPosVClusters[i]*TAGgeoTrafo::MmToMu());
      fResidualY->SetPointError(i, 1, fErrVClusters[i]*TAGgeoTrafo::MmToMu()*(1/TMath::Sqrt(fWeightQ[i])));
   }
   
   fResidualX->Fit("pol1", "Q");
   TF1* polyU = fResidualX->GetFunction("pol1");
   fResidualY->Fit("pol1", "Q");
   TF1* polyV = fResidualY->GetFunction("pol1");
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      if (i == 0) continue;
      if (fPosUClusters[i] == 999999 || fPosVClusters[i] == 999999) return false;
      intersectionU = polyU->GetParameter(0) + polyU->GetParameter(1)*fZposition[i]*TAGgeoTrafo::MmToMu();
      intersectionV = polyV->GetParameter(0) + polyV->GetParameter(1)*fZposition[i]*TAGgeoTrafo::MmToMu();
      if ((TMath::Abs(intersectionU - fPosUClusters[i]*TAGgeoTrafo::MmToMu()) > TMath::Sqrt(fSigmaMeasQfinal[i]*1e6)*2) ||
          (TMath::Abs(intersectionV - fPosVClusters[i]*TAGgeoTrafo::MmToMu()) > TMath::Sqrt(fSigmaMeasQfinal[i]*1e6)*2))return false;
   }
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      intersectionU = polyU->GetParameter(0) + polyU->GetParameter(1)*fZposition[i]*TAGgeoTrafo::MmToMu();
      intersectionV = polyV->GetParameter(0) + polyV->GetParameter(1)*fZposition[i]*TAGgeoTrafo::MmToMu();
      fpResXC[i]->Fill(intersectionU - fPosUClusters[i]*TAGgeoTrafo::MmToMu());
      fpResYC[i]->Fill(intersectionV - fPosVClusters[i]*TAGgeoTrafo::MmToMu());
      fpResTotXC->Fill(intersectionU - fPosUClusters[i]*TAGgeoTrafo::MmToMu());
      fpResTotYC->Fill(intersectionV - fPosVClusters[i]*TAGgeoTrafo::MmToMu());
   }
   
   return true;
}

//______________________________________________________________________________
//! The final vector containing the alignment parameters are calculated
void TAMPalignC::UpdateAlignmentParams()
{
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      Int_t iSensor = fSecArray[i];
      
      printf("Sensor: %d OffsetU: %5.1f OffsetV: %5.1f TiltW: %6.3f \n", iSensor+1,
             fAlign->GetOffsetU()[i]*TAGgeoTrafo::MmToMu(),
             fAlign->GetOffsetV()[i]*TAGgeoTrafo::MmToMu(),
             fAlign->GetTiltW()[i]*TMath::RadToDeg());
      
      fAlignmentU[i] += fAlign->GetOffsetU()[i];
      fAlignmentV[i] += fAlign->GetOffsetV()[i];
      fTiltW[i]      += fAlign->GetTiltW()[i];
   }
   return;
}

//______________________________________________________________________________
//! Definition of the weights of the measurements function of the scattering
//! due to the silicon and the air. By default, all the weights are equals.
//! In both cases the weight = 1/sigma and are normalized
//! such Sum(weightsQ) = 1.
//! These calculation are made thanks to the formulas implemented in Scattman
Bool_t TAMPalignC::DefineWeights()
{
   TAGparGeo* pGeoMapG = (TAGparGeo*) fpGeoMapG->Object();
   
   fEbeamInit = pGeoMapG->GetBeamPar().Energy*TAGgeoTrafo::GevToMev();
   fZbeam     = pGeoMapG->GetBeamPar().AtomicNumber;
   fAbeam     = pGeoMapG->GetBeamPar().AtomicMass;
   
   fpcInit    = fpDiff->GetPCC(fEbeamInit, fAbeam);
   fBetaInit  = fpDiff->GetBeta(fEbeamInit);
   
   fEbeam     = fEbeamInit;
   fpc        = fpcInit;
   fBeta      = fBetaInit;
   
   
   Double_t LrSi   = fpDiff->GetRadLength("Si");   // [g/cm^2] Radiation length for silicon
   Double_t rhoSi  = fpDiff->GetDensity("Si");     // [g/cm^3] density silicon
   Double_t LrAir  = fpDiff->GetRadLength("AIR");  // [g/cm^2] Radiation length for air
   Double_t rhoAir = fpDiff->GetDensity("AIR");    // [g/cm^3] density air
   
   Int_t    iSensor   = 0;
   Double_t sigmaMeas = 5.0;
   Float_t  wepl      = 0;
   Double_t km        = 0;
   Double_t factor1   = 0;
   Double_t factor2   = 0;
   
   Double_t sigmaAlfaMeasQ = 0;
   Double_t sigmaMeasQ = sigmaMeas * sigmaMeas * 1e-6; // mu2->mm2
   
   
   Float_t LrSum               = 0;
   Float_t rhodSum             = 0;
   Float_t logtermSum          = 0;
   Float_t nonLogtermSumQ      = 0;
   Float_t previousTermSumQ    = 0;
   Float_t previousDistanceSum = 0;
   
   wepl = fpDiff->GetFacWEPL("Air")* TMath::Abs(pGeoMapG->GetBeamPar().Position[2]*TAGgeoTrafo::CmToMm()-fZposition[iSensor]);

   fEbeam   = fpDiff->GetEnergyLoss(fEbeam, fAbeam, fZbeam, wepl);
   fpc      = fpDiff->GetPCC(fEbeam, fAbeam);
   fBeta    = fpDiff->GetBeta(fEbeam);
   
   if(fEbeam == 0){
      Error("DefineWeights()","Remaining energy in air is 0...");
      return false;
   }
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      iSensor = fSecArray[i];
      Double_t sigmaAlfaScattSi = 0;
      Double_t sigmaAlfaScattAir = 0;
      Float_t weplSi = fpDiff->GetFacWEPL("Si") * fThickDect[i];

      if (i > 0){
         Float_t distance = TMath::Abs(fZposition[i] - fZposition[i-1] - fThickDect[i]);
         sigmaAlfaScattSi = 14100.0/( fBeta * fpc ) * fZbeam * TMath::Sqrt(0.1*fThickDect[i]*rhoSi/LrSi) * (1+log10(0.1*fThickDect[i]*rhoSi/LrSi)/9.0);
         LrSum           += LrSi*0.1*fThickDect[i]*rhoSi;
         rhodSum         +=  0.1*fThickDect[i]*rhoSi;
         logtermSum       = 1+log10 ( rhodSum / (LrSum/rhodSum) )/9.0;
         nonLogtermSumQ  += (sigmaAlfaScattSi / (1+log10 ( 0.1*fThickDect[i]*rhoSi / LrSi )/9.0))*(sigmaAlfaScattSi / (1+log10 ( 0.1*fThickDect[i]*rhoSi / LrSi )/9.0));
         sigmaAlfaScattSi = TMath::Sqrt(logtermSum * logtermSum * nonLogtermSumQ - previousTermSumQ);
         factor1          = sigmaAlfaMeasQ + sigmaAlfaScattSi * sigmaAlfaScattSi;
         factor2          = sigmaAlfaMeasQ / factor1;
         sigmaMeasQ       = sigmaMeasQ + 1e-6 * km * km * factor2 * sigmaAlfaScattSi * sigmaAlfaScattSi; // mrad2 -> rad2
         sigmaAlfaMeasQ   = factor1;
         km               = km * factor2 + fThickDect[i]; // see scattman formalismus paper
         previousTermSumQ = previousTermSumQ + sigmaAlfaScattSi*sigmaAlfaScattSi;
         fEbeam           = fpDiff->GetEnergyLoss(fEbeam, fAbeam, fZbeam, weplSi*0.1);
         fpc              = fpDiff->GetPCC(fEbeam, fAbeam);
         fBeta            = fpDiff->GetBeta(fEbeam);
         
         if(fEbeam == 0){
            Error("DefineWeights()","Remaining energy is 0...");
            return false;
         }
         
         Int_t nLayers = 2;
         for (Int_t iLayer = 0; iLayer < nLayers; iLayer++){
            sigmaAlfaScattAir  =  14100.0/( fBeta * fpc ) * fZbeam * TMath::Sqrt( 0.1*distance/nLayers*rhoAir / LrAir ) *  (1+log10 ( 0.1*distance/nLayers*rhoAir / LrAir )/9.0);
            LrSum             += LrAir*0.1*distance/nLayers*rhoAir;
            rhodSum           +=  0.1*distance/nLayers*rhoAir;
            logtermSum         = 1+log10 ( rhodSum / (LrSum/rhodSum) )/9.0;
            nonLogtermSumQ    +=  (sigmaAlfaScattAir / (1+log10 ( 0.1*distance/nLayers*rhoAir / LrAir )/9.0))*(sigmaAlfaScattAir / (1+log10 ( 0.1*distance/nLayers*rhoAir / LrAir )/9.0));
            sigmaAlfaScattAir  = TMath::Sqrt(logtermSum * logtermSum * nonLogtermSumQ - previousTermSumQ);
            factor1            = sigmaAlfaMeasQ + sigmaAlfaScattAir * sigmaAlfaScattAir ;
            factor2            = sigmaAlfaMeasQ / factor1;
            sigmaMeasQ         = sigmaMeasQ + 1e-6 * km * km * factor2 * sigmaAlfaScattAir * sigmaAlfaScattAir;
            sigmaAlfaMeasQ     = factor1;
            km                 = km * factor2 + distance/nLayers ; // see scattman formalismus paper
            previousTermSumQ   = previousTermSumQ + sigmaAlfaScattAir*sigmaAlfaScattAir;
            wepl               = fpDiff->GetFacWEPL("Air")* distance/nLayers;
            fEbeam             = fpDiff->GetEnergyLoss(fEbeam, fAbeam, fZbeam, wepl*0.1);
            fpc                = fpDiff->GetPCC(fEbeam, fAbeam);
            fBeta              = fpDiff->GetBeta(fEbeam);
            
            if(fEbeam == 0){
               Error("DefineWeights()","Remaining energy is 0...");
               return false;
            }
         }
      }
      
      fSigmaMeasQfinal[i] = sigmaMeasQ + 1e-6 * km * km * sigmaAlfaMeasQ;
      if (i == 0) fSigmaAlfaDist[i] = TMath::Sqrt(previousTermSumQ*1e-6);
      if (i > 0) fSigmaAlfaDist[i] = TMath::Sqrt(previousTermSumQ*1e-6 - previousDistanceSum);
      previousDistanceSum += fSigmaAlfaDist[i] * fSigmaAlfaDist[i];
      if (i == 0 && fSigmaMeasQfinal[i] == 0){
         Error("DefineWeights()","Can't divide by 0.....");
         return false;
      }
      
      fWeightQ[i] = 1/(fSigmaMeasQfinal[i]);
      if (fWeighted != 1) fWeightQ[i] = 1;
      fSumWeightQ += fWeightQ[i];
   }
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++) {
      fWeightQ[i] = fWeightQ[i] / fSumWeightQ;
//      printf("%.1e %.1e\n",  fSigmaMeasQfinal[i], fSigmaAlfaDist[i]);
   }
   
   return true;
}
//______________________________________________________________________________
//! Update transfo for every loop when it changes the alignment parameters
//!
//! \param[in] idx sensor index
void TAMPalignC::UpdateTransfo(Int_t idx)
{
   TAMPparGeo* pGeoMap  = (TAMPparGeo*) fpGeoMapMp->Object();
   Int_t       iPlane   = fSecArray[idx];
   pGeoMap->GetSensorPar(iPlane).AlignmentU = fAlignmentU[idx];
   pGeoMap->GetSensorPar(iPlane).AlignmentV = fAlignmentV[idx];
   pGeoMap->GetSensorPar(iPlane).TiltW      = -fTiltW[idx];
   
   return;
}

//______________________________________________________________________________
//! Modification of the geomap file with the final results of alignment
void TAMPalignC::UpdateGeoMaps()
{
   fstream  configFileOld;
   fstream  configFileNew;
   Char_t configFileName[1000];
   
   TAMPparGeo* pGeoMap  = (TAMPparGeo*)fpGeoMapMp->Object();
   strcpy(configFileName, pGeoMap->GetFileName().Data());
   configFileOld.open(configFileName, ios::in);
   strcpy(configFileName, Form("%s_new", pGeoMap->GetFileName().Data()));
   configFileNew.open(configFileName, ios::out);
   
   
   Char_t tmp[255];
   
   printf("\nCreating new file %s with updated alignment parameters\n", configFileName);
   
   TString key;
   while (!configFileOld.eof()) {
      configFileOld.getline(tmp, 255);
      key = tmp;
      configFileNew << tmp;
      configFileNew << "\n";
      if (key.Contains("Inputs:")) {
         Int_t pos = key.First(':');
         TString sIdx = key(pos+1, key.Length());
         Int_t idx = (Int_t)sIdx.Atoi()-1;
         for (Int_t i = 0; i < fSecArray.GetSize(); ++i) {
            if (idx == fSecArray[i]){
               UpdateGeoMapsUVW(configFileOld, configFileNew, idx);
               break;
            }
         }
      }
   }
   configFileOld.close();

   
   return;
}

//______________________________________________________________________________
//! Modification of the geomap file with the final results of alignment for each plane
//!
//! \param[in] fileIn old alignment file
//! \param[in] fileOut new alignment file
//! \param[in] idx index sensor
void TAMPalignC::UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx)
{
   Char_t tmp[255];
   TString key;
   TAMPparGeo* pGeoMap  = (TAMPparGeo*)fpGeoMapMp->Object();
   
   Float_t alignU = pGeoMap->GetSensorPar(idx).AlignmentU*TAGgeoTrafo::MmToCm();
   Float_t alignV = pGeoMap->GetSensorPar(idx).AlignmentV*TAGgeoTrafo::MmToCm();
   Float_t tiltW  = pGeoMap->GetSensorPar(idx).TiltW*TMath::RadToDeg();
   
   while (!fileIn.eof()) {
      fileIn.getline(tmp, 255);
      key = tmp;
      if (key.Contains("AlignementU:")) {
         key = "AlignementU:";
         TString sIdx;
         sIdx = Form("      %7.4f\n", alignU);
         TString line = key+sIdx;
         fileOut << line.Data();
         fileIn.getline(tmp, 255);
         key = "AlignementV:";
         sIdx = Form("      %7.4f\n", alignV);
         line = key + sIdx;
         fileOut << line.Data();
         fileIn.getline(tmp, 255);
         key = "AlignementTilt:";
         sIdx = Form("   %6.3f\n", tiltW);
         line = key + sIdx;
         fileOut << line.Data();
         break;
      } else {
         fileOut << tmp;
         fileOut << "\n";
      }
   } 
   
   return;
}

