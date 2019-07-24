#include "TWaveformContainer.hxx"
#include "TAxis.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "Parameters.h"
#include <vector>

TWaveformContainer::TWaveformContainer()
{

  
	T=(Double_t *)malloc(sizeof(Double_t)*WAVEFORMBINS);
	W=(Double_t *)malloc(sizeof(Double_t)*WAVEFORMBINS);
	m_vectT.clear();
	m_vectW.clear();
	high_level_debug = false; //it can be set only hard-coding this variable

	Clear();
}

void TWaveformContainer::Clear()
{
	ChannelId=0;
	BoardId=0;
	ir_time=0;
	ir_chg=0;
	ir_pedestal=0;
	ir_amplitude=0;
	ir_pedestalstd=0;
	memset((void *)T,0,sizeof(Double_t)* WAVEFORMBINS);
	memset((void *)W,0,sizeof(Double_t)* WAVEFORMBINS);
	m_vectT.assign(WAVEFORMBINS,0);
	m_vectW.assign(WAVEFORMBINS,0);


	
}

TWaveformContainer::TWaveformContainer(const TWaveformContainer &other)
{

	ChannelId=other.ChannelId;
	BoardId=other.BoardId;
	ir_time=other.ir_time;
	ir_chg=other.ir_chg;
	ir_pedestal=other.ir_pedestal;
	ir_amplitude=other.ir_amplitude;
	memcpy(T, other.T, sizeof(Double_t)*WAVEFORMBINS);
	memcpy(W, other.W, sizeof(Double_t)*WAVEFORMBINS);
	m_vectT = other.m_vectT;
	m_vectW = other.m_vectW;

}

Double_t TWaveformContainer::ComputeCharge()
{
	ir_chg=0;
	for(int bin=CHARGESTARTBIN;bin<CHARGESTOPBIN;bin++)
	{
		ir_chg+=W[bin]-ir_pedestal;
	}
	// the minus is to return a positive charge
	ir_chg*=-1;
	return ir_chg;

}
Double_t TWaveformContainer::ComputePedestal()
{
	ir_pedestal=0;
	// this could be useful to flag pileup events
	ir_pedestalstd=TMath::RMS(PEDESTALSTOPBIN-PEDESTALSTARTBIN+1,&W[PEDESTALSTARTBIN]);
	ir_pedestal=TMath::Mean(PEDESTALSTOPBIN-PEDESTALSTARTBIN+1,&W[PEDESTALSTARTBIN]);
	return ir_pedestal;
}

Double_t TWaveformContainer::ComputeAmplitude()
{
  ir_amplitude=W[AMPLITUDESTARTBIN];
  // rember TW => negative signals
  ir_amplitude=-TMath::MinElement(AMPLITUDESTOPBIN-AMPLITUDESTARTBIN+1,&W[AMPLITUDESTARTBIN]);
  ir_amplitude-=ir_pedestal;
  return ir_amplitude;
}

Double_t TWaveformContainer::ComputeTimeStamp()
{
  TGraph gcfd = TGraph(5);
  // evaluate the absolute threshold
  Double_t AbsoluteThreshold=CFD_THREHSOLD*ir_amplitude+ir_pedestal;
  for(int bin=TIMESTAMPSTARTBIN;bin<TIMESTAMPSTOPBIN;++bin)
    {
      if(W[bin]<AbsoluteThreshold)
	{
	  //define  5 points to perform the linear fit
	  gcfd.SetPoint(0,T[bin-2],W[bin-2]);
	  gcfd.SetPoint(1,T[bin-1],W[bin-1]);
	  gcfd.SetPoint(2,T[bin]  ,W[bin]);
	  gcfd.SetPoint(3,T[bin+1],W[bin+1]);
	  gcfd.SetPoint(4,T[bin+2],W[bin+2]);
	  gcfd.Fit("pol1","Q","Q");
	  Double_t cfdp0=gcfd.GetFunction("pol1")->GetParameter(0);
	  Double_t cfdp1=gcfd.GetFunction("pol1")->GetParameter(1);
	  // extract the time from the parameters estimated by the fit
	  ir_time=(AbsoluteThreshold-cfdp0)/cfdp1;
	  return ir_time;
	}
    }
  return -1.;
}

