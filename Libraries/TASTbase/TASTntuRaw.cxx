/*!
  \file TASTntuRaw.cxx
  \brief   Implementation of TASTntuRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TASTntuRaw.hxx"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TVirtualFFT.h"
#include "TComplex.h"
#include "TAGrecoManager.hxx"

#define ST_AMP_THR 0.008

//! Class Imp
ClassImp(TASTrawHit);


//------------------------------------------+-----------------------------------
//! Default constructor.
TASTrawHit::TASTrawHit()
  : TAGbaseWD(){
   
  fBaseline = -1000;
  fPedestal = -1000;
  fChg = -1000;
  fAmplitude = -1000;
  fTime =-1000;
  fTimeOth =-1000;
  fPileUp = false;
}

//------------------------------------------+-----------------------------------
//! constructor.
TASTrawHit::TASTrawHit(TWaveformContainer *W, string algo, double frac, double del, TAGparaDsc *p_detconf, Bool_t isSuperHit)
  : TAGbaseWD(W){

  if(isSuperHit){
    TASTparConf *parConfSt = (TASTparConf*)p_detconf->Object();
    if(parConfSt->ApplyFFT()){
      vector<Double_t> re,im,fft,fftfilter,ampfftback;
      vector<Double_t> amp = W->GetVectA();
      vector<Double_t> time = W->GetVectT();

      TAGbaseWD::ComputeFFT(amp, re, im, fft);
      TAGbaseWD::SetFFT("MAG",fft);
      TAGbaseWD::SetFFT("RE",re);
      TAGbaseWD::SetFFT("IM",im);

      if(parConfSt->ApplyLowPass()){
	double fcutoff = parConfSt->GetLowPassCutoff();
	TAGbaseWD::LowPassFilterFFT(re,im,fcutoff,3);
      }
	    
      if(parConfSt->ApplySFCutoff()){
	vector<Double_t> freqcutoff = parConfSt->GetSFCutoff();    
	TAGbaseWD::FFTnotchFilter(freqcutoff,re,im);
      }
      
      if(parConfSt->ApplySFSmoothCutoff()){
	vector<Double_t> freqsmoothcutoff = parConfSt->GetSFSmoothCutoff();    
	TAGbaseWD::FFTnotchFilterSmooth(freqsmoothcutoff,re,im);
      }
      

      TAGbaseWD::ComputeInverseFFT(ampfftback, re, im);
      TAGbaseWD::ComputeFFT(ampfftback, re, im, fftfilter);
      W->GetVectA() = ampfftback;
      TAGbaseWD::SetFFTfilter("MAG", fftfilter);


      // TCanvas c("c","",600,600);
      // c.cd();

      // TMultiGraph mul;
  
      // TGraph gr(time.size(), &time[0], &amp[0]);
      // gr.SetMarkerColor(kBlack);
      // gr.SetMarkerSize(2.0);
      // gr.SetLineColor(kBlack);

      // TGraph grfft(time.size(), &time[0], &ampfftback[0]);
      // grfft.SetLineColor(kViolet+1);
      // grfft.SetMarkerColor(kViolet+1);
      

      // mul.Add(&gr);
      // mul.Add(&grfft);
      // mul.Draw("APL");
      // c.Print(Form("superhit_ev%d.pdf",W->GetNEvent()));


    }
  }
  
  
  fBaseline = ComputeBaseline(W);
  fPedestal = ComputePedestal(W,ST_AMP_THR);
  fChg = ComputeCharge(W,ST_AMP_THR);
  fAmplitude = ComputeAmplitude(W);
  fPileUp = false;
  fRiseTime = ComputeRiseTime(W);
  fSidebandValues.clear();
  fSidebandValues.insert(fSidebandValues.end(), W->GetVectA().begin()+FIRSTVALIDSAMPLE, W->GetVectA().begin()+NSIDESAMPLES);


  if(algo=="hwCFD"){
    fTime = ComputeTime(W,frac,del,-15,15);
  }else if(algo=="simpleCFD"){
    fTime = TAGbaseWD::ComputeTimeSimpleCFD(W,frac);
  }else if(algo=="zarrCFD"){
    fTime = TAGbaseWD::ComputeTimeTangentCFD(W,frac);
  }else{
    fTime = TAGbaseWD::ComputeTimeSimpleCFD(W,frac);
  }
}


//------------------------------------------+-----------------------------------
//! Destructor.
TASTrawHit::~TASTrawHit()
{

}



/*------------------------------------------+---------------------------------*/
//! Check for pileup in SC

