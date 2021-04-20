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
    fpParCal(dscbmcal)
{
  if (FootDebugLevel(1))
   cout<<"TABMactNtuTrack::default constructor::Creating the Beam Monitor Track ntuplizer"<<endl;
  AddDataOut(fpNtuTrk, "TABMntuTrack");
  AddDataIn(fpNtuHit,  "TABMntuHit");
  AddPara(fpParGeo,  "TABMparGeo");
  AddPara(fpParCon,  "TABMparConf");
  AddPara(fpParCal,  "TABMparCal");

  fpTmpTrack=new TABMtrack();

  //new chi2 ROOT based
  fpFunctor= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2,2);
  fpMinimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");

  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  //legendre
  fLegPolSum=new TH2F("fLegPolSum","Legendre polynomial space; m; q[cm]",p_bmcon->GetLegMBin(),-p_bmcon->GetLegMRange(),p_bmcon->GetLegMRange(), p_bmcon->GetLegRBin(),-p_bmcon->GetLegRRange(),p_bmcon->GetLegRRange());

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuTrack::~TABMactNtuTrack(){
delete fpTmpTrack;
delete fpMinimizer;
delete fpFunctor;
delete fLegPolSum;
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TABMactNtuTrack::CreateHistogram()
{
  DeleteHistogram();

  fpResTot = new TH2F("bmTrackResidual","Residual vs Rdrift; Residual [cm]; Measured rdrift [cm]", 600, -0.3, 0.3, 100, 0., 1.);
  AddHistogram(fpResTot);
  fpHisMap = new TH2F("bmTrackTargetMap","BM - Position of a single track event at target center", 250, -3., 3.,250 , -3, 3);
  AddHistogram(fpHisMap);
  fpHisMylar12d = new TH2F("bmTrackCenter","BM - Position of a single track event on the BM center plane", 500, -3., 3.,500 , -3., 3.);
  AddHistogram(fpHisMylar12d);
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
  fpHisNrejhitTrack = new TH1I("bmTrackNhitsRejectedxTrack","Number of hits rejected x track of a single track event; N hits; Events", 31, -0.5, 30.5);
  AddHistogram(fpHisNrejhitTrack);
  fpHisTrackFakeHit = new TH1I("bmTrackFakeHits","Selected hits vs fake hits; -1=Lost primary hit 0=Selected primary hit 1=Wrong hit selected 2=correct rejection of fake hit; Events", 4, -1.5, 2.5);
  AddHistogram(fpHisTrackFakeHit);
  fpHisChi2Red = new TH1F("bmTrackChi2Red","chi2red of a single track event", 1000, 0., 100.);
  AddHistogram(fpHisChi2Red);
  fpHisChi2XZ = new TH1F("bmTrackChi2xzView","chi2red on xz view of a single track event", 1000, 0., 100.);
  AddHistogram(fpHisChi2XZ);
  fpHisChi2YZ = new TH1F("bmTrackChi2yzView","chi2red on yz view of a single track event", 1000, 0., 100.);
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


   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuTrack::Action()
{
  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  p_ntutrk->Clear();
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpParGeo->Object();

  Int_t i_nhit = p_nturaw->GetHitsN();
  Int_t firedview=0, tmp_int;
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
      p_ntutrk->SetTrackStatus(-2);
      if(ValidHistogram()){
        fpHisTrackStatus->Fill(-2);
        fpNtotTrack->Fill(0);
        fpNXtrack->Fill(0);
        fpNYtrack->Fill(0);
      }
      fpNtuTrk->SetBit(kValid);
      return kTRUE;
  }else if(i_nhit<=p_bmcon->GetMinHitCut()){
     if(FootDebugLevel(1))
       cout<<"TABMactNtuTrack::no possible track!!:the number of hits is too low:  number of hit="<<i_nhit<<"  Minhitcut="<<p_bmcon->GetMinHitCut()<<endl;
       p_ntutrk->SetTrackStatus(-1);
       if(ValidHistogram()){
         fpHisTrackStatus->Fill(-1);
         fpNtotTrack->Fill(0);
         fpNXtrack->Fill(0);
         fpNYtrack->Fill(0);
       }
       fpNtuTrk->SetBit(kValid);
       return kTRUE;
   }

  //apply some cut on the minimum number of fired plane for each view
  if(p_nturaw->GetNtothitx()<p_bmcon->GetPlaneHitCut() || p_nturaw->GetNtothity()<p_bmcon->GetPlaneHitCut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!: number of xz view hit="<<p_nturaw->GetNtothitx()<<"  number of yz view hit="<<p_nturaw->GetNtothity()<<"   planehitcut="<<p_bmcon->GetPlaneHitCut()<<endl;
    if(p_nturaw->GetNtothity()<p_bmcon->GetPlaneHitCut())
      p_ntutrk->SetTrackStatus(1);
    else
      p_ntutrk->SetTrackStatus(2);
    if(ValidHistogram()){
      fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());
      fpNtotTrack->Fill(0);
      fpNXtrack->Fill(0);
      fpNYtrack->Fill(0);
    }
    fpNtuTrk->SetBit(kValid);
    return kTRUE;
  }

  //loop for track reconstruction on each view
  do{
    firedview=0;
    //check the number of hits for each view
    for(Int_t i_h = 0; i_h < i_nhit; i_h++) {
      TABMhit* p_hit = p_nturaw->GetHit(i_h);
      if(FootDebugLevel(3))
        cout<<"hit="<<i_h<<" plane="<<p_hit->GetPlane()<<"  view="<<p_hit->GetView()<<"  cell="<<p_hit->GetCell()<<"  plane="<<p_hit->GetPlane()<<"  rdrift="<<p_hit->GetRdrift()<<"  isfake="<<p_hit->GetIsFake()<<"  isselected="<<p_hit->GetIsSelected()<<"  fNowView="<<fNowView<<endl;
      if(p_hit->GetView()==fNowView && p_hit->GetIsSelected()<=0)
        ++firedview;
    }

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

    //tracking
    ChargeLegendrePoly();//hit selection with legendre transform

    //find the highest peak in legendre space
    tmp_int=FindLegendreBestValues();
    //check and flag the associated hits
    Int_t nselhit=CheckAssHits(p_bmcon->GetAssHitErr(),  p_bmcon->GetLegRRange()/p_bmcon->GetLegRBin());

    //hit selection done! now check for cuts
    if(nselhit<p_bmcon->GetPlaneHitCut()){
      if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack::The number of hits on one view is less than the minimum required:  min hit per view="<<p_bmcon->GetPlaneHitCut()<<"  number of selected hits="<<nselhit<<"  fTrackNum="<<fTrackNum<<"  fNowView="<<fNowView<<endl;
    }
    if(nselhit<p_bmcon->GetPlaneHitCut() && fNowView==0){
      fNowView=1;
      ++fTrackNum;
      continue;
    }
    if(nselhit<p_bmcon->GetPlaneHitCut() && fNowView==1)
      break;

     //ok we can reconstruct the particle track using the minimization of Chi2
    fpTmpTrack->Clean();
    if(fNowView==0){
      fpTmpTrack->SetSlopeY(fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin));
      fpTmpTrack->SetOriginY(fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin));
      fpTmpTrack->SethitsNy(nselhit);
    }else{
      fpTmpTrack->SetSlopeX(fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin));
      fpTmpTrack->SetOriginX(fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin));
      fpTmpTrack->SethitsNx(nselhit);
    }
    tmp_int= NumericalMinimizationDouble();
    if(tmp_int!=0){
      p_ntutrk->SetTrackStatus(8+tmp_int);
      if(ValidHistogram()){
        fpHisTrackStatus->Fill(8+tmp_int);
        fpNtotTrack->Fill(0);
        fpNXtrack->Fill(0);
        fpNYtrack->Fill(0);
      }
      fpNtuTrk->SetBit(kValid);
      return kTRUE;
    }else if(ComputeDataAll()){
      p_ntutrk->SetTrackStatus(8);//something is wrong, this should not happen!
      if(ValidHistogram()){
        fpHisTrackStatus->Fill(8);
        fpNtotTrack->Fill(0);
        fpNXtrack->Fill(0);
        fpNYtrack->Fill(0);
      }
      fpNtuTrk->SetBit(kValid);
      return kTRUE;
    }
    //add the reconstructed track
    if(fNowView==0)
      ytracktr.push_back(TABMtrack(*fpTmpTrack));
    else
      xtracktr.push_back(TABMtrack(*fpTmpTrack));
    ++fTrackNum;
  }while(fTrackNum<20);

  CombineTrack(ytracktr, xtracktr, p_ntutrk);

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
      fpHisMylar12d->Fill(savedtracktr->GetOrigin().X(), savedtracktr->GetOrigin().Y());
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

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:end of tracking"<<endl;

  fpNtuTrk->SetBit(kValid);
  return kTRUE;
}



