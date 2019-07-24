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
  nirhit(0), hir(0)
{
   SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TATWdatRaw::~TATWdatRaw() {
  delete hir;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TATWdatRaw::SetupClones()
{
  if (!hir) hir = new TClonesArray("TATWrawHit");
  return;
}


//------------------------------------------+-----------------------------------
//! Clear event.

void TATWdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  nirhit = 0;

  if (hir) hir->Clear();
  return;
}


void TATWdatRaw::NewHit(TWaveformContainer &W)
{

  W.SanitizeWaveform(); 

  TClonesArray &pixelArray = *hir;
  TATWrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWrawHit(W);
  nirhit++;
  return;
}





/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TATWdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TATWdatRaw " << GetName()
	 << " nirhit"    << nirhit
     << endl;
  return;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TATWrawHit* TATWdatRaw::Hit(Int_t i)
{
  return (TATWrawHit*) ((*hir)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TATWrawHit* TATWdatRaw::Hit(Int_t i) const
{
  return (const TATWrawHit*) ((*hir)[i]);;
}
