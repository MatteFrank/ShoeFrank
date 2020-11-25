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

Int_t TACAdatRaw::GetHitsN() const
{
  return fListOfHits->GetEntries();
}

//##############################################################################

ClassImp(TACAdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACAdatRaw::TACAdatRaw()
: TAGdata(),
  fHistN(0),
  fListOfHits(0)
{
  SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAdatRaw::~TACAdatRaw() {
  if(fListOfHits)delete fListOfHits;
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

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TACAdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TACAdatRaw " << GetName()
	 << " fHistN"    << fHistN
     << endl;
}




