// Macro to make reconstruction from MC data
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

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

#include "TAGparGeo.hxx"

#include "TASTparGeo.hxx"
#include "TASTparMap.hxx"
#include "TATWparMap.hxx"
#include "TATWparGeo.hxx"
#include "TAGbaseWDparMap.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGactWDreader.hxx"

#include "TASTntuRaw.hxx"
#include "TASTntuHit.hxx"
#include "TASTactNtuHit.hxx"

#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparConf.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"
#include "TABMtrack.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactNtuHit.hxx"
#include "TABMactNtuTrack.hxx"

#include "TAGrecoManager.hxx"

#endif

// main
TAGcampaignManager* campManager  = 0x0;
TAGactTreeWriter* outFile   = 0x0;
TAGactDaqReader*  daqActReader = 0x0;

TASTactNtuHit* stActNtuHit  = 0x0;
TAGactWDreader* fActWdRaw  = 0x0;
TABMactNtuRaw* bmActNtuRaw  = 0x0;
TABMactNtuHit* bmActNtuHit  = 0x0;
TABMactNtuTrack* bmActTrack = 0x0;


void FillBm(Int_t runNumber) {
   
   cout<<"start FillBm"<<endl;
   
   //parameters     
   TAGparaDsc* bmGeo    = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
   TABMparGeo* bmgeomap   = (TABMparGeo*) bmGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
   bmgeomap->FromFile(parFileName.Data());

   TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
   TABMparConf* bmparConf = (TABMparConf*)bmConf->Object();
   parFileName = campManager->GetCurConfFile(TABMparGeo::GetBaseName(), runNumber);
   bmparConf->FromFile(parFileName.Data());   

   TAGparaDsc*  bmCal  = new TAGparaDsc("bmCal", new TABMparCal());
   TABMparCal* bmparCal = (TABMparCal*)bmCal->Object();
   parFileName = campManager->GetCurCalFile(TABMparGeo::GetBaseName(), runNumber);
   bmparCal->FromFile(parFileName.Data());
   
   TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
   TABMparMap*  bmparMap = (TABMparMap*)bmMap->Object();
   parFileName = campManager->GetCurMapFile(TABMparGeo::GetBaseName(), runNumber);
   bmparMap->FromFile(parFileName.Data(), bmgeomap);
   
   TAGparaDsc* fpParGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
   TASTparGeo* stparGeo = (TASTparGeo*)fpParGeoSt->Object();
   parFileName = campManager->GetCurGeoFile(TASTparGeo::GetBaseName(), runNumber);
   stparGeo->FromFile(parFileName.Data());

   TAGparaDsc* fpParMapSt = new TAGparaDsc("stMap", new TASTparMap()); 
   TASTparMap* stparMap = (TASTparMap*) fpParMapSt->Object();
   parFileName = campManager->GetCurConfFile(TASTparGeo::GetBaseName(), runNumber);
   stparMap->FromFile(parFileName);
   
   TAGparaDsc* fpParMapTw = new TAGparaDsc("twMap", new TATWparMap());
   TATWparMap* twparMap = (TATWparMap*)fpParMapTw->Object();
   parFileName = campManager->GetCurMapFile(TATWparGeo::GetBaseName(), runNumber);
   twparMap->FromFile(parFileName.Data());
   
   TAGparaDsc* fpParMapWD = new TAGparaDsc("WDMap", new TAGbaseWDparMap());
   TAGbaseWDparMap* wdparMap = (TAGbaseWDparMap*)fpParMapWD->Object();
   parFileName = campManager->GetCurMapFile(TASTparGeo::GetBaseName(), runNumber);
   wdparMap->FromFile(parFileName.Data());
   
   TAGparaDsc* fpParTimeWD = new TAGparaDsc("WDTim", new TAGbaseWDparTime());
   TAGbaseWDparTime* wdparTime = (TAGbaseWDparTime*) fpParTimeWD->Object();
   parFileName = campManager->GetCurCalFile(TASTparGeo::GetBaseName(), runNumber);
   wdparTime->FromFile(parFileName.Data());
  
  //TAGdataDsc
   TAGdataDsc* bmDaq    = new TAGdataDsc("bmDaq", new TAGdaqEvent());
   daqActReader  = new TAGactDaqReader("daqActReader", bmDaq); 

  TAGdataDsc* fpDatRawSt   = new TAGdataDsc("stDat", new TASTntuRaw());
  TAGdataDsc* fpDatRawTw   = new TAGdataDsc("twdDat", new TATWntuRaw());
  TAGdataDsc* fpDatRawCa   = new TAGdataDsc("caDat", new TACAntuRaw());
  fActWdRaw  = new TAGactWDreader("wdActRaw", bmDaq, fpDatRawSt, fpDatRawTw, fpDatRawCa, fpParMapWD, fpParTimeWD);

  TAGdataDsc* fpNtuHitSt   = new TAGdataDsc("stNtu", new TASTntuHit());
  stActNtuHit = new TASTactNtuHit("stActNtu", fpDatRawSt, fpNtuHitSt, fpParMapSt);
   
  TAGdataDsc* bmNtuRaw    = new TAGdataDsc("bmDat", new TABMntuRaw());
  bmActNtuRaw  = new TABMactNtuRaw("bmActNtuRaw", bmNtuRaw, bmDaq, bmMap, bmCal, bmGeo, fpNtuHitSt);

  TAGdataDsc* bmNtuHit    = new TAGdataDsc("bmNtuHit", new TABMntuHit());
  bmActNtuHit  = new TABMactNtuHit("bmActNtuHit", bmNtuHit, bmNtuRaw, bmGeo, bmConf, bmCal);
  bmActNtuHit->CreateHistogram();   
  
  TAGdataDsc* bmNtuTrack = new TAGdataDsc("bmNtuTrack", new TABMntuTrack());
  bmActTrack  = new TABMactNtuTrack("bmActTrack", bmNtuTrack, bmNtuHit, bmGeo, bmConf, bmCal);
  bmActTrack->CreateHistogram();

  cout<<"end of FillBm"<<endl;

  outFile->SetupElementBranch(bmNtuRaw, TABMntuRaw::GetBranchName());
  outFile->SetupElementBranch(bmNtuHit, TABMntuHit::GetBranchName());
  outFile->SetupElementBranch(bmNtuTrack, TABMntuTrack::GetBranchName());
}

