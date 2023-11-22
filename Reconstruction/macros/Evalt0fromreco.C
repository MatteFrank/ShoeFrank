#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TProfile.h>
#include <TBrowser.h>
#include <cstdio>
#include <iostream>


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

void Evalt0fromreco(TString in_filename="reco.root",Bool_t t0eval=false, Bool_t evalreso=true, Bool_t selfstrel=false, TString outtxt=""){

  if(t0eval==false && evalreso==false && selfstrel==false){
    cout<<"All the flags are set as false... I should no nothing... check the flags, bye!"<<endl;
    return;
  }

  TFile *infile = new TFile(in_filename.Data(),"READ");
  if(!infile->IsOpen()){
    cout<<"I cannot open "<<in_filename.Data()<<endl;
    return;
  }
  if(outtxt!="")
    freopen(outtxt.Data(),"a",stdout);
  
  TString outfilename="Evalt0_"+in_filename;
  TFile *file_out = new TFile(outfilename,"RECREATE");  

  //calculate T0:
  if(t0eval){
    for(Int_t i=0;i<36;i++){
      TH1D *tdcraw=((TH1D*)infile->Get(Form("BM/bmRawTdcLessSyncCha_%d",i)));        //for bmgsi2020std.root
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
  }

  //evaluate resolution
  if(evalreso || selfstrel){
    TH1D *reso, *fpParNewDistRes;
    vector<Double_t> streltime, streldist;
    TH2D *bmres2d=((TH2D*)infile->Get("BM/bmTrackResidual"));
    TH1D *oldstrel=((TH1D*)infile->Get("BM/bmParSTrel"));
    if(evalreso)
      fpParNewDistRes = new TH1D("bmParNewDistRes","New resolution distribution; Drift distance [cm]; Resolution [cm]", 100, 0., 1.);
    if(selfstrel){
      oldstrel->Clone("bmOldSTRel");
    }
    TF1* gaus=new TF1("gaus","gaus",-0.1,0.1);
    bmres2d->Clone("BMTrackResiduals");
    for(int i=0;i<bmres2d->GetNbinsY();i++){
      reso=bmres2d->ProjectionX(Form("profX_%d",i),i,i);
      if(reso->GetEntries()>300){
        gaus->SetParameters(reso->GetEntries(),reso->GetMean(),reso->GetStdDev());
        reso->Fit(gaus,"QR+");
        if(evalreso)
          fpParNewDistRes->SetBinContent(i+1,gaus->GetParameter(2));
        if(selfstrel){
          streltime.push_back(oldstrel->GetBinCenter(oldstrel->FindFirstBinAbove(bmres2d->GetYaxis()->GetBinCenter(i))));
          streldist.push_back(oldstrel->GetBinContent(streltime.back())-gaus->GetParameter(1));
        }
      }else{
        cout<<"WARNING:: numevents < minimum  bin="<<i<<endl;
      }
    }
    
    //evaluate the resolution parameters from old output:
    /*  
    //~ for(Int_t i=0;i<100;i++){
      //~ // reso=((TH1D*)gDirectory->Get(Form("BM/bmTrackResDist_%d",i)));        //for gsi2021
      //~ reso=((TH1D*)gDirectory->Get(Form("BM/bmTrackResDist_%d",i)));          //for  bmgsi2020std.root
      //~ if(reso->GetEntries()>300){
        //~ gaus->SetParameters(reso->GetEntries(),reso->GetMean(),reso->GetStdDev());
        //~ reso->Fit(gaus,"QR+");
        //~ fpParNewDistRes->SetBinContent(i+1,gaus->GetParameter(2));
      //~ }else{
        //~ cout<<"WARNING:: numevents < minimum"<<endl;
      //~ }
    //~ }
    */
    
    //~ fpParNewDistRes->Draw();
    if(evalreso){
      //fit new parameters:
      TF1 *resofunc=new TF1("BMNewResoFunc","pol4",0.,0.8);
      fpParNewDistRes->Fit(resofunc,"QR+");
      cout<<"Resolution_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
      for(Int_t i=0;i<resofunc->GetNpar();i++)
        cout<<resofunc->GetParameter(i)<<"   ";
      cout<<endl<<endl;
    }
    
    if(selfstrel){
      TGraph *fpNewSTrel=new TGraph(streldist.size(),&streltime[0], &streldist[0]);
      fpNewSTrel->SetTitle("New Space time relations;Time [ns];Drift distance [cm]");
      fpNewSTrel->SetMarkerStyle(8);
      file_out->Append(fpNewSTrel);
      TF1 *strelfunc=new TF1("BMNewSTrel","pol4",0.,oldstrel->GetXaxis()->GetXmax());
      strelfunc->FixParameter(0,0);
      fpNewSTrel->Fit(strelfunc,"QR+");
      cout<<"Strel_function:  "<<strelfunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"STrel_number_of_parameters:  "<<strelfunc->GetNpar()<<endl;
      for(Int_t i=0;i<strelfunc->GetNpar();i++)
        cout<<strelfunc->GetParameter(i)<<"   ";
      cout<<endl;
    }
  }

  file_out->Write();
  file_out->Close();
  cout<<"program executed; output file= "<<outfilename.Data()<<endl;

  return ;
}
