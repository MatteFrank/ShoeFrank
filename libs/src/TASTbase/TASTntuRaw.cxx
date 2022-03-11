/*!
  \file
  \version $Id: TASTntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
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
#include "TCanvas.h"
#include "TF1.h"
/*!
  \class TASTntuRaw TASTntuRaw.hxx "TASTntuRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/


#define ST_AMP_THR 0.008

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
TASTrawHit::~TASTrawHit(){

}


// do not need these interfaces, done by compilator
double TASTrawHit::ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright){
  return  TAGbaseWD::ComputeTime(w, frac, del, tleft, tright);
}


double TASTrawHit::ComputeCharge(TWaveformContainer *w, double thr){
  return TAGbaseWD::ComputeCharge(w,thr);
}


double TASTrawHit::ComputeAmplitude(TWaveformContainer *w){
  return TAGbaseWD::ComputeAmplitude(w);
}


double TASTrawHit::ComputeBaseline(TWaveformContainer *w){
  return TAGbaseWD::ComputeBaseline(w);
}


double TASTrawHit::ComputePedestal(TWaveformContainer *w, double thr){
  return  TAGbaseWD::ComputePedestal(w,thr);
}



//##############################################################################

ClassImp(TASTntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTntuRaw::TASTntuRaw() :
  fHistN(0), fListOfHits(0), fSuperHit(0), fRunTime(0x0){

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
  fHistN = 0;
  
  
  if (fListOfHits) fListOfHits->Clear();
}


//-----------------------------------------------------------------------------
//! access to the hit
TASTrawHit* TASTntuRaw::GetHit(Int_t i){
  return (TASTrawHit*) ((*fListOfHits)[i]);;
}


//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TASTrawHit* TASTntuRaw::GetHit(Int_t i) const{
  return (const TASTrawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! New hit
void TASTntuRaw::NewHit(TWaveformContainer *W, string algo, double frac, double del){
  
  TClonesArray &pixelArray = *fListOfHits;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(W, algo, frac, del);
  fHistN++;
}

//------------------------------------------+-----------------------------------
//! new super hit
void TASTntuRaw::NewSuperHit(vector<TWaveformContainer*> vW, string algo, double frac, double del){


  if(!vW.size()){
    printf("Warning, ST waveforms not found!!\n");
    fSuperHit = new TASTrawHit();
    return;
  }

  TWaveformContainer *wsum = new TWaveformContainer;
  int ChannelId=-1;
  int BoardId = vW.at(0)->GetBoardId();
  int TrigType = vW.at(0)->GetTrigType();
  int TriggerCellId = vW.at(0)->GetTriggerCellId();

 
  vector<double> time(vW.at(0)->GetVectT());
  vector<double> amp(vW.at(0)->GetVectA());
 
  //I sum the signals
    for(int i=1;i<vW.size();i++){
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

  //cout << "trigid::" << TrigType << endl;
  
  fSuperHit = new TASTrawHit(wsum,algo, frac, del);


  // cout<< "superhit  algo::" << algo.data() << "   frac::" << frac << "  del::" << del << endl;
  
  delete wsum;
  
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TASTntuRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TASTntuRaw " << GetName()
	 << " fHistN"    << fHistN
     << endl;
}