void TABMactNtuTrack::ChargeLegendrePoly(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: start charge legendre pol graphs"<<endl;

  fLegPolSum->Reset("ICESM");
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  fLegPolSum->SetBins(p_bmcon->GetLegMBin(),-p_bmcon->GetLegMRange(),p_bmcon->GetLegMRange(), p_bmcon->GetLegRBin(),-p_bmcon->GetLegRRange(),p_bmcon->GetLegRRange());

  Float_t tmp_float;
  for(Int_t i_h = 0; i_h < p_nturaw->GetHitsN(); ++i_h) {
    TABMhit* p_hit = p_nturaw->GetHit(i_h);
    if(p_hit->GetView()==fNowView && p_hit->GetIsSelected()<=0){
      for(Int_t k=1;k<fLegPolSum->GetXaxis()->GetNbins();++k){
        tmp_float=fLegPolSum->GetXaxis()->GetBinCenter(k);
        fLegPolSum->Fill(tmp_float, ((fNowView==1) ? p_hit->GetWirePos().X() : p_hit->GetWirePos().Y()) -tmp_float*p_hit->GetWirePos().Z()+p_hit->GetRdrift()*sqrt(tmp_float*tmp_float+1.));
        fLegPolSum->Fill(tmp_float, ((fNowView==1) ? p_hit->GetWirePos().X() : p_hit->GetWirePos().Y()) -tmp_float*p_hit->GetWirePos().Z()-p_hit->GetRdrift()*sqrt(tmp_float*tmp_float+1.));
      }
    }
  }

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::legpolsum done"<<endl;

return;
}

