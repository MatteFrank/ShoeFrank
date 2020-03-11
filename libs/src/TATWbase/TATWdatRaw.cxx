/*!
  \file
  \version $Id: TATWdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TATWdatRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TATWdatRaw.hxx"
#include "TGraph.h"
#include "TF1.h"
/*!
  \class TATWdatRaw TATWdatRaw.hxx "TATWdatRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TATWrawHit);

TString TATWdatRaw::fgkBranchName   = "twdat.";

TATWrawHit::TATWrawHit(TWaveformContainer &W)
  : TAGbaseWD(W)
{
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



//##############################################################################

ClassImp(TATWdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWdatRaw::TATWdatRaw() :
  fHitsN(0), fListOfHits(0)
{
   SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TATWdatRaw::~TATWdatRaw()
{
  delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TATWdatRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TATWrawHit");
}

//------------------------------------------+-----------------------------------
Int_t TATWdatRaw::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TATWdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  fHitsN = 0;

  if (fListOfHits) fListOfHits->Clear();
}

//------------------------------------------+-----------------------------------
//! New hit.

void TATWdatRaw::NewHit(TWaveformContainer &W)
{
  W.SanitizeWaveform(); 

  TClonesArray &pixelArray = *fListOfHits;
  TATWrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWrawHit(W);
  fHitsN++;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TATWdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TATWdatRaw " << GetName()
	 << " fHitsN"    << fHitsN
     << endl;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TATWrawHit* TATWdatRaw::GetHit(Int_t i)
{
  return (TATWrawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TATWrawHit* TATWdatRaw::GetHit(Int_t i) const
{
  return (const TATWrawHit*) ((*fListOfHits)[i]);;
}
