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
  nirhit(0), hir(0), m_run_time(0x0)
{
   SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TASTdatRaw::~TASTdatRaw() {
  delete hir;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TASTdatRaw::SetupClones()
{
  if (!hir) hir = new TClonesArray("TASTrawHit");
  return;
}


//------------------------------------------+-----------------------------------
//! Clear event.

void TASTdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  nirhit = 0;

  if (hir) hir->Clear();
  return;
}


void TASTdatRaw::NewHit(TWaveformContainer &W)
{
  
  TClonesArray &pixelArray = *hir;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(W);
  nirhit++;
  return;
}



/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TASTdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TASTdatRaw " << GetName()
	 << " nirhit"    << nirhit
     << endl;
  return;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TASTrawHit* TASTdatRaw::Hit(Int_t i)
{
  return (TASTrawHit*) ((*hir)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access i 'th hit

const TASTrawHit* TASTdatRaw::Hit(Int_t i) const
{
  return (const TASTrawHit*) ((*hir)[i]);;
}
