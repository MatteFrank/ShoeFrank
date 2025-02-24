/*!
  \file TAGbaseWD.cxx
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
#include "TVirtualFFT.h"
#include "TSpectrum.h"


/*!
  \class TAGbaseWD
 \brief This class stores the params of a single channel waveform
*/





ClassImp(TAGbaseWD);

using namespace std;

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGbaseWD::TAGbaseWD()
 : fTime(999999.),
   fChg(0.),
   fChId(0),
   fPedestal(0),
   fBaseline(0),
   fAmplitude(0),
   fDeltaClk(-9999.),
   fBoardId(0),
   fRiseTime(-999.),
   fTriggerTypeId(0),
   fTriggerCellId(-1000),
   fMcId(-999)
{

  
  
}

//------------------------------------------+-----------------------------------
//! Constructor
TAGbaseWD::TAGbaseWD(TWaveformContainer *W)
{
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
  fDeltaClk=W->GetDeltaClk();
  fRiseTime = -999.;

  fSidebandValues.insert(fSidebandValues.end(), W->GetVectA().begin()+FIRSTVALIDSAMPLE, W->GetVectA().begin()+NSIDESAMPLES);
  

  
}

//------------------------------------------+-----------------------------------
//! Destructor
TAGbaseWD::~TAGbaseWD()
{
}

//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputeBaseline(TWaveformContainer *w)
{
  return TMath::Median(NSIDESAMPLES, &w->GetVectA()[FIRSTVALIDSAMPLE]);
}

//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputePedestal(TWaveformContainer *w, double thr)
{
  vector<double> tmp_amp = w->GetVectA();
  vector<double> tmp_time = w->GetVectT();
  double prod;
  double deltat=0.; //trapezi
  double b1=0, b2=0;
  double a1, a2;
  double m,q,t1,t2,thalf,dq;
  double pedestal=0;
  double pedestal_tot=0;
  double mean=0.0008816;

  for (int j = FIRSTVALIDSAMPLE; j<NSIDESAMPLES; j++){ 
    a1 = tmp_amp[j];
    a2 = tmp_amp[j+1];
    t1 = tmp_time[j];
    t2 = tmp_time[j+1];
    prod=a1*a2;
    if(prod<0){
      m = (a2-a1)/(t2-t1);
      q = a2-m*t2;
      thalf = -q/m;
      dq=0.5*a1*(thalf-t1)+0.5*a2*(t2-thalf);
    }else{
      dq = 0.5*(a1+a2)*(t2-t1);
    }
    if(tmp_amp[j]<(mean+thr)){
      pedestal+=dq;
    }
  }
  pedestal_tot= -(pedestal*tmp_amp.size()/(double)(NSIDESAMPLES));

  return pedestal_tot;
}

//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputeAmplitude(TWaveformContainer *w)
{
  return  -((TMath::MinElement(w->GetVectA().size()-5, &w->GetVectA()[FIRSTVALIDSAMPLE])) - fBaseline);
}

//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputeCharge(TWaveformContainer *w, double thr)
{
  vector<double> tmp_amp = w->GetVectA();
  vector<double> tmp_time = w->GetVectT();

  double charge = 0.;
  double prod;
  double chargetot=0;

  double deltat=0.; //trapezi
  double b1=0, b2=0;
  double a1, a2;
  double m,q,t1,t2,thalf, dq;
  double mean= 0.0008816;

  for (int j = 0; j<tmp_amp.size()-1; j++){
    a1 = tmp_amp[j];
    a2 = tmp_amp[j+1];
    t1 = tmp_time[j];
    t2 = tmp_time[j+1];
    prod=a1*a2;
    if(a1>fBaseline+thr)continue;
    if(prod<0){
      m = (a2-a1)/(t2-t1);
      q = a2-m*t2;
      thalf = -q/m;
      dq=0.5*a1*(thalf-t1)+0.5*a2*(t2-thalf);
    }else{
      dq = 0.5*(a1+a2)*(t2-t1);
    }

    if(tmp_amp[j]<(mean+thr)){
      charge+=dq;
    }

  }
   
  chargetot=charge;
   
  return -(chargetot + fPedestal);
}