// DEBUG ONLY
void TWaveformContainer::PlotWaveForm(int i)
{
  Double_t max=W[0];
  Double_t min=W[0];
  for (int bin=0;bin<WAVEFORMBINS;++bin)
    {
      if (W[bin]>max)
	{
	  max=W[bin];
	}
      if (W[bin]<min)
	{
	  min=W[bin];
	}
    }
  std::cout << " max " << max << " min "<< min <<std::endl;
  TCanvas *c = new TCanvas;
  c->Range(T[0],min,T[WAVEFORMBINS-1],max);
  TGraph *g=new TGraph(WAVEFORMBINS,T,W);
  g->GetXaxis()->SetTitle("t (ns)");
  g->GetYaxis()->SetTitle("Amplitude (V)");
  g->SetTitle(TString::Format(" Board %d Channel %d",BoardId,ChannelId));
  
  TLine *ped = new TLine(T[0],ir_pedestal,T[WAVEFORMBINS-1],ir_pedestal);
  ped->SetLineWidth(2);
  ped->SetLineColor(kGreen);
  TLine *ampl = new TLine(T[0],ir_amplitude+ir_pedestal,T[WAVEFORMBINS-1],ir_pedestal+ir_amplitude);
  Double_t clocktime=FindFirstRaisingEdgeTime();
  std::cout  <<"Tempo " << clocktime << std::endl;
  TLine *clock = new TLine(clocktime,min,clocktime,max);
  ped->SetLineWidth(2);
  ped->SetLineColor(kRed);
  
  clock->SetLineColor(kBlue);
  clock->SetLineWidth(2);
  g->Draw("AC");
  ped->Draw();
  ampl->Draw();
  clock->Draw();
  c->SaveAs(TString::Format("Test%d.pdf",i));
  c->Close();
  gSystem->ProcessEvents();
}

// DEBUG ONLY
void TWaveformContainer::GraphWaveForm(TH1F *wv0)
{
  double min = TMath::MinElement(WAVEFORMBINS,T);
  double max = TMath::MaxElement(WAVEFORMBINS,T);
  wv0->SetBins(WAVEFORMBINS,min,max);
  for (int bin=0;bin<WAVEFORMBINS;++bin){
    wv0->SetBinContent(bin+1,W[bin]);
  }
  wv0->GetXaxis()->SetTitle("t (s)");
  wv0->GetYaxis()->SetTitle("Amplitude (V)");
  wv0->SetTitle(TString::Format(" Board %d Channel %d",BoardId,ChannelId));
  return ; 
}

void TWaveformContainer::SanitizeWaveform()
{
  Double_t old=W[WAVEFORMBINS-1];
  if (old<-0.8)
    {
      old+=1;
      W[WAVEFORMBINS-1]+=1;
    }
  for (int bin=WAVEFORMBINS-2;bin>0;--bin)
    {
      Double_t derivative=(W[bin]-old);
      if (fabs(derivative)>VOLTAGE_TS)
	{
	  W[bin]-=TMath::Sign(1,derivative);
	}
      old=W[bin];
    }
}

bool TWaveformContainer::IsAClock()
{
  if (ChannelId==16 || ChannelId==17)
    {
      return true;
    }
  return false;
}

