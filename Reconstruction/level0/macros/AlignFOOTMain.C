#include "AlignFOOTFunc.C"




// main
void AlignFOOTMain(TString nameFile = "", Int_t nentries = 0, Int_t indoalign=5, TString extname="")

{

  //Open the input file
  TTree *tree = 0;
  TFile *f = new TFile(nameFile.Data());
  if(f->IsOpen()==false){
    cout<<"FATAL ERROR: I cannot open the input file, check it!  input file="<<nameFile.Data()<<endl;
    return;
  }
  tree = (TTree*)f->Get("tree");
  if(tree==nullptr){
    cout<<"FATAL ERROR: your input file do not contains a tree, check it!  input file="<<nameFile.Data()<<endl;
    return;
  }

  doalign=indoalign;

  //define and charge varaibles and geometrical parameters
  TAGroot gTAGroot;
  runinfo=(TAGrunInfo*)(f->Get("runinfo"));
  const TAGrunInfo construninfo(*runinfo);
  gTAGroot.SetRunInfo(construninfo);
  TString expName=runinfo->CampaignName();
  if(expName.EndsWith("/")) //fix a bug present in shoe
    expName.Remove(expName.Length()-1);
  Int_t runNumber=runinfo->RunNumber();
  TAGrecoManager::Instance(expName);
  TAGcampaignManager* campManager = new TAGcampaignManager(expName);
  campManager->FromFile();

  IncludeReg=runinfo->GetGlobalPar().EnableRegionMc;
  IncludeTOE=(tree->FindBranch("glbtrack.")!=nullptr);
  IncludeDAQ=(tree->FindBranch("evt.")!=nullptr);
  IncludeMC=campManager->GetCampaignPar(campManager->GetCurrentCamNumber()).McFlag;
  IncludeDI=campManager->IsDetectorOn("DI");
  IncludeSC=campManager->IsDetectorOn("ST");
  IncludeBM=campManager->IsDetectorOn("BM");
  IncludeTG=campManager->IsDetectorOn("TG");
  IncludeVT=campManager->IsDetectorOn("VT");
  IncludeIT=campManager->IsDetectorOn("IT");
  IncludeMSD=campManager->IsDetectorOn("MSD");
  IncludeTW=campManager->IsDetectorOn("TW");
  IncludeCA=campManager->IsDetectorOn("CA");

  if(IncludeMC && IncludeDAQ){
    cout<<"IncludeMC and IncludeDAQ are both true... check your input file and the configuration files, this program will be ended"<<endl;
    return;
  }

  //define and charge the detectors par files

  geoTrafo = new TAGgeoTrafo();
  TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
  geoTrafo->FromFile(parFileName);

  TAGparaDsc* parGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
  stparGeo = (TASTparGeo*)parGeoSt->Object();
  parFileName = campManager->GetCurGeoFile(TASTparGeo::GetBaseName(), runNumber);
  stparGeo->FromFile(parFileName);

  TAGparaDsc* parGeoBm = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
  bmparGeo = (TABMparGeo*)parGeoBm->Object();
  parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
  bmparGeo->FromFile(parFileName);

  TAGparaDsc* parGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
  vtparGeo = (TAVTparGeo*)parGeoVtx->Object();
  parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
  vtparGeo->FromFile(parFileName);

  TAGparaDsc* parGeoIt = new TAGparaDsc(TAITparGeo::GetDefParaName(), new TAITparGeo());
  itparGeo = (TAITparGeo*)parGeoIt->Object();
  parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
  itparGeo->FromFile(parFileName);

  TAGparaDsc* parGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
  msdparGeo = (TAMSDparGeo*)parGeoMsd->Object();
  parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
  msdparGeo->FromFile(parFileName);

  TAGparaDsc* parGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
  twparGeo = (TATWparGeo*)parGeoTw->Object();
  parFileName = campManager->GetCurGeoFile(TATWparGeo::GetBaseName(), runNumber);
  twparGeo->FromFile(parFileName);

  //define and load the detector interesting quantities that will be passed in the analysis methods
  if(IncludeSC>0){
    stNtuHit= new TASTntuHit();
    tree->SetBranchAddress(TASTntuHit::GetBranchName(), &stNtuHit);
  }

  if(IncludeBM){
    bmNtuHit = new TABMntuHit();
    tree->SetBranchAddress(TABMntuHit::GetBranchName(), &bmNtuHit);
    bmNtuTrack = new TABMntuTrack();
    tree->SetBranchAddress(TABMntuTrack::GetBranchName(), &bmNtuTrack);
    if(IncludeMC){
      bmNtuEve = new TAMCntuHit();
      tree->SetBranchAddress(TAMCntuHit::GetBmBranchName(), &bmNtuEve);
    }
  }

  if(IncludeVT){
    vtxNtuVertex = new TAVTntuVertex();
    tree->SetBranchAddress(TAVTntuVertex::GetBranchName(), &vtxNtuVertex);
    vtxNtuTrack = new TAVTntuTrack();
    tree->SetBranchAddress(TAVTntuTrack::GetBranchName(), &vtxNtuTrack);
    vtxNtuCluster = new TAVTntuCluster();
    tree->SetBranchAddress(TAVTntuCluster::GetBranchName(), &vtxNtuCluster);
    vtClus = new TAVTntuCluster();
    vtClus->SetParGeo(vtparGeo);
    tree->SetBranchAddress(TAVTntuCluster::GetBranchName(), &vtClus);
    if(IncludeMC){
      vtMc = new TAMCntuHit();
      tree->SetBranchAddress(TAMCntuHit::GetVtxBranchName(), &vtMc);
    }
  }

  if(IncludeIT){
    itClus = new TAITntuCluster();
    tree->SetBranchAddress(TAITntuCluster::GetBranchName(), &itClus);
  }

  if(IncludeMSD){
    msdntuclus= new TAMSDntuCluster();
    tree->SetBranchAddress(TAMSDntuCluster::GetBranchName(), &msdntuclus);
    msdntutrack= new TAMSDntuTrack();
    tree->SetBranchAddress(TAMSDntuTrack::GetBranchName(), &msdntutrack);
  }

  if(IncludeTW){
    twNtuPoint = new TATWntuPoint();
    tree->SetBranchAddress(TATWntuPoint::GetBranchName(), &twNtuPoint);
  }

  if(IncludeMC>0){
    mcNtuPart = new TAMCntuPart();
    tree->SetBranchAddress(TAMCntuPart::GetBranchName(), &mcNtuPart);
  }

  if(IncludeDAQ){
    tgevent= new TAGntuEvent();
    tree->SetBranchAddress("evt.", &tgevent);
  }

  if(IncludeTOE){
    glbntutrk = new TAGntuGlbTrack();
    tree->SetBranchAddress(TAGntuGlbTrack::GetBranchName(), &glbntutrk);
  }

  if (nentries == 0 || nentries>tree->GetEntries())
    maxentries = tree->GetEntries();
  else
    maxentries=nentries;


  Int_t pos = nameFile.Last('.');
  TString nameOut;
  if(extname.Length()==0){
    nameOut = nameFile(nameFile.Last('/')+1, pos);
    nameOut.Prepend("shoetree_");
    nameOut.Append("_out.root");
  }else
    nameOut=extname;

  int status;
  TFile *file_out = new TFile(nameOut,"RECREATE");
  status=Booking(file_out);

  cout<<"input file="<<nameFile.Data()<<endl;
  cout<<"I'll process "<<maxentries<<" events. The input tree contains a total of "<<tree->GetEntries()<<" events."<<endl;
  cout<<"Now I'll start the event loop"<<endl;

  for (evnum = 0; evnum < maxentries; ++evnum) {

    if(evnum%100==0) printf("Processed Events: %d\n", evnum);
    tree->GetEntry(evnum);

    if(IncludeSC)
      status=StartCounter();
    if(IncludeBM)
      status=BeamMonitor();
    if(IncludeVT)
      status=Vertex();
    if(IncludeMSD)
      status=MSD();
    if(IncludeTW)
      status=TofWall();
    if(IncludeDAQ)
      status=DataAcquisition();
    if(bmNtuTrack->GetTracksN()==1 && vtxmatchvertex!=nullptr)
      status=VTXSYNC();

    //combined analysis
    status=BMVTX();
  }//Loop on event

  file_out->Write();
  file_out->Close();

  cout<<"program executed; output file= "<<nameOut.Data()<<endl;
}
