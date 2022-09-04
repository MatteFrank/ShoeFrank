/*!
  \file TASTntuHit.cxx
  \brief   Implementation of TASTntuHit.
*/

#include "TString.h"

#include "TASTntuHit.hxx"

//! Class Imp
ClassImp(TASThit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TASThit::TASThit()
 : TAGobject(),
   fCharge(0.),
   fTime(0.),
   fDe(0.)
{
  Clear();
}

//------------------------------------------+-----------------------------------
//! Constructor
TASThit::TASThit(Double_t charge, Double_t De, Double_t time)
 : TAGobject(),
   fCharge(charge),
   fTime(time),
   fDe(De)
{
   
}

//------------------------------------------+-----------------------------------
//! Destructor.

TASThit::~TASThit()
{
}

//------------------------------------------+-----------------------------------
void TASThit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//------------------------------------------+-----------------------------------
void TASThit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}


//##############################################################################
//! Class Imp
ClassImp(TASTntuHit);

TString TASTntuHit::fgkBranchName   = "strh.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTntuHit::TASTntuHit()
: TAGdata(),
 fListOfHits(0)
{
  fCharge=-1000;
  fTrigTime=-1000;
  fTrigTimeOth=-1000;
  fTrigType=-1000;
  SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TASTntuHit::~TASTntuHit()
{
  delete fListOfHits;
}

//------------------------------------------+-----------------------------------
Int_t TASTntuHit::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//______________________________________________________________________________
//
TASThit* TASTntuHit::NewHit(double charge, double de, double time)
{
   TClonesArray &pixelArray = *fListOfHits;
   TASThit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASThit(charge, de, time);
   
   return hit;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TASThit* TASTntuHit::GetHit(Int_t i)
{
   return (TASThit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TASThit* TASTntuHit::GetHit(Int_t i) const
{
   return (const TASThit*) ((*fListOfHits)[i]);;
}

   
//------------------------------------------+-----------------------------------
//! Setup clones.

void TASTntuHit::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TASThit");
   
  return;
}
   

//------------------------------------------+-----------------------------------
//! Clear event.

void TASTntuHit::Clear(Option_t*)
{
  TAGdata::Clear();
  if (fListOfHits) fListOfHits->Clear("C");

  return;
}


