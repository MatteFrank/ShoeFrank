/*!
  \file
  \version $Id: TABMdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TABMdatRaw.
*/

//using namespace std;
#include "TClonesArray.h"

#include "TABMdatRaw.hxx"


/*!
  \class TABMdatRaw TABMdatRaw.hxx "TABMdatRaw.hxx"
  \brief Mapping and Geometry parameters for Tof wall. **
*/


ClassImp(TABMdatRaw);

TString TABMdatRaw::fgkBranchName   = "bmdat.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMdatRaw::TABMdatRaw()
 : TAGdata(),
   fListOfHits(0x0),
   fiNDrop(0),
   fTrigTime(0.)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMdatRaw::~TABMdatRaw()
{
   delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup mapping data for a single slat.

TABMrawHit* TABMdatRaw::NewHit(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time)
{
   TClonesArray &pixelArray = *fListOfHits;
   TABMrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TABMrawHit(id, lay, view, cell, time);
   
   return hit;
}

//------------------------------------------+-----------------------------------
// ! Get number of hits
Int_t TABMdatRaw::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit
TABMrawHit* TABMdatRaw::GetHit(Int_t i)
{
   return (TABMrawHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TABMrawHit* TABMdatRaw::GetHit(Int_t i) const
{
   return (const TABMrawHit*) ((*fListOfHits)[i]);;
}
//------------------------------------------+-----------------------------------
//! Setup clones.

void TABMdatRaw::SetupClones()
{
   if (!fListOfHits) fListOfHits = new TClonesArray("TABMrawHit");
   return;
}

/*------------------------------------------+---------------------------------*/
//! Set statistics counters.

void TABMdatRaw::AddDischarged()
{
  fiNDrop++;
  return;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TABMdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();

  fListOfHits->Delete();
  fiNDrop=0;
  fTrigTime=-1000;
}

/*------------------------------------------+---------------------------------*/
//! helper to format one converter value

static void print_value(ostream& os, Int_t i_val)
{
  char c_ran = (i_val & 0x1000) ? 'h' : 'l';
  os << Form("%4d", i_val & 0x0fff) << " " << c_ran;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TABMdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TABMdatRaw " << GetName()
     << Form("  nhit=%3d", GetHitsN())
     << Form("  ndrop=%3d", NDrop())
     << endl;
  
  os << "slat stat    adct    adcb    tdct    tdcb" << endl;
  for (Int_t i = 0; i < GetHitsN(); i++) {
    const TABMrawHit* hit = GetHit(i);
    os << Form("%4d", hit->GetCell());
    os << "  "; print_value(os, hit->GetPlane());
    os << endl;
  }
  return;
}
