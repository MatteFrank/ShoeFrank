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
   fIsValid(true),
   fCharge(-1),
   fTime(-1),
   fDe(-1)
{
  Clear();
}

//------------------------------------------+-----------------------------------
//! Constructor
TASThit::TASThit(Double_t charge, Double_t De, Double_t time, bool pileup)
 : TAGobject(),
   fIsValid(true),
   fCharge(charge),
   fTime(time),
   fDe(De),
   fPileUp(pileup)
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
  fRiseTime = -9999;
  
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
   return fListOfHits->GetEntriesFast();
}

//______________________________________________________________________________
//
TASThit* TASTntuHit::NewHit(double charge, double de, double time, bool pileup)
{
   TClonesArray &pixelArray = *fListOfHits;
   //cout <<" sono in new hit: "<< pileup << endl;
   TASThit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASThit(charge, de, time, pileup);
   //cout << "sono hit: " << hit -> GetPileUp() << endl;
   

   return hit;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit
TASThit* TASTntuHit::GetHit(Int_t i)
{
  if(i>=0 && i<GetHitsN())
    return (TASThit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TASThit* TASTntuHit::GetHit(Int_t i) const
{
  if(i>=0 && i<GetHitsN())
    return (const TASThit*) ((*fListOfHits)[i]);
  else
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TASTntuHit::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TASThit");
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TASTntuHit::Clear(Option_t*)
{
  TAGdata::Clear();
  if (fListOfHits) fListOfHits->Clear("C");

  return;
}


