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

/*!
  \class TACAdatRaw TACAdatRaw.hxx "TACAdatRaw.hxx"
  \brief Mapping and Geometry parameters for CA detectors. **
*/

ClassImp(TACArawHit);

//------------------------------------------+-----------------------------------
//! Destructor.

TACArawHit::~TACArawHit()
{}

//------------------------------------------+-----------------------------------
//! Default constructor.

TACArawHit::TACArawHit()
  : fTime(999999.), fCharge(0.), fType(0), fChannelID(0)
{
}

TACArawHit::TACArawHit(int typ, int cha, double charge, double time) {

  fTime = time;
  fCharge  = charge;
  fType  = typ;
  fChannelID   = cha;

}

void TACArawHit::SetData(Int_t type, Int_t id, Double_t time, Double_t charge) {

  fTime = time;
  fCharge  = charge;
  fType  = type;
  fChannelID   = id;
  return;
}
//##############################################################################

ClassImp(TACAdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAdatRaw::TACAdatRaw() :
  fHitsN(0), fListOfHits(0) {
}


//------------------------------------------+-----------------------------------
//! Destructor.

TACAdatRaw::~TACAdatRaw() {
  delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TACAdatRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TACArawHit");
  return;
}


/*------------------------------------------+---------------------------------*/
//! Set statistics counters.

void TACAdatRaw::SetCounter(Int_t i_ntdc, Int_t i_nadc, Int_t i_ndrop)
{
  fiNTdc  = i_ntdc;
  fiNAdc  = i_nadc;
  fiNDrop = i_ndrop;
  return;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TACAdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();

  fHitsN = 0;
  if (fListOfHits) fListOfHits->Clear();

  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TACAdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TACAdatRaw " << GetName()
     << endl;
  return;
}