Bool_t TASTrawHit::CheckForPileUp(TWaveformContainer *w, Int_t event)
{
  std::vector<double>* w_ptr = &(w->GetVectA());
  std::vector<double>* t_ptr = &(w->GetVectT());

	std::vector<Double_t> Der_W, Der_T;

	Double_t* x_down = &t_ptr->at(0);
	Double_t* x_up = &t_ptr->at(4);
	Double_t* y_down = &w_ptr->at(0);
	Double_t* y_up = &w_ptr->at(4);
	Double_t max = 0;

	for(int i = 0; i < w_ptr->size() - 5; ++i)
	{
		Der_T.push_back( *(x_down + 2) );
    Der_W.push_back( 1E-9*(*y_up - *y_down)/(*x_up - *x_down) );
		x_down++;
		x_up++;
		y_down++;
		y_up++;
		if(Der_W[i] > max) max = Der_W[i];
   
	}

	Float_t PileUpThreshold = 0.5* max;
  Int_t Ncross = 0;
	Int_t slope=-1;
	Bool_t endloop=false;
	for(int i=1; !endloop && i<Der_W.size(); ++i)
	{
		if( (fabs( Der_W[i] ) - PileUpThreshold)*(fabs( Der_W[i-1] ) - PileUpThreshold) < 0)
			Ncross++;
	}

  bool Debug = false;
  // if(Ncross != 4)
  //   Debug = true;
 /* if (fTime< 60) {
    Debug = true;
    cout << "event "<< event<<": time = "<<fTime<<"; charge = " << fChg <<";"<<endl;
  }
*/

  
	if (Debug) {      //stamp waveform and derivative waveform for debug
    
    w -> PlotWaveForm(event);       //stamp waveform

    // -------------- Stamp derivative and threshold
    TCanvas c1("c1","",1200,1000);
    c1.cd();

    //Rescale time for the plot
		for(int i = 0; i < Der_T.size(); ++i)	Der_T[i] *= 1E9;

		//Save some other graphs if in debug mode
		TGraph* WaveGraphDer = new TGraph(Der_T.size(), &Der_T[0], &Der_W[0]);
      
    WaveGraphDer->Draw("APL");
		WaveGraphDer->SetTitle("SC WF der; Time [ns]; Derivative [V/ns]");
		WaveGraphDer->SetLineWidth(2);
		WaveGraphDer->SetLineColor(kBlue);
  
    TGraph* lineUp = new TGraph(2);
    lineUp->Draw("PL SAME");
		lineUp->SetPoint(0, Der_T[0], PileUpThreshold);
		lineUp->SetPoint(1, Der_T[Der_T.size() - 1], PileUpThreshold);
    lineUp->GetYaxis()->SetRangeUser(0, 1E-9);
		lineUp->SetLineColor(kRed);
		lineUp->SetLineStyle(kDashed);
		lineUp->SetLineWidth(2);

		TGraph* lineDown = new TGraph(2);
    lineDown->Draw("PL SAME");
		lineDown->SetPoint(0, Der_T[0], -PileUpThreshold);
		lineDown->SetPoint(1, Der_T[Der_T.size() - 1], -PileUpThreshold);
    lineDown->GetYaxis()->SetRangeUser( -1E-9,0);
		lineDown->SetLineColor(kRed);
		lineDown->SetLineStyle(kDashed);
		lineDown->SetLineWidth(2);

    c1.Print(Form("SC_WAVE_derivative_Cross%d_ev_%i.png",Ncross,event));

		delete WaveGraphDer;
		delete lineUp;
		delete lineDown;

    //------------end stamp derivative
    

	}

	if(Ncross == 4) {
    fPileUp = false;
    }
		    
	  else {
    fPileUp = true;
    }

  //if (fTime< 60) fPileUp = true;        // to filter out all the "wrong" signal before the time windows of the triggers

  return fPileUp;
    
}