//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright)
{
  vector<double> time = w->GetVectT();
  vector<double> amp = w->GetVectA();

  //I create the waveform graph
  TGraph wgr(time.size(),&time[0], &amp[0]);


  //I select the time window in which applying the cfd 
  int peak_bin= TMath::LocMin(amp.size(), &amp[0]);
  if(peak_bin<0 || peak_bin>time.size()-1){
    return -10000;
  }
  double timepeak = time[peak_bin];
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
  int maximum_bin= TMath::LocMax(amp_sum_cfd.size()-2, &amp_sum_cfd[2]);
  int bin_zero_crossing= (maximum_bin>0) ? maximum_bin : 1;
  //  int bin_zero_crossing=minimum_bin;
  bool foundZeroCrossing=false;
  do{
    double a1 = amp_sum_cfd[bin_zero_crossing];
    double a2 = amp_sum_cfd[bin_zero_crossing-1];
    if(a1*a2<0){
      foundZeroCrossing=true;
    }else{
      bin_zero_crossing--;
      //    bin_zero_crossing++;
    }
  }while(!foundZeroCrossing && bin_zero_crossing>0);


  // I compute the zero-crossing time
  double tarr=0;
  double m,q;
  double t1,t2,a1,a2;
  if(foundZeroCrossing){
    t1 = time_cfd[bin_zero_crossing-1];
    t2 = time_cfd[bin_zero_crossing];
    a1 = amp_sum_cfd[bin_zero_crossing-1];
    a2 = amp_sum_cfd[bin_zero_crossing];
    m = (a2-a1)/(t2-t1);
    q = a2 - m*t2;
    tarr = -q/m;
  }else{
    tarr=-1000;
  }

  
   // TCanvas c("c","",600,600);
   // c.cd();
   // wsum_cfd.Draw("APL");
   // wsum_cfd.SetMarkerSize(0.5);
   // wsum_cfd.SetMarkerStyle(22);
   // wsum_cfd.SetMarkerColor(kBlue);
   // wsum_cfd.GetXaxis()->SetRangeUser(0,300);
   // TGraph min;
   // min.SetPoint(0,tarr,0);
   // min.SetMarkerStyle(29);
   // min.SetMarkerSize(1.5);
   // min.SetMarkerColor(kRed);
   // min.Draw("Psame");
   // c.Print(Form("waveform_ch%d_superhit%d.png", w->GetChannelId(),w->GetNEvent()));


   
  return tarr;
}

//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputeTimeSimpleCFD(TWaveformContainer *w, double frac)
{
  // evaluate the absolute threshold
  Double_t AbsoluteThreshold=-frac*fAmplitude+fBaseline;
  
  int i_ampmin = TMath::LocMin(w->GetVectA().size(),&(w->GetVectA())[0]);
  Int_t i_thr = i_ampmin;

  double t_arr=-1000;
  bool foundthreshold = false;
  while(!foundthreshold && i_thr<w->GetVectA().size()-1 && i_thr>1){
    double a1 = w->GetVectA()[i_thr];
    double a2 = w->GetVectA()[i_thr+1];
    double t1 = w->GetVectT()[i_thr];
    double t2 = w->GetVectT()[i_thr+1];
    double m = (a2-a1)/(t2-t1);
    double q = a1 - m*t1;
    t_arr = (AbsoluteThreshold-q)/m;
    if(AbsoluteThreshold>a2 && AbsoluteThreshold<=a1)foundthreshold =true;
    i_thr--;
  }
  
  return t_arr;
}

//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputeTimeTangentCFD(TWaveformContainer *w, double frac)
{
  // evaluate the absolute threshold
  Double_t AbsoluteThreshold=-frac*fAmplitude+fBaseline;
  
  int i_ampmin = TMath::LocMin(w->GetVectA().size(),&(w->GetVectA())[0]);
  Int_t i_thr = i_ampmin;
  bool foundthreshold = false;

  //found the threshold bin
  while(!foundthreshold && i_thr>=0 && i_thr< w->GetVectA().size()){
    double a1 = w->GetVectA()[i_thr];
    double a2 = w->GetVectA()[i_thr+1];
    if(AbsoluteThreshold>a2 && AbsoluteThreshold<=a1){
      foundthreshold =true;
    }else{
      i_thr--;
    }
  }

  //get 4 points around the threshold
  vector<double> times,amplitudes;
  int size = (int)w->GetVectA().size();
  for(int i=i_thr-1;i<=i_thr+2;i++){
    if(i>=0 && i<size){
      amplitudes.push_back(w->GetVectA()[i]);
      times.push_back(w->GetVectT()[i]);
      //cout << "t" <<i << "   ::" << w->GetVectT()[i] << endl;
    }
  }

  //compute the ang. coefficient (least squares)
  double m,q;
  double xy=0,x=0,y=0, xsq=0;
  double N=(double)times.size();
  for(int i=0;i<times.size();i++){
    x+=times[i]/N;
    y+=amplitudes[i]/N;
    xy+=times[i]*amplitudes[i]/N;
    xsq+=times[i]*times[i]/N;
  }

  m = (xy-x*y)/(xsq-x*x);
  q = (xsq*y-x*xy)/(xsq-x*x);

  double t_arr = -q/m;

  return t_arr;
}



//------------------------------------------+-----------------------------------
double TAGbaseWD::ComputeRiseTime(TWaveformContainer *w)
{
  // evaluate the absolute threshold
  Double_t AbsoluteThreshold_low=-0.10*fAmplitude+fBaseline;
  Double_t AbsoluteThreshold_up=-0.90*fAmplitude+fBaseline;
  
  int i_ampmin = TMath::LocMin(w->GetVectA().size(),&(w->GetVectA())[0]);
  Int_t i_thr = i_ampmin;


  bool foundthreshold_low = false;
  bool foundthreshold_up = false;
  double tup=10000000000, tlow=-10000000000;
  while(!(foundthreshold_up && foundthreshold_low) && i_thr<w->GetVectA().size()-1 && i_thr>1){
    double a1 = w->GetVectA()[i_thr];
    double a2 = w->GetVectA()[i_thr+1];
    double t1 = w->GetVectT()[i_thr];
    double t2 = w->GetVectT()[i_thr+1];
    double m = (a2-a1)/(t2-t1);
    double q = a1 - m*t1;
    if(AbsoluteThreshold_up>a2 && AbsoluteThreshold_up<=a1){
      foundthreshold_up =true;
      tup = (AbsoluteThreshold_up-q)/m;
    }
    if(AbsoluteThreshold_low>a2 && AbsoluteThreshold_low<=a1){
      foundthreshold_low =true;
      tlow = (AbsoluteThreshold_low-q)/m;
    }

    i_thr--;
  }
  
  return tup-tlow;
}




