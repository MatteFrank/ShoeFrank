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
  Input of TAIRtracks, vertex Sensor already aligned, all vertex Sensors nust be set as reference (status = 1)
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
   fAGRoot        = new TAGroot();
   fInfile        = new TAGactTreeReader("inFile");
   
   // VTX
   fpGeoMapVtx    = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
   TAVTparGeo* geomapVtx   = (TAVTparGeo*) fpGeoMapVtx->Object();
   TString parFile = "./geomaps/TAVTdetector.map";
   geomapVtx->FromFile(parFile.Data());
      
   fpConfigVtx    = new TAGparaDsc("vtConf", new TAVTparConf());
   parFile = "./config/TAVTdetector_align.cfg";
   TAVTparConf* parConfVtx = (TAVTparConf*) fpConfigVtx->Object();
   parConfVtx->FromFile(parFile.Data());
   
   // ITR
   fpGeoMapItr    = new TAGparaDsc(TAITparGeo::GetItDefParaName(), new TAITparGeo());
   TAITparGeo* geomapMsd   = (TAITparGeo*) fpGeoMapItr->Object();
    parFile = "./geomaps/TAITdetector.map";
   geomapMsd->FromFile(parFile.Data());
   
   fpConfigItr    = new TAGparaDsc("itConf", new TAITparConf());
   TAITparConf* parConfItr = (TAITparConf*) fpConfigItr->Object();
   parFile = "./config/TAITdetector_align.cfg";
   parConfItr->FromFile(parFile.Data());
   
   //IR
   fpNtuTrackIr  = new TAGdataDsc("itTrack", new TAIRntuTrack());
   fInfile->SetupBranch(fpNtuTrackIr, TAIRntuTrack::GetBranchName());

   Int_t devsNtot =  parConfVtx->GetSensorsN() + parConfItr->GetSensorsN();
   fDevStatus     = new Int_t[devsNtot];
   
   FillStatus();   
   InitParameters();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAIRalignM::~TAIRalignM()
{
   delete[] fDevStatus;
}

//______________________________________________________________________________
//
// Create all the residuals histograms
void TAIRalignM::InitParameters()
{   
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
      
      fDevStatus[fSecArray.GetSize()-1] = parConf->GetStatus(i);
   }
}

//______________________________________________________________________________
//
// Create all the residuals histograms
void TAIRalignM::CreateHistogram()
{   
   Int_t iSensor = 0;
   for (Int_t i = 0; i < fSecArray.GetSize(); i++) {
      iSensor = fSecArray[i];
      
      fpResXC[i] = new TH1F(Form("ResX%dC", i+1), Form("ResidualX of sensor %d", iSensor+1), 400, -200, 200);
      fpResYC[i] = new TH1F(Form("ResY%dC", i+1), Form("ResidualY of sensor %d", iSensor+1), 400, -200, 200);
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

   Int_t   nSensors  = fSecArray.GetSize();
   Int_t nParSensors = 3; // Alignment X-Y and rotation around Z
   Int_t nParam     = nParSensors*nSensors;
   Int_t nLocPar    = 4;
   Int_t stdDev     = 3;
   
   Double_t parameters[nParam];
   Double_t errors[nParam];
   Double_t pulls[nParam];
   Double_t gloConst[nSensors];
   
   for(Int_t k = 0; k < nParam; ++k) {
      parameters[k] = 0.;
      errors[k]     = 0.;
      pulls[k]      = 0.;
      gloConst[k]   = 0.;
   }
   
   Double_t trackParams[nLocPar*2];
   for(Int_t k = 0; k < nLocPar*2; ++k)
      trackParams[k] = 0.;
   
   fAGRoot->AddRequiredItem(fInfile);
   if (fInfile->Open(fFileName.Data())) return;
   
   CreateHistogram();
   
   fAGRoot->BeginEventLoop();
   fAGRoot->Print();
   
   // init millepede
   Init(nParam, nLocPar, stdDev, nSensors);
   SetIterations(20);
   
   for(Int_t k = 0; k < nSensors; ++k) {
      if (fDevStatus[k] == -1) continue;
      if (fDevStatus[k] == 1) {
         FixSensor(k);
      } else {
         FixParameter(nParSensors*k+0, 0.08);
         FixParameter(nParSensors*k+1, 0.08);
         FixParameter(nParSensors*k+2, 0.001);
      }
   }
   
   InitGlobalParameters(parameters);
   AddConstraint(gloConst, 0.0);
   
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
         
      } // end loop on tracks
      
   }
   
   GlobalFit(parameters,errors,pulls);
   UpdateAlignmentParams(parameters);
   
   UpdateGeoMaps();
   
   fInfile->Reset();

   // Now display the filled histos
   
