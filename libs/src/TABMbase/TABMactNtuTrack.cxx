/*!
  \file
  \version $Id: TABMactNtuTrack.cxx,v 1.12 2003/07/08 18:58:35 mueller Exp $
  \brief   Implementation of TABMactNtuTrack.
*/

#include "TABMactNtuTrack.hxx"

using namespace std;
/*!
  \class TABMactNtuTrack TABMactNtuTrack.hxx "TABMactNtuTrack.hxx"
  \brief Track builder for Beam Monitor. **
*/

ClassImp(TABMactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuTrack::TABMactNtuTrack(const char* name,
                                 TAGdataDsc* dscntutrk,
                                 TAGdataDsc* dscnturaw,
                                 TAGparaDsc* dscbmgeo,
                                 TAGparaDsc* dscbmcon,
                                 TAGparaDsc* dscbmcal)
  : TAGaction(name, "TABMactNtuTrack - Track finder for BM"),
    fpNtuTrk(dscntutrk),
    fpNtuHit(dscnturaw),
    fpParGeo(dscbmgeo),
    fpParCon(dscbmcon),
    fpParCal(dscbmcal), 
    legendreFit(nullptr), minuitFit(nullptr),
    clk("TABMactNtuTrack")
{
  if (FootDebugLevel(1))
    cout<<"TABMactNtuTrack::default constructor::Creating the Beam Monitor Track ntuplizer"<<endl;
  AddDataOut(fpNtuTrk, "TABMntuTrack");
  AddDataIn(fpNtuHit,  "TABMntuHit");
  AddPara(fpParGeo,  "TABMparGeo");
  AddPara(fpParCon,  "TABMparConf");
  AddPara(fpParCal,  "TABMparCal");

  fpTmpTrack=new TABMtrack();

 
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  
  //legendre
  legendreFit = ::new TABMLegendreFitter("LegendreFitter", p_bmcon);

  minuitFit = ::new TABMMinuitFitter("MinuitFitter", p_bmcon);

  //  fLegPolSum=new TH2F("fLegPolSum","Legendre polynomial space; m; q[cm]",p_bmcon->GetLegMBin(),-p_bmcon->GetLegMRange(),p_bmcon->GetLegMRange(), p_bmcon->GetLegRBin(),-p_bmcon->GetLegRRange(),p_bmcon->GetLegRRange());

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuTrack::~TABMactNtuTrack(){
  delete fpTmpTrack;
  delete legendreFit;
  delete minuitFit;
  clk.print();
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TABMactNtuTrack::CreateHistogram()
{
  DeleteHistogram();

  fpResTot = new TH2F("bmTrackResidual","Residual vs Rdrift; Residual [cm]; Measured rdrift [cm]", 600, -0.3, 0.3, 100, 0., 1.);
  AddHistogram(fpResTot);
  fpResTot1 = new TH2F("bmTrackResClean","Residual vs Rdrift; Residual [cm]; Measured rdrift [cm]", 600, -0.3, 0.3, 100, 0., 1.);
  AddHistogram(fpResTot1);
  fpHisMap = new TH2F("bmTrackTargetMap","BM - Position of the tracks at the target center; X[cm]; Y[cm]", 250, -3., 3.,250 , -3, 3);
  AddHistogram(fpHisMap);
  fpHisMapTW = new TH2F("bmTrackTWMap","BM - Position of the tracks at the TW center; X[cm]; Y[cm]", 500, -10., 10.,500 , -10, 10);
  AddHistogram(fpHisMapTW);
  fpHisMylar12d = new TH2F("bmTrackCenter","BM - Position of the tracks on the BM center plane; X[cm]; Y[cm]", 500, -3., 3.,500 , -3., 3.);
  AddHistogram(fpHisMylar12d);
  // fpHis0MSD = new TH2F("bmMsd0","BM - Position of the tracks on the MSD 0 plane in global ref", 500, -5., 5.,500 , -5., 5.);
  // AddHistogram(fpHis0MSD);
  // fpHis1MSD = new TH2F("bmMsd1","BM - Position of the tracks on the MSD 1 plane in global ref", 500, -5., 5.,500 , -5., 5.);
  // AddHistogram(fpHis1MSD);
  // fpHis2MSD = new TH2F("bmMsd2","BM - Position of the tracks on the MSD 2 plane in global ref", 500, -5., 5.,500 , -5., 5.);
  // AddHistogram(fpHis2MSD);//new plots for gsi2021
  fpHisAngleX = new TH1F("bmTrackAngleX","BM track XZ Angular spread; XZ Angle [rad]; Events", 200, -0.3, 0.3);
  AddHistogram(fpHisAngleX);
  fpHisAngleY = new TH1F("bmTrackAngleY","BM track YZ Angular spread; YZ Angle [rad]; Events", 200, -0.3, 0.3);
  AddHistogram(fpHisAngleY);
  fpHisTrackStatus = new TH1I("bmTrackStatus","Track status; -2=maxhitcut -1=minhitcut 0=ok 1/2=firedplane 3=hitrejected 4=noconv 5=chi2cut 6=noxztrack 7=noyztrack 8+=Error; Events", 18, -2.5, 15.5);
  AddHistogram(fpHisTrackStatus);
  fpHisNhitTotTrack = new TH1I("bmTrackNtotHitsxTrack","Number of hits x track; N hits; Events", 31, -0.5, 30.5);
  AddHistogram(fpHisNhitTotTrack);
  fpHisNhitXTrack = new TH1I("bmTrackNXHitsxTrack","Number of hits x track on the XZ view ; N hits; Events", 17, -0.5, 16.5);
  AddHistogram(fpHisNhitXTrack);
  fpHisNhitYTrack = new TH1I("bmTrackNYHitsxTrack","Number of hits x track on the YZ view; N hits; Events", 17, -0.5, 16.5);
  AddHistogram(fpHisNhitYTrack);
  fpHisNrejhitTrack = new TH1I("bmTrackNhitsRejectedxTrack","Number of hits rejected x track; N hits; Events", 31, -0.5, 30.5);
  AddHistogram(fpHisNrejhitTrack);
  fpHisTrackFakeHit = new TH1I("bmTrackFakeHits","Selected hits vs fake hits; -1=Lost primary hit 0=Selected primary hit 1=Wrong hit selected 2=correct rejection of fake hit; Events", 4, -1.5, 2.5);
  AddHistogram(fpHisTrackFakeHit);
  fpHisChi2Red = new TH1F("bmTrackChi2Red","total chi2red", 1000, 0., 100.);
  AddHistogram(fpHisChi2Red);
  fpHisChi2XZ = new TH1F("bmTrackChi2xzView","chi2red on xz view", 1000, 0., 100.);
  AddHistogram(fpHisChi2XZ);
  fpHisChi2YZ = new TH1F("bmTrackChi2yzView","chi2red on yz view", 1000, 0., 100.);
  AddHistogram(fpHisChi2YZ);
  fpNtotTrack = new TH1I("bmTrackTotNumber","Total number of tracks per event; Number of tracks; evts", 7, -0.5, 6.5);
  AddHistogram(fpNtotTrack);
  fpNXtrack = new TH1I("bmTrackXzviewNumber","Number of tracks on XZ plane per event; Number of tracks; evts", 7, -0.5, 6.5);
  AddHistogram(fpNXtrack);
  fpNYtrack = new TH1I("bmTrackYzviewNumber","Number of tracks on YZ plane per event; Number of tracks; evts", 7, -0.5, 6.5);
  AddHistogram(fpNYtrack);
  fpTrackAngles = new TH1F("bmTrackMultiAngles","Angles btw reco tracks; deg; tracks", 500, 0, 10.);
  AddHistogram(fpTrackAngles);
  fpTrackSep = new TH1F("bmTrackMultiSeparation","Separation btw multi-tracks of the same event in the BM sys of ref. origin; cm; tracks", 500, 0, 5.);
  AddHistogram(fpTrackSep);

  //control graphs
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparCal* p_bmcal = (TABMparCal*) fpParCal->Object();
  if(p_bmcal->GetResoFunc()!=nullptr){
    fpParRes=(TH1F*)p_bmcal->GetResoFunc()->GetHistogram()->Clone("bmParResolution");
    fpParRes->SetTitle("BM input resolution; Drift distance [cm]; Resolution [cm]");
    fpParRes->SetAxisRange(0,0.8);
    AddHistogram(fpParRes);
  }
  if(p_bmcal->GetSTrelFunc()!=nullptr){
    fpParSTrel = new TH1F("bmParSTrel","Space time relations; time [ns]; Space [cm]", p_bmcon->GetHitTimeCut(), 0, p_bmcon->GetHitTimeCut());
    for(Int_t i=1;i<p_bmcon->GetHitTimeCut();++i)
      fpParSTrel->SetBinContent(i, p_bmcal->STrelEval(i+1));
    AddHistogram(fpParSTrel);
  }

  //STREL calibration
  if(TAGrecoManager::GetPar()->CalibBM()>0){
    Int_t nbin=(int)(p_bmcon->GetHitTimeCut()/10.);
    fpResTimeTot = new TH2F("bmTrackTimeResidual","Residual vs Time; Residual [cm]; Time [ns]", 600, -0.3, 0.3,nbin, 0., p_bmcon->GetHitTimeCut());
    AddHistogram(fpResTimeTot);
    fpParNewSTrel = new TH1F("bmParNewSTrel","New space time relations; time [ns]; Space [cm]", nbin, 0, p_bmcon->GetHitTimeCut());
    AddHistogram(fpParNewSTrel);
    fpParNewTimeRes = new TH1F("bmParNewTimeRes","New resolution distribution; time [ns]; Resolution [cm]", nbin, 0, p_bmcon->GetHitTimeCut());
    AddHistogram(fpParNewTimeRes);
    fpNewStrelDiff = new TH1F("bmStrelDiff","Differences between old and new strel; Time [ns]; New-old strel [cm]", nbin, 0., p_bmcon->GetHitTimeCut());
    AddHistogram(fpNewStrelDiff);
    fpNewResoDistDiff = new TH1F("bmResDistDiff","Differences between old and new resolution; Drift distance [cm]; New-old Spatial resolution [cm]", 100, 0., 1.);
    AddHistogram(fpNewResoDistDiff);
    fpParNewDistRes = new TH1D("bmParNewDistRes","New resolution distribution; Drift distance [cm]; Resolution [cm]", 100, 0., 1.);
    AddHistogram(fpParNewDistRes);
    TH1F *RawResPlot;
    for(Int_t i=0;i<nbin;i++){
      TString title="bmTrackResTime_";
      title+=i;
      RawResPlot=new TH1F( title.Data(), "Residual;Residual [cm]; Number of hits", 600, -0.3, 0.3);
      fpResTimeBin.push_back(RawResPlot);
      AddHistogram(RawResPlot);
    }
    for(Int_t i=0;i<100;i++){
      TString title="bmTrackResDist_";
      title+=i;
      RawResPlot=new TH1F( title.Data(), "Residual;Residual [cm]; Number of hits", 600, -0.3, 0.3);
      fpResDistBin.push_back(RawResPlot);
      AddHistogram(RawResPlot);
    }
  }


  fpFitIters = new TH1I("bmFitIters","Number of Minuit Iterations; Iterations; Entries",101,-0.5, 100.5);
  fpFitPulls1 = new TH2F("bmPulls1","Approximate pulls;  pull distribution; distance [cm]", 100, -5.,5., 100,0.,1.);
  fpFitPulls2 = new TH2F("bmPulls2","Better pulls; pull distribution; distance [cm]", 100,-5.,5.,100,0.,1.);
  fpFitQ = new TH1F("bmFitQ","Constant term; Constant term: fit-initial value [cm]; Entries",300,-1.5, 1.5);
  fpFitM = new TH1F("bmFitM","Slope variable; Slope: initial value; Entries",100,-0.02, 0.02);
  fpFitQdiff = new TH1F("bmFitQdiff","Constant term; Constant term: initial value [cm]; Entries",100,-1.5, 1.5);
  fpFitMdiff = new TH1F("bmFitMdiff","Slope variable; Slope: fit-initial value; Entries",100,-0.02, 0.02);
  fpFitQvalue = new TH1F("bmFitQvalue",
			 "Constant term value; Fit constant term [cm]; Entries",
			 300,-1.5, 1.5);
  fpFitMvalue = new TH1F("bmFitMvalue","Slope variable; fit slope; Entries",
			 100,-0.02, 0.02);
  fpFitQFvsQ = new TH2F("bmFitQFvsQ","Constant term: fit value vs starting value; Q starting value [cm]; Q fit value [cm]",
			300,-1.5, 1.5, 300, -1.5, 1.5);
  fpFitMFvsM = new TH2F("bmFitMFvsM","Slope term: fit value vs starting value; M starting value []; M fit value []",
			100,-0.02, 0.02, 100, -0.005, 0.005);
  fpFitMFvsQF = new TH2F("bmFitMFvsQF","Fit results: slope fit value vs Q constant term; Q fit value [cm]; M fit value []",
			300,-1.5, 1.5, 100, -0.02, 0.02);
  fpRebinM = new TH1I("bmRebinM","Number of rebins for the Slope; # slope rebins; entries",5,-0.5,4.5);
  fpRebinQ = new TH1I("bmRebinQ","Number of rebins for the constant term; # constant term rebins; entries",5,-0.5,4.5);


  AddHistogram(fpFitIters);
  AddHistogram(fpFitPulls1);
  AddHistogram(fpFitPulls2);
  AddHistogram(fpFitQ);
  AddHistogram(fpFitM);
  AddHistogram(fpFitQdiff);
  AddHistogram(fpFitMdiff);
  AddHistogram(fpFitQvalue);
  AddHistogram(fpFitMvalue);
  AddHistogram(fpFitQFvsQ);
  AddHistogram(fpFitMFvsM);
  AddHistogram(fpFitMFvsQF);
  AddHistogram(fpRebinM);
  AddHistogram(fpRebinQ);





  SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuTrack::Action()
{
  p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  p_ntutrk->Clear();
  p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpParGeo->Object();

  i_nhit = p_nturaw->GetHitsN();
  Int_t firedview=0;
  //NB.: If the preselection reject the event no track will be saved
  fTrackNum=0;
  fNowView=0;
  vector<TABMtrack> xtracktr;
  vector<TABMtrack> ytracktr;
  if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack::Action::start"<<endl;

  //check for some cuts
  if(i_nhit>=p_bmcon->GetMaxHitCut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!:the number of hits is too high:  number of hit="<<i_nhit<<"  Maxhitcut="<<p_bmcon->GetMaxHitCut()<<endl;
    EarlyReturn(-2);
    return kTRUE;
  }else if(i_nhit<=p_bmcon->GetMinHitCut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!:the number of hits is too low:  number of hit="<<i_nhit<<"  Minhitcut="<<p_bmcon->GetMinHitCut()<<endl;
    EarlyReturn(-1);
    return kTRUE;
  }

  //apply some cut on the minimum number of fired plane for each view
  if(p_nturaw->GetTothitsNx()<p_bmcon->GetPlaneHitCut() || p_nturaw->GetTothitsNy()<p_bmcon->GetPlaneHitCut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!: number of xz view hit="<<p_nturaw->GetTothitsNx()<<"  number of yz view hit="<<p_nturaw->GetTothitsNy()<<"   planehitcut="<<p_bmcon->GetPlaneHitCut()<<endl;
    if(p_nturaw->GetTothitsNy()<p_bmcon->GetPlaneHitCut())
      EarlyReturn(1);
    else
      EarlyReturn(2);
    return kTRUE;
  }

  //loop for track reconstruction on each view
  do{
    firedview = CountRemainingHits(fNowView);

    //Apply cuts on the number of hits
    if(firedview<p_bmcon->GetPlaneHitCut()){
      if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack::no possible track!!: firedview="<<firedview<<"  fNowView="<<fNowView<<"   planehitcut="<<p_bmcon->GetPlaneHitCut()<<"  iteration fTrackNum="<<fTrackNum<<endl;
    }
    if(fNowView==1 && firedview<p_bmcon->GetPlaneHitCut())
      break;
    if(firedview<p_bmcon->GetPlaneHitCut()){
      fNowView=1;
      continue;
    }

    // tracking
    Int_t nselhits = legendreFit->doLegendreFit(p_nturaw, fTrackNum, fNowView);

    //hit selection done! now check for cuts
    if(nselhits<p_bmcon->GetPlaneHitCut()){
      if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack::The number of hits on one view is less than the minimum required:  min hit per view="<<p_bmcon->GetPlaneHitCut()<<"  number of selected hits="<<nselhits<<"  fTrackNum="<<fTrackNum<<"  fNowView="<<fNowView<<endl;
    }
    if( nselhits<p_bmcon->GetPlaneHitCut() && fNowView==0 ){
      fNowView=1;
      ++fTrackNum;
      continue;
    }
    if(nselhits<p_bmcon->GetPlaneHitCut() && fNowView==1)
      break;

    //ok we can reconstruct the particle track using the minimization of Chi2
    fpTmpTrack->Clean();
    minuitFit->Prepare(p_nturaw, fTrackNum, fNowView);
	
    minuitFit->Minimize(legendreFit->GetBestQValue(), 
			legendreFit->GetBestMValue(), fpTmpTrack);

    Int_t status = minuitFit->Status();
    static int maxprintouts=500;
    if( maxprintouts>0 ){
      maxprintouts--;
      legendreFit->ToStream();
      minuitFit->ToStream();
    }

    if(status!=0){
      EarlyReturn(8+status);
      return kTRUE; // ?? why this return ?? // there might be another view!
    }
	
    minuitFit->ComputeAll();

    if(status==0){
      FillFitHistograms();
    }

    //add the reconstructed track
    if(fNowView==0){
      fpTmpTrack->SetTrackIdY(fTrackNum+1);
      ytracktr.push_back(TABMtrack(*fpTmpTrack));
    }else{
      fpTmpTrack->SetTrackIdX(fTrackNum+1);
      xtracktr.push_back(TABMtrack(*fpTmpTrack));
    }
    ++fTrackNum;
  } while( fTrackNum<20 );

  if(p_bmcon->GetInvertTrack())
    InvertTracks(ytracktr, p_bmcon->GetInvertTrack());

  //if there are two tracks on the same view and only one on the other view, delete the second track since it is probably due to noises
  if(ytracktr.size()==2 && xtracktr.size()==1){
    p_ntutrk->NewPrunedTrack(ytracktr.at(1),0);
    ytracktr.pop_back();
  }else if(xtracktr.size()==2 && ytracktr.size()==1){
    p_ntutrk->NewPrunedTrack(xtracktr.at(1),1);
    xtracktr.pop_back();
  }

  CombineTrack(ytracktr, xtracktr);

  FillTrackHistograms( ytracktr, xtracktr );

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:end of tracking"<<endl;

  fpNtuTrk->SetBit(kValid);
  return kTRUE;
}

Int_t TABMactNtuTrack::CountRemainingHits(Int_t view){
  //check the number of hits in this view
  Int_t nhits=0;
  for(Int_t i_h = 0; i_h < i_nhit; i_h++) {
    TABMhit* p_hit = p_nturaw->GetHit(i_h);
    if(FootDebugLevel(3))
      cout<<"hit="<<i_h<<" plane="<<p_hit->GetPlane()<<"  view="<<p_hit->GetView()<<"  cell="<<p_hit->GetCell()<<"  plane="<<p_hit->GetPlane()<<"  rdrift="<<p_hit->GetRdrift()<<"  isfake="<<p_hit->GetIsFake()<<"  isselected="<<p_hit->GetIsSelected()<<"  View="<<view<<endl;
    if(p_hit->GetView()==view && p_hit->GetIsSelected()<=0)
      ++nhits;
  }
  return nhits;
}


//invert the direction and the position of the ytracktr track parameters
void TABMactNtuTrack::InvertTracks(vector<TABMtrack> &tracktrvec, Int_t InvertView){

  if (FootDebugLevel(2))
    cout<<"TABMactNtuTrack::InvertTracks: start; Number of tracks="<<tracktrvec.size()<<"  InvertView="<<InvertView<<endl;

  if(InvertView==2 || InvertView==3){
    for(Int_t i=0;i<tracktrvec.size();i++){
      tracktrvec.at(i).SetOriginY(-tracktrvec.at(i).GetOrigin().Y());
      tracktrvec.at(i).SetSlopeY(-tracktrvec.at(i).GetSlope().Y());
    }
  }
  
  if(InvertView==1 || InvertView==3){
    for(Int_t i=0;i<tracktrvec.size();i++){
      tracktrvec.at(i).SetOriginX(-tracktrvec.at(i).GetOrigin().X());
      tracktrvec.at(i).SetSlopeX(-tracktrvec.at(i).GetSlope().X());
    }
  }

  if (FootDebugLevel(2))
    cout<<"TABMactNtuTrack::InvertTracks:done"<<endl;

  return;
}


//combine the tracks reconstructed for each view
void TABMactNtuTrack::CombineTrack(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr ){

  if (FootDebugLevel(2))
    cout<<"TABMactNtuTrack::CombineTrack: start; Number of tracks on yz="<<ytracktr.size()<<"  number of tracks on xz="<<xtracktr.size()<<endl;

  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  for(Int_t i=0;i<xtracktr.size();++i){
    for(Int_t k=0;k<ytracktr.size();++k){
      TABMtrack currtracktr=xtracktr.at(i);
      Int_t status=currtracktr.mergeTrack(ytracktr.at(k));
      if(status==0){
        TABMtrack *trk=p_ntutrk->NewTrack(currtracktr);
        if(p_ntutrk->GetTrackStatus()!=0)
          p_ntutrk->SetTrackStatus( (trk->GetChiSquare()>=p_bmcon->GetChi2Cut())? 5:0);
      }else {
        p_ntutrk->SetTrackStatus( (status<0) ? 8 : 4);
        if(FootDebugLevel(2))
          cout<<"TABMactNtuTrack::CombineTrack: failed i="<<i<<" k="<<k<<"  status="<<status<<endl;
      }
    }
  }

  if(xtracktr.size()==0)
    p_ntutrk->SetTrackStatus(6);
  else if( ytracktr.size()==0)
    p_ntutrk->SetTrackStatus(7);
  if(ValidHistogram()){
    fpNtotTrack->Fill(p_ntutrk->GetTracksN());
    fpNXtrack->Fill(xtracktr.size());
    fpNYtrack->Fill(ytracktr.size());
  }
  if (FootDebugLevel(2))
    cout<<"TABMactNtuTrack::CombineTrack: end; total number of saved tracks="<<p_ntutrk->GetTracksN()<<endl;

  return;
}


void TABMactNtuTrack::FitWriteCalib(TF1 *newstrel, TF1 *resofunc, Double_t &meanTimeReso, Double_t &meanDistReso){

  TF1* gaus=new TF1("gaus","gaus",-0.1,0.1);
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparCal* p_bmcal = (TABMparCal*) fpParCal->Object();

  //strel evaluation
  for(Int_t i=0;i<fpResTimeBin.size();i++){
    if(fpResTimeBin.at(i)->GetEntries()>300){
      gaus->SetParameters(fpResTimeBin.at(i)->GetEntries(),fpResTimeBin.at(i)->GetMean(),fpResTimeBin.at(i)->GetStdDev());
      fpResTimeBin.at(i)->Fit(gaus,"QR+");
      fpParNewSTrel->SetBinContent(i+1,p_bmcal->STrelEval(fpParNewSTrel->GetXaxis()->GetBinCenter(i+1))-gaus->GetParameter(1));
      fpParNewTimeRes->SetBinContent(i+1,gaus->GetParameter(2));
      fpNewStrelDiff->SetBinContent(i+1,gaus->GetParameter(1));
    }else{
      fpParNewSTrel->SetBinContent(i+1,p_bmcal->STrelEval(fpParNewSTrel->GetXaxis()->GetBinCenter(i+1)));
      fpParNewTimeRes->SetBinContent(i+1,p_bmcal->ResoEval(p_bmcal->STrelEval(fpParNewTimeRes->GetXaxis()->GetBinCenter(i+1))));
      fpNewStrelDiff->SetBinContent(i+1,0.);
    }
  }
  TH1D* p_ResProj=(TH1D*)fpResTimeTot->ProjectionX();
  gaus->SetParameters(p_ResProj->GetEntries(),p_ResProj->GetMean(),p_ResProj->GetStdDev());
  p_ResProj->Fit(gaus,"QR+");
  meanTimeReso=gaus->GetParameter(2);

  //Spatial resolution evaluation
  for(Int_t i=0;i<fpResDistBin.size();i++){
    if(fpResDistBin.at(i)->GetEntries()>300){
      gaus->SetRange(-1.5,(i<3) ? 0. : 1.5);
      gaus->SetParameters(fpResDistBin.at(i)->GetEntries(),fpResDistBin.at(i)->GetMean(),fpResDistBin.at(i)->GetStdDev());
      fpResDistBin.at(i)->Fit(gaus,"QR+");
      fpParNewDistRes->SetBinContent(i+1,gaus->GetParameter(2));
      fpNewResoDistDiff->SetBinContent(i+1,p_bmcal->ResoEval(fpParNewDistRes->GetXaxis()->GetBinCenter(i+1))-gaus->GetParameter(2));
    }else{
      fpParNewDistRes->SetBinContent(i+1,p_bmcal->ResoEval(fpParNewDistRes->GetXaxis()->GetBinCenter(i+1)));
      fpNewResoDistDiff->SetBinContent(i+1,0.);
    }
  }
  p_ResProj=(TH1D*)fpResTot->ProjectionX();
  gaus->SetParameters(p_ResProj->GetEntries(),p_ResProj->GetMean(),p_ResProj->GetStdDev());
  p_ResProj->Fit(gaus,"QR+");
  meanDistReso=gaus->GetParameter(2);

  //fit the new strel and spatial resolutions
  newstrel->FixParameter(0,0.);
  fpParNewSTrel->Fit(newstrel,"QBR+");
  fpParNewDistRes->Fit(resofunc,"QR+");

  return;
}


void  TABMactNtuTrack::EarlyReturn( Int_t code){
  p_ntutrk->SetTrackStatus(code);
  if(ValidHistogram()){
    fpHisTrackStatus->Fill(code);
    fpNtotTrack->Fill(0);
    fpNXtrack->Fill(0);
    fpNYtrack->Fill(0);
  }
  fpNtuTrk->SetBit(kValid);
}


void TABMactNtuTrack::FillTrackHistograms(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr ){
  if (ValidHistogram()){
    fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());
    for(Int_t i=0;i<xtracktr.size();++i){
      fpHisChi2XZ->Fill(xtracktr.at(i).GetChiSquareX());
      fpHisNhitXTrack->Fill(xtracktr.at(i).GetHitsNx());
    }
    for(Int_t i=0;i<ytracktr.size();++i){
      fpHisChi2YZ->Fill(ytracktr.at(i).GetChiSquareY());
      fpHisNhitYTrack->Fill(ytracktr.at(i).GetHitsNy());
    }
    for(Int_t i=0;i<p_ntutrk->GetTracksN();++i){
      TABMtrack *savedtracktr=p_ntutrk->GetTrack(i);
      TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
      Float_t posZ = geoTrafo->FromGlobalToBMLocal(TVector3(0,0,0)).Z();
      TVector3 pos = savedtracktr->PointAtLocalZ(posZ);
      fpHisMap->Fill(pos[0], pos[1]);
      pos = savedtracktr->PointAtLocalZ(geoTrafo->FromGlobalToBMLocal(geoTrafo->FromTWLocalToGlobal(TVector3(0,0,0))).Z());
      fpHisMapTW->Fill(pos.X(), pos.Y());
      fpHisMylar12d->Fill(savedtracktr->GetOrigin().X(), savedtracktr->GetOrigin().Y());
      // posZ = geoTrafo->FromGlobalToBMLocal(geoTrafo->FromMSDLocalToGlobal(0.,0.,0.)).Z(); //msd first plane z position in global position
      // pos = savedtracktr->PointAtLocalZ(posZ);
      // fpHis0MSD->Fill(pos[0], pos[1]);
      
      fpHisAngleX->Fill(savedtracktr->GetSlope().X()/savedtracktr->GetSlope().Z());
      fpHisAngleY->Fill(savedtracktr->GetSlope().Y()/savedtracktr->GetSlope().Z());
      fpHisChi2Red->Fill(savedtracktr->GetChiSquare());
      fpHisNhitTotTrack->Fill(savedtracktr->GetHitsNtot());
      fpHisNrejhitTrack->Fill(p_nturaw->GetHitsN()-savedtracktr->GetHitsNtot());
      for(Int_t k=0;k<i;++k){
        fpTrackAngles->Fill(p_ntutrk->GetTrack(i)->GetSlope().Angle(p_ntutrk->GetTrack(k)->GetSlope())*TMath::RadToDeg());
        fpTrackSep->Fill((p_ntutrk->GetTrack(i)->GetOrigin()-p_ntutrk->GetTrack(k)->GetOrigin()).Mag());
      }
    }
    for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
      TABMhit* p_hit=p_nturaw->GetHit(i);
      if(p_hit->GetIsSelected()>0){
        fpResTot->Fill(p_hit->GetResidual(),p_hit->GetRdrift());
        if(TAGrecoManager::GetPar()->CalibBM()){
          fpResTimeTot->Fill(p_hit->GetResidual(),p_hit->GetTdrift());
          if((int)(p_hit->GetTdrift()/10.)<fpResTimeBin.size())
            fpResTimeBin.at((int)(p_hit->GetTdrift()/10.))->Fill(p_hit->GetResidual());
          if((int)(p_hit->GetRdrift()*100)<fpResDistBin.size())
            fpResDistBin.at((int)(p_hit->GetRdrift()*100))->Fill(p_hit->GetResidual());
        }
      }
      if(p_hit->GetIsSelected()>0 && p_hit->GetIsFake()==0){
        fpHisTrackFakeHit->Fill(0);
      }else if(p_hit->GetIsSelected()<=0 && p_hit->GetIsFake()>0){
        fpHisTrackFakeHit->Fill(2);
      }else if (p_hit->GetIsFake()>0  &&  p_hit->GetIsSelected()>0){
        fpHisTrackFakeHit->Fill(1);
      }else if(p_hit->GetIsSelected()<=0 && p_hit->GetIsFake()==0){
        fpHisTrackFakeHit->Fill(-1);
      }
    }
  }
}


void TABMactNtuTrack::FillFitHistograms(){

  Float_t Qstart, Mstart, Qfit, Mfit;
  Qfit = minuitFit->Qfit();
  Mfit = minuitFit->Mfit();
  Qstart = legendreFit->GetBestQValue();
  Mstart = legendreFit->GetBestMValue();
  fpFitM->Fill(Mstart);
  fpFitQ->Fill(Qstart);
  fpFitMdiff->Fill(Mfit-Mstart);
  fpFitQdiff->Fill(Qfit-Qstart);
  fpFitMvalue->Fill(Mfit);
  fpFitQvalue->Fill(Qfit);
  fpFitQFvsQ->Fill(Qstart, Qfit);
  fpFitMFvsM->Fill(Mstart, Mfit);
  fpFitMFvsQF->Fill(Qfit, Mfit);
  fpFitIters->Fill(minuitFit->NCalls());
  fpRebinM->Fill(legendreFit->GetMRebins());
  fpRebinQ->Fill(legendreFit->GetQRebins());

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMhit* p_hit=p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()==(fTrackNum+1)){
      
      Float_t distance = p_hit->GetRdrift();
      Float_t residual = p_hit->GetResidual();
      Float_t pull = residual/p_hit->GetSigma();
      fpFitPulls1->Fill(pull,distance);
      fpResTot1->Fill(residual,distance);
    }
  }

  minuitFit->EvaluatePulls(fpFitPulls2);

}
