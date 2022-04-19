/*!
  \file TAMCntuHit.cxx
  \brief   Implementation of TAMChit.
*/

#include "TAMCntuHit.hxx"

/*!
  \class TAMChit 
  \brief Ntuplize ToF raw data - hit object. **
*/

//! Class Imp
ClassImp(TAMChit);

TString  TAMCntuHit::fgkStcBranchName = "mcst.";
TString  TAMCntuHit::fgkBmBranchName  = "mcbm.";
TString  TAMCntuHit::fgkVtxBranchName = "mcvt.";
TString  TAMCntuHit::fgkItrBranchName = "mcit.";
TString  TAMCntuHit::fgkMsdBranchName = "mcmsd.";
TString  TAMCntuHit::fgkTofBranchName = "mctw.";
TString  TAMCntuHit::fgkCalBranchName = "mcca.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMChit::TAMChit()
 : fID(-99),
   fInPosition(0,0,0),
   fOutPosition(0,0,0),
   fInMomentum(0,0,0),
   fOutMomentum(0,0,0),
   fDeltaE(0.),
   fTof(0.),
   fTrackId(-1)
{
}

//------------------------------------------+-----------------------------------
//! Construct with data
//!
//! \param[in] id sensor id
//! \param[in] layer layer id
//! \param[in] view view id
//! \param[in] cell cell id
//! \param[in] inpos input position
//! \param[in] outpos output position
//! \param[in] inmom input momentum
//! \param[in] outmom output momentum
//! \param[in] de energy loss
//! \param[in] tof time
//! \param[in] trackId track index
TAMChit::TAMChit(Int_t id, Int_t layer, Int_t view, Int_t cell, TVector3 inpos, TVector3 outpos,TVector3 inmom, TVector3 outmom, Double_t de, Double_t tof, Int_t trackId)
 : fID(id),
   fLayer(layer),
   fView(view),
   fCell(cell),
   fInPosition(inpos),
   fOutPosition(outpos),
   fInMomentum(inmom),
   fOutMomentum(outmom),
   fDeltaE(de),
   fTof(tof),
   fTrackId(trackId)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMChit::~TAMChit()
{
}

//##############################################################################

/*!
  \class TAMCntuHit TAMCntuHit.hxx "TAMCntuHit.hxx"
  \brief Ntuplize ToF raw data - container. **
*/

//! Class Imp
ClassImp(TAMCntuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMCntuHit::TAMCntuHit()
 : TAGdata(),
   fListOfHits(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMCntuHit::~TAMCntuHit()
{
   delete fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMCntuHit::SetupClones()
{
  if (!fListOfHits)
     fListOfHits = new TClonesArray("TAMChit");
  return;
}

//------------------------------------------+-----------------------------------
//! Get number of hits
Int_t TAMCntuHit::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
//!
//! \param[in] id hit index
TAMChit* TAMCntuHit::GetHit(Int_t id)
{
   if (id >=0 || id < fListOfHits->GetEntriesFast()) {
      return (TAMChit*)fListOfHits->At(id);
   } else {
      cout << Form("Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a hit for a given index (const)
//!
//! \param[in] id hit index
const TAMChit* TAMCntuHit::GetHit(Int_t id) const
{
   if (id >=0 || id < fListOfHits->GetEntriesFast()) {
      return (TAMChit*)fListOfHits->At(id);
   } else {
      Error("GetPixel()", "Wrong sensor number %d\n", id);
      return 0x0;
   }
}

//______________________________________________________________________________
//! New hit
//!
//! \param[in] id sensor id
//! \param[in] layer layer id
//! \param[in] view view id
//! \param[in] cell cell id
//! \param[in] inpos input position
//! \param[in] outpos output position
//! \param[in] inmom input momentum
//! \param[in] outmom output momentum
//! \param[in] de energy loss
//! \param[in] tof time
//! \param[in] trackId track index
TAMChit* TAMCntuHit::NewHit(Int_t id, Int_t layer, Int_t view, Int_t cell, TVector3 inpos, TVector3 outpos, TVector3 inmom, TVector3 outmom, Double_t de, Double_t tof, Int_t trackId)
{
   TClonesArray &pixelArray = *fListOfHits;
   TAMChit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TAMChit(id, layer, view, cell, inpos, outpos, inmom, outmom, de, tof, trackId);
   return hit;
}

//------------------------------------------+-----------------------------------
//! Clear event.
//!
//! \param[in] opt option for clearing (not used)
void TAMCntuHit::Clear(Option_t*)
{
  fListOfHits->Clear("C");

  return;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAMCntuHit::ToStream(ostream& os, Option_t* option) const
{
  os << "TAMCntuHit" 
     << Form("  nhit=%3d", GetHitsN())
     << endl;

  if (GetHitsN() == 0) return;
  
  os << "ind slat stat  adct  adcb  tdct  tdcb" << endl;
  
  for (Int_t i = 0; i < GetHitsN(); i++) {
    const TAMChit* p_hit = GetHit(i);
    os << Form("%3d %3d", i, p_hit->GetTrackIdx())
       << endl;
  }

  return;
}

