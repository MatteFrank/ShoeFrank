/*!
  \file
  \version $Id: TACAdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TACAdatRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TACAdatRaw.hxx"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
/*!
  \class TACAdatRaw TACAdatRaw.hxx "TACAdatRaw.hxx"
  \brief Container for dat raw ntu. **
*/

ClassImp(TACArawHit);

TString TACAdatRaw::fgkBranchName   = "cadat.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TACArawHit::TACArawHit()
  : TAGbaseWD(){
   
  fBaseline = -1000;
  fPedestal = -1000;
  fChg = -1000;
  fAmplitude = -1000;
  fTime =-1000;
}

//------------------------------------------+-----------------------------------
//! constructor.
TACArawHit::TACArawHit(TWaveformContainer *W)
  : TAGbaseWD(W){

  fBaseline = ComputeBaseline(W);
  fPedestal = ComputePedestal(W);
  fChg = ComputeCharge(W);
  fAmplitude = ComputeAmplitude(W);
  fTime = ComputeTime(W,0.3,2.0,-5,2);
  fTimeOth = TAGbaseWD::ComputeTimeSimpleCFD(W,0.3);

}

//------------------------------------------+-----------------------------------
//! Destructor.
TACArawHit::~TACArawHit(){

}


// do not need these interfaces, done by compilator
double TACArawHit::ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright){
  return  TAGbaseWD::ComputeTime(w, frac, del, tleft, tright);
}


double TACArawHit::ComputeCharge(TWaveformContainer *w){
  return TAGbaseWD::ComputeCharge(w);
}


double TACArawHit::ComputeAmplitude(TWaveformContainer *w){
  return TAGbaseWD::ComputeAmplitude(w);
}


double TACArawHit::ComputeBaseline(TWaveformContainer *w){
  return TAGbaseWD::ComputeBaseline(w);
}


double TACArawHit::ComputePedestal(TWaveformContainer *w){
  return  TAGbaseWD::ComputePedestal(w);
}



//##############################################################################

ClassImp(TACAdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACAdatRaw::TACAdatRaw() :
  fHistN(0), fListOfHits(0), fSuperHit(0), fRunTime(0x0){

  SetupClones();
}



//------------------------------------------+-----------------------------------
//! Destructor.

TACAdatRaw::~TACAdatRaw() {
  if(fListOfHits)delete fListOfHits;
  if(fSuperHit) delete fSuperHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TACAdatRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TACArawHit");
}


//------------------------------------------+-----------------------------------
//! Clear event.

void TACAdatRaw::Clear(Option_t*){
  TAGdata::Clear();
  fHistN = 0;

  
  if (fListOfHits) fListOfHits->Clear();
}


//-----------------------------------------------------------------------------
//! access to the hit
TACArawHit* TACAdatRaw::GetHit(Int_t i){
  return (TACArawHit*) ((*fListOfHits)[i]);;
}


//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TACArawHit* TACAdatRaw::GetHit(Int_t i) const{
  return (const TACArawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! New hit
void TACAdatRaw::NewHit(TWaveformContainer *W){
  
  TClonesArray &pixelArray = *fListOfHits;
  TACArawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TACArawHit(W);
  fHistN++;
}

//------------------------------------------+-----------------------------------
//! new super hit
void TACAdatRaw::NewSuperHit(vector<TWaveformContainer*> vW){


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
  
  fSuperHit = new TACArawHit(wsum);

  delete wsum;
  
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TACAdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TACAdatRaw " << GetName()
	 << " fHistN"    << fHistN
     << endl;
}