// adopted to save the legpolsum th2d
void TABMactNtuTrack::SaveLegpol(){

  TString tmp_tstring="legpolsum_";
  tmp_tstring+=gTAGroot->CurrentEventNumber();
  tmp_tstring+="_";
  tmp_tstring+=fTrackNum;
  tmp_tstring+=(fNowView==0) ? "_y_" : "_x_";
  tmp_tstring+=".root";
  fLegPolSum->SaveAs(tmp_tstring.Data());

  return;
}

Int_t TABMactNtuTrack::FindLegendreBestValues(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: start"<<endl;

  Int_t first_maxbin,last_maxbin;
  //X values
  fBestMBin=-1;
  Int_t rebinm=0;
  do{
    first_maxbin=fLegPolSum->FindFirstBinAbove(fLegPolSum->GetMaximum()-1);
    last_maxbin=fLegPolSum->FindLastBinAbove(fLegPolSum->GetMaximum()-1);
    if(first_maxbin==last_maxbin){
      fBestMBin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<3){//value to optimize
      fBestMBin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      fLegPolSum->RebinX();
      rebinm++;
    }
  }while(fBestMBin==-1);

  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: check on m done, now I'll check on q  fBestMBin="<<fBestMBin<<endl;

  //check on r
  fBestRBin=-1;
  Int_t rebinr=0;
  do{
    first_maxbin=fLegPolSum->FindFirstBinAbove(fLegPolSum->GetMaximum()-1,2);
    last_maxbin=fLegPolSum->FindLastBinAbove(fLegPolSum->GetMaximum()-1,2);
    if(first_maxbin==last_maxbin){
      fBestRBin=first_maxbin;
    }else if((last_maxbin-first_maxbin)<3){//value to optimize
      fBestRBin=(Int_t)((last_maxbin+first_maxbin)/2);
    }else{
      fLegPolSum->RebinY();
      rebinr++;
    }
  }while(fBestRBin==-1);

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::FindLegendreBestValues:check on r done  fBestRBin="<<fBestRBin<<"   bincontent="<<fLegPolSum->GetBinContent(fBestMBin,fBestRBin)<<"  best m="<<fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin)<<"  best q="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin)<<"   rebinm="<<rebinm<<"  rebinr="<<rebinr<<endl;

return rebinm+rebinr;
}


