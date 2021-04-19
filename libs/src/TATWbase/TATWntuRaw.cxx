/*!
  \file
  \version $Id: TATWntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
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
#include "TF1.h"
/*!
  \class TATWntuRaw TATWntuRaw.hxx "TATWntuRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TATWrawHit);

TString TATWntuRaw::fgkBranchName   = "twdat.";

TATWrawHit::TATWrawHit(TWaveformContainer *W)
  : TAGbaseWD(W){

  fBaseline = ComputeBaseline(W);
  fPedestal = ComputePedestal(W);
  fChg = ComputeCharge(W);
  fAmplitude = ComputeAmplitude(W);
  fTime = ComputeTime(W,0.3,10.0,-30,20);
  fTimeOth = ComputeTimeSimpleCFD(W,0.3);

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


double TATWrawHit::ComputeCharge(TWaveformContainer *w){
  return  TAGbaseWD::ComputeCharge(w);
}


double TATWrawHit::ComputeAmplitude(TWaveformContainer *w){
  return  TAGbaseWD::ComputeAmplitude(w);
}


double TATWrawHit::ComputeBaseline(TWaveformContainer *w){
  return TAGbaseWD::ComputeBaseline(w);
}


double TATWrawHit::ComputePedestal(TWaveformContainer *w){
  return  TAGbaseWD::ComputePedestal(w);
}



//##############################################################################

ClassImp(TATWntuRaw);


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
   return fListOfHits->GetEntries();
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


void TATWntuRaw::NewHit(TWaveformContainer *W)
{

  //  W->SanitizeWaveform();

  TClonesArray &pixelArray = *fListOfHits;
  TATWrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWrawHit(W);
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
