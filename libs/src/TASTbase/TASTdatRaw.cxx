/*!
  \file
  \version $Id: TASTdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TASTdatRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TASTdatRaw.hxx"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
/*!
  \class TASTdatRaw TASTdatRaw.hxx "TASTdatRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TASTrawHit);

TString TASTdatRaw::fgkBranchName   = "stdat.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TASTrawHit::TASTrawHit()
  : TAGbaseWD(){
   
  baseline = -1000;
  pedestal = -1000;
  chg = -1000;
  amplitude = -1000;
  time =-1000;
}

//------------------------------------------+-----------------------------------
//! constructor.
TASTrawHit::TASTrawHit(TWaveformContainer *W)
  : TAGbaseWD(W){

  baseline = ComputeBaseline(W);
  pedestal = ComputePedestal(W);
  chg = ComputeCharge(W);
  amplitude = ComputeAmplitude(W);
  time = ComputeTime(W,0.3,2.0,-5,2);
  time_oth = TAGbaseWD::ComputeTimeSimpleCFD(W,0.3);

}

//------------------------------------------+-----------------------------------
//! Destructor.
TASTrawHit::~TASTrawHit(){

}


// do not need these interfaces, done by compilator
double TASTrawHit::ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright){
  return  TAGbaseWD::ComputeTime(w, frac, del, tleft, tright);
}


double TASTrawHit::ComputeCharge(TWaveformContainer *w){
  return TAGbaseWD::ComputeCharge(w);
}


double TASTrawHit::ComputeAmplitude(TWaveformContainer *w){
  return TAGbaseWD::ComputeAmplitude(w);
}


double TASTrawHit::ComputeBaseline(TWaveformContainer *w){
  return TAGbaseWD::ComputeBaseline(w);
}


double TASTrawHit::ComputePedestal(TWaveformContainer *w){
  return  TAGbaseWD::ComputePedestal(w);
}



//##############################################################################

ClassImp(TASTdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTdatRaw::TASTdatRaw() :
  fHistN(0), fListOfHits(0), fSuperHit(0), fRunTime(0x0){

  SetupClones();
}



//------------------------------------------+-----------------------------------
//! Destructor.

TASTdatRaw::~TASTdatRaw() {
  if(fListOfHits)delete fListOfHits;
  if(fSuperHit) delete fSuperHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TASTdatRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TASTrawHit");
}


//------------------------------------------+-----------------------------------
//! Clear event.

void TASTdatRaw::Clear(Option_t*){
  TAGdata::Clear();
  fHistN = 0;

  
  if (fListOfHits) fListOfHits->Clear();
}


//-----------------------------------------------------------------------------
//! access to the hit
TASTrawHit* TASTdatRaw::GetHit(Int_t i){
  return (TASTrawHit*) ((*fListOfHits)[i]);;
}


//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TASTrawHit* TASTdatRaw::GetHit(Int_t i) const{
  return (const TASTrawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! New hit
void TASTdatRaw::NewHit(TWaveformContainer *W){
  
  TClonesArray &pixelArray = *fListOfHits;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(W);
  fHistN++;
}

//------------------------------------------+-----------------------------------
//! new super hit
void TASTdatRaw::NewSuperHit(vector<TWaveformContainer*> vW){


  if(!vW.size()){
    printf("Warning, ST waveforms not found!!\n");
    return;
  }

  TWaveformContainer *wsum = new TWaveformContainer;
  int ChannelId=-1;
  int BoardId = vW.at(0)->GetBoardId();
  int TrigType = vW.at(0)->GetTrigType();
  int TriggerCellId = vW.at(0)->GetTriggerCellId();

  

  //I define the time window
  int i_ampmin = TMath::LocMin(vW.at(0)->GetVectA().size(),&(vW.at(0)->GetVectA())[0]);
  double t_ampmin = vW.at(0)->GetVectT().at(i_ampmin);
  double tmin = (t_ampmin-20 > vW.at(0)->GetVectT().at(0)) ? t_ampmin-20 : vW.at(0)->GetVectT().at(0);
  double tmax = (t_ampmin+5 < vW.at(0)->GetVectT().at(vW.at(0)->GetVectT().size()-1)) ? t_ampmin+5 : vW.at(0)->GetVectT().at(vW.at(0)->GetVectT().size()-1);
  vector<double> time,amp;
  double tmpt=tmin;
  while(tmpt<tmax){
    time.push_back(tmpt);
    tmpt+=0.2;
  }
  amp.assign(time.size(),0);

  //I sum the signals
    for(int i=0;i<vW.size();i++){
    vector<double> tmpamp = vW.at(i)->GetVectA();
    vector<double> tmptime = vW.at(i)->GetVectT();
    TGraph tmpgr(tmptime.size(), &tmptime[0], &tmpamp[0]);
    for(int isa=0;isa<time.size();isa++){
      amp.at(isa)+=(tmpgr.Eval(time.at(isa)));
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
  
  fSuperHit = new TASTrawHit(wsum);

  delete wsum;
  
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TASTdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TASTdatRaw " << GetName()
	 << " fHistN"    << fHistN
     << endl;
}




