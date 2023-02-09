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

#include "TAGrecoManager.hxx"

#define ST_AMP_THR 0.008

//! Class Imp
ClassImp(TASTrawHit);

TString TASTntuRaw::fgkBranchName   = "stdat.";


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
TASTrawHit::TASTrawHit(TWaveformContainer *W, string algo, double frac, double del)
  : TAGbaseWD(W){

  fBaseline = ComputeBaseline(W);
  fPedestal = ComputePedestal(W,ST_AMP_THR);
  fChg = ComputeCharge(W,ST_AMP_THR);
  //  cout << "cha::" << W->GetChannelId() << "  fCGH::" << fChg << endl;
  fAmplitude = ComputeAmplitude(W);
  fPileUp = false;
  if(algo=="hwCFD"){
    fTime = ComputeTime(W,frac,del,-10,10);
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
void TASTntuRaw::NewHit(TWaveformContainer *W, string algo, double frac, double del)
{
  TClonesArray &pixelArray = *fListOfHits;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(W, algo, frac, del);
  fHitsN++;
}

//------------------------------------------+-----------------------------------
//! new super hit
void TASTntuRaw::NewSuperHit(vector<TWaveformContainer*> vW, string algo, double frac, double del)
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

 
  vector<double> time(&vW.at(0)->GetVectT()[5], &vW.at(0)->GetVectT()[1018]);
  vector<double> amp(&vW.at(0)->GetVectA()[5], &vW.at(0)->GetVectA()[1018]);
 
  //I sum the signals
    for(int i=1;i<vW.size();i++){
    // vector<double> tmpamp = vW.at(i)->GetVectA();
    // vector<double> tmptime = vW.at(i)->GetVectT();
    double* time_up = &vW.at(i)->GetVectT()[0];
    double* amp_up = &vW.at(i)->GetVectA()[0];
    // TGraph tmpgr(tmptime.size(), &tmptime[0], &tmpamp[0]);
    for(int isa=0;isa<time.size();isa++){
      while(time[isa] >= *time_up)
      {
        time_up++;
        amp_up++;
      }
      amp[isa] += *(amp_up-1) + ( time[isa] - *(time_up-1))*(*amp_up - *(amp_up-1))/(*time_up - *(time_up-1));
      // amp.at(isa)+=(tmpgr.Eval(time.at(isa)));
    }
  }
   
  wsum->SetChannelId(ChannelId);
  wsum->SetBoardId(BoardId);
  wsum->SetTrigType(TrigType);
  wsum->SetTriggerCellId(TriggerCellId);
  wsum->GetVectA() = amp;
  wsum->GetVectT() = time;
  wsum->GetVectRawT() = time;
  wsum->SetNEvent(vW.at(0)->GetNEvent());
  
  fSuperHit = new TASTrawHit(wsum,algo, frac, del);
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








