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
                                 TAGparaDsc* dsctggeo)
  : TAGaction(name, "TABMactNtuTrack - Track finder for BM"),
    fpNtuTrk(dscntutrk),
    fpNtuHit(dscnturaw),
    fpBMGeo(dscbmgeo),
    fpBMCon(dscbmcon),
    fpTGgeo(dsctggeo)
{
  if (FootDebugLevel(1))
   cout<<"TABMactNtuTrack::default constructor::Creating the Beam Monitor Track ntuplizer"<<endl;
  AddDataOut(fpNtuTrk, "TABMntuTrack");
  AddDataIn(fpNtuHit,  "TABMntuRaw");
  AddPara(fpBMGeo,  "TABMparGeo");
  AddPara(fpBMCon,  "TABMparCon");

  TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();

  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  fpTmpTrack=new TABMntuTrackTr();

  //new chi2 ROOT based
  fpFunctor= new ROOT::Math::Functor(this,&TABMactNtuTrack::EvaluateChi2,2);
  fpMinimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili2");

  //legendre
  fLegPolSum=new TH2F("fLegPolSum","Legendre polynomial space; m; q[cm]",p_bmcon->GetLegMBin(),-p_bmcon->GetLegMRange(),p_bmcon->GetLegMRange(), p_bmcon->GetLegRBin(),-p_bmcon->GetLegRRange(),p_bmcon->GetLegRRange());

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuTrack::~TABMactNtuTrack(){
delete fpTmpTrack;
delete fpMinimizer;
delete fpFunctor;
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TABMactNtuTrack::CreateHistogram()
{
   DeleteHistogram();

   fpResTot = new TH2F("BM_Track_bm_residual_tot","Residual vs Rdrift; Residual [cm]; Measured rdrift [cm]", 6000, -0.3, 0.3,250 , 0., 1.);
   AddHistogram(fpResTot);
   fpHisMap = new TH2F("BM_Track_bmMap","BM - Position of a single track event at target center", 250, -3., 3.,250 , -3, 3);
   AddHistogram(fpHisMap);
   fpHisMylar12d = new TH2F("BM_Track_bmmylar1_2d","BM - Position of a single track event on the BM center plane", 500, -3., 3.,500 , -3., 3.);
   AddHistogram(fpHisMylar12d);
   fpHisAngleX = new TH1F("BM_Track_AngleX","BM track XZ Angular spread; XZ Angle [rad]; Events", 100, -0.5, 0.5);
   AddHistogram(fpHisAngleX);
   fpHisAngleY = new TH1F("BM_Track_AngleY","BM track YZ Angular spread; YZ Angle [rad]; Events", 100, -0.5, 0.5);
   AddHistogram(fpHisAngleY);
   fpHisTrackStatus = new TH1I("BM_Track_status","Track status; -2=maxhitcut -1=minhitcut 0=ok 1/2=firedplane 3=hitrejected 4=noconv 5=chi2cut 6=planehitcut 7+=Error; Events", 20, -5, 15);
   AddHistogram(fpHisTrackStatus);
   fpHisNhitTotTrack = new TH1I("BM_Trak_ntothitsxtrack","Number of hits x track; N hits; Events", 30, 0, 30);
   AddHistogram(fpHisNhitTotTrack);
   fpHisNhitXTrack = new TH1I("BM_Trak_nhitsxXtrack","Number of hits x track on the XZ view ; N hits; Events", 16, 0, 16);
   AddHistogram(fpHisNhitXTrack);
   fpHisNhitYTrack = new TH1I("BM_Trak_nhitsxYtrack","Number of hits x track on the YZ view; N hits; Events", 16, 0, 16);
   AddHistogram(fpHisNhitYTrack);
   fpHisNrejhitTrack = new TH1I("BM_Trak_nhitsrejectedxtrack","Number of hits rejected x track of a single track event; N hits; Events", 30, 0, 30);
   AddHistogram(fpHisNrejhitTrack);
   fpHisTrackFakeHit = new TH1I("BM_Trak_fakehits","Selected hits vs fake hits; -1=Lost primary hit 0=Selected primary hit 1=Wrong hit selected; Events", 8, -2, 2);
   AddHistogram(fpHisTrackFakeHit);
   fpHisChi2Red = new TH1F("BM_Track_chi2red","chi2red of a single track event", 1000, 0., 100.);
   AddHistogram(fpHisChi2Red);
   fpHisChi2XZ = new TH1F("BM_Track_chi2xz","chi2red on xz view of a single track event", 1000, 0., 100.);
   AddHistogram(fpHisChi2XZ);
   fpHisChi2YZ = new TH1F("BM_Track_chi2yz","chi2red on yz view of a single track event", 1000, 0., 100.);
   AddHistogram(fpHisChi2YZ);
   fpNtotTrack = new TH1I("BM_NtotTracks","Total number of tracks per event; Number of tracks; evts", 10, 0, 10);
   AddHistogram(fpNtotTrack);
   fpNXtrack = new TH1I("BM_NXtracks","Number of tracks on XZ plane per event; Number of tracks; evts", 10, 0, 10);
   AddHistogram(fpNXtrack);
   fpNYtrack = new TH1I("BM_NYtracks","Number of tracks on YZ plane per event; Number of tracks; evts", 10, 0, 10);
   AddHistogram(fpNYtrack);
   fpTrackAngles = new TH1F("BM_TrackN_angles","Angles btw reco tracks; deg; tracks", 500, 0, 10.);
   AddHistogram(fpTrackAngles);
   fpTrackSep = new TH1F("BM_TrackN_separation","Separation btw multi-tracks of the same event in the BM sys of ref. origin; cm; tracks", 500, 0, 5.);
   AddHistogram(fpTrackSep);

   //control graphs
   TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();
   if(p_bmcon->GetResoFunc()!=nullptr){
     fpParRes=(TH1F*)p_bmcon->GetResoFunc()->GetHistogram()->Clone("BM_parResolution");
     fpParRes->SetTitle("BM input resolution; Drift distance [cm]; Resolution [#mu m]");
     fpParRes->GetXaxis()->SetRange(0,0.8);
     AddHistogram(fpParRes);
   }
   if(p_bmcon->GetSTrelFunc()!=nullptr){
     fpParSTrel=(TH1F*)p_bmcon->GetSTrelFunc()->GetHistogram()->Clone("BM_parSTrel");
     fpParSTrel->SetTitle("Space time resolution; time [ns]; Space [cm]");
     fpParSTrel->GetXaxis()->SetRange(0,0.8);
     AddHistogram(fpParSTrel);
   }
   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuTrack::Action()
{
  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  p_ntutrk->Clear();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
  TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpBMGeo->Object();

  Int_t i_nhit = p_nturaw->GetHitsN();
  Int_t firedview=0, tmp_int;
  //NB.: If the preselection reject the event no track will be saved
  fTrackNum=0;
  fNowView=0;
  vector<TABMntuTrackTr> xtracktr;
  vector<TABMntuTrackTr> ytracktr;
  if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack::Action::start"<<endl;

  //check for some cuts
  if(i_nhit>=p_bmcon->GetMaxHitCut()){
   if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::no possible track!!:the number of hits is too high:  number of hit="<<i_nhit<<"  Maxhitcut="<<p_bmcon->GetMaxHitCut()<<endl;
      p_ntutrk->GetTrackStatus()=-2;
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
       p_ntutrk->GetTrackStatus()=-1;
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
      p_ntutrk->GetTrackStatus()=1;
    else
      p_ntutrk->GetTrackStatus()=2;
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
      TABMntuHit* p_hit = p_nturaw->Hit(i_h);
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
    Int_t nselhit=CheckAssHits(p_bmcon->GetAssHitErr());

    //hit selection done! now check for cuts
    if(nselhit<p_bmcon->GetPlaneHitCut()){
      if(FootDebugLevel(1))
        cout<<"TABMactNtuTrack::The number of hits on one view is less than the minimum required:  min hit per view="<<p_bmcon->GetPlaneHitCut()<<"  number of selected hits="<<nselhit<<"  fTrackNum="<<fTrackNum<<"  fNowView="<<fNowView<<endl;
      if(fTrackNum==0){
        p_ntutrk->GetTrackStatus()=6;
        if(ValidHistogram()){
          fpHisTrackStatus->Fill(6);
          fpNtotTrack->Fill(0);
          fpNXtrack->Fill(0);
          fpNYtrack->Fill(0);
        }
        fpNtuTrk->SetBit(kValid);
        return kTRUE;
      }
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
      fpTmpTrack->SetNhitY(nselhit);
    }else{
      fpTmpTrack->SetSlopeX(fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin));
      fpTmpTrack->SetOriginX(fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin));
      fpTmpTrack->SetNhitX(nselhit);
    }
    tmp_int= NumericalMinimizationDouble();
    if(tmp_int!=0){
      p_ntutrk->GetTrackStatus()=7+tmp_int;
      if(ValidHistogram()){
        fpHisTrackStatus->Fill(7+tmp_int);
        fpNtotTrack->Fill(0);
        fpNXtrack->Fill(0);
        fpNYtrack->Fill(0);
      }
      fpNtuTrk->SetBit(kValid);
      return kTRUE;
    }else if(ComputeDataAll()){
      p_ntutrk->GetTrackStatus()=7;//something is wrong, this should not happen!
      if(ValidHistogram()){
        fpHisTrackStatus->Fill(7);
        fpNtotTrack->Fill(0);
        fpNXtrack->Fill(0);
        fpNYtrack->Fill(0);
      }
      fpNtuTrk->SetBit(kValid);
      return kTRUE;
    }
    //add the reconstructed track
    if(fNowView==0)
      ytracktr.push_back(TABMntuTrackTr(*fpTmpTrack));
    else
      xtracktr.push_back(TABMntuTrackTr(*fpTmpTrack));
    ++fTrackNum;
  }while(fTrackNum<100);

  CombineTrack(ytracktr, xtracktr, p_ntutrk);

  if (ValidHistogram()){
    fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());
    for(Int_t i=0;i<xtracktr.size();++i){
      fpHisChi2XZ->Fill(xtracktr.at(i).GetChiSquareX());
      fpHisNhitXTrack->Fill(xtracktr.at(i).GetNhitX());
    }
    for(Int_t i=0;i<ytracktr.size();++i){
      fpHisChi2YZ->Fill(ytracktr.at(i).GetChiSquareY());
      fpHisNhitYTrack->Fill(ytracktr.at(i).GetNhitY());
    }
    for(Int_t i=0;i<p_ntutrk->GetTracksN();++i){
      TABMntuTrackTr *savedtracktr=p_ntutrk->Track(i);
      TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
      Float_t posZ = geoTrafo->FromGlobalToBMLocal(TVector3(0,0,0)).Z();
      TVector3 pos = savedtracktr->PointAtLocalZ(posZ);
      fpHisMap->Fill(pos[0], pos[1]);
      pos=savedtracktr->PointAtLocalZ(p_bmgeo->GetMylar1().Z());
      fpHisMylar12d->Fill(pos[0], pos[1]);
      fpHisAngleX->Fill(savedtracktr->GetSlope().X()/savedtracktr->GetSlope().Z());
      fpHisAngleY->Fill(savedtracktr->GetSlope().Y()/savedtracktr->GetSlope().Z());
      fpHisChi2Red->Fill(savedtracktr->GetChiSquare());
      fpHisNhitTotTrack->Fill(savedtracktr->GetNhitTot());
      fpHisNrejhitTrack->Fill(p_nturaw->GetHitsN()-savedtracktr->GetNhitTot());
      for(Int_t k=0;k<i;++k){
        fpTrackAngles->Fill(p_ntutrk->Track(i)->GetSlope().Angle(p_ntutrk->Track(k)->GetSlope())*TMath::RadToDeg());
        fpTrackSep->Fill((p_ntutrk->Track(i)->GetOrigin()-p_ntutrk->Track(k)->GetOrigin()).Mag());
      }
    }
    for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
      TABMntuHit* p_hit=p_nturaw->Hit(i);
      if(p_hit->GetIsSelected()>0)
        fpResTot->Fill(p_hit->GetResidual(),p_hit->GetRdrift());
      if( (p_hit->GetIsSelected()>0 && p_hit->GetIsFake()==0) || ((p_hit->GetIsSelected()<=0 && p_hit->GetIsFake()>0) )){
          fpHisTrackFakeHit->Fill(0);
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
  TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
  fLegPolSum->SetBins(p_bmcon->GetLegMBin(),-p_bmcon->GetLegMRange(),p_bmcon->GetLegMRange(), p_bmcon->GetLegRBin(),-p_bmcon->GetLegRRange(),p_bmcon->GetLegRRange());

  Float_t tmp_float;
  for(Int_t i_h = 0; i_h < p_nturaw->GetHitsN(); ++i_h) {
    TABMntuHit* p_hit = p_nturaw->Hit(i_h);
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
Int_t TABMactNtuTrack::CheckAssHits(const Float_t asshiterror) {

  if(FootDebugLevel(2)){
    cout<<"TABMactNtuTrack::CheckAssHits with fNowView="<<fNowView<<" start"<<endl;
      cout<<"first estimate of the track: slope="<<fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin)<<"  Mbinwidth="<<fLegPolSum->GetXaxis()->GetBinWidth(fBestMBin)<<"  R0="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin)<<"  R0binwidth="<<fLegPolSum->GetYaxis()->GetBinWidth(fBestRBin)<<endl;
  }

  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
  Int_t wireplane[6]={-1,-1,-1,-1,-1,-1};
  Int_t selview=0;
  Float_t xvalue, yvaluemax, yvaluemin,yvalue, res, diff, legvalue, mindist, maxdist, a0pos;

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i) {
    TABMntuHit* p_hit = p_nturaw->Hit(i);
    if(p_hit->GetIsSelected()<=0){
      maxdist=p_hit->GetRdrift()+p_hit->GetSigma()*asshiterror;
      mindist=p_hit->GetRdrift()-p_hit->GetSigma()*asshiterror;
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
void TABMactNtuTrack::CheckPossibleHits(Int_t wireplane[], Float_t yvalue, Float_t diff, Float_t res, Int_t &selview, const Int_t hitnum, TABMntuHit* p_hit){

  TABMntuHit*   p_doublehit;
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();

  if(wireplane[p_hit->GetPlane()]==-1){
    if(FootDebugLevel(4))
      cout<<"TABMactNtuTrack::CheckAssHits: hit selected hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  cellid="<<p_hit->GetIdCell()<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestRBin)<<"  diff="<<diff<<"   residual="<<diff/res<<endl;
    wireplane[p_hit->GetPlane()]=hitnum;
    p_hit->SetIsSelected(fTrackNum+1);
    p_hit->SetResidual(diff/res);
    selview++;
  }else{
    p_doublehit=p_nturaw->Hit(wireplane[p_hit->GetPlane()]);
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
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();

  if(fNowView==0){
    vers.SetXYZ(0., params[0], 1.);
    r0.SetXYZ(0., params[1], 0.);
  }else{
    vers.SetXYZ(params[0], 0., 1.);
    r0.SetXYZ(params[1], 0., 0.);
  }

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMntuHit* p_hit = p_nturaw->Hit(i);
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

  TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();

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

    fpTmpTrack->SetIsConverged(1);
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



Double_t TABMactNtuTrack::EvaluateChi2GSI(const double *params){

  Double_t chi2=0, res, newrdrift, newresolution;
  TVector3 vers,r0;
  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();
  TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();

  if(fNowView==0){
    vers.SetXYZ(0., params[0], 1.);
    r0.SetXYZ(0., params[1], 0.);
  }else{
    vers.SetXYZ(params[0], 0., 1.);
    r0.SetXYZ(params[1], 0., 0.);
  }

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMntuHit* p_hit = p_nturaw->Hit(i);
    if(p_hit->GetIsSelected()==(fTrackNum+1) && p_hit->GetView()==fNowView){
      newrdrift=p_bmcon->STrelEval(p_hit->GetTdrift()+params[2]);
      newresolution=p_bmcon->ResoEval(newrdrift);
      res=newrdrift - p_bmgeo->FindRdrift(r0, vers, p_hit->GetWirePos(), p_hit->GetWireDir(), true);
      chi2+=res*res/newresolution/newresolution;
    }
  }
  return chi2;
}



Bool_t TABMactNtuTrack::ComputeDataAll(){

  TABMntuRaw* p_nturaw = (TABMntuRaw*) fpNtuHit->Object();
  TABMparGeo* p_bmgeo  = (TABMparGeo*) fpBMGeo->Object();

  Float_t res, chi2red=0.;
  Int_t nselhit=0;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMntuHit* p_hit=p_nturaw->Hit(i);
    if(p_hit->GetIsSelected()==(fTrackNum+1)){
      ++nselhit;
      res=p_hit->GetRdrift()-p_bmgeo->FindRdrift(fpTmpTrack->GetOrigin(), fpTmpTrack->GetSlope(), p_hit->GetWirePos(), p_hit->GetWireDir(),true);
      p_hit->SetResidual(res);
      p_hit->SetChi2(res*res/p_hit->GetSigma()/p_hit->GetSigma());
      chi2red+=p_hit->GetChi2();
    }
  }

  if(nselhit<3 || nselhit!=fpTmpTrack->GetNhitTot() ){
    cout<<"TABMactNtuTrack::ComputeDataAll::Error!!!  nselhit="<<nselhit<<"   fpTmpTrack->GetNhitX()="<<fpTmpTrack->GetNhitX()<<"   fpTmpTrack->GetNhitY()="<<fpTmpTrack->GetNhitY()<<endl;
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
void TABMactNtuTrack::CombineTrack(vector<TABMntuTrackTr> &ytracktr, vector<TABMntuTrackTr> &xtracktr, TABMntuTrack* p_ntutrk){

  if (FootDebugLevel(2))
    cout<<"TABMactNtuTrack::CombineTrack: start; Number of tracks on yz="<<ytracktr.size()<<"  number of tracks on xz="<<xtracktr.size()<<endl;

  TABMparCon* p_bmcon = (TABMparCon*) fpBMCon->Object();
  TABMntuTrackTr *trk;
  for(Int_t i=0;i<xtracktr.size();++i){
    for(Int_t k=0;k<ytracktr.size();++k){
      TABMntuTrackTr currtracktr=xtracktr.at(i);
      Bool_t status=currtracktr.mergeTrack(ytracktr.at(k));
      if(status==0){
        trk = new((*(p_ntutrk->GetListOfTracks()))[p_ntutrk->GetTracksN()]) TABMntuTrackTr(currtracktr);
        if(p_ntutrk->GetTrackStatus()!=0)
          p_ntutrk->GetTrackStatus()= (trk->GetChiSquare()>=p_bmcon->GetChi2Cut())? 5:0;
      }else if(FootDebugLevel(2))
        cout<<"TABMactNtuTrack::CombineTrack: failed i="<<i<<" k="<<k<<"  status="<<status<<endl;
    }
  }

  if(ValidHistogram()){
    fpNtotTrack->Fill(p_ntutrk->GetTracksN());
    fpNXtrack->Fill(xtracktr.size());
    fpNYtrack->Fill(ytracktr.size());
  }
  if (FootDebugLevel(2))
  cout<<"TABMactNtuTrack::CombineTrack: end; total number of saved tracks="<<p_ntutrk->GetTracksN()<<endl;

  return;
}
