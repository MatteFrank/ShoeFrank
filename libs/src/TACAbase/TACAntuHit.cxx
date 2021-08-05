/*!
 \file
 \version $Id: TACAntuHit.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TACAntuHit.
 */

#include "TACAntuHit.hxx"

/*!
 \class TACAntuHit TACAntuHit.hxx "TACAntuHit.hxx"
 \brief Mapping and Geometry parameters for CA detectors. **
 */

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

//------------------------------------------+-----------------------------------
TACAhit::TACAhit(int cha, double charge, double time, int typ)
 : TAGobject(),
   fTime(time),
   fCharge(charge),
   fCrystalId(cha),
   fType(typ),
   fPosition(),
   fIsValid(true)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAhit::~TACAhit()
{}

//______________________________________________________________________________
//
void TACAhit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//______________________________________________________________________________
//
void TACAhit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;

   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}

//##############################################################################

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
Int_t TACAntuHit::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
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
//! return a pixel for a given sensor
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
//
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

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TACAntuHit::ToStream(ostream& os, Option_t* option) const
{
   os << "TACAntuHit " << GetName()
   << endl;
   return;
}
