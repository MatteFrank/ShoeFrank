#include "AlignFOOTFunc.h"

// main
// nameFile=Input file name
// entries: number of events to be processed (use 0 to process the whole file)
// alignStraight: calculate the straight alignment parameters
// alignVs: calculate the straight alignment parameters with respect to the VTX (or other fixed detector)
// check: check the sub-detector response for the input file
// printFile: print the alignment and/or the checkup results in an external text file
void AlignFOOTMain(TString nameFile = "", Int_t nentries = 0, Bool_t alignStraight=false, Bool_t alignVs=false, Bool_t check=true, Bool_t printFile=false)

{

  //Open the input file
  TTree *tree = 0;
  TFile *inputFile = new TFile(nameFile.Data());
  if(inputFile->IsOpen()==false){
    cout<<"FATAL ERROR: I cannot open the input file, check it!  input file="<<nameFile.Data()<<endl;
    return;
  }
  tree = (TTree*)inputFile->Get("tree");
  if(tree==nullptr){
    cout<<"FATAL ERROR: your input file do not contains a tree, check it!  input file="<<nameFile.Data()<<endl;
    return;
  }

  //define and charge varaibles and geometrical parameters
  TAGroot gTAGroot;
  runinfo=(TAGrunInfo*)(inputFile->Get("runinfo"));
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

  TAGparaDsc* fpParGeoG = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
  parGeo = (TAGparGeo*)fpParGeoG->Object();
  parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
  parGeo->FromFile(parFileName.Data());


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
    vtxNtuCluster->SetParGeo(vtparGeo);
    tree->SetBranchAddress(TAVTntuCluster::GetBranchName(), &vtxNtuCluster);
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
    msdNtuPoint= new TAMSDntuPoint();
    tree->SetBranchAddress(TAMSDntuPoint::GetBranchName(), &msdNtuPoint);
    msdNtuHit= new TAMSDntuHit();
    tree->SetBranchAddress(TAMSDntuHit::GetBranchName(), &msdNtuHit);
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

  if (nentries == 0 || nentries>tree->GetEntriesFast())
    maxentries = tree->GetEntriesFast();
  else
    maxentries=nentries;


  Int_t pos = nameFile.Last('.');
  TString nameOut, txtout;
  nameOut = nameFile(nameFile.Last('/')+1, pos);
  nameOut.Prepend("alignout_");
  txtout=nameOut;
  nameOut.Append("_out.root");
  txtout.Append("_out.txt");
  if(printFile==true)
    freopen(txtout.Data(),"w",stdout);

  //vectors of tracks of primaries reconstructed by the single detectors and adopted for the alignment. All values are in local frames,
  vector<beamtrk> bmtrk;
  vector<beamtrk> vttrk;
  vector<beamtrk> msdtrk;

  TFile *file_out = new TFile(nameOut,"RECREATE");
  Booking(file_out);

  cout<<"input file="<<nameFile.Data()<<endl;
  cout<<"I'll process "<<maxentries<<" events. The input tree contains a total of "<<tree->GetEntriesFast()<<" events."<<endl;
  Zbeam = parGeo->GetBeamPar().AtomicNumber;
  cout<<"ZBEAM = "<<Zbeam<<endl;

  for (evnum = 0; evnum < maxentries; ++evnum) {

    if(evnum%100==0 && printFile==false)
      printf("Processed Events: %d\n", evnum);

    tree->GetEntry(evnum);

    if(IncludeBM)
      BeamMonitor();
    if(IncludeVT)
      Vertex();
    if(IncludeMSD)
      MSD();
    if(IncludeTW)
      TofWall();
    if(IncludeDAQ)
      DataAcquisition();
    if(IncludeBM && IncludeVT)
      VTXSYNC(); //to check the VTX and the BM synchronization

    if(IncludeTW && (IncludeVT || IncludeMSD))
      FillTWalign(); //Align the TW with the VT or MSD tracks
    
    if((IncludeBM && IncludeVT) || (IncludeBM && IncludeMSD) || (IncludeVT && IncludeMSD))
      FillCorr();

    FillTrackVect(bmtrk, vttrk, msdtrk); //fill the tracks in GLOBAL FRAME adopted for alignment
  }//Loop on events

  TString foldername;
  // extrapolate the alignment parameter of all the detectors with respect to the target
  if(alignStraight){
    foldername="BM";
    AlignWrtTarget(bmtrk,foldername);
    foldername="VT";
    AlignWrtTarget(vttrk,foldername);
    foldername="MSD";
    AlignWrtTarget(msdtrk,foldername);
  }

  if(alignVs){
    //here the VT is fixed, the BM will be shifted and tilted
    foldername="BMVT";
    AlignDetaVsDetb(bmtrk, vttrk, foldername, geoTrafo->GetDeviceCenter("BM"), geoTrafo->GetDeviceAngle("BM"));
    //here the VT is fixed, the MSD will be shifted and tilted
    foldername="MSDVT";
    AlignDetaVsDetb(msdtrk, vttrk, foldername, geoTrafo->GetDeviceCenter("MSD"), geoTrafo->GetDeviceAngle("MSD"));
  }


  // if((alignVs || alignStraight) && IncludeTW && (IncludeVT || IncludeMSD))
  //   AlignTWOld();
  if((alignVs || alignStraight) && IncludeTW && (IncludeVT || IncludeMSD)){
    AlignTW(0); //layer 0 (vertical layer, provide X pos)
    AlignTW(1); //layer 1 
  }

  file_out->Write();

  //Check the detector status for the current run:
  CheckUp(inputFile, file_out);

  file_out->Close();

  cout<<"program executed; output file= "<<nameOut.Data()<<endl;
}
