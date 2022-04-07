/*!
\file   TABMntuRaw.cxx
\brief   Declaration of TABMntuRaw, the container of the BM raw hits (TABMntuRaw)
*/

//using namespace std;
#include "TClonesArray.h"
#include "TABMntuRaw.hxx"


/*!
  \file   TABMntuRaw.cxx
  \brief   Declaration of TABMntuRaw, the container of the BM raw hits (TABMntuRaw)
*/


ClassImp(TABMntuRaw);

TString TABMntuRaw::fgkBranchName   = "bmdat.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMntuRaw::TABMntuRaw()
 : TAGdata(),
   fListOfHits(0x0),
   fiNDrop(0),
   fTrigTime(-1000)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMntuRaw::~TABMntuRaw()
{
   delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Create the BM raw hit
//!
//! \param[in] id hit idcell [0-35]
//! \param[in] lay layer of the hit [0-5]
//! \param[in] view view of the hit [0-1]
//! \param[in] cell cell of the hit [0-2]
//! \param[in] time time  of the hit [ns]
TABMrawHit* TABMntuRaw::NewHit(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time)
{
   TClonesArray &pixelArray = *fListOfHits;
   TABMrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TABMrawHit(id, lay, view, cell, time);

   return hit;
}

//------------------------------------------+-----------------------------------
// ! Get number of hits
Int_t TABMntuRaw::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit
TABMrawHit* TABMntuRaw::GetHit(Int_t i)
{
   return (TABMrawHit*) ((*fListOfHits)[i]);
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TABMrawHit* TABMntuRaw::GetHit(Int_t i) const
{
   return (const TABMrawHit*) ((*fListOfHits)[i]);
}
//------------------------------------------+-----------------------------------
//! Setup clones.

void TABMntuRaw::SetupClones()
{
   if (!fListOfHits) fListOfHits = new TClonesArray("TABMrawHit");
   return;
}

/*------------------------------------------+---------------------------------*/
//! increase the number of discharged raw hits

void TABMntuRaw::AddDischarged()
{
  fiNDrop++;
  return;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TABMntuRaw::Clear(Option_t*)
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

void TABMntuRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TABMntuRaw " << GetName()
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