//   TCanvas* canvas1 = new TCanvas("canvas1", "ResidualX");
//   canvas1->Divide(fSecArray.GetSize()/2,3);
//   gStyle->SetOptStat(1111);
//
//   TCanvas* canvas2 = new TCanvas("canvas2", "ResidualY");
//   canvas2->Divide(fSecArray.GetSize()/2,3);
//   gStyle->SetOptStat(1111);
//
//   TCanvas* canvas3 = new TCanvas("canvas3", "ResidualTotX");
//   gStyle->SetOptStat(1111);
//
//   TCanvas* canvas4 = new TCanvas("canvas4", "ResidualTotY");
//   gStyle->SetOptStat(1111);
//
//
//   for(Int_t i = 0; i < fSecArray.GetSize(); i++) {
//      canvas1->cd(i+1);
//      fpResXC[i]->Draw();
//
//      canvas2->cd(i+1);
//      fpResYC[i]->Draw();
//   }
//
//   canvas3->cd();
//   fpResTotXC->Draw();
//
//   canvas4->cd();
//   fpResTotYC->Draw();
   
}

//_________________________________________________________________
void TAIRalignM::Init(Int_t nGlobal,  /* number of global paramers */
                      Int_t nLocal,   /* number of local parameters */
                      Int_t nStdDev,  /* std dev cut */
                      Int_t nSensors   /* number of Sensors */)
{
   /// Initialization of AliMillepede. Fix parameters, define constraints ...
   fMillepede->InitMille(nGlobal,nLocal,nStdDev,fResCut,fResCutInitial);
   fNGlobal   = nGlobal;
   fNLocal    = nLocal;
   fNStdDev   = nStdDev;
   fNSensors   = nSensors;
   fNParSensor = nGlobal/nSensors;
   
   // Set iterations
   if (fStartFac > 1) fMillepede->SetIterations(fStartFac);
   
   printf("fNSensors: %i\t fNGlobal: %i\t fNLocal: %i\n", fNSensors, fNGlobal, fNLocal);
   
   ResetLocalEquation();
   printf("Parameters initialized to zero\n");
   
}