//for Legendre polynomy tracking, set the associated hits
Int_t TABMactNtuTrack::CheckAssHits(const Float_t asshiterror, const Float_t minRerr) {

  if(FootDebugLevel(2)){
    cout<<"TABMactNtuTrack::CheckAssHits with fNowView="<<fNowView<<" start"<<endl;
      cout<<"first estimate of the track: slope="<<fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin)<<"  Mbinwidth="<<fLegPolSum->GetXaxis()->GetBinWidth(fBestMBin)<<"  R0="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin)<<"  R0binwidth="<<fLegPolSum->GetYaxis()->GetBinWidth(fBestRBin)<<endl;
  }

  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  Int_t wireplane[6]={-1,-1,-1,-1,-1,-1};
  Int_t selview=0;
  Float_t xvalue, yvaluemax, yvaluemin,yvalue, res, diff, legvalue, mindist, maxdist, a0pos;

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i) {
    TABMhit* p_hit = p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()<=0){
      Double_t maxerror= (p_hit->GetSigma()*asshiterror > minRerr) ? p_hit->GetSigma()*asshiterror : minRerr;
      maxdist=p_hit->GetRdrift()+maxerror;
      mindist=p_hit->GetRdrift()-maxerror;
      if(mindist<0)
        mindist=0;
      if(p_hit->GetView()==fNowView){
        a0pos=(fNowView==0) ? p_hit->GetWirePos().Y() : p_hit->GetWirePos().X();
        xvalue=fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin);
        yvaluemax=a0pos -xvalue*p_hit->GetWirePos().Z()+maxdist*sqrt(xvalue*xvalue+1.);
        yvaluemin=a0pos -xvalue*p_hit->GetWirePos().Z()+mindist*sqrt(xvalue*xvalue+1.);
        yvalue=a0pos -xvalue*p_hit->GetWirePos().Z()+p_hit->GetRdrift()*sqrt(xvalue*xvalue+1.);
        res=max(p_hit->GetSigma(),fLegPolSum->GetYaxis()->GetBinWidth(fBestRBin));
        legvalue=fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin);
        diff=fabs(yvalue - legvalue);
        if(yvaluemax>=legvalue && yvaluemin<=legvalue){
          CheckPossibleHits(wireplane,yvalue,diff,res,selview,i, p_hit);
        }else{
          yvaluemin=a0pos-xvalue*p_hit->GetWirePos().Z()-maxdist*sqrt(xvalue*xvalue+1.);
          yvaluemax=a0pos-xvalue*p_hit->GetWirePos().Z()-mindist*sqrt(xvalue*xvalue+1.);
          yvalue=a0pos-xvalue*p_hit->GetWirePos().Z()-p_hit->GetRdrift()*sqrt(xvalue*xvalue+1.);
          diff=fabs(yvalue - legvalue);
          if(yvaluemax>=legvalue && yvaluemin<=legvalue){
            CheckPossibleHits(wireplane,yvalue,diff,res,selview,i, p_hit);
          }else{
            if(FootDebugLevel(4))
              cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<i<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<0<<"  yvalue="<<yvalue<<"  legvalue="<<legvalue<<"  diff="<<diff<<"   res="<<res<<endl;
            p_hit->SetIsSelected(-1);
          }
        }
      }
    }
  }

  if(FootDebugLevel(2))
      cout<<"TABMactNtuTrack::CheckAssHits: done"<<endl;

return selview;
}

//used in CheckAssHits
void TABMactNtuTrack::CheckPossibleHits(Int_t wireplane[], Float_t yvalue, Float_t diff, Float_t res, Int_t &selview, const Int_t hitnum, TABMhit* p_hit){

  TABMhit*   p_doublehit;
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();

  if(wireplane[p_hit->GetPlane()]==-1){
    if(FootDebugLevel(4))
      cout<<"TABMactNtuTrack::CheckAssHits: hit selected hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  cellid="<<p_hit->GetIdCell()<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin)<<"  diff="<<diff<<"   residual="<<diff/res<<endl;
    wireplane[p_hit->GetPlane()]=hitnum;
    p_hit->SetIsSelected(fTrackNum+1);
    p_hit->SetResidual(diff/res);
    selview++;
  }else{
    p_doublehit=p_nturaw->GetHit(wireplane[p_hit->GetPlane()]);
    if(p_doublehit->GetResidual()> diff/res){
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: hit replaced! selected= hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  cellid="<<p_hit->GetIdCell()<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin)<<"  diff="<<diff<<"   residual="<<diff/res<<"  replace the hit num="<<wireplane[p_hit->GetPlane()]<<endl;
      p_hit->SetIsSelected(fTrackNum+1);
      p_hit->SetResidual(diff/res);
      wireplane[p_hit->GetPlane()]=hitnum;
      p_doublehit->SetIsSelected(-1);
    }else{
      p_hit->SetIsSelected(-1);
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin)<<"  diff="<<diff<<"   res="<<res<<endl;
    }
  }

  return;
}



Double_t TABMactNtuTrack::EvaluateChi2(const double *params){

  Double_t chi2=0, res;
  TVector3 vers,r0;
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpParGeo->Object();

  if(fNowView==0){
    vers.SetXYZ(0., params[0], 1.);
    r0.SetXYZ(0., params[1], 0.);
  }else{
    vers.SetXYZ(params[0], 0., 1.);
    r0.SetXYZ(params[1], 0., 0.);
  }

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMhit* p_hit = p_nturaw->GetHit(i);
    if( p_hit->GetView()==fNowView && p_hit->GetIsSelected()==(fTrackNum+1) ){
      res=p_hit->GetRdrift()-p_bmgeo->FindRdrift(r0, vers, p_hit->GetWirePos(), p_hit->GetWireDir(), true);
      chi2+=res*res/p_hit->GetSigma()/p_hit->GetSigma();
    }
  }

  return chi2;
}


