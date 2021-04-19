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

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

#include "TAGparGeo.hxx"

#include "TASTparMap.hxx"
#include "TASTntuRaw.hxx"
#include "TASTactDatRaw.hxx"

#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparConf.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactNtuHit.hxx"
#include "TABMactNtuTrack.hxx"

#include "TAGrecoManager.hxx"

#endif

// main
TAGactTreeWriter* outFile   = 0x0;

TAGactDaqReader*  daqActReader = 0x0;
TASTactDatRaw* stActDatRaw  = 0x0;
TABMactNtuRaw* bmActDatRaw  = 0x0;
TABMactNtuHit* bmActNtuRaw  = 0x0;
TABMactNtuTrack* bmActTrack = 0x0;

int GetRunNumber(TString name){
   // Done by hand shoud be given by DAQ header
   if (name.IsNull()) return 0;
   // protection about file name starting with .
   if (name[0] == '.')
      name.Remove(0,1);
   Int_t pos1   = name.First(".");
   Int_t len    = name.Length();
   TString tmp1 = name(pos1+1, len);
   Int_t pos2   = tmp1.First(".");
   TString tmp  = tmp1(0, pos2);
   return tmp.Atoi();
 }

void FillBm(TString fExpName) {
   
   cout<<"start FillBm"<<endl;
   
   //parameters
   TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
   parGeo->FromFile();
     
   TAGparaDsc* bmGeo    = new TAGparaDsc("bmGeo", new TABMparGeo());
   TABMparGeo* bmgeomap   = (TABMparGeo*) bmGeo->Object();
   TString parFileName = "./geomaps/TABMdetector.geo";
   bmgeomap->FromFile(parFileName.Data());

   TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
   TABMparConf* parConf = (TABMparConf*)bmConf->Object();
   parFileName = "./config/TABMdetector.cfg";
   parConf->FromFile(parFileName.Data());
   parFileName = Form("./config/%sTABM_T0_Calibration.cal", fExpName.Data());
   parConf->loadT0s(parFileName);
   
   TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
   TABMparMap*  bmparMap = (TABMparMap*)bmMap->Object();
   parFileName = Form("./config/%sTABMdetector.map", fExpName.Data());
   bmparMap->FromFile(parFileName.Data(), bmgeomap);
   
   TAGparaDsc* fpParTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
   TASTparTime* parTimeSt = (TASTparTime*) fpParTimeSt->Object();
   parTimeSt->FromFile("GSI", 2190);
  
   TAGparaDsc*  stMap  = new TAGparaDsc("stMap", new TASTparMap());
   TASTparMap*  stparMap = (TASTparMap*)stMap->Object();
   parFileName="./config/TASTdetector.cfg";
   stparMap->FromFile(parFileName.Data());
  
  //TAGdataDsc
   TAGdataDsc* bmDaq    = new TAGdataDsc("bmDaq", new TAGdaqEvent());
   daqActReader  = new TAGactDaqReader("daqActReader", bmDaq); 
  
   TAGdataDsc* stDatRaw    = new TAGdataDsc("stDat", new TASTntuRaw());
   stActDatRaw  = new TASTactDatRaw("stActDatRaw", stDatRaw,bmDaq,stMap, fpParTimeSt);
   
   TAGdataDsc* bmDatRaw    = new TAGdataDsc("bmDat", new TABMntuRaw());
   bmActDatRaw  = new TABMactNtuRaw("bmActDatRaw", bmDatRaw, bmDaq, bmMap, bmConf, bmGeo,stDatRaw);

   TAGdataDsc* bmNtuRaw    = new TAGdataDsc("bmNtuRaw", new TABMntuHit());
   bmActNtuRaw  = new TABMactNtuHit("bmActNtuRaw", bmNtuRaw, bmDatRaw, bmGeo, bmConf);
   bmActNtuRaw->CreateHistogram();   
   
   TAGdataDsc* bmTrack = new TAGdataDsc("bmTrack", new TABMntuTrack());
   bmActTrack  = new TABMactNtuTrack("bmActTrack", bmTrack, bmNtuRaw, bmGeo, bmConf, tgGeo);
   bmActTrack->CreateHistogram();

   cout<<"end of FillBm"<<endl;

   outFile->SetupElementBranch(bmDatRaw, TABMntuRaw::GetBranchName());
   outFile->SetupElementBranch(bmNtuRaw, TABMntuHit::GetBranchName());
   outFile->SetupElementBranch(bmTrack, TABMntuTrack::GetBranchName());
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


//~ void ReadBmRaw(TString name = "data_test.00001462.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data")
//~ void ReadBmRaw(TString name = "data/GSI_electronic/DataGSI_match/data_built.2212.physics_foot.daq.VTX.1.dat")
void ReadBmRaw(TString name = "data/GSI_electronic/DataGSI_match/data_built.2242.physics_foot.daq.VTX.1.dat")
{  
   Int_t maxevents=1000;
   TString fExpName="GSI/";
   TAGrecoManager::Instance();
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();

   TAGroot tagr;
   TAGgeoTrafo geoTrafo;
   TString parFileName = Form("./geomaps/%sFOOT.geo", fExpName.Data());
   geoTrafo.FromFile(parFileName);


   tagr.SetCampaignNumber(1);
   tagr.SetRunNumber(GetRunNumber(name));

   outFile = new TAGactTreeWriter("outFile");
   FillBm(fExpName);
   daqActReader->Open(name);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(stActDatRaw);
   tagr.AddRequiredItem(bmActDatRaw);
   tagr.AddRequiredItem(bmActNtuRaw);
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
   bmActDatRaw->SetHistogramDir(outFile->File());
   bmActNtuRaw->SetHistogramDir(outFile->File());
   bmActTrack->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   Booking(f_out);
   
   TABMntuRaw* pbmdatraw;
   TABMntuHit* pbmnturaw;
   TABMntuTrack* pbmntutrack;
   TABMntuTrackTr* pbmntutracktr;   
   
   TABMparConf* bmcon = (TABMparConf*) (tagr.FindParaDsc("bmConf", "TABMparConf")->Object()); 
   TABMparGeo* bmgeo = (TABMparGeo*) (tagr.FindParaDsc("bmGeo", "TABMparGeo")->Object());
   TABMparMap* bmmap = (TABMparMap*) (tagr.FindParaDsc("bmMap", "TABMparMap")->Object());  
       
   TVector3 bm_project;    
   for (Long64_t ientry = 0; ientry < maxevents; ientry++) {
     cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
     if(!tagr.NextEvent()) 
       break; 
     pbmnturaw = (TABMntuHit*) (tagr.FindDataDsc("bmNtuRaw", "TABMntuHit")->Object());
     pbmntutrack = (TABMntuTrack*) (tagr.FindDataDsc("bmTrack", "TABMntuTrack")->Object());
     pbmntutracktr=pbmntutrack->Track(0);
     ((TH1D*)(f_out->Get("bm_tracknum")))->Fill(pbmntutrack->GetTracksN());
     if(pbmntutrack->GetTracksN()==1){
       bm_project=bmgeo->ProjectFromPversR0(pbmntutracktr->GetPvers(), pbmntutracktr->GetR0(),bmgeo->GetMylar1().Z());
       ((TH2D*)(f_out->Get("BM_mylar1_bmsys")))->Fill(bm_project.X(),bm_project.Y());
       bm_project=geoTrafo.FromBMLocalToGlobal(bm_project);
       ((TH2D*)(f_out->Get("BM_mylar1_glbsys")))->Fill(bm_project.X(),bm_project.Y());
       bm_project=bmgeo->ProjectFromPversR0(pbmntutracktr->GetPvers(), pbmntutracktr->GetR0(),bmgeo->GetMylar2().Z());
       ((TH2D*)(f_out->Get("BM_mylar2_bmsys")))->Fill(bm_project.X(),bm_project.Y());
       ((TH1D*)(f_out->Get("BM_mylar2Z_bmsys")))->Fill(bm_project.Z());   
       bm_project=geoTrafo.FromBMLocalToGlobal(bm_project);
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


