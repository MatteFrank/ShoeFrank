/*!
  \file TATWntuRaw.cxx
  \brief   Implementation of TATWntuRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TATWntuRaw.hxx"
#include "TGraph.h"

//! Class Imp
ClassImp(TATWrawHit);

TString TATWntuRaw::fgkBranchName   = "twdat.";

//______________________________________________________________________________
//
TATWrawHit::TATWrawHit(TWaveformContainer *W, string algo, double frac, double del)
  : TAGbaseWD(W)
{
  fBaseline = ComputeBaseline(W);
  fPedestal = ComputePedestal(W,0.0);
  fChg = ComputeCharge(W, 0.0);
  fAmplitude = ComputeAmplitude(W);
  if(algo=="hwCFD"){
    fTime = ComputeTime(W,frac,del,-30,20);
  }else if(algo=="simpleCFD"){
    fTime = TAGbaseWD::ComputeTimeSimpleCFD(W,frac);
  }else if(algo=="zarrCFD"){
    fTime = TAGbaseWD::ComputeTimeTangentCFD(W,frac);
  }else{
    fTime = TAGbaseWD::ComputeTimeSimpleCFD(W,frac);
  }
}

//______________________________________________________________________________
//
TATWrawHit::~TATWrawHit()
{
}

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWrawHit::TATWrawHit()
  : TAGbaseWD()
{
}



double TATWrawHit::ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright){
  return  TAGbaseWD::ComputeTime(w, frac, del, tleft, tright);
}


double TATWrawHit::ComputeCharge(TWaveformContainer *w, double thr){
  return  TAGbaseWD::ComputeCharge(w,thr);
}


double TATWrawHit::ComputeAmplitude(TWaveformContainer *w){
  return  TAGbaseWD::ComputeAmplitude(w);
}


double TATWrawHit::ComputeBaseline(TWaveformContainer *w){
  return TAGbaseWD::ComputeBaseline(w);
}


double TATWrawHit::ComputePedestal(TWaveformContainer *w, double thr){
  return  TAGbaseWD::ComputePedestal(w, thr);
}



//##############################################################################

//Class Imp
ClassImp(TATWntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWntuRaw::TATWntuRaw() :
  fHitsN(0), fListOfHits(0), fRunTime(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWntuRaw::~TATWntuRaw()
{
  delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TATWntuRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TATWrawHit");
  return;
}


Int_t TATWntuRaw::GetHitsN() const
{
   return fListOfHits->GetEntriesFast();
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TATWntuRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  fHitsN = 0;

  if (fListOfHits) fListOfHits->Clear();
  return;
}


void TATWntuRaw::NewHit(TWaveformContainer *W,  string algo, double frac, double del)
{

  //  W->SanitizeWaveform();

  TClonesArray &pixelArray = *fListOfHits;
  TATWrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWrawHit(W,algo, frac, del);
  fHitsN++;

  return;
}





/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TATWntuRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TATWntuRaw " << GetName()
	 << " fHitsN"    << fHitsN
     << endl;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TATWrawHit* TATWntuRaw::GetHit(Int_t i)
{
  return (TATWrawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TATWrawHit* TATWntuRaw::GetHit(Int_t i) const
{
  return (const TATWrawHit*) ((*fListOfHits)[i]);;
}