Int_t TABMactNtuTrack::NumericalMinimizationDouble(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble start, fNowView="<<fNowView<<"  fTrackNum="<<fTrackNum<<endl;

  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();

  // Set the variables to be minimized
  fpMinimizer->SetFunction(*fpFunctor);
  fpMinimizer->SetMaxFunctionCalls(p_bmcon->GetNumIte());
  fpMinimizer->SetMaxIterations(p_bmcon->GetNumIte());
  fpMinimizer->SetTolerance(p_bmcon->GetParMove());
  fpMinimizer->SetPrintLevel(0);
  fpMinimizer->SetVariable(0,"m",0., p_bmcon->GetParMove());
  fpMinimizer->SetVariable(1,"q",0., p_bmcon->GetParMove());
  fpMinimizer->SetVariableValue(0,((fNowView==0) ? fpTmpTrack->GetSlope().Y() : fpTmpTrack->GetSlope().X())/fpTmpTrack->GetSlope().Z());
  fpMinimizer->SetVariableValue(1,(fNowView==0) ? fpTmpTrack->GetOrigin().Y() : fpTmpTrack->GetOrigin().X());

  fpMinimizer->Minimize();

 Int_t status=fpMinimizer->Status();
  if(status==0){
    if(fNowView==0){
      fpTmpTrack->SetSlopeY(fpMinimizer->X()[0]);
      fpTmpTrack->SetOriginY(fpMinimizer->X()[1]);
    }else{
      fpTmpTrack->SetSlopeX(fpMinimizer->X()[0]);
      fpTmpTrack->SetOriginX(fpMinimizer->X()[1]);
    }

    fpTmpTrack->SetIsConv(1);
    if(FootDebugLevel(1)){
      cout<<"TABMactNtuTrack:: NumericalMinimizationDouble done:  fNowView="<<fNowView<<"   The track parameters are:"<<endl;
      cout<<"Pvers=("<<fpTmpTrack->GetSlope().X()<<", "<<fpTmpTrack->GetSlope().Y()<<", "<<fpTmpTrack->GetSlope().Z()<<")"<<endl;
      cout<<"R0=("<<fpTmpTrack->GetOrigin().X()<<", "<<fpTmpTrack->GetOrigin().Y()<<", "<<fpTmpTrack->GetOrigin().Z()<<")"<<endl;
    }

    fpMinimizer->Clear();
    return 0;
  }else if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack:: NumericalMinimizationDouble: minimization failed: status="<<status<<endl;

  fpMinimizer->Clear();
  return status;
}



Bool_t TABMactNtuTrack::ComputeDataAll(){

  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpParGeo->Object();

  Float_t res, chi2red=0.;
  Int_t nselhit=0;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMhit* p_hit=p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()==(fTrackNum+1)){
      ++nselhit;
      res=p_hit->GetRdrift()-p_bmgeo->FindRdrift(fpTmpTrack->GetOrigin(), fpTmpTrack->GetSlope(), p_hit->GetWirePos(), p_hit->GetWireDir(),true);
      p_hit->SetResidual(res);
      p_hit->SetChi2(res*res/p_hit->GetSigma()/p_hit->GetSigma());
      chi2red+=p_hit->GetChi2();
    }
  }

  if(nselhit<3 || nselhit!=fpTmpTrack->GetHitsNtot() ){
    cout<<"TABMactNtuTrack::ComputeDataAll::Error!!!  nselhit="<<nselhit<<"   fpTmpTrack->GetHitsNx()="<<fpTmpTrack->GetHitsNx()<<"   fpTmpTrack->GetHitsNy()="<<fpTmpTrack->GetHitsNy()<<endl;
    return kTRUE;
  }
  if(fNowView==0)
    fpTmpTrack->SetChiSquareY(chi2red/(nselhit-2.));
  else
    fpTmpTrack->SetChiSquareX(chi2red/(nselhit-2.));

  if (FootDebugLevel(2))
      cout<<"TABMactNtuTrack::ComputeDataAll finished: track has been reconstructed,  chi2red="<<chi2red/(nselhit-2.)<<"  fNowView="<<fNowView<<endl;

  return kFALSE;
}

//combine the tracks reconstructed for each view
void TABMactNtuTrack::CombineTrack(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr, TABMntuTrack* p_ntutrk){

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