void TAGbaseWD::ComputeFFT(vector<double> &amp, vector<double> &re, vector<double> &im, vector<double> &mag){

  int n = amp.size();
  double vre, vim,vmag;
  re.assign(n,0);
  im.assign(n,0);
  mag.assign(n,0);

  //*get the fft transform
  TVirtualFFT *fft = TVirtualFFT::FFT(1, &n, "R2C EX K");
  fft->SetPoints(&amp[0]);
  fft->Transform();
  for(int i=0;i<n/2+1;i++){
    fft->GetPointComplex(i, vre, vim);
    vmag = sqrt(vre*vre+vim*vim); 
    re.at(i)  = vre;
    im.at(i)  = vim;
    mag.at(i) = vmag;
  }

  delete fft;
  
  return;
  
}



void TAGbaseWD::ComputeInverseFFT(vector<double> &amp, vector<double> re, vector<double> im){


  int n = re.size();
   
  double vre, vim,vmag;

  amp.assign(n,0);
  TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &n, "C2R EX K");

  for(int i=0;i<n/2+1;i++){
    TComplex c(re[i],im[i]);
    fft_back->SetPointComplex(i, c);
  }

  fft_back->Transform();
  
  for(int i=0;i<n;i++){
    vmag = fft_back->GetPointReal(i);
    amp.at(i) = vmag/(double)n;
  }


  
  delete fft_back;
  return;
  
}


void TAGbaseWD::FFTnotchFilter(vector<double> FreqCut, vector<double> &re, vector<double> &im){

    
  double freq=3.0;
  int n = re.size();
  double vre, vim, vmag;
  
  vector<int> icut;
  for(int j=0;j<FreqCut.size();j++){
    icut.push_back((int)(FreqCut.at(j)/(freq/(double)n)));
  }
 
  for(int i=0; i<n/2+1;i++){
    if(find(icut.begin(),icut.end(),i)!=icut.end()){
      int ii = (i>=30) ? i-30 : 0;
      int iff = (i<=n/2+1) ? i+30 : n/2+1;
      for(int j=ii;j<=iff;j++){
	double sigma =1;
	double fact = 1-TMath::Gaus(j-i, 0 ,sigma);
	re[j]*=fact;
	im[j]*=fact;
      }
    }
  }
   
  return ;
}





void TAGbaseWD::FFTnotchFilterSmooth(vector<double> FreqCut, vector<double> &re, vector<double> &im){

    

  double freq=3.0;
  int n = re.size();
  double vre, vim, vmag;

  vector<int> icut;
  for(int j=0;j<FreqCut.size();j++){
    icut.push_back((int)(FreqCut.at(j)/(freq/(double)n)));
  }
  
  for(int i=0; i<n/2+1;i++){
    if(find(icut.begin(),icut.end(),i)!=icut.end()){
      double ii = (i-4<0) ? 0 : i-4;
      double iff = (i+4>re.size()-1) ? re.size()-1  : i+4;
      double mre = (re[iff]-re[ii])/(iff-ii);
      double qre = re[ii]-mre*ii;
      double mim = (im[iff]-im[ii])/(iff-ii);
      double qim = im[ii]-mim*ii;
      double vmag1 = sqrt(re[ii]*re[ii]+im[ii]*im[ii]);
      double vmag2 = sqrt(re[iff]*re[iff]+im[iff]*im[iff]);
      double mmag = (vmag2-vmag1)/(double)(iff-ii);
      double qmag = vmag1-mmag*ii;
      for(int j=ii;j<=iff;j++){
	double vmag = sqrt(re[j]*re[j]+im[j]*im[j]);
	re[j]*=(mmag*j+qmag)/vmag;
	im[j]*=(mmag*j+qmag)/vmag;
      }

    }
  }
  
  return ;
}




void TAGbaseWD::LowPassFilterFFT(vector<double>& re, vector<double>& im, double cutoffFrequency, int order) {

   
    int n = re.size();

    // Applica il filtro passa-basso
    for (int i = 0; i < n/2; ++i) {
      double Frequency = 3.0*i/(double)n;
      double filterResponse = 1.0 / sqrt(1.0 + pow(Frequency /cutoffFrequency, 2 * order));
      double vre = re[i];
      double vim = im[i];
      
      
      re[i] = vre*(filterResponse);
      // re[re.size()-1-i] = vre*(filterResponse);
      im[i] = vim*(filterResponse);
      //im[im.size()-1-i] = vim*(filterResponse);
    }
}