//##############################################################################

//! Class Imp
ClassImp(TASTntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTntuRaw::TASTntuRaw() :
  fHitsN(0), fListOfHits(0), fSuperHit(0), fRunTime(0x0){

  SetupClones();

  TVirtualFFT::SetTransform(0);
  
}

//------------------------------------------+-----------------------------------
//! Destructor.
TASTntuRaw::~TASTntuRaw() {
  if(fListOfHits)delete fListOfHits;
  if(fSuperHit) delete fSuperHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TASTntuRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TASTrawHit");
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TASTntuRaw::Clear(Option_t*){
  TAGdata::Clear();
  fHitsN = 0;
  
  if (fListOfHits) fListOfHits->Clear();
}


//-----------------------------------------------------------------------------
//! access to the hit
TASTrawHit* TASTntuRaw::GetHit(Int_t i)
{
  if(i>=0 && i < fHitsN)
    return (TASTrawHit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}


//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TASTrawHit* TASTntuRaw::GetHit(Int_t i) const
{
  if(i>=0 && i < fHitsN)
    return (const TASTrawHit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! New hit
void TASTntuRaw::NewHit(TWaveformContainer *W, string algo, double frac, double del,  TAGparaDsc *p_detconf, Bool_t isSuperHit)
{
  TClonesArray &pixelArray = *fListOfHits;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(W, algo, frac, del, p_detconf,isSuperHit);
  fHitsN++;
}

//------------------------------------------+-----------------------------------
//! new super hit
void TASTntuRaw::NewSuperHit(vector<TWaveformContainer*> vW, string algo, double frac, double del, TAGparaDsc *p_detconf)
{
  if(!vW.size()){
    if(FootDebugLevel(1))
      printf("Warning, ST waveforms not found!!\n");
    fSuperHit = new TASTrawHit();
    return;
  }

  TWaveformContainer *wsum = new TWaveformContainer;
  int ChannelId=-1;
  int BoardId = vW.at(0)->GetBoardId();
  int TrigType = vW.at(0)->GetTrigType();
  int TriggerCellId = vW.at(0)->GetTriggerCellId();

   

  vector<double> time;
  vector<double> amp;
  vector<double> amp_noise;
  vector<double> time_noise;

  double timebin = 1/3.0;
  
  //I compute the time vector
  for(int i=0;i<NSAMPLES;i++){
    amp.push_back(0);
    time.push_back(timebin*(i+1));
  }
    
  //I sum the signals
  for(int i=0;i<vW.size();i++){
    double* time_up = &vW.at(i)->GetVectT()[0];
    double* time_max = &vW.at(i)->GetVectT()[NSAMPLES-1];
    double* time_min = &vW.at(i)->GetVectT()[0];
    double* amp_up = &vW.at(i)->GetVectA()[0];
    for(int isa=0;isa<time.size();isa++){

      while(time[isa]>=*time_up    &&   time_up<time_max      &&        time_up >= time_min)
	{
	  time_up++;
	  amp_up++;
	  
	}
      
      if(time_up-1 >= time_max)break;

      if(time_up-1 <= time_min)continue;
      
      amp[isa] += *(amp_up-1) + ( time[isa] - *(time_up-1))*(*amp_up - *(amp_up-1))/(*time_up - *(time_up-1));

    }
  }

  wsum->SetChannelId(ChannelId);
  wsum->SetBoardId(BoardId);
  wsum->SetTrigType(TrigType);
  wsum->SetTriggerCellId(TriggerCellId);
  wsum->GetVectT() = time;
  wsum->GetVectA() = amp;
  wsum->GetVectRawT() = time;
  wsum->SetNEvent(vW.at(0)->GetNEvent());

  fSuperHit = new TASTrawHit(wsum, algo, frac, del, p_detconf, true);
  fSuperHit -> CheckForPileUp(wsum , wsum->GetNEvent()); //check for Pile up
  

  

  delete wsum;
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TASTntuRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TASTntuRaw " << GetName()
	 << " fHitsN"    << fHitsN
     << endl;
}








