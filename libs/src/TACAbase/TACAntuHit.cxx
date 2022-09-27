/*!
 \file TACAntuHit.cxx
 \brief   Implementation of TACAntuHit.
 */

#include "TACAntuHit.hxx"

//! Class Imp
ClassImp(TACAhit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACAhit::TACAhit()
 : TAGobject(),
   fTime(999999.),
   fCharge(0.),
   fCrystalId(0),
   fType(0),
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
TACAhit::TACAhit(int cha, double charge, double time, int type)
 : TAGobject(),
   fTime(time),
   fCharge(charge),
   fCrystalId(cha),
   fType(type),
   fPosition(),
   fIsValid(true)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TACAhit::~TACAhit()
{}

//______________________________________________________________________________
//! Clear
void TACAhit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//! Add MC track and hit indexes
//!
//! \param[in] trackId MC track index
//! \param[in] mcId MC hit index
void TACAhit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;

   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}

//##############################################################################

//! Class Imp
ClassImp(TACAntuHit);

TString TACAntuHit::fgkBranchName   = "carh.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TACAntuHit::TACAntuHit()
 : TAGdata(),
   fListOfHits(0x0)
{
   SetupClones();
   fiNDrop=0;
}


//------------------------------------------+-----------------------------------
//! Destructor.
TACAntuHit::~TACAntuHit()
{
   delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TACAntuHit::SetupClones()
{
   if (!fListOfHits) fListOfHits = new TClonesArray("TACAhit");
}

//------------------------------------------+-----------------------------------
//! return number of hits
Int_t TACAntuHit::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given crystal
//!
//! \param[in] id crystal id
TACAhit* TACAntuHit::GetHit(Int_t id)
{
   if (id >=0 || id < 22*22) {
      return (TACAhit*)fListOfHits->At(id);
   } else {
      cout << Form("Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given crystal const
//!
//! \param[in] id crystal id
const TACAhit* TACAntuHit::GetHit(Int_t id) const
{
   if (id >=0 || id < 22*22) {
      return (TACAhit*)fListOfHits->At(id);
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
//! \param[in] type given type
TACAhit* TACAntuHit::NewHit(int crys, double charge, double time, int type)
{
   TClonesArray &pixelArray = *fListOfHits;

   TACAhit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TACAhit(crys, charge, time, type);
   return hit;

}

//------------------------------------------+-----------------------------------
//! Clear event.
void TACAntuHit::Clear(Option_t*)
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
void TACAntuHit::ToStream(ostream& os, Option_t* option) const
{
   os << "TACAntuHit " << GetName()
   << endl;
   return;
}
