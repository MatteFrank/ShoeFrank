/*!
  \file TANEntuRaw.cxx
  \brief   Implementation of TANEntuRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TANEntuRaw.hxx"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"

//! Class Imp
ClassImp(TANErawHit);

TString TANEntuRaw::fgkBranchName   = "cadat.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TANErawHit::TANErawHit()
  : TAGbaseWD(){
   
  fBaseline = -1000;
  fPedestal = -1000;
  fChg = -1000;
  fAmplitude = -1000;
  fTime =-1000;
}

//------------------------------------------+-----------------------------------
//! constructor.
TANErawHit::TANErawHit(TWaveformContainer *W)
  : TAGbaseWD(W)
{
  fBaseline = ComputeBaseline(W);
  fPedestal = ComputePedestal(W,0.0);
  fChg = ComputeCharge(W,0.0);
  fAmplitude = ComputeAmplitude(W);
  fTime = ComputeTime(W,0.3,2.0,-5,2);
  fTimeOth = TAGbaseWD::ComputeTimeSimpleCFD(W,0.3);

}

//------------------------------------------+-----------------------------------
//! Destructor.
TANErawHit::~TANErawHit()
{
}

//------------------------------------------+-----------------------------------
//! Get number of hits
Int_t TANEntuRaw::GetHitsN() const
{
  return fListOfHits->GetEntriesFast();
}

//##############################################################################

//! Class Imp
ClassImp(TANEntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TANEntuRaw::TANEntuRaw()
: TAGdata(),
  fListOfHits(0)
{
  SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TANEntuRaw::~TANEntuRaw()
{
  if(fListOfHits)delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TANEntuRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TANErawHit");
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TANEntuRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  if (fListOfHits) fListOfHits->Clear();
}


//------------------------------------------+-----------------------------------
//! return a hit
//!
//! \param[in] i hit index
TANErawHit* TANEntuRaw::GetHit(Int_t i)
{
  if(i >= 0 && i < GetHitsN())
    return (TANErawHit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}


//------------------------------------------+-----------------------------------
//! return a hit (const)
//!
//! \param[in] i hit index
const TANErawHit* TANEntuRaw::GetHit(Int_t i) const
{
  if(i >= 0 && i < GetHitsN())
    return (const TANErawHit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! New hit from wave form
//!
//! \param[in] W wave form container
void TANEntuRaw::NewHit(TWaveformContainer *W)
{  
  TClonesArray &pixelArray = *fListOfHits;
  TANErawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TANErawHit(W);
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TANEntuRaw::ToStream(ostream& os, Option_t* option) const
{
   os << "TANEntuRaw " << GetName()
      << endl;
}




