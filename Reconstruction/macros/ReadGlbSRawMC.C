// Macro to make Glb track reconstruction when no magnetic field from MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>

#include "TAGrecoManager.hxx"

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

//ST
#include "TASTntuHit.hxx"

// VTX
#include "TAGparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparMap.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGactTreeReader.hxx"
#include "TAVTactNtuHitMC.hxx"
#include "TAVTactNtuCluster.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"
#include "TAVTactNtuVertex.hxx"

//ITR
#include "TAITparGeo.hxx"
#include "TAITparMap.hxx"
#include "TAITparConf.hxx"
#include "TAITntuHit.hxx"
#include "TAITntuCluster.hxx"

#include "TAITactNtuHitMC.hxx"
#include "TAITactNtuCluster.hxx"

//MSD
#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"

#include "TAMSDactNtuHitMC.hxx"
#include "TAMSDactNtuCluster.hxx"

//TW
#include "TATWparGeo.hxx"
#include "TATWparConf.hxx"
#include "TATWparCal.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"

#include "TATWactNtuHitMC.hxx"
#include "TATWactNtuPoint.hxx"

// CA
#include "TACAparGeo.hxx"
#include "TACAparCal.hxx"
#include "TACAparConf.hxx"
#include "TACAntuHit.hxx"
#include "TACAntuCluster.hxx"

#include "TACAactNtuHitMC.hxx"
#include "TACAactNtuCluster.hxx"

// GLB
#include "TAGntuGlbTrack.hxx"
#include "TAGactNtuGlbTrackS.hxx"


#endif

// main
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter*   outFile     = 0x0;
TAGactTreeReader*   vtActReader = 0x0;

// VTX
TAGparaDsc*         tgGeo       = 0x0;
TAGparaDsc*         vtGeo       = 0x0;
TAGdataDsc*         vtEve       = 0x0;
TAGdataDsc*         vtTrck      = 0x0;
TAGdataDsc*         vtVtx       = 0x0;
TAVTactNtuHitMC*    vtActNtu    = 0x0;
TAVTactNtuCluster* vtActClus   = 0x0;
TAVTactNtuTrackF*   vtActTrck   = 0x0;
TAVTactNtuVertex*   vtActVtx    = 0x0;

//ITR
TAGparaDsc*         itGeo       = 0x0;
TAGparaDsc*         itConf      = 0x0;
TAGdataDsc*         itClus      = 0x0;
TAITactNtuHitMC*    itActNtu    = 0x0;
TAITactNtuCluster* itActClus   = 0x0;

//MSD
TAGparaDsc*         msdGeo      = 0x0;
TAGdataDsc*         msdClus     = 0x0;
TAMSDactNtuHitMC*   msdActNtu   = 0x0;
TAMSDactNtuCluster* msdActClus  = 0x0;

//TOF
TAGparaDsc*         twGeo       = 0x0;
TAGdataDsc*         twRec       = 0x0;
TATWactNtuHitMC*    twActNtu    = 0x0;
TATWactNtuPoint*    twActRec    = 0x0;

//CA
TAGparaDsc*         caGeo       = 0x0;
TAGdataDsc*         caClus      = 0x0;
TACAactNtuHitMC*    caActNtu    = 0x0;
TACAactNtuCluster*  caActClus   = 0x0;


//GLB
TAGactNtuGlbTrackS*   glbActTrack = 0x0;

void FillMCVertex(Int_t runNumber) {
   
   /*Ntupling the MC Vertex information*/
   tgGeo = new TAGparaDsc("tgGeo", new TAGparGeo());
   TAGparGeo* geomapg = (TAGparGeo*)tgGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
   geomapg->FromFile(parFileName.Data());
   
   vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   parFileName = campManager->GetCurGeoFile(FootBaseName("TAVTparGeo"), runNumber);
   geomap->FromFile(parFileName.Data());
   Int_t sensorsN = geomap->GetSensorsN();

   vtEve  = new TAGdataDsc("vtEve", new TAMCntuPart());
   TAGdataDsc* vtMc   = new TAGdataDsc("vtMc", new TAMCntuHit());
   
   TAGdataDsc* vtNtu    = new TAGdataDsc("vtHit", new TAVTntuHit(sensorsN));
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster(sensorsN));
   vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());
   vtVtx    = new TAGdataDsc("vtVtx", new TAVTntuVertex());
   
   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parconf->FromFile();
   
   vtActReader  = new TAGactTreeReader("vtActEvtReader");
   vtActReader->SetupBranch(vtMc, TAMCntuHit::GetVtxBranchName());
   vtActReader->SetupBranch(vtEve,TAMCntuPart::GetBranchName());
   
   vtActNtu  = new TAVTactNtuHitMC("vtActNtu", vtMc, vtEve, vtNtu, vtGeo);
   vtActClus = new TAVTactNtuCluster("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
   vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
   vtActVtx  = new TAVTactNtuVertex("vtActVtx", vtTrck, vtVtx, vtConf, vtGeo, tgGeo);
}