//_________________________________________________________________
void TAIRalignM::SetNonLinear(Bool_t *lChOnOff,Bool_t *lVarXYT)
{
   /// Set non linear parameter flag selected sensors and degrees of freedom
   for (Int_t i = 0; i < fNSensors; i++){
      if (lChOnOff[i]){
         if (lVarXYT[0]) { // X constraint
            SetNonLinear(i*fNParSensor+0);
         }
         if (lVarXYT[1]) { // Y constraint
            SetNonLinear(i*fNParSensor+1);
         }
         if (lVarXYT[2]) { // T constraint
            SetNonLinear(i*fNParSensor+2);
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
void TAIRalignM::FixSensor(Int_t iSensor)
{
   /// Fix all detection elements of sensor
   FixParameter(iSensor*fNParSensor+0, 0.0);
   FixParameter(iSensor*fNParSensor+1, 0.0);
   FixParameter(iSensor*fNParSensor+2, 0.0);
}

//_________________________________________________________________
void TAIRalignM::FixParameter(Int_t iPar, Double_t value)
{
   /// Parameter iPar is encourage to vary in [-value;value].
   /// If value == 0, parameter is fixed
   fMillepede->SetParSigma(iPar, value);
   if (fDebugLevel> 1)
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
void TAIRalignM::AllowVariations(Bool_t *bSensorOnOff)
{
   /// Set allowed variation for selected sensors based on fDoF and fAllowVar
   for (Int_t iSensor = 0; iSensor < fNSensors; iSensor++) {
      if (bSensorOnOff[iSensor]) {
         for (int i=0; i<fNParSensor; i++) {
            if (fDebugLevel)
               printf("fDoF[%d]= %d",i,fDoF[i]);
            if (fDoF[i]) {
               FixParameter(iSensor*fNParSensor+i, fAllowVar[i]);
               
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
      SetGlobalDerivative(planeNumber*fNParSensor+0, 0.);
      SetGlobalDerivative(planeNumber*fNParSensor+1, 0.);
      SetGlobalDerivative(planeNumber*fNParSensor+2, 0.);
   } else {
      SetGlobalDerivative(planeNumber*fNParSensor+0, 1.);
      SetGlobalDerivative(planeNumber*fNParSensor+1, 0.);
      SetGlobalDerivative(planeNumber*fNParSensor+2, -(param[4]+param[6]*pos(2)));
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
      SetGlobalDerivative(planeNumber*fNParSensor+0, 0.);
      SetGlobalDerivative(planeNumber*fNParSensor+1, 0.);
      SetGlobalDerivative(planeNumber*fNParSensor+2, 0.);
   } else {
      SetGlobalDerivative(planeNumber*fNParSensor+0, 0.);
      SetGlobalDerivative(planeNumber*fNParSensor+1, 1.);
      SetGlobalDerivative(planeNumber*fNParSensor+2, -(param[0]+param[2]*pos(2)));
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
   for (int i = 0; i < fNSensors; i++){
      printf("%d\t %5.0f (%5.0f)mu\t %5.0f (%5.0f)mu\t %5.2f (%5.2f) deg\n",i+1,
             parameters[i*fNParSensor+0]*TAGgeoTrafo::CmToMu(), errors[i*fNParSensor+0]*TAGgeoTrafo::CmToMu(),
             parameters[i*fNParSensor+1]*TAGgeoTrafo::CmToMu(), errors[i*fNParSensor+1]*TAGgeoTrafo::CmToMu(),
             parameters[i*fNParSensor+2]*TMath::RadToDeg(),     errors[i*fNParSensor+2]*TMath::RadToDeg());
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
// The final vector containing the alignment parameters are calculated
void TAIRalignM::UpdateAlignmentParams(Double_t* parameters)
{
   TAITparGeo* pGeoMap  = (TAITparGeo*) fpGeoMapItr->Object();
   
   for (Int_t i = 0; i < fSecArray.GetSize(); i++){
      Int_t iSensor = fSecArray[i];
      if (fDevStatus[i] != 2) continue; //only IT

      pGeoMap->GetSensorPar(iSensor).AlignmentU = parameters[i*fNParSensor + 0];
      pGeoMap->GetSensorPar(iSensor).AlignmentV = parameters[i*fNParSensor + 1];
      pGeoMap->GetSensorPar(iSensor).TiltW      = parameters[i*fNParSensor + 2];
   }
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
// Modification of the geomap file with the final results of alignment for each Sensor
void TAIRalignM::UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx)
{
   Char_t tmp[255];
   TString key;
   TAITparGeo* pGeoMap  = (TAITparGeo*)fpGeoMapItr->Object();
   
   Float_t alignU = pGeoMap->GetSensorPar(idx).AlignmentU*TAGgeoTrafo::CmToMu();
   Float_t alignV = pGeoMap->GetSensorPar(idx).AlignmentV*TAGgeoTrafo::CmToMu();
   Float_t tiltW  = pGeoMap->GetSensorPar(idx).TiltW*TMath::RadToDeg();
   
   while (!fileIn.eof()) {
      fileIn.getline(tmp, 255);
      key = tmp;
      if (key.Contains("AlignementU:")) {
         key = "AlignementU:";
         TString sIdx;
         sIdx = Form("      %.1f\n", alignU);
         TString line = key+sIdx;
         fileOut << line.Data();
         fileIn.getline(tmp, 255);
         key = "AlignementV:";
         sIdx = Form("      %.1f\n", alignV);
         line = key + sIdx;
         fileOut << line.Data();
         fileIn.getline(tmp, 255);
         key = "AlignementTilt:";
         sIdx = Form("   %.1f\n", tiltW);
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

