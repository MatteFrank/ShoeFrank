/*!
 \file
 \version $Id: TACAntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TACAntuRaw.
 */

#include "TACAntuRaw.hxx"

/*!
 \class TACAntuRaw TACAntuRaw.hxx "TACAntuRaw.hxx"
 \brief Mapping and Geometry parameters for CA detectors. **
 */

ClassImp(TACAntuHit);

//------------------------------------------+-----------------------------------
//! Destructor.

TACAntuHit::~TACAntuHit()
{}

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAntuHit::TACAntuHit()
 : TAGobject(),
   fTime(999999.),
   fCharge(0.),
   fCrystalId(0),
   fType(0),
   fPosition()
{
}

//------------------------------------------+-----------------------------------
TACAntuHit::TACAntuHit(int cha, double charge, double time, int typ)
 : TAGobject(),
   fTime(time),
   fCharge(charge),
   fCrystalId(cha),
   fType(typ),
   fPosition()
{
}

//______________________________________________________________________________
//
void TACAntuHit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackId.Set(0);
}

//______________________________________________________________________________
//
void TACAntuHit:: AddMcTrackId(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackId.Set(fMcTrackId.GetSize()+1);
   fMcTrackId[fMcTrackId.GetSize()-1] = trackId;
}

//##############################################################################

ClassImp(TACAntuRaw);

TString TACAntuRaw::fgkBranchName   = "carh.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAntuRaw::TACAntuRaw()
 : TAGdata(),
   fListOfHits(0x0)
{
   SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TACAntuRaw::~TACAntuRaw()
{
   delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TACAntuRaw::SetupClones()
{
   if (!fListOfHits) fListOfHits = new TClonesArray("TACAntuHit");
}

//------------------------------------------+-----------------------------------
Int_t TACAntuRaw::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TACAntuHit* TACAntuRaw::GetHit(Int_t id)
{
   if (id >=0 || id < 22*22) {
      return (TACAntuHit*)fListOfHits->At(id);
   } else {
      cout << Form("Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TACAntuHit* TACAntuRaw::GetHit(Int_t id) const
{
   if (id >=0 || id < 22*22) {
      return (TACAntuHit*)fListOfHits->At(id);
   } else {
      Error("GetPixel()", "Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//______________________________________________________________________________
//
TACAntuHit* TACAntuRaw::NewHit(int crys, double charge, double time, int type)
{
   TClonesArray &pixelArray = *fListOfHits;

   TACAntuHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TACAntuHit(crys, charge, time, type);
   return hit;

}

//------------------------------------------+-----------------------------------
//! Clear event.

void TACAntuRaw::Clear(Option_t*)
{
   fListOfHits->Clear("C");
   
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TACAntuRaw::ToStream(ostream& os, Option_t* option) const
{
   os << "TACAntuRaw " << GetName()
   << endl;
   return;
}