void FillMCInnerTracker(Int_t runNumber) {
   
   /*Ntupling the MC Inner tracker information*/
   itGeo    = new TAGparaDsc("itGeo", new TAITparGeo());
   TAITparGeo* geomap   = (TAITparGeo*) itGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(FootBaseName("TAITparGeo"), runNumber);
   geomap->FromFile(parFileName.Data());
   Int_t sensorsN = geomap->GetSensorsN();

   TAGdataDsc* itMc     = new TAGdataDsc("itMc", new TAMCntuHit());
   TAGdataDsc* itNtu    = new TAGdataDsc("itHit", new TAITntuHit(sensorsN));
   itClus   = new TAGdataDsc("itClus", new TAITntuCluster(sensorsN));
   
   itConf  = new TAGparaDsc("itConf", new TAITparConf());
   TAITparConf* parconf = (TAITparConf*) itConf->Object();
   parconf->FromFile();
   
   vtActReader->SetupBranch(itMc, TAMCntuHit::GetItrBranchName());
   itActNtu= new TAITactNtuHitMC("itActNtu", itMc, vtEve, itNtu, itGeo);
   
   itActClus = new TAITactNtuCluster("itActClus", itNtu, itClus, itConf, itGeo);
}

void FillMCMsd(Int_t runNumber) {
   
   /*Ntupling the MC Vertex information*/
   msdGeo    = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
   TAMSDparGeo* geomap = (TAMSDparGeo*) msdGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(FootBaseName("TAMSDparGeo"), runNumber);
   geomap->FromFile(parFileName.Data());
   Int_t sensorsN = geomap->GetSensorsN();

   TAGdataDsc* msdMc     = new TAGdataDsc("msdMc", new TAMCntuHit());
   TAGdataDsc* msdNtu    = new TAGdataDsc("msdNtu", new TAMSDntuHit(sensorsN));
   msdClus   = new TAGdataDsc("msdClus", new TAMSDntuCluster(sensorsN));
   
   TAGparaDsc*  msdConf  = new TAGparaDsc("msdConf", new TAMSDparConf());
   TAMSDparConf* parconf = (TAMSDparConf*) msdConf->Object();
   
   vtActReader->SetupBranch(msdMc, TAMCntuHit::GetMsdBranchName());
   msdActNtu  = new TAMSDactNtuHitMC("msdActNtu", msdMc, vtEve, msdNtu, msdGeo);
   msdActClus = new TAMSDactNtuCluster("msdActClus", msdNtu, msdClus, msdConf, msdGeo);
}

