/*!
  \file
  \version $Id: TASTdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TASTdatRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TASTdatRaw.hxx"
#include "TGraph.h"
#include "TF1.h"
/*!
  \class TASTdatRaw TASTdatRaw.hxx "TASTdatRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TASTrawHit);

TString TASTdatRaw::fgkBranchName   = "stdat.";

TASTrawHit::TASTrawHit(TWaveformContainer &W)
  : TAGbaseWD(W)
{
}

//______________________________________________________________________________
//
TASTrawHit::~TASTrawHit()
{
}

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTrawHit::TASTrawHit()
  : TAGbaseWD()
{
}




//##############################################################################

ClassImp(TASTdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTdatRaw::TASTdatRaw() :
  fHitsN(0), fListOfHits(0), m_run_time(0x0)
{
   SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TASTdatRaw::~TASTdatRaw()
{
  delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TASTdatRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TASTrawHit");
  return;
}


//------------------------------------------+-----------------------------------
//! Clear event.

void TASTdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  fHitsN = 0;

  if (fListOfHits) fListOfHits->Clear();
}

//------------------------------------------+-----------------------------------
//! New hit.

void TASTdatRaw::NewHit(TWaveformContainer &W)
{
  
  TClonesArray &pixelArray = *fListOfHits;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(W);
  fHitsN++;
}

//------------------------------------------+-----------------------------------
Int_t TASTdatRaw::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TASTdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TASTdatRaw " << GetName()
	 << " fHitsN"    << fHitsN
     << endl;
}

//------------------------------------------+-----------------------------------
//! Access i 'th hit

TASTrawHit* TASTdatRaw::GetHit(Int_t i)
{
  return (TASTrawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TASTrawHit* TASTdatRaw::GetHit(Int_t i) const
{
  return (const TASTrawHit*) ((*fListOfHits)[i]);;
}