Double_t TWaveformContainer::FindFirstRaisingEdgeTime(){

  
  vector<double> tmp_unc;
  tmp_unc.assign(1024,0.007);

  vector<TF1> fclocks;
  vector<double> tarrclk_s, n_clk_s;

  
  double tleft=0, tright=0;
  int bin_trigTime=0;
  double t0 = 0;
  double tarr=0.;
  double tarrSum=0.;
  int count=0.;

  tarrSum =0.;
  bin_trigTime=20.;
  
  while(bin_trigTime > 10 && bin_trigTime <990){
    double amp = W[bin_trigTime];

    while(amp>0){
      bin_trigTime++;
      amp = W[bin_trigTime];
    }
    bin_trigTime+=5;
    while(amp<0){
      bin_trigTime++;
      amp = W[bin_trigTime];
    }
     
    t0 = T[bin_trigTime];
    amp = W[bin_trigTime];

    int ibin = bin_trigTime;
    double t1 = t0;
    double t2 = T[ibin+1];
    double a1 = amp;
    double a2 = W[ibin+1];
    double deriv=  (a2-a1)/(t2-t1);
    while(deriv > 0.015){
      ibin++;
      t1 = T[ibin];
      t2 = T[ibin+1];
      a1 = W[ibin];
      a2 = W[ibin+1];
      deriv=  (a2-a1)/(t2-t1);
    }
    tright = T[ibin];
     
    ibin=bin_trigTime;
    t1 = T[ibin-1];
    t2 = t0;
    a1 = W[ibin-1];
    a2 = amp;
    deriv=  (a2-a1)/(t2-t1);
    while(deriv > 0.015){
      ibin--;
      t1 = T[ibin-1];
      t2 = T[ibin];
      a1 = W[ibin-1];
      a2 = W[ibin];
      deriv=  (a2-a1)/(t2-t1);
    }
    tleft = T[ibin];
     
    TF1 fclock ("fclock","[0]/(1+TMath::Exp(-(x-[1])/[2]))-[3]",tleft,tright);
    fclock.SetParameter(0,0.40);
    fclock.SetParameter(1,t0+5);
    fclock.SetParameter(2,5);
    fclock.SetParameter(3,0.2);
     
    TGraphErrors WaveGraph(WAVEFORMBINS, T, W, 0, &tmp_unc[0]);
    WaveGraph.Fit(&fclock,"Q","",tleft,tright);   //&fclock
     
    fclocks.push_back(fclock);     

    count++;
    
    tarr = fclock.GetX(0);
    tarrclk_s.push_back(tarr);
    n_clk_s.push_back(count);
    
    bin_trigTime+=25;
  }

  
  TGraph WaveGraph(count, &n_clk_s[0], &tarrclk_s[0]);
  TF1 f("fun","[0]+[1]*x",0,250);
  f.SetParName(0, "phase");
  f.SetParName(1, "period");
  f.SetParameter(0,7);
  f.SetParameter(1,12.5);
  WaveGraph.Fit(&f,"RQ");   //&fclock
  
  return f.GetParameter(0);

 }


 TWaveformContainer::~TWaveformContainer()
 {
   if (T!=nullptr) { free(T); }
   if (W!=nullptr) { free(W); }
 }





