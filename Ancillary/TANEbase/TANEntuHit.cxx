/*!
 \file TANEntuHit.cxx
 \brief   Implementation of TANEntuHit.
 */

#include "TANEntuHit.hxx"

//! Class Imp
ClassImp(TANEhit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TANEhit::TANEhit()
 : TAGobject(),
   fTime(999999.),
   fCharge(0.),
   fCrystalId(0),
   fPosition(),
   fIsValid(true)
{
  Clear();
}

//______________________________________________________________________________
//!  build hit
//!
//! \param[in] cha crystal index
//! \param[in] charge measured charge
//! \param[in] time measured time
//! \param[in] type given type
TANEhit::TANEhit(int cha, double charge, double time)
 : TAGobject(),
   fTime(time),
   fCharge(charge),
   fCrystalId(cha),
   fPosition(),
   fIsValid(true)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TANEhit::~TANEhit()
{}

//______________________________________________________________________________
//! Clear
void TANEhit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//! Add MC track and hit indexes
//!
//! \param[in] trackId MC track index
//! \param[in] mcId MC hit index
void TANEhit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;

   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}

//##############################################################################

//! Class Imp
ClassImp(TANEntuHit);

TString TANEntuHit::fgkBranchName   = "nerh.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TANEntuHit::TANEntuHit()
 : TAGdata(),
   fListOfHits(0x0)
{
   SetupClones();
   fiNDrop=0;
}


//------------------------------------------+-----------------------------------
//! Destructor.
TANEntuHit::~TANEntuHit()
{
   delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TANEntuHit::SetupClones()
{
   if (!fListOfHits) fListOfHits = new TClonesArray("TANEhit");
}

//------------------------------------------+-----------------------------------
//! return number of hits
Int_t TANEntuHit::GetHitsN() const
{
   return fListOfHits->GetEntriesFast();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given crystal
//!
//! \param[in] id crystal id
TANEhit* TANEntuHit::GetHit(Int_t id)
{
   if (id >=0 || id < 22*22) {
      return (TANEhit*)fListOfHits->At(id);
   } else {
      cout << Form("Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given crystal const
//!
//! \param[in] id crystal id
const TANEhit* TANEntuHit::GetHit(Int_t id) const
{
   if (id >=0 || id < 22*22) {
      return (TANEhit*)fListOfHits->At(id);
   } else {
      Error("GetPixel()", "Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//______________________________________________________________________________
//! new hit
//!
//! \param[in] crys crystal index
//! \param[in] charge measured charge
//! \param[in] time measured time
TANEhit* TANEntuHit::NewHit(int crys, double charge, double time)
{
   TClonesArray &pixelArray = *fListOfHits;

   TANEhit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TANEhit(crys, charge, time);
   return hit;

}

//------------------------------------------+-----------------------------------
//! Clear event.
void TANEntuHit::Clear(Option_t*)
{
  TAGdata::Clear();
  if (fListOfHits) fListOfHits->Clear("C");

   return;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TANEntuHit::ToStream(ostream& os, Option_t* option) const
{
   os << "TANEntuHit " << GetName()
   << endl;
   return;
}
