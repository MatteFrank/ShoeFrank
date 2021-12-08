
#include "TAMCntuRegion.hxx"

/*!
  \class TAMCregion
  \brief Crossing region - region object. **
*/

ClassImp(TAMCregion);

      TString TAMCntuRegion::fgkBranchName  = "mcreg.";
const TString TAMCntuRegion::fgkDefParaName = "regMc";

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCregion::TAMCregion()
 : TObject(),
   fID(-99),
   fCrossN(0),
   fOldCrossN(0),
   fPosition(0,0,0),
   fMomentum(0,0,0),
   fMass(0.),
   fCharge(0.),
   fTime(0.)
{
}

//------------------------------------------+-----------------------------------
//! Construct with data
TAMCregion::TAMCregion(Int_t id, Int_t nreg, Int_t nregold, TVector3 pos, TVector3 mom, Double_t mass,
                 Double_t charge, Double_t time)
 : TObject(),
   fID(id),
   fCrossN(nreg),
   fOldCrossN(nregold),
   fPosition(pos),
   fMomentum(mom),
   fMass(mass),
   fCharge(charge),
   fTime(time)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCregion::~TAMCregion()
{
}

//##############################################################################

/*!
  \class TAMCntuRegion
  \brief Crossing region - container. **
*/

ClassImp(TAMCntuRegion);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCntuRegion::TAMCntuRegion()
 : TAGdata(),
   fListOfRegions(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCntuRegion::~TAMCntuRegion()
{
   delete fListOfRegions;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TAMCntuRegion::SetupClones()
{
  if (!fListOfRegions)
     fListOfRegions = new TClonesArray("TAMCregion");
}

//------------------------------------------+-----------------------------------
Int_t TAMCntuRegion::GetRegionsN() const
{
   return fListOfRegions->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TAMCregion* TAMCntuRegion::GetRegion(Int_t id)
{
   if (id >=0 || id < fListOfRegions->GetEntriesFast()) {
      return (TAMCregion*)fListOfRegions->At(id);
   } else {
      cout << Form("Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAMCregion* TAMCntuRegion::GetRegion(Int_t id) const
{
   if (id >=0 || id < fListOfRegions->GetEntriesFast()) {
      return (TAMCregion*)fListOfRegions->At(id);
   } else {
      Error("GetRegion()", "Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//______________________________________________________________________________
//
TAMCregion* TAMCntuRegion::NewRegion(Int_t id, Int_t nreg, Int_t nregold,TVector3 pos,TVector3 mom, Double_t mass,
                            Double_t charge, Double_t time)
{
   TClonesArray &pixelArray = *fListOfRegions;
   TAMCregion* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TAMCregion(id, nreg, nregold, pos, mom, mass,
                                                                       charge, time);
   return hit;
}

//------------------------------------------+-----------------------------------
//! Clear.

void TAMCntuRegion::Clear(Option_t*)
{
  fListOfRegions->Clear("C");

  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAMCntuRegion::ToStream(ostream& os, Option_t* option) const
{
  os << "TAMCntuRegion"
     << Form("  nreg=%3d", GetRegionsN())
     << endl;

  if (GetRegionsN() == 0) return;
    
  for (Int_t i = 0; i < GetRegionsN(); i++) {
    const TAMCregion* p_reg = GetRegion(i);
    os << Form("%3d %3d", i, p_reg->GetID())
       << endl;
  }

  return;
}

