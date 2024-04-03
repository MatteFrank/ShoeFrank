/*!
  \file
  \version $Id: TABMactBaseNtuTrack.cxx,v 1.12 2003/07/08 18:58:35 mueller Exp $
  \brief   Implementation of TABMactBaseNtuTrack.
*/

#include "TABMactBaseNtuTrack.hxx"

using namespace std;
/*!
  \class TABMactBaseNtuTrack TABMactBaseNtuTrack.hxx "TABMactBaseNtuTrack.hxx"
  \brief Track builder for Beam Monitor with a row estimate of the track parameters by the Legendre polynomials method
*/

ClassImp(TABMactBaseNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactBaseNtuTrack::TABMactBaseNtuTrack(const char* name,
                                 TAGdataDsc* dscntutrk,
                                 TAGdataDsc* dscnturaw,
                                 TAGparaDsc* dscbmgeo,
                                 TAGparaDsc* dscbmcon,
                                 TAGparaDsc* dscbmcal)
  : TAGaction(name, "TABMactBaseNtuTrack - Track finder for BM with Legendre method"),
    fpNtuTrk(dscntutrk),
    fpNtuHit(dscnturaw),
    fpParGeo(dscbmgeo),
    fpParCon(dscbmcon),
    fpParCal(dscbmcal)
{
  if (FootDebugLevel(1))
   cout<<"TABMactBaseNtuTrack::default constructor::Creating the Beam Monitor Track ntuplizer"<<endl;

  AddDataIn(fpNtuHit,  "TABMntuHit");
  AddDataOut(fpNtuTrk, "TABMntuTrack");
  AddPara(fpParGeo,  "TABMparGeo");
  AddPara(fpParCon,  "TABMparConf");
  AddPara(fpParCal,  "TABMparCal");

  fpTmpTrack=new TABMtrack();

  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  //legendre
  fLegPolSum=new TH2I("fLegPolSum","Legendre polynomial space; m; q[cm]",p_bmcon->GetLegMBin(),-p_bmcon->GetLegMRange(),p_bmcon->GetLegMRange(), p_bmcon->GetLegRBin(),-p_bmcon->GetLegRRange(),p_bmcon->GetLegRRange());
  //~ fLegPolmatrix=vector<vector<Int_t>> (p_bmcon->GetLegMBin(), std::vector<int>(p_bmcon->GetLegRBin(), 0));
  fbincenterX.clear();
  fbincenterY.clear();
  flegfixvalue.clear();
  for(Int_t k=1;k<fLegPolSum->GetXaxis()->GetNbins();++k){
    fbincenterX.push_back(fLegPolSum->GetXaxis()->GetBinCenter(k));
    flegfixvalue.push_back(sqrt(fbincenterX.back()*fbincenterX.back()+1.));
  }
  for(Int_t v=1;v<fLegPolSum->GetYaxis()->GetNbins();++v){
    fbincenterY.push_back(fLegPolSum->GetYaxis()->GetBinCenter(v));
  }
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactBaseNtuTrack::~TABMactBaseNtuTrack(){
  delete fpTmpTrack;
  delete fLegPolSum;
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TABMactBaseNtuTrack::CreateHistogram()
{
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparCal* p_bmcal = (TABMparCal*) fpParCal->Object();
  DeleteHistogram();

  fpHisSelhHitDrift = new TH1I("bmSelHitDrift","Hit elected for tracking drift distance;Drift distance [cm];N hits", 100, 0., 1.);
  AddHistogram(fpHisSelhHitDrift);
  fpHisSelhHitTime = new TH1I("bmSelitTime","Hit selected for tracking time;Time [ns];N hits", 400, 0., 400.);
  AddHistogram(fpHisSelhHitTime);
  fpHisDischHitDrift = new TH1I("bmDiscHitDrift","Hit not selected for tracking drift distance;Drift distance [cm];N hits", 100, 0., 1.);
  AddHistogram(fpHisDischHitDrift);
  fpHisDischHitTime = new TH1I("bmDiscHitTime","Hit not selected for tracking time;Time [ns];N hits", 400, 0., 400.);
  AddHistogram(fpHisDischHitTime);
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
  fpNtotTrack = new TH1I("bmTrackTotNumber","Total number of tracks per event; Number of tracks; evts", 7, -0.5, 6.5);
  AddHistogram(fpNtotTrack);
  fpNXtrack = new TH1I("bmTrackXzviewNumber","Number of tracks on XZ plane per event; Number of tracks; evts", 7, -0.5, 6.5);
  AddHistogram(fpNXtrack);
  fpNYtrack = new TH1I("bmTrackYzviewNumber","Number of tracks on YZ plane per event; Number of tracks; evts", 7, -0.5, 6.5);
  AddHistogram(fpNYtrack);

  //control graphs
  if(p_bmcal->GetResoFunc()!=nullptr){
    fpParRes=(TH1D*)(p_bmcal->GetResoFunc()->GetHistogram()->Clone("bmParResolution"));
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

  SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactBaseNtuTrack::LegendreFit()
{

  if(FootDebugLevel(1))
    cout<<"TABMactBaseNtuTrack::Action::start"<<endl;

  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();

  //Apply some cuts on the number of HITS. NB.: If the preselection reject the event no track will be saved
  if(ApplyBMCuts())
    return kTRUE;

  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpParGeo->Object();
  Int_t firedview=0, tmp_int;
  fTrackNum=0;
  fNowView=0;
  vector<TABMtrack> xtracktr;
  vector<TABMtrack> ytracktr;

  //loop for track reconstruction on each view
  do{
    firedview=0;
    //check the number of hits for each view
    for(Int_t i_h = 0; i_h < p_nturaw->GetHitsN(); i_h++) {
      TABMhit* p_hit = p_nturaw->GetHit(i_h);
      if(FootDebugLevel(3))
        cout<<"hit="<<i_h<<" plane="<<p_hit->GetPlane()<<"  view="<<p_hit->GetView()<<"  cell="<<p_hit->GetCell()<<"  plane="<<p_hit->GetPlane()<<"  rdrift="<<p_hit->GetRdrift()<<"  isfake="<<p_hit->GetIsFake()<<"  isselected="<<p_hit->GetIsSelected()<<"  fNowView="<<fNowView<<endl;
      if(p_hit->GetView()==fNowView && p_hit->GetIsSelected()<=0)
        ++firedview;
    }

    //Apply cuts on the number of hits
    if(firedview<p_bmcon->GetPlaneHitCut()){
      if(FootDebugLevel(1))
        cout<<"TABMactBaseNtuTrack::no possible track!!: firedview="<<firedview<<"  fNowView="<<fNowView<<"   planehitcut="<<p_bmcon->GetPlaneHitCut()<<"  iteration fTrackNum="<<fTrackNum<<endl;
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
    Int_t nselhit=CheckAssHits(p_bmcon->GetAssHitErr(),  p_bmcon->GetLegRRange()*2./p_bmcon->GetLegRBin());

    //hit selection done! now check for cuts
    if(nselhit<p_bmcon->GetPlaneHitCut()){
      if(FootDebugLevel(1))
        cout<<"TABMactBaseNtuTrack::The number of hits on one view is less than the minimum required:  min hit per view="<<p_bmcon->GetPlaneHitCut()<<"  number of selected hits="<<nselhit<<"  fTrackNum="<<fTrackNum<<"  fNowView="<<fNowView<<endl;
    }
    if(nselhit<p_bmcon->GetPlaneHitCut() && fNowView==0){
      fNowView=1;
      ++fTrackNum;
      continue;
    }
    if(nselhit<p_bmcon->GetPlaneHitCut() && fNowView==1)
      break;

     //ok we can reconstruct the particle track with the Legendre coordinates
    fpTmpTrack->Clean();
    if(fNowView==0){
      fpTmpTrack->SetSlopeY(fBestM);
      fpTmpTrack->SetOriginY(fBestR);
      fpTmpTrack->SethitsNy(nselhit);
    }else{
      fpTmpTrack->SetSlopeX(fBestM);
      fpTmpTrack->SetOriginX(fBestR);
      fpTmpTrack->SethitsNx(nselhit);
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
  }while(fTrackNum<20);

  //if there are two tracks on the same view and only one on the other view, delete the second track since it is probably due to noises
  if(ytracktr.size()==2 && xtracktr.size()==1){
    p_ntutrk->NewPrunedTrack(ytracktr.at(1),0);
    ytracktr.pop_back();
  }else if(xtracktr.size()==2 && ytracktr.size()==1){
    p_ntutrk->NewPrunedTrack(xtracktr.at(1),1);
    xtracktr.pop_back();
  }

  CombineTrack(ytracktr, xtracktr);

  if (ValidHistogram()){
    fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());
    for(Int_t i=0;i<xtracktr.size();++i){
      fpHisNhitXTrack->Fill(xtracktr.at(i).GetHitsNx());
    }
    for(Int_t i=0;i<ytracktr.size();++i){
      fpHisNhitYTrack->Fill(ytracktr.at(i).GetHitsNy());
    }
    for(Int_t i=0;i<p_ntutrk->GetTracksN();++i){
      TABMtrack *savedtracktr=p_ntutrk->GetTrack(i);
      fpHisNhitTotTrack->Fill(savedtracktr->GetHitsNtot());
      fpHisNrejhitTrack->Fill(p_nturaw->GetHitsN()-savedtracktr->GetHitsNtot());
    }
    for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
      TABMhit* p_hit=p_nturaw->GetHit(i);
      if(p_hit->GetIsSelected()<=0){
        fpHisDischHitTime->Fill(p_hit->GetTdrift());
        fpHisDischHitDrift->Fill(p_hit->GetRdrift());
      }else{
        fpHisSelhHitTime->Fill(p_hit->GetTdrift());
        fpHisSelhHitDrift->Fill(p_hit->GetRdrift());
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
    cout<<"TABMactBaseNtuTrack:end of tracking with Legendre method"<<endl;

  fpNtuTrk->SetBit(kValid);
  return kFALSE;
}

Bool_t TABMactBaseNtuTrack::ApplyBMCuts() {
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  Int_t i_nhit = p_nturaw->GetHitsN();
  if(i_nhit>=p_bmcon->GetMaxHitCut()){
   if(FootDebugLevel(1))
      cout<<"TABMactBaseNtuTrack::no possible track!!:the number of hits is too high:  number of hit="<<i_nhit<<"  Maxhitcut="<<p_bmcon->GetMaxHitCut()<<endl;
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
       cout<<"TABMactBaseNtuTrack::no possible track!!:the number of hits is too low:  number of hit="<<i_nhit<<"  Minhitcut="<<p_bmcon->GetMinHitCut()<<endl;
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
  if(p_nturaw->GetTothitsNx()<p_bmcon->GetPlaneHitCut() || p_nturaw->GetTothitsNy()<p_bmcon->GetPlaneHitCut()){
    if(FootDebugLevel(1))
      cout<<"TABMactBaseNtuTrack::no possible track!!: number of xz view hit="<<p_nturaw->GetTothitsNx()<<"  number of yz view hit="<<p_nturaw->GetTothitsNy()<<"   planehitcut="<<p_bmcon->GetPlaneHitCut()<<endl;
    if(p_nturaw->GetTothitsNy()<p_bmcon->GetPlaneHitCut())
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
  return kFALSE;
}

void TABMactBaseNtuTrack::ChargeLegendrePoly(){

  if(FootDebugLevel(2))
    cout<<"TABMactBaseNtuTrack:: start charge legendre pol graphs"<<endl;

  fLegPolSum->Reset("ICESM");
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  fLegPolSum->SetBins(p_bmcon->GetLegMBin(),-p_bmcon->GetLegMRange(),p_bmcon->GetLegMRange(), p_bmcon->GetLegRBin(),-p_bmcon->GetLegRRange(),p_bmcon->GetLegRRange());

  for(Int_t i_h = 0; i_h < p_nturaw->GetHitsN(); ++i_h) {
    TABMhit* p_hit = p_nturaw->GetHit(i_h);
    if(p_hit->GetView()==fNowView && p_hit->GetIsSelected()<=0){
      for(Int_t k=1;k<fLegPolSum->GetXaxis()->GetNbins();++k) {
        Int_t ybin=fLegPolSum->GetYaxis()->FindBin(((fNowView==1) ? p_hit->GetWirePos().X() : p_hit->GetWirePos().Y()) -fbincenterX.at(k-1)*p_hit->GetWirePos().Z()+p_hit->GetRdrift()*flegfixvalue.at(k-1));
        fLegPolSum->SetBinContent(k, ybin, fLegPolSum->GetBinContent(k,ybin)+3);
        fLegPolSum->SetBinContent(k, ybin+1, fLegPolSum->GetBinContent(k,ybin+1)+2);
        fLegPolSum->SetBinContent(k, ybin-1, fLegPolSum->GetBinContent(k,ybin-1)+2);
        fLegPolSum->SetBinContent(k, ybin+2, fLegPolSum->GetBinContent(k,ybin+2)+1);
        fLegPolSum->SetBinContent(k, ybin-2, fLegPolSum->GetBinContent(k,ybin-2)+1);
        ybin=fLegPolSum->GetYaxis()->FindBin(((fNowView==1) ? p_hit->GetWirePos().X() : p_hit->GetWirePos().Y()) -fbincenterX.at(k-1)*p_hit->GetWirePos().Z()-p_hit->GetRdrift()*flegfixvalue.at(k-1));
        fLegPolSum->SetBinContent(k, ybin, fLegPolSum->GetBinContent(k,ybin)+3);
        fLegPolSum->SetBinContent(k, ybin+1, fLegPolSum->GetBinContent(k,ybin+1)+2);
        fLegPolSum->SetBinContent(k, ybin-1, fLegPolSum->GetBinContent(k,ybin-1)+2);
        fLegPolSum->SetBinContent(k, ybin+2, fLegPolSum->GetBinContent(k,ybin+2)+1);
        fLegPolSum->SetBinContent(k, ybin-2, fLegPolSum->GetBinContent(k,ybin-2)+1);
      }
    }
  }

  if(FootDebugLevel(2))
    cout<<"TABMactBaseNtuTrack::legpolsum done"<<endl;

return;
}

// adopted to save the legpolsum th2d, used only for heavy debug purpouse
void TABMactBaseNtuTrack::SaveLegpol(TString addtxt){

  TString tmp_tstring="legpolsum_";
  tmp_tstring+=gTAGroot->CurrentEventNumber();
  tmp_tstring+="_";
  tmp_tstring+=fTrackNum;
  tmp_tstring+=(fNowView==0) ? "_y_" : "_x_";
  tmp_tstring+=addtxt;
  tmp_tstring+=".root";
  fLegPolSum->SaveAs(tmp_tstring.Data());

  return;
}

Int_t TABMactBaseNtuTrack::FindLegendreBestValues(){

  if(FootDebugLevel(2))
    cout<<"TABMactBaseNtuTrack::FindLegendreBestValues: start"<<endl;

  Int_t x,y,z;
  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  fLegPolSum->GetBinXYZ(fLegPolSum->GetMaximumBin(),x,y,z);
  fBestM=fLegPolSum->GetXaxis()->GetBinCenter(x);
  fBestR=fLegPolSum->GetYaxis()->GetBinCenter(y);

return 0;
}


//for Legendre polynomy tracking, set the associated hits
Int_t TABMactBaseNtuTrack::CheckAssHits(const Float_t asshiterror, const Float_t minRerr) {

  if(FootDebugLevel(2)){
    cout<<"TABMactBaseNtuTrack::CheckAssHits with fNowView="<<fNowView<<" start"<<endl;
      cout<<"first estimate of the track: slope="<<fBestM<<"  Mbinwidth="<<"  R0="<<fBestR<<endl;
  }

  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();
  Int_t wireplane[6]={-1,-1,-1,-1,-1,-1};
  Int_t selview=0;
  Float_t xvalue, yvaluemax, yvaluemin,yvalue, res, diff, legvalue, mindist, maxdist, a0pos;

  for(Int_t i=0;i<p_nturaw->GetHitsN();++i) {
    TABMhit* p_hit = p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()<=0 && p_hit->GetView()==fNowView){
      Double_t maxerror= (p_hit->GetSigma()*asshiterror > minRerr) ? p_hit->GetSigma()*asshiterror : minRerr;
      maxdist=p_hit->GetRdrift()+maxerror;
      mindist=p_hit->GetRdrift()-maxerror;
      if(mindist<0)
        mindist=0;
      if(maxdist<0)
        maxdist=0.;
      a0pos=(fNowView==0) ? p_hit->GetWirePos().Y() : p_hit->GetWirePos().X();
      xvalue=fBestM;
      yvaluemax=a0pos -xvalue*p_hit->GetWirePos().Z()+maxdist*sqrt(xvalue*xvalue+1.);
      yvaluemin=a0pos -xvalue*p_hit->GetWirePos().Z()+mindist*sqrt(xvalue*xvalue+1.);
      yvalue=a0pos -xvalue*p_hit->GetWirePos().Z()+p_hit->GetRdrift()*sqrt(xvalue*xvalue+1.);
      res=max(p_hit->GetSigma(),fBestR);
      legvalue=fBestR;
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
            cout<<"TABMactBaseNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<i<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<0<<"  yvalue="<<yvalue<<"  legvalue="<<legvalue<<"  diff="<<diff<<"   res="<<res<<endl;
          p_hit->SetIsSelected(-1);
        }
      }
    }
  }

  if(FootDebugLevel(2))
      cout<<"TABMactBaseNtuTrack::CheckAssHits: done"<<endl;

return selview;
}

//used in CheckAssHits
void TABMactBaseNtuTrack::CheckPossibleHits(Int_t wireplane[], Float_t yvalue, Float_t diff, Float_t res, Int_t &selview, const Int_t hitnum, TABMhit* p_hit){

  TABMhit*   p_doublehit;
  TABMntuHit* p_nturaw = (TABMntuHit*) fpNtuHit->Object();

  if(wireplane[p_hit->GetPlane()]==-1){
    if(FootDebugLevel(4))
      cout<<"TABMactBaseNtuTrack::CheckAssHits: hit selected hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  cellid="<<p_hit->GetIdCell()<<"  yvalue="<<yvalue<<"  bestvalue="<<fBestR<<"  diff="<<diff<<"   residual="<<diff/res<<endl;
    wireplane[p_hit->GetPlane()]=hitnum;
    p_hit->SetIsSelected(fTrackNum+1);
    p_hit->SetResidual(diff/res);
    selview++;
  }else{
    p_doublehit=p_nturaw->GetHit(wireplane[p_hit->GetPlane()]);
    if(p_doublehit->GetResidual()> diff/res){
      if(FootDebugLevel(4))
        cout<<"TABMactBaseNtuTrack::CheckAssHits: hit replaced! selected= hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  cellid="<<p_hit->GetIdCell()<<"  yvalue="<<yvalue<<"  bestvalue="<<fBestR<<"  diff="<<diff<<"   residual="<<diff/res<<"  replace the hit num="<<wireplane[p_hit->GetPlane()]<<endl;
      p_hit->SetIsSelected(fTrackNum+1);
      p_hit->SetResidual(diff/res);
      wireplane[p_hit->GetPlane()]=hitnum;
      p_doublehit->SetIsSelected(-1);
    }else{
      p_hit->SetIsSelected(-1);
      if(FootDebugLevel(4))
        cout<<"TABMactBaseNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  yvalue="<<yvalue<<"  bestvalue="<<fBestR<<"  diff="<<diff<<"   res="<<res<<endl;
    }
  }

  return;
}


//combine the tracks reconstructed for each view
void TABMactBaseNtuTrack::CombineTrack(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr){

  if (FootDebugLevel(2))
    cout<<"TABMactBaseNtuTrack::CombineTrack: start; Number of tracks on yz="<<ytracktr.size()<<"  number of tracks on xz="<<xtracktr.size()<<endl;

  TABMparConf* p_bmcon = (TABMparConf*) fpParCon->Object();
  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  for(Int_t i=0;i<xtracktr.size();++i){
    for(Int_t k=0;k<ytracktr.size();++k){
      TABMtrack currtracktr=xtracktr.at(i);
      Int_t status=currtracktr.mergeTrack(ytracktr.at(k));
      if(status==0){
        TABMtrack *trk=p_ntutrk->NewTrack(currtracktr);
        if(p_ntutrk->GetTrackStatus()!=0)
          p_ntutrk->SetTrackStatus( (trk->GetChiSquare()>=p_bmcon->GetChi2Cut())? 5:0);
      }else {
        p_ntutrk->SetTrackStatus(8);
        if(FootDebugLevel(2))
          cout<<"TABMactBaseNtuTrack::CombineTrack: failed i="<<i<<" k="<<k<<"  status="<<status<<endl;
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
    cout<<"TABMactBaseNtuTrack::CombineTrack: end; total number of saved tracks="<<p_ntutrk->GetTracksN()<<endl;

  return;
}
