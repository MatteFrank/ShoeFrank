/*!
 // C.A. Reidel & Ch. Finck
 
 \file
 \brief   Implementation of TAIRalignM.
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

#include "TAGactTreeReader.hxx"
#include "TAGdataDsc.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"

#include "TAIRntuTrack.hxx"

#include "TAIRalignM.hxx"

/*!
 \class TAIRalignM
 \brief Alignment class for vertex tracks. **
  Input of TAIRtracks, vertex plane already aligned, all vertex planes nust be set as reference (status = 1)
  All inner tracker set to status 2.
 */

ClassImp(TAIRalignM);

TAIRalignM* TAIRalignM::fgInstance = 0x0;
//__________________________________________________________
TAIRalignM* TAIRalignM::Instance(const TString name)
{
   if (fgInstance == 0x0)
      fgInstance = new TAIRalignM(name);
   
   return fgInstance;
}

//------------------------------------------+-----------------------------------
//! Default constructor.
TAIRalignM::TAIRalignM(const TString name)
 : TObject(),
   fFileName(name),
   fStartFac(16.),
   fResCutInitial(100.),
   fResCut(100.),
   fMillepede(new TAIRmillepede()),
   fNGlobal(0),
   fNLocal(0),
   fNStdDev(3),
   fPhi(0.),
   fCosPhi(1.),
   fSinPhi(0.),
   fDebugLevel(0)
{
      
   Int_t devsNtot = 0;
   
   fAGRoot        = new TAGroot();
   fInfile        = new TAGactTreeReader("inFile");

   // VTX
   fpGeoMapVtx    = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
   TAVTparGeo* geomapVtx   = (TAVTparGeo*) fpGeoMapVtx->Object();
   TString parFile = "./geomaps/TAVTdetector.map";
   geomapVtx->FromFile(parFile.Data());
      
   fpConfigVtx    = new TAGparaDsc("vtConf", new TAVTparConf());
   parFile = "./config/TAVTdetector.cfg";
   TAVTparConf* parConfVtx = (TAVTparConf*) fpConfigVtx->Object();
   parConfVtx->FromFile(parFile.Data());
      
   devsNtot += parConfVtx->GetSensorsN();
   
   // ITR
   fpGeoMapItr    = new TAGparaDsc(TAITparGeo::GetDefParaName(), new TAITparGeo());
   TAITparGeo* geomapMsd   = (TAITparGeo*) fpGeoMapItr->Object();
    parFile = "./geomaps/TAMSDdetector.map";
   geomapMsd->FromFile(parFile.Data());
   
   fpConfigItr    = new TAGparaDsc("msdConf", new TAITparConf());
   TAITparConf* parConfItr = (TAITparConf*) fpConfigItr->Object();
   parFile = "./config/TAMSDdetector.cfg";
   parConfItr->FromFile(parFile.Data());
      
   devsNtot += parConfItr->GetSensorsN();
   
   //IR
   fpNtuTrackIr  = new TAGdataDsc("itTrack", new TAIRntuTrack());
   fInfile->SetupBranch(fpNtuTrackIr, TAIRntuTrack::GetBranchName());
   

   if (devsNtot <= 2) {
      Error("TAIRalignM", "No enough sensors to align");
      exit(0);
   }

   fDevStatus = new Int_t[devsNtot];

   FillStatus();
   
   InitParameters();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAIRalignM::~TAIRalignM()
{
   delete[] fTiltW;
   delete[] fAlignmentU;
   delete[] fAlignmentV;
   delete[] fStatus;
   delete[] fDevStatus;
}

//______________________________________________________________________________
//
// Create all the residuals histograms
void TAIRalignM::InitParameters()
{
   fTiltW           = new Float_t[fSecArray.GetSize()];
   fAlignmentU      = new Float_t[fSecArray.GetSize()];
   fAlignmentV      = new Float_t[fSecArray.GetSize()];
   fStatus          = new Bool_t[fSecArray.GetSize()];
   
   for(Int_t i = 0; i < fSecArray.GetSize(); i++){
      fTiltW[i]           = 0;
      fAlignmentU[i]      = 0;
      fAlignmentV[i]      = 0;
      fStatus[i]          = false;
   }
   
   /// Default constructor, setting define alignment parameters
   fSigma[0] = 10;
   fSigma[1] = 10;
   
   fDoF[0] = kTRUE;  fDoF[1] = kTRUE;  fDoF[2] = kTRUE;
   fAllowVar[0] = 500;  fAllowVar[1] = 500;  fAllowVar[2] = 0.1;
   
}

//______________________________________________________________________________
//
// Create all the residuals histograms
void TAIRalignM::FillStatus()
{
   TAVTbaseParConf* parConf = 0x0;
   fOffsetItr = 0;

   parConf = (TAVTparConf*) fpConfigVtx->Object();
   FillStatus(parConf);
   fOffsetItr += fSecArray.GetSize();
   
   parConf = (TAITparConf*) fpConfigItr->Object();
   FillStatus(parConf, fOffsetItr);
}

//______________________________________________________________________________
//
// Fill Status
void TAIRalignM::FillStatus(TAVTbaseParConf* parConf, Int_t offset)
{
   for (Int_t i = 0; i < parConf->GetSensorsN(); i++) {
      if (parConf->GetStatus(i) != -1) {
         fSecArray.Set(fSecArray.GetSize()+1);
         fSecArray.AddAt(i, fSecArray.GetSize()-1);
      }
      
      Int_t iSensor = fSecArray.GetSize()-1;
      fDevStatus[fSecArray.GetSize()-1] = parConf->GetStatus(iSensor);
   }
}

//______________________________________________________________________________
//
// Create all the residuals histograms
void TAIRalignM::CreateHistogram()
{   
   Int_t iPlane = 0;
   for (Int_t i = 0; i < fSecArray.GetSize(); i++) {
      iPlane = fSecArray[i];
      
      fpResXC[i] = new TH1F(Form("ResX%dC", i+1), Form("ResidualX of sensor %d", iPlane+1), 400, -200, 200);
      fpResYC[i] = new TH1F(Form("ResY%dC", i+1), Form("ResidualY of sensor %d", iPlane+1), 400, -200, 200);
   }
   
   fpResTotXC = new TH1F("TotResXC", "Total residualX", 400, -200, 200);
   fpResTotYC = new TH1F("TotResYC", "Total residualY", 400, -200, 200);
   
   return;
}

//_____________________________________________________________________________
//
void TAIRalignM::LoopEvent(Int_t nEvts)
{
   //init Millipede
   TAIRntuTrack* pNtuTrk = (TAIRntuTrack*) fpNtuTrackIr->Object();

   Int_t   nPlanes  = fSecArray.GetSize();
   Int_t nParPlanes = 3;
   Int_t nParam     = nParPlanes*nPlanes;
   Int_t nLocPar    = 4;
   Int_t stdDev     = 3;
   
   Double_t parameters[nParam];
   Double_t errors[nParam];
   Double_t pulls[nParam];
   Double_t gloConst[nPlanes];
   
   for(Int_t k = 0; k < nParam; ++k) {
      parameters[k] = 0.;
      errors[k]     = 0.;
      pulls[k]      = 0.;
      gloConst[k]   = 0.;
   }
   
   Double_t trackParams[nLocPar*2];
   for(Int_t k = 0; k < nLocPar*2; ++k)
      trackParams[k] = 0.;
   
   // init millepede
   Init(nParam, nLocPar, stdDev, nPlanes);
   SetIterations(20);

   for(Int_t k = 0; k < nPlanes; ++k) {
      if (fDevStatus[k] == -1) continue;
      if (fDevStatus[k] == 1) {
         FixPlane(k);
      } else {
         FixParameter(nParPlanes*k+0, 600);
         FixParameter(nParPlanes*k+1, 600);
         FixParameter(nParPlanes*k+2, 0.2);
      }
   }
   
   InitGlobalParameters(parameters);
   //AddConstraint(gloConst, 0.0);
   
   fAGRoot->AddRequiredItem(fInfile);
   if (fInfile->Open(fFileName.Data())) return;
   
   CreateHistogram();
   
   fAGRoot->BeginEventLoop();
   fAGRoot->Print();
   
   if (nEvts > fInfile->NEvents() || nEvts == 1) nEvts = fInfile->NEvents();
   printf("Number of total entries: %d \n", fInfile->NEvents());
   printf("Number of events: %d \n", nEvts);
   
   fInfile->Reset();
   
   // loop over events
   Int_t nTrack = 0;
   
   for (Int_t i = 0; i < nEvts; ++i ) {
      if(i % 10000 == 0) {
         std::cout << "Loaded Event:: " << i << std::endl;
      }
      if (!fAGRoot->NextEvent()) break;
      
      for( Int_t iTrack = 0; iTrack < pNtuTrk->GetTracksN(); ++iTrack ) {
         TAIRtrack* aTrack = pNtuTrk->GetTrack(iTrack);
         
         ProcessTrack(aTrack);
         LocalFit(nTrack,trackParams,1);
         
         ProcessTrack(aTrack, trackParams);
         LocalFit(nTrack++,trackParams,0);
         
         //       printf("param %6.3f %6.3f %e %e    %6.3f %6.3f %e %e\n",
         //            aTrack->GetTrackLine().GetOrigin()(0), trackParams[0], aTrack->GetTrackLine().GetSlopeZ()(0), trackParams[2],
         //            aTrack->GetTrackLine().GetOrigin()(1), trackParams[4], aTrack->GetTrackLine().GetSlopeZ()(1), trackParams[6] );
         
      } // end loop on tracks
      
   }
   
   GlobalFit(parameters,errors,pulls);
   UpdateAlignmentParams(trackParams);
   
   UpdateGeoMaps();
   
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMapItr->Object();
   Float_t limitShift  = 5;
   Float_t limitTilt   = 0.1*TMath::DegToRad();

   // Now display the filled histos
   printf("------------------------------------------------------------\n");
   printf("--------------------- Alignment RESULT --------------------\n");
   printf("------------------------------------------------------------\n");
   
   for(Int_t i = 0; i < fSecArray.GetSize(); i++) {
      if (fDevStatus[i] != 2) continue; //only IT
      Int_t iPlane = fSecArray[i];
     fStatus[i] = (pGeoMap->GetSensorPar(iPlane).AlignmentU < limitShift) && (pGeoMap->GetSensorPar(iPlane).AlignmentV < limitShift) && (pGeoMap->GetSensorPar(iPlane).TiltW <limitTilt);
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
//
// The final vector containing the alignment parameters are calculated
void TAIRalignM::UpdateAlignmentParams(Double_t* parameters)
{
   TAITparGeo* pGeoMap  = (TAITparGeo*) fpGeoMapItr->Object();

   for (Int_t iPlane = 0; iPlane < fSecArray.GetSize(); iPlane++){
      Int_t iSensor = fSecArray[iPlane];
      if (fDevStatus[iPlane] != 2) continue; //only IT

      printf("Sensor: %d OffsetU: %5.1f OffsetV: %5.1f TiltW: %6.3f \n", iSensor+1,
             pGeoMap->GetSensorPar(iPlane).AlignmentU,
             pGeoMap->GetSensorPar(iPlane).AlignmentV,
             pGeoMap->GetSensorPar(iPlane).TiltW*TMath::RadToDeg());
      
      fAlignmentU[iPlane] = pGeoMap->GetSensorPar(iPlane).AlignmentU - parameters[iPlane*GetNParPlane() + 0];
      fAlignmentV[iPlane] = pGeoMap->GetSensorPar(iPlane).AlignmentV - parameters[iPlane*GetNParPlane() + 1];
      fTiltW[iPlane]      = pGeoMap->GetSensorPar(iPlane).TiltW      - parameters[iPlane*GetNParPlane() + 2];
   }
   return;
}

//_________________________________________________________________
void TAIRalignM::Init(Int_t nGlobal,  /* number of global paramers */
                      Int_t nLocal,   /* number of local parameters */
                      Int_t nStdDev,  /* std dev cut */
                      Int_t nPlanes   /* number of planes */)
{
   /// Initialization of AliMillepede. Fix parameters, define constraints ...
   fMillepede->InitMille(nGlobal,nLocal,nStdDev,fResCut,fResCutInitial);
   fNGlobal   = nGlobal;
   fNLocal    = nLocal;
   fNStdDev   = nStdDev;
   fNPlanes   = nPlanes;
   fNParPlane = nGlobal/nPlanes;
   
   // Set iterations
   if (fStartFac > 1) fMillepede->SetIterations(fStartFac);
   
   printf("fNPlanes: %i\t fNGlobal: %i\t fNLocal: %i\n", fNPlanes, fNGlobal, fNLocal);
   
   ResetLocalEquation();
   printf("Parameters initialized to zero\n");
   
}

//_________________________________________________________________
void TAIRalignM::SetNonLinear(Bool_t *lChOnOff,Bool_t *lVarXYT)
{
   /// Set non linear parameter flag selected chambers and degrees of freedom
   for (Int_t i = 0; i < fNPlanes; i++){
      if (lChOnOff[i]){
         if (lVarXYT[0]) { // X constraint
            SetNonLinear(i*fNParPlane+0);
         }
         if (lVarXYT[1]) { // Y constraint
            SetNonLinear(i*fNParPlane+1);
         }
         if (lVarXYT[2]) { // T constraint
            SetNonLinear(i*fNParPlane+2);
         }
      }
   }
}

//_________________________________________________________________
void TAIRalignM::SetIterations(Int_t iter)
{
   fMillepede->SetIterations(iter);
}

//_________________________________________________________________
void TAIRalignM::AddConstraint(Double_t *par, Double_t value) {
   /// Constrain equation defined by par to value
   fMillepede->SetGlobalConstraint(par, value);
   printf("Adding constraint\n");
}

//_________________________________________________________________
void TAIRalignM::InitGlobalParameters(Double_t *par)
{
   /// Initialize global parameters with par array
   fMillepede->SetGlobalParameters(par);
   printf("Init Global Parameters\n");
}

//_________________________________________________________________
void TAIRalignM::FixPlane(Int_t iPlane)
{
   /// Fix all detection elements of chamber iCh
   
   FixParameter(iPlane*fNParPlane+0, 0.0);
   FixParameter(iPlane*fNParPlane+1, 0.0);
   FixParameter(iPlane*fNParPlane+2, 0.0);
   
}

//_________________________________________________________________
void TAIRalignM::FixParameter(Int_t iPar, Double_t value)
{
   /// Parameter iPar is encourage to vary in [-value;value].
   /// If value == 0, parameter is fixed
   fMillepede->SetParSigma(iPar, value);
   if (value==0) printf("Parameter %i Fixed\n", iPar);
}

//_________________________________________________________________
void TAIRalignM::ResetLocalEquation()
{
   /// Reset the derivative vectors
   for(int i=0; i<fNLocal; i++) {
      fLocalDerivatives[i] = 0.0;
   }
   for(int i=0; i<fNGlobal; i++) {
      fGlobalDerivatives[i] = 0.0;
   }
}

//_________________________________________________________________
void TAIRalignM::AllowVariations(Bool_t *bPlaneOnOff)
{
   /// Set allowed variation for selected chambers based on fDoF and fAllowVar
   for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++) {
      if (bPlaneOnOff[iPlane]) {
         for (int i=0; i<fNParPlane; i++) {
            if (fDebugLevel)
               printf("fDoF[%d]= %d",i,fDoF[i]);
            if (fDoF[i]) {
               FixParameter(iPlane*fNParPlane+i, fAllowVar[i]);
               
            }
         }
      }
   }
}
//_________________________________________________________________
void TAIRalignM::SetNonLinear(Int_t iPar  /* set non linear flag */ ) {
   /// Set nonlinear flag for parameter iPar
   fMillepede->SetNonLinear(iPar);
   printf("Parameter %i set to non linear\n", iPar);
}

//_________________________________________________________________
void TAIRalignM::LocalEquationX(TAIRcluster* cluster, Double_t* param)
{
   /// Define local equation for current track and cluster in x coor. measurement
   // set local derivatives   
   Int_t planeNumber = cluster->GetPlaneNumber();
   TVector3 pos      = cluster->GetPositionG();

   SetLocalDerivative(0, 1.);
   SetLocalDerivative(1, pos(2));
   SetLocalDerivative(2, 0.);
   SetLocalDerivative(3, 0.);
   
   // set global derivatives
   if (param == 0x0) {
      SetGlobalDerivative(planeNumber*fNParPlane+0, 0.);
      SetGlobalDerivative(planeNumber*fNParPlane+1, 0.);
      SetGlobalDerivative(planeNumber*fNParPlane+2, 0.);
   } else {
      SetGlobalDerivative(planeNumber*fNParPlane+0, 1.);
      SetGlobalDerivative(planeNumber*fNParPlane+1, 0.);
      SetGlobalDerivative(planeNumber*fNParPlane+2, -(param[4]+param[6]*pos(2)));
   }
   
   fMillepede->SetLocalEquation(fGlobalDerivatives, fLocalDerivatives, fMeas[0], fSigma[0]);
}

//_________________________________________________________________
void TAIRalignM::LocalEquationY(TAIRcluster* cluster, Double_t* param)
{
   /// Define local equation for current track and cluster in y coor. measurement
   // set local derivatives
   Int_t planeNumber = cluster->GetPlaneNumber();
   TVector3 pos      = cluster->GetPositionG();
   
   SetLocalDerivative(0, 0.);
   SetLocalDerivative(1, 0.);
   SetLocalDerivative(2, 1.);
   SetLocalDerivative(3, pos(2));
   
   // set global derivatives
   if (param == 0x0) {
      SetGlobalDerivative(planeNumber*fNParPlane+0, 0.);
      SetGlobalDerivative(planeNumber*fNParPlane+1, 0.);
      SetGlobalDerivative(planeNumber*fNParPlane+2, 0.);
   } else {
      SetGlobalDerivative(planeNumber*fNParPlane+0, 0.);
      SetGlobalDerivative(planeNumber*fNParPlane+1, 1.);
      SetGlobalDerivative(planeNumber*fNParPlane+2, -(param[0]+param[2]*pos(2)));
   }
   
   fMillepede->SetLocalEquation(fGlobalDerivatives, fLocalDerivatives, fMeas[1], fSigma[1]);
}

//_________________________________________________________________
void TAIRalignM::ProcessTrack(TAIRtrack* track, Double_t* param)
{
   /// Process track; Loop over clusters and set local equations
   // get size of arrays
   Int_t nClusters = track->GetClustersN();
   
   if (fDebugLevel)
      printf("Number of track param entries : %i ", nClusters);
   
   for(Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
      TAIRcluster* cluster = track->GetCluster(iCluster);
      if (!cluster) continue;
      // fill local variables for this position --> one measurement
      
      fMeas[0]  = cluster->GetPositionG()(0);
      fMeas[1]  = cluster->GetPositionG()(1);
      fSigma[0] = cluster->GetPosError()(0);
      fSigma[1] = cluster->GetPosError()(1);
      
      if (fDebugLevel > 0)
         printf("fMeas[0]: %f\t fMeas[1]: %f\t fSigma[0]: %f\t fSigma[1]: %f\n", fMeas[0], fMeas[1], fSigma[0], fSigma[1]);
      
      // Set local equations
      LocalEquationY(cluster, param);
      LocalEquationX(cluster, param);
   }
}

//_________________________________________________________________
void TAIRalignM::LocalFit(Int_t iTrack, Double_t *lTrackParam, Int_t lSingleFit)
{
   /// Call local fit for this tracks
   Int_t iRes = fMillepede->LocalFit(iTrack,lTrackParam,lSingleFit);
   if (iRes && !lSingleFit) {
      fMillepede->SetNLocalEquations(fMillepede->GetNLocalEquations()+1);
   }
}


//_________________________________________________________________
void TAIRalignM::GlobalFit(Double_t *parameters,Double_t *errors,Double_t *pulls)
{
   /// Call global fit; Global parameters are stored in parameters
   fMillepede->GlobalFit(parameters,errors,pulls);
   
   printf("Done fitting global parameters!\n");
   for (int i = 0; i < fNPlanes; i++){
      printf("%d\t %8.3f (%8.3f)\t %8.3f (%8.3f)\t %7.4f (%7.4f)\n",i+1,
             parameters[i*fNParPlane+0], errors[i*fNParPlane+0],
             parameters[i*fNParPlane+1], errors[i*fNParPlane+1],
             parameters[i*fNParPlane+2]*TMath::RadToDeg(), errors[i*fNParPlane+2]*TMath::RadToDeg());
   }
}

//_________________________________________________________________
Double_t TAIRalignM::GetParError(Int_t iPar)
{
   /// Get error of parameter iPar
   Double_t lErr = fMillepede->GetParError(iPar);
   return lErr;
}

//_________________________________________________________________
void TAIRalignM::PrintGlobalParameters()
{
   /// Print global parameters
   fMillepede->PrintGlobalParameters();
}


//______________________________________________________________________________
//
// Modification of the geomap file with the final results of alignment
void TAIRalignM::UpdateGeoMaps()
{
   fstream  configFileOld;
   fstream  configFileNew;
   Char_t configFileName[1000];
   
   TAITparGeo* pGeoMap  = (TAITparGeo*)fpGeoMapItr->Object();
   sprintf(configFileName,"%s", pGeoMap->GetFileName().Data());
   configFileOld.open(configFileName, ios::in);
   sprintf(configFileName,"%s_new", pGeoMap->GetFileName().Data());
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
            if (fDevStatus[i] != 2) continue;
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
//
// Modification of the geomap file with the final results of alignment for each plane
void TAIRalignM::UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx)
{
   Char_t tmp[255];
   TString key;
   TAITparGeo* pGeoMap  = (TAITparGeo*)fpGeoMapItr->Object();
   
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

