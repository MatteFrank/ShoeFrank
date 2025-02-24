/*!
  \file TACAntuRaw.cxx
  \brief   Implementation of TACAntuRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TACAntuRaw.hxx"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"

//! Class Imp
ClassImp(TACArawHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACArawHit::TACArawHit()
  : TAGbaseWD(){
   
  fBaseline = -1000;
  fPedestal = -1000;
  fChg = -1000;
  fAmplitude = -1000;
  fTime =-1000;
  fTemp = 25;
}

//------------------------------------------+-----------------------------------
//! constructor.
TACArawHit::TACArawHit(TWaveformContainer *W, double temp = 25)
  : TAGbaseWD(W){

  fTemp = temp;
  fBaseline = ComputeBaseline(W);
  fPedestal = ComputePedestal(W,0.0);
  fChg = ComputeCharge(W,0.0);
  fAmplitude = ComputeAmplitude(W);
  fTime = ComputeTimeTangentCFD(W,0.3);
  fTimeOth = TAGbaseWD::ComputeTimeSimpleCFD(W,0.3);
  fRiseTime = ComputeRiseTime(W);
  
}

//------------------------------------------+-----------------------------------
//! Destructor.
TACArawHit::~TACArawHit()
{
}

//------------------------------------------+-----------------------------------
//! Get number of hits
Int_t TACAntuRaw::GetHitsN() const
{
  return fListOfHits->GetEntriesFast();
}

//##############################################################################

//! Class Imp
ClassImp(TACAntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACAntuRaw::TACAntuRaw()
: TAGdata(),
  fHitsN(0),
  fListOfHits(0)
{
  SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TACAntuRaw::~TACAntuRaw()
{
  if(fListOfHits)delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TACAntuRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TACArawHit");
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TACAntuRaw::Clear(Option_t*){
  TAGdata::Clear();
  fHitsN = 0;

  if (fListOfHits) fListOfHits->Clear();
}


//------------------------------------------+-----------------------------------
//! return a hit
//!
//! \param[in] i hit index
TACArawHit* TACAntuRaw::GetHit(Int_t i)
{
  if(i>=0 && i < GetHitsN())
    return (TACArawHit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}


//------------------------------------------+-----------------------------------
//! return a hit (const)
//!
//! \param[in] i hit index
const TACArawHit* TACAntuRaw::GetHit(Int_t i) const
{
  if(i>=0 && i < GetHitsN())
    return (const TACArawHit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! New hit from wave form
//!
//! \param[in] W wave form container
//! \param[in] temp temperature
void TACAntuRaw::NewHit(TWaveformContainer *W, double temp)
{  
  TClonesArray &pixelArray = *fListOfHits;
  TACArawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TACArawHit(W, temp);
  fHitsN++;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TACAntuRaw::ToStream(ostream& os, Option_t* option) const
{
   os << "TACAntuRaw " << GetName()
	   << " fHitsN" << fHitsN
      << endl;
}




