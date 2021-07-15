#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TBrowser.h>


#include "TAGparGeo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGactTreeReader.hxx"

#include "TABMparGeo.hxx"
#include "TABMparConf.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TABMntuTrack.hxx"

#include "TAGgeoTrafo.hxx"


using namespace std;
using namespace TMath;

void evalt0fromreco(TString in_filename, Bool_t evalreso=false){

  TFile *infile = new TFile(in_filename.Data(),"READ");
  if(!infile->IsOpen()){
    cout<<"I cannot open "<<in_filename.Data()<<endl;
    return;
  }

  infile->cd();
  infile->ls();

  //calculate T0:
  TH1D *tdcraw, *reso;
  for(Int_t i=0;i<36;i++){
    tdcraw=((TH1D*)gDirectory->Get(Form("BM/bmraw/TdcLessSyncCha_%d",i)));        //for bmgsi2020std.root
    // tdcraw=((TH1D*)gDirectory->Get(Form("BM/bmraw/bmRawTdcLessSyncCha_%d",i))); //for gsi2021 output
    Int_t start_bin=-1000;
    Int_t peak_bin=tdcraw->GetMaximumBin();
    for(Int_t j=tdcraw->GetMaximumBin();j>0;j--)
      if(tdcraw->GetBinContent(j)>tdcraw->GetBinContent(tdcraw->GetMaximumBin())/10.)
        start_bin=j;
        cout<<"i="<<i<<"  t0="<<tdcraw->GetBinCenter(start_bin)<<endl;
    if(start_bin==-1000)
      cout<<"WARNING: check if your tdc bin is ok!!! "<<"  tdcchannel="<<i<<"   peak_bin="<<peak_bin<<"   start_bin="<<start_bin<<"   MaximumBin="<<tdcraw->GetMaximumBin()<<endl;
    if(tdcraw->GetEntries()<1000)
      cout<<"WARNING: the number of hits is low:   tdcchannel="<<i<<"  i="<<i<<" number of hits:"<<tdcraw->GetEntries()<<endl;
  }


  if(evalreso){
    TH1D*  fpParNewDistRes = new TH1D("bmParNewDistRes","New resolution distribution; Drift distance [cm]; Resolution [cm]", 100, 0., 1.);
    //evaluate the resolution parameters:
    TF1* gaus=new TF1("gaus","gaus",-0.1,0.1);
    for(Int_t i=0;i<100;i++){
      // reso=((TH1D*)gDirectory->Get(Form("BM/bmTrackResDist_%d",i)));        //for gsi2021
      reso=((TH1D*)gDirectory->Get(Form("BM/bmTrackResDist_%d",i)));          //for  bmgsi2020std.root
      if(reso->GetEntries()>300){
        gaus->SetParameters(reso->GetEntries(),reso->GetMean(),reso->GetStdDev());
        reso->Fit(gaus,"QR+");
        fpParNewDistRes->SetBinContent(i+1,gaus->GetParameter(2));
      }else{
        cout<<"WARNING:: numevents < minimum"<<endl;
      }
    }
    fpParNewDistRes->Draw();
    //fit new parameters:
    TF1 *resofunc=new TF1("BMNewResoFunc","pol10",0.,0.8);
    fpParNewDistRes->Fit(resofunc,"QR+");
    cout<<"Resolution_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
    cout<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
    for(Int_t i=0;i<resofunc->GetNpar();i++)
      cout<<resofunc->GetParameter(i)<<"   ";
    cout<<endl;
  }

  return ;
}
