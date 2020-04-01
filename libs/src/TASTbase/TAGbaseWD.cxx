/*!
  \file
  \version $Id: TAGbaseWD.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TAGbaseWD.
*/

#include <string.h>
#include <fstream>
#include <bitset>

#include <algorithm>
#include "TString.h"
#include "TAGbaseWD.hxx"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
/*!
  \class TAGbaseWD TAGbaseWD.hxx "TAGbaseWD.hxx"
*/

using namespace std;

ClassImp(TAGbaseWD);


//------------------------------------------+-----------------------------------
//! Default constructor.
TAGbaseWD::TAGbaseWD(): fTime(999999.), fChg(0.), fChId(0),fPedestal(0), fBaseline(0),
			fAmplitude(0),fBoardId(0),fTriggerTypeId(0), fTriggerCellId(-1000),  fMcId(-999){
}


TAGbaseWD::TAGbaseWD(TWaveformContainer *W){

  fChId=W->GetChannelId();
  fBoardId=W->GetBoardId();
  fTriggerTypeId = W->GetTrigType();
  fTriggerCellId = W->GetTriggerCellId();
  fTime = -9999;
  fChg = -99999;
  fAmplitude = -9999;
  fPedestal = -9999999999;
  fBaseline = -9999999999;
  fMcId = -999;
  

}

TAGbaseWD::~TAGbaseWD(){

  
}




double TAGbaseWD::ComputeBaseline(TWaveformContainer *w){

  return TMath::Mean(w->GetVectA().begin()+2, w->GetVectA().begin()+27);

}


double TAGbaseWD::ComputePedestal(TWaveformContainer *w){

  return fBaseline*(w->GetVectT().at(w->GetVectT().size()-1)- w->GetVectT().at(0));

}


double TAGbaseWD::ComputeAmplitude(TWaveformContainer *w){

  return  -((TMath::MinElement(w->GetVectA().size()-5, &w->GetVectA()[5])) - fBaseline);

}



double TAGbaseWD::ComputeCharge(TWaveformContainer *w){

  vector<double> tmp_amp = w->GetVectA();
  vector<double> tmp_time = w->GetVectT();
  
  double charge = 0.;
  double prod=1;
  
  double deltat=0.; //trapezi
  double b1=0, b2=0;
  double a1, a2;
  double m,q,t1,t2,thalf;
  for (int j = 0; j<tmp_amp.size()-1; j++){
    a1 = tmp_amp.at(j);
    a2 = tmp_amp.at(j+1);
    t1 = tmp_time.at(j);
    t2 = tmp_time.at(j+1);
    prod=a1*a2;
    if(prod<0){
      m = (a2-a1)/(t2-t1);
      q = a2-m*t2;
      thalf = -q/m;
      charge+=0.5*a1*(thalf-t1);
      charge+=0.5*a2*(t2-thalf);
    }else{
      charge += 0.5*(a1+a2)*(t2-t1);
    }
  }

  return -(charge - fPedestal);

}




double TAGbaseWD::ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright){
  
  vector<double> time = w->GetVectT();
  vector<double> amp = w->GetVectA();

  //I create the waveform graph
  TGraph wgr(time.size(),&time[0], &amp[0]);


  //I select the time window in which applying the cfd 
  int peak_bin= TMath::LocMin(amp.size(), &amp[0]);
  if(peak_bin<0 || peak_bin>time.size()-1){
    return -10000;
  }
  double timepeak = time.at(peak_bin);
  double tmin= timepeak+tleft;
  double tmax = timepeak+tright;

  
  // I build the bi-polar function 
  double a=0;
  double t = tmin;
  vector<double> time_cfd, amp_sum_cfd;
  while(t<tmax){
    a=(frac*(wgr.Eval(t)-fBaseline)-(wgr.Eval(t-del)-fBaseline));
    amp_sum_cfd.push_back(a);
    time_cfd.push_back(t);
    t+=0.2;
  }

  // //i find the zero crossing
  TGraph wsum_cfd(time_cfd.size(), &time_cfd[0], &amp_sum_cfd[0]);

  
  int minimum_bin= TMath::LocMin(amp_sum_cfd.size()-2, &amp_sum_cfd[2]);
  int bin_zero_crossing=minimum_bin;
  bool foundZeroCrossing=false;
  double tmp_a=amp_sum_cfd.at(minimum_bin);
  while(!foundZeroCrossing){
    tmp_a=amp_sum_cfd.at(bin_zero_crossing);
    if(tmp_a>0)foundZeroCrossing=true;
    if(bin_zero_crossing == amp_sum_cfd.size()-1 || bin_zero_crossing==1){
      break;
    }
    bin_zero_crossing++;
  }


  // I compute the zero-crossing time
  double tarr=0;
  double m,q;
  double t1,t2,a1,a2;
  if(foundZeroCrossing){
    t1 = time_cfd.at(bin_zero_crossing-1);
    t2 = time_cfd.at(bin_zero_crossing);
    a1 = amp_sum_cfd.at(bin_zero_crossing-1);
    a2 = amp_sum_cfd.at(bin_zero_crossing);
    m = (a2-a1)/(t2-t1);
    q = a2 - m*t2;
    tarr = -q/m;
  }else{
    tarr=-1000;
  }
  
  return tarr;

  
}


double TAGbaseWD::ComputeTimeSimpleCFD(TWaveformContainer *w, double frac){


  // evaluate the absolute threshold
  Double_t AbsoluteThreshold=-frac*fAmplitude+fBaseline;

  
  int i_ampmin = TMath::LocMin(w->GetVectA().size(),&(w->GetVectA())[0]);
  Int_t i_thr = i_ampmin;

  double t_arr=-1000;
  bool foundthreshold = false;
  while(!foundthreshold && i_thr<w->GetVectA().size()-1 && i_thr>1){
    double a1 = w->GetVectA().at(i_thr);
    double a2 = w->GetVectA().at(i_thr+1);
    double t1 = w->GetVectT().at(i_thr);
    double t2 = w->GetVectT().at(i_thr+1);
    double m = (a2-a1)/(t2-t1);
    double q = a1 - m*t1;
    t_arr = (AbsoluteThreshold-q)/m;
    if(AbsoluteThreshold>a2 && AbsoluteThreshold<=a1)foundthreshold =true;
    i_thr--;
  }
  

  return t_arr;




}



