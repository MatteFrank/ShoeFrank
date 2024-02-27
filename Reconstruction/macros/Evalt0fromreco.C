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
  
  Int_t pos = in_filename.Last('/');
  TString outfilename(in_filename);
  outfilename.Remove(1, pos);
  outfilename.Prepend("Evalt0_");
  TFile *file_out = new TFile(outfilename,"RECREATE");    
  
  //calculate T0:
  if(t0eval){
    TH1D *tdcmaj, *tdcfrag;
    for(Int_t i=0;i<36;i++){
      tdcmaj= ((TH1D*)infile->Get(Form("BM/bmRawTdcLessSyncCha_%d",i)));        //for bmgsi2020std.root
      tdcfrag=((TH1D*)infile->Get(Form("BM/bmRawTdcFragLessSyncCha_%d",i)));        //for bmgsi2020std.root
      tdcmaj=(TH1D*)tdcmaj->Clone(Form("bmRawTdcLessSyncCha_%d",i));
      tdcfrag=((TH1D*)infile->Clone(Form("bmRawTdcFragLessSyncCha_%d",i)));        //for bmgsi2020std.root
      // tdcraw=((TH1D*)gDirectory->Get(Form("BM/bmraw/bmRawTdcLessSyncCha_%d",i))); //for gsi2021 output
      Int_t start_bin=-1000, end_bin=-1000;
      for(Int_t j=tdcmaj->GetMaximumBin();j>0;j--)
        if(tdcmaj->GetBinContent(j)>tdcmaj->GetBinContent(tdcmaj->GetMaximumBin())/10.)
          start_bin=j;
      if(start_bin>0){
        for(Int_t j=start_bin+1;j<=tdcmaj->GetMaximumBin();j++){
          if(tdcmaj->GetBinContent(j)<tdcmaj->GetBinContent(j-1)){
            end_bin=j-1;
            break;
          }
        }
        if(end_bin==-1000)
          end_bin=tdcmaj->GetMaximumBin();
      }else{
        cout<<"ERROR in i="<<i<<"  start_bin="<<start_bin<<endl;
      }
      if(start_bin!=-1000 && end_bin!=-1000){
        //~ cout<<"i="<<i<<"  t0="<<tdcmaj->GetBinCenter(start_bin)<<endl;
        cout<<"cellid= "<<i<<"  T0_time= "<<(tdcmaj->GetBinCenter(start_bin)+tdcmaj->GetBinCenter(end_bin))/2.<<"  to_Fragtime=  "<<10<<endl;
      }
      if(start_bin==-1000 || end_bin==-1000)
        cout<<"WARNING: check if your tdc bin is ok!!! "<<"  tdcchannel="<<i<<"   start_bin="<<start_bin<<"  end_bin="<<end_bin<<"   MaximumBin="<<tdcmaj->GetMaximumBin()<<endl;
      if(tdcmaj->GetEntries()<1000)
        cout<<"WARNING: the number of hits is low:   tdcchannel="<<i<<"  i="<<i<<" number of hits:"<<tdcmaj->GetEntries()<<endl;
    }
  }

  //evaluate resolution
  if(evalreso || selfstrel){
    TH1D *reso, *fpParNewDistResFit, *fpParNewDistResRaw,*fpParNewDistResFitRawMean, *fpReSkew, *fpPullCheckFitMean, *fpPullCheckFitDevStd, *fpPullCheckRawMean, *fpPullCheckRawDevStd, *fpResFitMean, *fpResRawMean, *fpResFromPull, *fpResMaxPullOldres;
    TH1D *fpResTimeFitMean, *fpParTimeNewDistResFit, *fpParTimeNewDistResRaw, *fpResTimeRawMean;
    vector<Double_t> streltime, streldist, strelrawdist, strelnewdist, strelnewtime, streltimetime, streltimedist, streltimerawdist;
    TH2D *bmres2d=((TH2D*)infile->Get("BM/bmTrackResidualSelected"));
    TH2D *bmrestime2d=((TH2D*)infile->Get("BM/bmTrackResidualTimeSelected"));
    TH2D *bmpull2d=((TH2D*)infile->Get("BM/bmPulls"));
    TH1D *oldstrel=((TH1D*)infile->Get("BM/bmParSTrel"));
    TH1D *oldparres=((TH1D*)infile->Get("BM/bmParResolution"));
    TF1* oldresfunc=new TF1("oldresfunc","pol4",0.,1.);
    oldresfunc->SetParameters( 0,   0.0120769  , -0.0870719  , 0.422187  , -0.882696  , 0.640147   );
    if(evalreso){
      fpResFitMean = new TH1D("bmResFitMean","Fitted Residual mean values; Drift distance [cm];Mean [cm]", bmres2d->GetNbinsY(), 0., bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      fpResRawMean = new TH1D("bmResRawMean","Fitted Residual mean values; Drift distance [cm];Mean [cm]", bmres2d->GetNbinsY(), 0., bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      fpParNewDistResFit = new TH1D("bmParNewDistResFit","New fitted resolution distribution; Drift distance [cm];Resolution [cm]", bmres2d->GetNbinsY(), 0., bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      fpParNewDistResFitRawMean = new TH1D("bmParNewDistResFitRawMean","New fitted resolution distribution; Drift distance [cm];Resolution [cm]", bmres2d->GetNbinsY(), 0., bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      fpParNewDistResRaw = new TH1D("bmParNewDistResRaw","New raw resolution distribution; Drift distance [cm];Resolution [cm]", bmres2d->GetNbinsY(), 0., bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      fpReSkew = new TH1D("bmResSkew","Residual plot skewness; Drift distance [cm];Resolution [cm]", bmres2d->GetNbinsY(), 0., bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      fpPullCheckFitMean = new TH1D("bmPullCheckFitMean","BM Pull distribution Fit Mean values; Drift distance [cm];Mean value[cm]", bmpull2d->GetNbinsY(), 0., bmpull2d->GetYaxis()->GetBinCenter(bmpull2d->GetYaxis()->GetLast()));
      fpPullCheckFitDevStd = new TH1D("bmPullCheckFitDevStd","BM Pull distribution Fit DevStd values; Drift distance [cm];DevStd[cm]", bmpull2d->GetNbinsY(), 0., bmpull2d->GetYaxis()->GetBinCenter(bmpull2d->GetYaxis()->GetLast()));
      fpPullCheckRawMean = new TH1D("bmPullCheckRawMean","BM Pull distribution Fit Mean values; Drift distance [cm];Mean value[cm]", bmpull2d->GetNbinsY(), 0., bmpull2d->GetYaxis()->GetBinCenter(bmpull2d->GetYaxis()->GetLast()));
      fpPullCheckRawDevStd = new TH1D("bmPullCheckRawDevStd","BM Pull distribution Fit DevStd values; Drift distance [cm];DevStd[cm]", bmpull2d->GetNbinsY(), 0., bmpull2d->GetYaxis()->GetBinCenter(bmpull2d->GetYaxis()->GetLast()));
      fpResFromPull = new TH1D("bmResFromPull","BM resolution from Pull distribution;Drift distance [cm];Resolution", bmpull2d->GetNbinsY(), 0., bmpull2d->GetYaxis()->GetBinCenter(bmpull2d->GetYaxis()->GetLast()));
      fpResMaxPullOldres = new TH1D("bmResMaxPullOldres","BM resolution from Pull distribution and old res;Drift distance [cm];Resolution", bmpull2d->GetNbinsY(), 0., bmpull2d->GetYaxis()->GetBinCenter(bmpull2d->GetYaxis()->GetLast()));
      fpResTimeFitMean = new TH1D("bmResTimeFitMean","Fitted Residual mean values; Drift time [ns];Mean [cm]", bmrestime2d->GetNbinsY(), 0., bmrestime2d->GetYaxis()->GetBinCenter(bmrestime2d->GetYaxis()->GetLast()));
      fpParTimeNewDistResFit = new TH1D("bmParTimeNewDistResFit","New fitted resolution distribution; Drift Time [ns];Resolution [cm]", bmrestime2d->GetNbinsY(), 0., bmrestime2d->GetYaxis()->GetBinCenter(bmrestime2d->GetYaxis()->GetLast()));      
      fpParTimeNewDistResRaw = new TH1D("bmParTimeNewDistResRaw","New raw resolution distribution; Drift Time [ns];Resolution [cm]", bmrestime2d->GetNbinsY(), 0., bmrestime2d->GetYaxis()->GetBinCenter(bmrestime2d->GetYaxis()->GetLast()));
      fpResTimeRawMean = new TH1D("bmResTimeRawMean","Fitted Residual mean values; Drift Time [ns];Mean [cm]", bmrestime2d->GetNbinsY(), 0., bmrestime2d->GetYaxis()->GetBinCenter(bmrestime2d->GetYaxis()->GetLast()));          
    }
    if(selfstrel){
      oldstrel->Clone("bmOldSTRel");
      oldparres->Clone("bmOldResolution");
      oldparres->Fit(oldresfunc,"QR+");
    }
    TF1 *oldstrelbyhand=new TF1("OldStRelByHand","([p0]*x+[p1]*x*x+[p2]*x*x*x+[p3]*x*x*x*x)",0.,0.8);
    oldstrelbyhand->SetParameters(0.0073687  , -3.98333e-05 ,  1.16193e-07  , -1.20181e-10);
    TF1* gaus=new TF1("gaus","gaus",-0.02,0.02);
    bmres2d->Clone("BMTrackResiduals");
    bmpull2d->Clone("bmPulls");
    bmrestime2d->Clone("BMTrackTimeResiduals");
    for(int i=0;i<bmres2d->GetNbinsY();i++){
      reso=bmres2d->ProjectionX(Form("ResProfX_%d",i),i,i);
      if(reso->GetEntries()>100){
        gaus->SetParameters(reso->GetEntries(),reso->GetMean(),reso->GetStdDev());
        reso->Fit(gaus,"QR+");
        if(evalreso){
          fpResFitMean->SetBinContent(i+1,gaus->GetParameter(1));
          fpParNewDistResFit->SetBinContent(i+1,gaus->GetParameter(2));
          fpParNewDistResFitRawMean->SetBinContent(i+1,(gaus->GetParameter(2)+reso->GetStdDev())/2.);
          fpParNewDistResRaw->SetBinContent(i+1,reso->GetStdDev());
          fpResRawMean->SetBinContent(i+1,reso->GetMean());
          fpReSkew->SetBinContent(i+1,reso->GetSkewness());
        }
        if(selfstrel){
          streltime.push_back( bmrestime2d->GetYaxis()->GetBinCenter(i));
          streldist.push_back(oldstrel->GetBinContent(streltime.back())-gaus->GetParameter(1));
          strelrawdist.push_back(oldstrel->GetBinContent(streltime.back())-reso->GetMean());
          strelnewdist.push_back(oldstrelbyhand->Eval(i*0.01)-gaus->GetParameter(1));
          strelnewtime.push_back(i*0.01);
        }
      }else{
        cout<<"WARNING:: numevents < minimum  bin="<<i<<endl;
      }
    }
    //time
    for(int i=0;i<bmrestime2d->GetNbinsY();i++){
      reso=bmrestime2d->ProjectionX(Form("ResTimeProfX_%d",i),i,i);
      if(reso->GetEntries()>100){
        gaus->SetParameters(reso->GetEntries(),reso->GetMean(),reso->GetStdDev());
        reso->Fit(gaus,"QR+");
        if(evalreso){
          //for checking, not useful for parameters
          fpResTimeFitMean->SetBinContent(i+1,gaus->GetParameter(1));
          fpParTimeNewDistResFit->SetBinContent(i+1,gaus->GetParameter(2));
          fpParTimeNewDistResRaw->SetBinContent(i+1,reso->GetStdDev());
          fpResTimeRawMean->SetBinContent(i+1,reso->GetMean());
        }
        if(selfstrel){
          streltimetime.push_back(oldstrel->GetBinCenter(oldstrel->FindFirstBinAbove(bmres2d->GetYaxis()->GetBinCenter(i))));
          streltimedist.push_back(oldstrel->GetBinContent(streltimetime.back())-gaus->GetParameter(1));
          streltimerawdist.push_back(oldstrel->GetBinContent(streltimetime.back())-reso->GetMean());
        }
      }else{
        cout<<"WARNING:: numevents < minimum  bin="<<i<<endl;
      }
    }    
    for(int i=0;i<bmpull2d->GetNbinsY();i++){
      reso=bmpull2d->ProjectionX(Form("PullProfX_%d",i),i,i);
      gaus->SetRange(-3,3);
      if(reso->GetEntries()>300){
        gaus->SetParameters(reso->GetEntries(),reso->GetMean(),reso->GetStdDev());
        reso->Fit(gaus,"QR+");
        fpPullCheckFitMean->SetBinContent(i+1,gaus->GetParameter(1));
        fpPullCheckFitDevStd->SetBinContent(i+1,gaus->GetParameter(2));
        fpPullCheckRawMean->SetBinContent(i+1,reso->GetMean());
        fpPullCheckRawDevStd->SetBinContent(i+1,reso->GetStdDev());
        fpResFromPull->SetBinContent(i+1, (gaus->GetParameter(2)>0) ? oldresfunc->Eval(fpResFromPull->GetXaxis()->GetBinCenter(i+1))*gaus->GetParameter(2) : oldresfunc->Eval(fpResFromPull->GetXaxis()->GetBinCenter(i+1)));
        fpResMaxPullOldres->SetBinContent(i+1, (gaus->GetParameter(2) > 1) ? oldresfunc->Eval(fpResFromPull->GetXaxis()->GetBinCenter(i+1))*gaus->GetParameter(2) : oldresfunc->Eval(fpResFromPull->GetXaxis()->GetBinCenter(i+1)));
      }else{
        cout<<"WARNING:: numevents < minimum  bin="<<i<<endl;
        fpResMaxPullOldres->SetBinContent(i+1, oldresfunc->Eval(fpResFromPull->GetXaxis()->GetBinCenter(i+1)));
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
      fpParNewDistResFit->Fit(resofunc,"QR+");
      cout<<"Resolution_Fitted_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
      for(Int_t i=0;i<resofunc->GetNpar();i++)
        cout<<resofunc->GetParameter(i)<<"   ";
      cout<<endl<<endl;
      fpParNewDistResRaw->Fit(resofunc,"QR+");
      cout<<"Resolution_Raw_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
      for(Int_t i=0;i<resofunc->GetNpar();i++)
        cout<<resofunc->GetParameter(i)<<"   ";
      cout<<endl<<endl;
      fpParNewDistResFitRawMean->Fit(resofunc,"QR+");
      cout<<"Resolution_RawFitMean_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
      for(Int_t i=0;i<resofunc->GetNpar();i++)
        cout<<resofunc->GetParameter(i)<<"   ";
      cout<<endl<<endl;
      //pull
      fpResFromPull->Fit(resofunc,"QR+");
      cout<<"Resolution_FromPull_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
      for(Int_t i=0;i<resofunc->GetNpar();i++)
        cout<<resofunc->GetParameter(i)<<"   ";
      cout<<endl<<endl;      
      fpResMaxPullOldres->Fit(resofunc,"QR+");
      cout<<"Resolution_MaxPull_function:  "<<resofunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"Resolution_number_of_parameters:  "<<resofunc->GetNpar()<<endl;
      for(Int_t i=0;i<resofunc->GetNpar();i++)
        cout<<resofunc->GetParameter(i)<<"   ";
      cout<<endl<<endl;      
    }
    
    if(selfstrel){
      TGraph *fpNewSTrel=new TGraph(streldist.size(),&streltime[0], &streldist[0]);
      fpNewSTrel->SetTitle("New Space time relations from fit;Time [ns];Drift distance [cm]");
      fpNewSTrel->SetName("NewSTrel");
      fpNewSTrel->SetMarkerStyle(8);
      file_out->Append(fpNewSTrel);
      TF1 *strelfunc=new TF1("BMNewSTrel","pol4",0.,bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      strelfunc->FixParameter(0,0);
      fpNewSTrel->Fit(strelfunc,"QR+");
      cout<<"Strel_function:  "<<strelfunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"STrel_number_of_parameters:  "<<strelfunc->GetNpar()<<endl;
      for(Int_t i=0;i<strelfunc->GetNpar();i++)
        cout<<strelfunc->GetParameter(i)<<"   ";
      cout<<endl;
      
      TGraph *fpNewRawSTrel=new TGraph(streldist.size(),&streltime[0], &strelrawdist[0]);
      fpNewRawSTrel->SetTitle("New Space time relations from raw;Time [ns];Drift distance [cm]");
      fpNewRawSTrel->SetName("NewRawSTrel");
      fpNewRawSTrel->SetMarkerStyle(8);
      file_out->Append(fpNewRawSTrel);
      TF1 *rawstrelfunc=new TF1("BMNewRawSTrel","pol4",0.,bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      rawstrelfunc->FixParameter(0,0);
      fpNewRawSTrel->Fit(rawstrelfunc,"QR+");
      cout<<"Strel_function:  "<<rawstrelfunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"STrel_number_of_parameters:  "<<rawstrelfunc->GetNpar()<<endl;
      for(Int_t i=0;i<rawstrelfunc->GetNpar();i++)
        cout<<rawstrelfunc->GetParameter(i)<<"   ";
      cout<<endl;
      
      TGraph *fpNewNewSTrel=new TGraph(strelnewdist.size(),&strelnewtime[0], &strelnewdist[0]);
      fpNewNewSTrel->SetTitle("New Space time relations from raw;Time [ns];Drift distance [cm]");
      fpNewNewSTrel->SetName("NewNewSTrel");
      fpNewNewSTrel->SetMarkerStyle(8);
      file_out->Append(fpNewNewSTrel);
      TF1 *newnewstrelfunc=new TF1("newnewstrelfunc","pol4",0.,bmres2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      newnewstrelfunc->FixParameter(0,0);
      fpNewNewSTrel->Fit(newnewstrelfunc,"QR+");
      cout<<"NewNewStrel_function:  "<<newnewstrelfunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"STrel_number_of_parameters:  "<<newnewstrelfunc->GetNpar()<<endl;
      for(Int_t i=0;i<newnewstrelfunc->GetNpar();i++)
        cout<<newnewstrelfunc->GetParameter(i)<<"   ";
      cout<<endl;
      
      TGraph *fpNewTimeSTrel=new TGraph(streltimedist.size(),&streltimetime[0], &streltimedist[0]);
      fpNewTimeSTrel->SetTitle("New Space time relations from time dist;Time [ns];Drift distance [cm]");
      fpNewTimeSTrel->SetName("NewTimeSTrel");
      fpNewTimeSTrel->SetMarkerStyle(8);
      file_out->Append(fpNewTimeSTrel);
      TF1 *newtimestrelfunc=new TF1("newtimestrelfunc","pol4",0.,bmrestime2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      newtimestrelfunc->FixParameter(0,0);
      fpNewTimeSTrel->Fit(newtimestrelfunc,"QR+");
      cout<<"NewTimeStrel_function:  "<<newtimestrelfunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"STrel_number_of_parameters:  "<<newtimestrelfunc->GetNpar()<<endl;
      for(Int_t i=0;i<newtimestrelfunc->GetNpar();i++)
        cout<<newtimestrelfunc->GetParameter(i)<<"   ";
      cout<<endl;
      
      TGraph *fpNewTimeRawSTrel=new TGraph(streltimerawdist.size(),&streltimetime[0], &streltimerawdist[0]);
      fpNewTimeRawSTrel->SetTitle("New Space time relations from time raw dist;Time [ns];Drift distance [cm]");
      fpNewTimeRawSTrel->SetName("NewTimeRawSTrel");
      fpNewTimeRawSTrel->SetMarkerStyle(8);
      file_out->Append(fpNewTimeRawSTrel);
      TF1 *newtimerawstrelfunc=new TF1("newtimerawstrelfunc","pol4",0.,bmrestime2d->GetYaxis()->GetBinCenter(bmres2d->GetYaxis()->GetLast()));
      newtimerawstrelfunc->FixParameter(0,0);
      fpNewTimeRawSTrel->Fit(newtimerawstrelfunc,"QR+");
      cout<<"NewTimeRawStrel_function:  "<<newtimerawstrelfunc->GetFormula()->GetExpFormula().Data()<<endl;
      cout<<"STrel_number_of_parameters:  "<<newtimerawstrelfunc->GetNpar()<<endl;
      for(Int_t i=0;i<newtimerawstrelfunc->GetNpar();i++)
        cout<<newtimerawstrelfunc->GetParameter(i)<<"   ";
      cout<<endl;
    
    }
  }

  file_out->Write();
  file_out->Close();
  std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());  
  cout<<"program executed; output file= "<<outfilename.Data()<<"   program finished at: "<<std::ctime(&end_time)<<endl;

  return ;
}
