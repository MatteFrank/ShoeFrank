/*!
  \file
  \version $Id: TACAntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
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
/*!
  \class TACAntuRaw TACAntuRaw.hxx "TACAntuRaw.hxx"
  \brief Container for dat raw ntu. **
*/

ClassImp(TACArawHit);

TString TACAntuRaw::fgkBranchName   = "cadat.";


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



Int_t TACAntuRaw::GetHitsN() const
{
  return fListOfHits->GetEntries();
}

//##############################################################################

ClassImp(TACAntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACAntuRaw::TACAntuRaw()
: TAGdata(),
  fHistN(0),
  fListOfHits(0)
{
  SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAntuRaw::~TACAntuRaw() {
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
  fHistN = 0;

  
  if (fListOfHits) fListOfHits->Clear();
}


//-----------------------------------------------------------------------------
//! access to the hit
TACArawHit* TACAntuRaw::GetHit(Int_t i){
  return (TACArawHit*) ((*fListOfHits)[i]);;
}


//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TACArawHit* TACAntuRaw::GetHit(Int_t i) const{
  return (const TACArawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! New hit
void TACAntuRaw::NewHit(TWaveformContainer *W){
  
  TClonesArray &pixelArray = *fListOfHits;
  TACArawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TACArawHit(W);
  fHistN++;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TACAntuRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TACAntuRaw " << GetName()
	 << " fHistN"    << fHistN
     << endl;
}




