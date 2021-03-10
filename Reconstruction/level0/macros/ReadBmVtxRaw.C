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
#include "TASTdatRaw.hxx"
#include "TASTactDatRaw.hxx"

#include "TAVTparMap.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTactNtuHit.hxx"
#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuTrackF.hxx"

#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparConf.hxx"
#include "TABMdatRaw.hxx"
#include "TABMntuRaw.hxx"
//~ #include "TABMntuHit.hxx"
#include "TABMntuTrack.hxx"
//~ #include "TABMntuTrackTr.hxx"
#include "TABMactDatRaw.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactNtuTrack.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGactTreeWriter* outFile   = 0x0;

TAGactDaqReader*  daqActReader = 0x0;

TASTactDatRaw* stActDatRaw  = 0x0;

TABMactDatRaw* bmActDatRaw  = 0x0;
TABMactNtuRaw* bmActNtuRaw  = 0x0;
TABMactNtuTrack* bmActTrack = 0x0;

TAVTactNtuHit*      vtActRaw  = 0x0;
TAVTactNtuClusterF* vtActClus = 0x0;
TAVTactNtuTrackF*   vtActTrck = 0x0;

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
  TString  parFileName = "./geomaps/TABMdetector.geo";
  bmgeomap->FromFile(parFileName.Data());

  TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
  TABMparConf* parConf = (TABMparConf*)bmConf->Object();
  parFileName = "./config/TABMdetector.cfg";
  parConf->FromFile(parFileName.Data());
    
  //~ parFileName = "./config/T0evaluation_data_built.2242.physics_foot.daq.VTX.1.dat.cfg";
  parFileName = "./config/T0evaluation_data_built.2211.physics_foot.daq.VTX.1.dat.cfg";
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
 
  TAGparaDsc* vtMap    = new TAGparaDsc("vtMap", new TAVTparMap());
  TAVTparMap* map   = (TAVTparMap*) vtMap->Object();
  map->FromFile("./config/TAVTdetector.map");
  
  TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
  TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
  geomap->FromFile("./geomaps/TAVTdetector.geo");
  
  TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
  TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
  parconf->FromFile("./config/TAVTdetector.cfg");
  TAVTparConf::SetHistoMap();
  
 
 
  //TAGdataDsc
  TAGdataDsc* bmDaq    = new TAGdataDsc("bmDaq", new TAGdaqEvent());
  daqActReader  = new TAGactDaqReader("daqActReader", bmDaq); 
 
  TAGdataDsc* stDatRaw    = new TAGdataDsc("stDat", new TASTdatRaw());
  stActDatRaw  = new TASTactDatRaw("stActDatRaw", stDatRaw,bmDaq,stMap, fpParTimeSt);
  
  TAGdataDsc* bmDatRaw    = new TAGdataDsc("bmDat", new TABMdatRaw());
  bmActDatRaw  = new TABMactDatRaw("bmActDatRaw", bmDatRaw, bmDaq, bmMap, bmConf, bmGeo,stDatRaw);
  bmActDatRaw->CreateHistogram();

  TAGdataDsc* bmNtuRaw    = new TAGdataDsc("bmNtuRaw", new TABMntuRaw());
  bmActNtuRaw  = new TABMactNtuRaw("bmActNtuRaw", bmNtuRaw, bmDatRaw, bmGeo, bmConf);
  bmActNtuRaw->CreateHistogram();   
  
  TAGdataDsc* bmTrack = new TAGdataDsc("bmTrack", new TABMntuTrack());
  bmActTrack  = new TABMactNtuTrack("bmActTrack", bmTrack, bmNtuRaw, bmGeo, bmConf, tgGeo);
  bmActTrack->CreateHistogram();

  TAGdataDsc* vtNtu    = new TAGdataDsc("vtNtu", new TAVTntuHit());
  vtActRaw  = new TAVTactNtuHit("vtActRaw", vtNtu, bmDaq, vtGeo, vtConf, vtMap);
  vtActRaw->CreateHistogram();
  
  TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
  vtActClus =  new TAVTactNtuClusterF("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
  vtActClus->CreateHistogram();
  
  TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());
  vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
  vtActTrck->CreateHistogram();
  
  cout<<"end of FillBm"<<endl;

  outFile->SetupElementBranch(bmDatRaw, TABMdatRaw::GetBranchName());
  outFile->SetupElementBranch(bmNtuRaw, TABMntuRaw::GetBranchName());
  outFile->SetupElementBranch(bmTrack, TABMntuTrack::GetBranchName());
  outFile->SetupElementBranch(vtNtu, "vtrh.");
  outFile->SetupElementBranch(vtClus, "vtclus.");
  outFile->SetupElementBranch(vtTrck, "vtTrack.");
}