void FillMCTw(Int_t runNumber)
{
   TAGparGeo* parGeoG = (TAGparGeo*)tgGeo->Object();
   
   Int_t Z_beam = parGeoG->GetBeamPar().AtomicNumber;
   Int_t A_beam = parGeoG->GetBeamPar().AtomicMass;
   TString ion_name = parGeoG->GetBeamPar().Material;
   Float_t kinE_beam = parGeoG->GetBeamPar().Energy; //GeV/u
   
   twGeo    = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* geomap  = (TATWparGeo*) twGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(FootBaseName("TATWparGeo"), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGparaDsc* twConf = new TAGparaDsc("twConf", new TATWparConf());
   TATWparConf* parConf = (TATWparConf*)twConf->Object();
   parFileName = campManager->GetCurConfFile(FootBaseName("TATWparGeo"), runNumber);
   parConf->FromFile(parFileName.Data());
      
   TAGparaDsc* twCal   = new TAGparaDsc("twCal", new TATWparCal());
   TATWparCal* parCal   = (TATWparCal*) twCal->Object();
   parFileName = campManager->GetCurMapFile(FootBaseName("TATWparGeo"), runNumber);
   parCal->FromBarStatusFile(parFileName.Data());
   
   Bool_t isTof_calib = false;
   parFileName = campManager->GetCurCalFile(FootBaseName("TATWparGeo"), runNumber,
                                            isTof_calib,false);
   parCal->FromCalibFile(parFileName.Data(),isTof_calib,false);
   
   isTof_calib = true;
   parFileName = campManager->GetCurCalFile(FootBaseName("TATWparGeo"), runNumber,
                                            isTof_calib,false);
   parCal->FromCalibFile(parFileName.Data(),isTof_calib,false);
   
   parFileName = campManager->GetCurConfFile(FootBaseName("TATWparGeo"), runNumber,
                                             Form("%d%s", A_beam,ion_name.Data()),
                                             (int)(kinE_beam*TAGgeoTrafo::GevToMev()));
   parCal->FromFileZID(parFileName.Data(),Z_beam);
   
   TAGdataDsc* stMc  = new TAGdataDsc("stMc", new TAMCntuHit());
   TAGdataDsc* twMc  = new TAGdataDsc("twMc", new TAMCntuHit());
   TAGdataDsc* stNtu = new TAGdataDsc("stNtu", new TASTntuHit());
   TAGdataDsc* twNtu = new TAGdataDsc("twNtu", new TATWntuHit());
   twRec = new TAGdataDsc("twPoint", new TATWntuPoint());
   
   vtActReader->SetupBranch(stMc, TAMCntuHit::GetStcBranchName());
   vtActReader->SetupBranch(twMc, TAMCntuHit::GetTofBranchName());
   twActNtu  = new TATWactNtuHitMC("twActNtu", twMc, stMc, stNtu, vtEve, twNtu, twConf, twCal, tgGeo);
   twActRec  = new TATWactNtuPoint("twActRec", twNtu, twRec, twGeo, twCal);
}

void FillMCCa(Int_t runNumber)
{
   /*Ntupling the MC Vertex information*/
   caGeo    = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
   TACAparGeo* geomap   = (TACAparGeo*) caGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(FootBaseName("TACAparGeo"), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGparaDsc* caConf = new TAGparaDsc("caConf", new TACAparConf());
   TACAparConf* parConf = (TACAparConf*)caConf->Object();
   parFileName = campManager->GetCurConfFile(FootBaseName("TACAparGeo"), runNumber);
   parConf->FromFile(parFileName.Data());
   
   TAGparaDsc* caCal  = new TAGparaDsc("caCal", new TACAparCal());
   TACAparCal* parCal = (TACAparCal*)caCal->Object();
   parFileName = campManager->GetCurMapFile(FootBaseName("TACAparGeo"), runNumber);
   parCal->FromCrysStatusFile(parFileName.Data());
   
   parFileName = campManager->GetCurCalFile(FootBaseName("TACAparGeo"), runNumber);
   parCal->LoadEnergyCalibrationMap(parFileName.Data());
   
   TAGdataDsc* caMc     = new TAGdataDsc("caMc", new TAMCntuHit());
   TAGdataDsc* caNtu    = new TAGdataDsc("caNtu", new TACAntuHit());
   caClus   = new TAGdataDsc("caClus", new TACAntuCluster());
   
   vtActReader->SetupBranch(caMc, TAMCntuHit::GetCalBranchName());
   caActNtu  = new TACAactNtuHitMC("caActNtu", caMc, vtEve, caNtu, caGeo, caCal, tgGeo);
   caActClus = new TACAactNtuCluster("caActClus", caNtu, caClus, caGeo, 0x0, 0x0, twRec);
}

void ReadGlbSRawMC(TString filename = "12C_C_200noB.root", Int_t nMaxEvts = 3, TString expName = "12C_200", Int_t runNumber = 1)
{
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   TAGroot tagr;
   
   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();
   
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(FootBaseName("TAGgeoTrafo"), runNumber);
   geoTrafo->FromFile(parFileName);
   
   TFile* f = new TFile(filename.Data());
   outFile = new TAGactTreeWriter("outFile");
   
   FillMCVertex(runNumber);
   FillMCInnerTracker(runNumber);
   FillMCMsd(runNumber);
   FillMCTw(runNumber);
   FillMCCa(runNumber);
   
   TAGdataDsc* glbTrack   = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
   glbActTrack = new TAGactNtuGlbTrackS("glbActTrack",  vtVtx, itClus, msdClus, twRec, caClus, glbTrack, vtGeo, itGeo, msdGeo, twGeo, tgGeo);
   glbActTrack->CreateHistogram();
   
   //  outFile->SetupElementBranch(glbTrack, TAGntuGlbTrack::GetBranchName());
   
   vtActReader->Open(filename, "READ", "EventTree");
   
   tagr.AddRequiredItem("vtActNtu");
   tagr.AddRequiredItem("vtActClus");
   tagr.AddRequiredItem("vtActTrck");
   tagr.AddRequiredItem("vtActVtx");
   
   tagr.AddRequiredItem("itActNtu");
   tagr.AddRequiredItem("itActClus");
   
   tagr.AddRequiredItem("msdActNtu");
   tagr.AddRequiredItem("msdActClus");
   
   tagr.AddRequiredItem("twActNtu");
   tagr.AddRequiredItem("twActRec");
   
   tagr.AddRequiredItem("glbActTrack");
   
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = filename.Last('.');
   TString nameOut = filename(0, pos);
   nameOut.Append("_Out.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   
   
   glbActTrack->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){
      
      // printf("\n");
      if (++nEvents % 100 == 0)
         printf("Event: %d\n", nEvents);
      
      if (nEvents == nMaxEvts)
         break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


