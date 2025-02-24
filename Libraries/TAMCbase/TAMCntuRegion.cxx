/*!
 \file TAMCntuRegion.cxx
 \brief   Implementation of TAMCregion.
 */

#include "TAMCntuRegion.hxx"

/*!
  \class TAMCregion
  \brief Crossing region - region object. **
*/

//! Draw Imp
ClassImp(TAMCregion);

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
//!
//! \param[in] id sensor id
//! \param[in] nreg region number
//! \param[in] nregold old region number
//! \param[in] pos position
//! \param[in] mom momentum
//! \param[in] mass mass of particle
//! \param[in] charge atomic number
//! \param[in] time time of particle
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

//! Class Imp
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
//! Get number of regions
Int_t TAMCntuRegion::GetRegionsN() const
{
   return fListOfRegions->GetEntriesFast();
}

//------------------------------------------+-----------------------------------
//! return a region for a given index
//!
//! \param[in] id index region
TAMCregion* TAMCntuRegion::GetRegion(Int_t id)
{
   if (id >=0 && id < fListOfRegions->GetEntriesFast()) {
      return (TAMCregion*)fListOfRegions->At(id);
   } else {
      cout << Form("Wrong region number %d\n", id);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a region for a given index (const)
//!
//! \param[in] id index region
const TAMCregion* TAMCntuRegion::GetRegion(Int_t id) const
{
   if (id >=0 && id < fListOfRegions->GetEntriesFast()) {
      return (TAMCregion*)fListOfRegions->At(id);
   } else {
      Error("GetRegion()", "Wrong region number %d\n", id);
      return 0x0;
   }
}

//______________________________________________________________________________
//! New region
//!
//! \param[in] id sensor id
//! \param[in] nreg region number
//! \param[in] nregold old region number
//! \param[in] pos position
//! \param[in] mom momentum
//! \param[in] mass mass of particle
//! \param[in] charge atomic number
//! \param[in] time time of particle
TAMCregion* TAMCntuRegion::NewRegion(Int_t id, Int_t nreg, Int_t nregold,TVector3 pos,TVector3 mom, Double_t mass,
                            Double_t charge, Double_t time)
{
   TClonesArray &pixelArray = *fListOfRegions;
   TAMCregion* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TAMCregion(id, nreg, nregold, pos, mom, mass,
                                                                       charge, time);
   return hit;
}

//------------------------------------------+-----------------------------------
//! Clear event.
//!
//! \param[in] opt option for clearing (not used)
void TAMCntuRegion::Clear(Option_t*)
{
  fListOfRegions->Clear("C");

  return;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
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