void Booking(TFile *f_out){
  //~ outFile->File()->cd();
  f_out->cd();
  TH1D* h;
  TH2D* h2;
  h=new TH1D("bm_tracknum","Number of bm tracks;Number of bm tracks; Number of events",3,0.,2.);
  h=new TH1D("vtx_tracknum","Number of vtx tracks;Number of tracks; Number of events",10,0.,10.);
  h=new TH1D("Combo_bm_r0x_vtxref","bm r0x projected in vtx system of ref ;r0x; Number of events",1000,-5.,5.);
  h=new TH1D("Combo_vtx_tracknum","Number of vtx tracks in combined events;Number of tracks; Number of events",10,0.,10.);
  h2=new TH2D("Combo_vtx_bm_r0x","Bm r0x vs vtx r0x;BM r0x [cm]; VTX r0x [cm]",100,-5.,5.,100,-5.,5.);
  h2=new TH2D("Combo_vtx_bm_r0y","Bm r0y vs vtx r0y;BM r0y [cm]; VTX r0y [cm]",100,-5.,5.,100,-5.,5.);
  h2=new TH2D("Combo_vtx_bm_mx","mx Bm vs vtx ;BM mx; VTX my",1000,-0.5,0.5,1000,-0.5,0.5);
  h2=new TH2D("Combo_vtx_bm_my","mx Bm vs vtx r0y;BM mx; VTX my",1000,-0.5,0.5,1000,-0.5,0.5);
  h=new TH1D("Combo_vtx_bm_r0x_xevt","Bm r0x - vtx r0x x event;Number of events; BMr0x-VTXr0x",100000,0.,100000.);
  h=new TH1D("Combo_vtx_bm_r0y_xevt","Bm r0y - vtx r0y x event;Number of events; BMr0y-VTXr0y",100000,0.,100000.);
  h=new TH1D("Combo_vtx_bm_mx_xevt","BMmx-VTXmx x event;Number of events; BMmx-VTXmx",100000,0.,100000.);
  h=new TH1D("Combo_vtx_bm_my_xevt","BMmx-VTXmx x event;Number of events; BMmy-VTXmy",100000,0.,100000.);
  h2=new TH2D("Combo_VTX_r0xy","Projections of VTX tracks in vtx sys;x[cm]; y[cm]",100,-5.,5.,100,-5.,5.);
  h2=new TH2D("BM_mylar1_bmsys","Projections of BM tracks on mylar1 in BM sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);
  h2=new TH2D("BM_mylar2_bmsys","Projections of BM tracks on mylar2 in BM sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);
  h2=new TH2D("BM_mylar1_glbsys","Projections of BM tracks on mylar1 in global sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);
  h2=new TH2D("BM_mylar2_glbsys","Projections of BM tracks on mylar2 in global sys;x[cm]; y[cm]",1000,-5.,5.,1000,-5.,5.);

  return;
}

//~ void ReadBmVtxRaw(TString name = "data_test.00001462.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data")
void ReadBmVtxRaw(TString name = "data/GSI_electronic/DataGSI_match/data_built.2211.physics_foot.daq.VTX.1.dat")
//~ void ReadBmVtxRaw(TString name = "data/GSI_electronic/DataGSI_match/data_built.2242.physics_foot.daq.VTX.1.dat")
{  
  Int_t maxevents=999999999999;
  TString fExpName="GSI/";
  GlobalPar::Instance();
  GlobalPar::GetPar()->Print();

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
  tagr.AddRequiredItem(vtActRaw);
  tagr.AddRequiredItem(vtActClus);
  tagr.AddRequiredItem(vtActTrck);
  tagr.AddRequiredItem(outFile);
  tagr.Print();
  
  TString nameOut = name(name.Last('/')+1, name.Last('.'));
  nameOut.Append("_readbmvtxraw_Out.root");
  TString myfileoutname=name(name.Last('/')+1, name.Last('.'));
  myfileoutname.Prepend("my_");
  myfileoutname.Append("_myreadbmvtxraw_Out.root");
  TFile *f_out = new TFile(myfileoutname.Data(),"RECREATE"); 
  
  if (outFile->Open(nameOut.Data(), "RECREATE")) return;
  bmActDatRaw->SetHistogramDir(outFile->File());
  bmActNtuRaw->SetHistogramDir(outFile->File());
  bmActTrack->SetHistogramDir(outFile->File());
  vtActRaw->SetHistogramDir(outFile->File());
  vtActClus->SetHistogramDir(outFile->File());
  vtActTrck->SetHistogramDir(outFile->File());
  
  cout<<" Beginning the Event Loop "<<endl;
  tagr.BeginEventLoop();
  TStopwatch watch;
  watch.Start();
  
  Booking(f_out);
  
  TABMdatRaw* pbmdatraw;
  TABMntuRaw* pbmnturaw;
  TABMntuTrack* pbmntutrack;
  TABMntuTrackTr* pbmntutracktr;
  
  TAVTntuTrack* pvtxntutrack;
  TAVTtrack* pvtxtrack;
    
  TABMparConf* bmcon = (TABMparConf*) (tagr.FindParaDsc("bmConf", "TABMparConf")->Object()); 
  TABMparGeo* bmgeo = (TABMparGeo*) (tagr.FindParaDsc("bmGeo", "TABMparGeo")->Object());
  TABMparMap* bmmap = (TABMparMap*) (tagr.FindParaDsc("bmMap", "TABMparMap")->Object());   
  
  double vtx_bestchi2;
  int vtx_best_chi2_index, vtx_best_res_index;
  TVector3 bm_project;
  for (Long64_t ientry = 0; ientry < maxevents; ientry++) {
    cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
    //~ outFile->File()->cd();
    if(!tagr.NextEvent()) 
      break; 
    //~ pbmdatraw = (TABMdatRaw*) (tagr.FindDataDsc(TABMdatRaw::GetBranchName(), "TABMdatRaw")->Object());
    pbmnturaw = (TABMntuRaw*) (tagr.FindDataDsc("bmNtuRaw", "TABMntuRaw")->Object());
    pbmntutrack = (TABMntuTrack*) (tagr.FindDataDsc("bmTrack", "TABMntuTrack")->Object());
    pvtxntutrack = (TAVTntuTrack*) (tagr.FindDataDsc("vtTrck", "TAVTntuTrack")->Object());
    //~ for(int i=0;i<pbmnturaw->GetHitsN();i++){
      //~ const TABMntuHit* pbmntuhit=pbmnturaw->Hit(i);  
    //~ }
    ((TH1D*)(f_out->Get("bm_tracknum")))->Fill(pbmntutrack->GetTracksN());
    vtx_bestchi2=1e7;
    vtx_best_chi2_index=0;
    if(pbmntutrack->GetTracksN()==1){
      ((TH1D*)(f_out->Get("vtx_tracknum")))->Fill(pvtxntutrack->GetTracksN());
      for(int i=0;i<pvtxntutrack->GetTracksN();i++){//select the best vtx track
        pvtxtrack=pvtxntutrack->GetTrack(i);
        if(pvtxtrack->GetChi2()<vtx_bestchi2){
          vtx_best_chi2_index=i;
          vtx_bestchi2=pvtxtrack->GetChi2();
        }
      }
      
      //at the moment the BM has only 1 track
      pbmntutracktr=pbmntutrack->Track(0);
      if(vtx_bestchi2<1000.){//select the vtx and bm tracks performing some cuts (to be studied)
        pvtxtrack=pvtxntutrack->GetTrack(vtx_best_chi2_index);
        ((TH1D*)(f_out->Get("Combo_vtx_tracknum")))->Fill(pvtxntutrack->GetTracksN());
        ((TH2D*)(f_out->Get("Combo_vtx_bm_r0x")))->Fill(pbmntutracktr->GetR0().X(),pvtxtrack->GetOrigin().X());
        ((TH2D*)(f_out->Get("Combo_vtx_bm_r0y")))->Fill(pbmntutracktr->GetR0().Y(),pvtxtrack->GetOrigin().Y());
        ((TH2D*)(f_out->Get("Combo_vtx_bm_mx")))->Fill(pbmntutracktr->GetPvers().X()/pbmntutracktr->GetPvers().Z(),pvtxtrack->.GetSlopeZ().X()/pvtxtrack->.GetSlopeZ().Z());
        ((TH2D*)(f_out->Get("Combo_vtx_bm_my")))->Fill(pbmntutracktr->GetPvers().Y()/pbmntutracktr->GetPvers().Z(),pvtxtrack->.GetSlopeZ().Y()/pvtxtrack->.GetSlopeZ().Z());
        ((TH2D*)(f_out->Get("Combo_VTX_r0xy")))->Fill(pvtxtrack->.GetOrigin().X(),pvtxtrack->.GetOrigin().Y());
        if(ientry<((TH1D*)(f_out->Get("Combo_vtx_bm_r0x_xevt")))->GetXaxis()->GetXmax()){
          ((TH1D*)(f_out->Get("Combo_vtx_bm_r0x_xevt")))->SetBinContent(ientry, pbmntutracktr->GetR0().X()-pvtxtrack->.GetOrigin().X());
          ((TH1D*)(f_out->Get("Combo_vtx_bm_r0y_xevt")))->SetBinContent(ientry, pbmntutracktr->GetR0().Y()-pvtxtrack->.GetOrigin().Y());
          ((TH1D*)(f_out->Get("Combo_vtx_bm_mx_xevt")))->SetBinContent(ientry, pbmntutracktr->GetPvers().X()/pbmntutracktr->GetPvers().Z()-pvtxtrack->.GetSlopeZ().X()/pvtxtrack->.GetSlopeZ().Z());
          ((TH1D*)(f_out->Get("Combo_vtx_bm_my_xevt")))->SetBinContent(ientry,pbmntutracktr->GetPvers().Y()/pbmntutracktr->GetPvers().Z()-pvtxtrack->.GetSlopeZ().Y()/pvtxtrack->.GetSlopeZ().Z());
        }
      }//end of bm+vtx combo events
      bm_project=bmgeo->ProjectFromPversR0(pbmntutracktr->GetPvers(), pbmntutracktr->GetR0(),bmgeo->GetMylar1().Z());
      ((TH2D*)(f_out->Get("BM_mylar1_bmsys")))->Fill(bm_project.X(),bm_project.Y());
      bm_project=geoTrafo.FromBMLocalToGlobal(bm_project);
      ((TH2D*)(f_out->Get("BM_mylar1_glbsys")))->Fill(bm_project.X(),bm_project.Y());
      bm_project=bmgeo->ProjectFromPversR0(pbmntutracktr->GetPvers(), pbmntutracktr->GetR0(),bmgeo->GetMylar2().Z());
      ((TH2D*)(f_out->Get("BM_mylar2_bmsys")))->Fill(bm_project.X(),bm_project.Y());
      bm_project=geoTrafo.FromBMLocalToGlobal(bm_project);
      ((TH2D*)(f_out->Get("BM_mylar2_glbsys")))->Fill(bm_project.X(),bm_project.Y());
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