void Booking(TFile *f_out){
  //~ outFile->File()->cd();
  f_out->cd();
  TH1D* h;
  TH2D* h2;
  h=new TH1D("bm_tracknum","Number of bm tracks;Number of bm tracks; Number of events",3,0.,2.);
  h2=new TH2D("BM_mylar1_bmsys","Projections of BM tracks on mylar1 in BM sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);
  h2=new TH2D("BM_mylar2_bmsys","Projections of BM tracks on mylar2 in BM sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);
  h2=new TH2D("BM_mylar1_glbsys","Projections of BM tracks on mylar1 in global sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);
  h2=new TH2D("BM_mylar2_glbsys","Projections of BM tracks on mylar2 in global sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);
  h=new TH1D("BM_mylar2Z_bmsys","BM z coordinate of mylar2 projection in bmsys;Z [cm]; Number of tracks",100,0.,20.);
  h=new TH1D("BM_mylar2Z_glbsys","BM z coordinate of mylar2 projection in glbsys;Z [cm]; Number of tracks",100,-20.,-10.);

  return;
}


void ReadBmRaw(TString name = "mydata/2019_GSI_foot/data_built.2242.physics_foot.daq.WD.1.dat", Int_t maxevents = 10000,
                TString expName = "GSI", Int_t runNumber = 2242)
{  
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();

   TAGroot tagr;

   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();

   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   geoTrafo->FromFile(parFileName);

   outFile = new TAGactTreeWriter("outFile");
   FillBm(runNumber);
   daqActReader->Open(name);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(stActNtuHit);
   tagr.AddRequiredItem(fActWdRaw);
   tagr.AddRequiredItem(bmActNtuRaw);
   tagr.AddRequiredItem(bmActNtuHit);
   tagr.AddRequiredItem(bmActTrack);
   tagr.AddRequiredItem(outFile);
   tagr.Print();
   
   TString nameOut = name(name.Last('/')+1, name.Last('.'));
   nameOut.Append("_readbmraw_Out.root");
   TString myfileoutname=name(name.Last('/')+1, name.Last('.'));
   myfileoutname.Prepend("my_");
   myfileoutname.Append("_myreadbmvtxraw_Out.root");
   TFile *f_out = new TFile(myfileoutname.Data(),"RECREATE");   
   
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   bmActNtuRaw->SetHistogramDir(outFile->File());
   bmActNtuHit->SetHistogramDir(outFile->File());
   bmActTrack->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   Booking(f_out);
   
   TABMntuRaw* pbmnturaw;
   TABMntuHit* pbmntuhit;
   TABMntuTrack* pbmntutrack;
   TABMtrack* pbmtrack;   
   
   TABMparConf* bmcon = (TABMparConf*) (tagr.FindParaDsc("bmConf", "TABMparConf")->Object()); 
   TABMparGeo* bmgeo = (TABMparGeo*) (tagr.FindParaDsc("bmGeo", "TABMparGeo")->Object());
   TABMparMap* bmmap = (TABMparMap*) (tagr.FindParaDsc("bmMap", "TABMparMap")->Object());  
   TABMparCal* bmcal = (TABMparCal*) (tagr.FindParaDsc("bmCal", "TABMparCal")->Object());  
       
   TVector3 bm_project;    
   for (Long64_t ientry = 0; ientry < maxevents; ientry++) {
     cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
     if(!tagr.NextEvent()) 
       break; 
     pbmntutrack = (TABMntuTrack*) (tagr.FindDataDsc("bmNtuTrack", "TABMntuTrack")->Object());
     ((TH1D*)(f_out->Get("bm_tracknum")))->Fill(pbmntutrack->GetTracksN());
     if(pbmntutrack->GetTracksN()==1){
       pbmtrack=pbmntutrack->GetTrack(0);
       bm_project=bmgeo->ProjectFromPversR0(pbmtrack->GetSlope(), pbmtrack->GetOrigin(),bmgeo->GetMylar1().Z());
       ((TH2D*)(f_out->Get("BM_mylar1_bmsys")))->Fill(bm_project.X(),bm_project.Y());
       bm_project=geoTrafo->FromBMLocalToGlobal(bm_project);
       ((TH2D*)(f_out->Get("BM_mylar1_glbsys")))->Fill(bm_project.X(),bm_project.Y());
       bm_project=bmgeo->ProjectFromPversR0(pbmtrack->GetSlope(), pbmtrack->GetOrigin(),bmgeo->GetMylar2().Z());
       ((TH2D*)(f_out->Get("BM_mylar2_bmsys")))->Fill(bm_project.X(),bm_project.Y());
       ((TH1D*)(f_out->Get("BM_mylar2Z_bmsys")))->Fill(bm_project.Z());   
       bm_project=geoTrafo->FromBMLocalToGlobal(bm_project);
       ((TH2D*)(f_out->Get("BM_mylar2_glbsys")))->Fill(bm_project.X(),bm_project.Y());   
       ((TH1D*)(f_out->Get("BM_mylar2Z_glbsys")))->Fill(bm_project.Z());   
     }
   }
   
   tagr.EndEventLoop();
   
  outFile->Print();
  outFile->Close();
  
  f_out->Write();
  f_out->Close();
   
   watch.Print();
   cout<<"job done!  the output file are:"<<endl<<myfileoutname.Data()<<endl<<nameOut.Data()<<endl;

}