Double_t TWaveformContainer::ComputeArrivalTime(){

  vector<double> tmp_amp = m_vectW;
  tmp_amp.erase(tmp_amp.begin(),tmp_amp.begin()+2); 
  vector<double> tmp_time = m_vectT;
  tmp_time.erase(tmp_time.begin(),tmp_time.begin()+2);
  vector<double> tmp_unc;
  tmp_unc.assign(1022,0.01);
  
  int min_bin = std::distance(tmp_amp.begin(), std::min_element(tmp_amp.begin(), tmp_amp.end()));
  int max_bin = std::distance(tmp_amp.begin(), std::max_element(tmp_amp.begin(), tmp_amp.end()));
  int cross_bin=min_bin;
  
  double time_crossbin = tmp_time.at(cross_bin);
  double time_binmin = tmp_time.at(min_bin);
  double time_binmax = tmp_time.at(max_bin);

  double amp_crossbin = tmp_amp.at(cross_bin);
  double amp_binmin = tmp_amp.at(min_bin);
  double amp_binmax = tmp_amp.at(max_bin);


  double tleft= time_crossbin-m_binmin;
  double tright= time_crossbin+m_binmax;

  double tmp_baseline = tmp_amp.at(5);
  
  //double fermi-dirac fit to extrapolate the waveform shape
  //  TF1 f("f", "-[0]/(1+TMath::Exp(-(x-[1])/[2]))/(1+TMath::Exp((x-[3])/[4]))+[5]",tleft,tright);
  // f.SetParameter(1,time_crossbin);
  // f.SetParameter(2,30);
  // f.SetParameter(3,time_crossbin+2);
  // f.SetParameter(4,2);
  // f.SetParameter(5,tmp_baseline);

  //log-normal fit to extrapolate the waveform shape
  TF1 f("f","-[0]*ROOT::Math::lognormal_pdf(x,[1],[2],[3])+[4]", tleft, tright); 
  f.SetParameter(0,1);
  f.SetParameter(1,0.1);
  f.SetParameter(2,1);
  f.SetParameter(3,time_crossbin-3);
  f.SetParameter(4,0);

  
  TGraphErrors WaveGraph(tmp_time.size(), &tmp_time[0], &tmp_amp[0], 0, &tmp_unc[0]);
  WaveGraph.Fit(&f,"Q", "",tleft, tright);
  m_chisquare = f.GetChisquare()/(double)f.GetNDF();
  
  char text[1000]="";

  //CFD method implemented with HARDCODED params.
  TF1 fcfd("fcfd","f+f",0,200);

  //if using log-normal fit
  fcfd.FixParameter(0, m_frac*f.GetParameter(0));
  fcfd.FixParameter(1, f.GetParameter(1));
  fcfd.FixParameter(2, f.GetParameter(2));
  fcfd.FixParameter(3, f.GetParameter(3));
  fcfd.FixParameter(4, 0);
  fcfd.FixParameter(5, -f.GetParameter(0));
  fcfd.FixParameter(6, f.GetParameter(1));
  fcfd.FixParameter(7, f.GetParameter(2));
  fcfd.FixParameter(8, f.GetParameter(3)+m_del);
  fcfd.FixParameter(9, 0);

  
  //if using fermi-dirac fit
  //fcfd.FixParameter(0, m_frac*f.GetParameter(0));
  // fcfd.FixParameter(1, f.GetParameter(1)+m_del);
  // fcfd.FixParameter(2, f.GetParameter(2));
  // fcfd.FixParameter(3, f.GetParameter(3)+m_del);
  // fcfd.FixParameter(4, f.GetParameter(4));
  // fcfd.FixParameter(5, 0);
  // fcfd.FixParameter(6, -f.GetParameter(0));
  // fcfd.FixParameter(7, f.GetParameter(1)+m_del);
  // fcfd.FixParameter(8, f.GetParameter(2));
  // fcfd.FixParameter(9, f.GetParameter(3)+m_del);
  // fcfd.FixParameter(10,f.GetParameter(4));
  // fcfd.FixParameter(11, 0);
 
  
  tleft = fcfd.GetMinimumX();
  tright = fcfd.GetMaximumX();

   //evaluate the zero-crossin point of the CFD signal
  double time = fcfd.GetX(0.0,tleft,tright);
  
  if(high_level_debug){
    sprintf(text,"wave_board%d_channel%d_evt%d.pdf", BoardId, ChannelId, m_nEvent);
    TCanvas c("c","",600,600);
    TLine l(time, -10, time,10);
    l.SetLineWidth(3);
    WaveGraph.GetXaxis()->SetLimits(time-30,time+50);
    WaveGraph.Draw("AP");
    f.Draw("same");
    l.Draw("same");
    c.SaveAs(text);    
  }
  


  return time;

  
 
}

Double_t TWaveformContainer::ComputeChargeST() {

  double dt = 0, amp;
  double q=0;
  const double mulf = 1/50.; //1/50 Ohm. When applied, you get the charge in nC 

  double window=50; //integration time window width
  double time_amin=0;
  int bin_amin=0;
  double tleft=0, tright=0;
  
  bin_amin = std::distance(m_vectW.begin(), std::min_element(m_vectW.begin(),m_vectW.end()));
  time_amin = m_vectW.at(bin_amin);
  tleft = time_amin-5;
  tright = time_amin+window-5; //this window has to be tuned
    
  for(int iSa=0;iSa<m_vectW.size()-1;iSa++){
    dt = m_vectT.at(iSa+1) - m_vectT.at(iSa);
    amp = -m_vectW.at(iSa); //the signal amplitude is negative
    if(m_vectT.at(iSa)>tleft && m_vectT.at(iSa)< tright)q+=amp*dt*mulf;
  }
    
  return q;

}

Double_t TWaveformContainer::ClockWave(Double_t *x, Double_t *par) {

 double fun=0;
  
  double phase, period;
  phase= par[2];
  period = par[3];

  double t=0;
  double r=0;
  t = (x[0]-phase);
  r= t/period -(int)(t/period);
  
  if(r>0.5) {
    return par[0]+ par[1]/(1+TMath::Exp(-(x[0]-phase)/par[4]));
  }else{
    TF1::RejectPoint();
     return 0;

  }

  
 
  
}


/*
AS: to be implemented if needed

double TASTactNtuRaw::ComputeMaxAmplitude(TASTrawHit*myHit){


  double window=50; //integration time window width
  double time_amin=0;
  int bin_amin=0;
  double tleft=0, tright=0;

  double max_amp=0;
  bin_amin = std::distance(m_vectW.begin(), std::min_element(m_vectW.begin(),m_vectW.end()));
  
  return -m_vectW.at(bin_amin);

}
*/

