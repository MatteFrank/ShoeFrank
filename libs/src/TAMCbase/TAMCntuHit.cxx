/*!
  \file
  \version $Id: TATntuRaw.cxx,v 1.10 2003/06/22 11:48:38 mueller Exp $
  \brief   Implementation of TATntuRaw.
*/

#include "TAMCntuHit.hxx"

/*!
  \class TAMChit TAMCntuHit.hxx "TAMCntuHit.hxx"
  \brief Ntuplize ToF raw data - hit object. **
*/

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
Int_t TAMCntuHit::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
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
//! return a pixel for a given sensor
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
//
TAMChit* TAMCntuHit::NewHit(Int_t id, Int_t layer, Int_t view, Int_t cell, TVector3 inpos, TVector3 outpos, TVector3 inmom, TVector3 outmom, Double_t de, Double_t tof, Int_t trackId)
{
   TClonesArray &pixelArray = *fListOfHits;
   TAMChit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TAMChit(id, layer, view, cell, inpos, outpos, inmom, outmom, de, tof, trackId);
   return hit;
}

//------------------------------------------+-----------------------------------
//! Clear.

void TAMCntuHit::Clear(Option_t*)
{
  fListOfHits->Clear("C");

  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

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

