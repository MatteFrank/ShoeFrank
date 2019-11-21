/*!
  \file
  \version $Id: TASTntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TASTntuRaw.
*/

#include "TString.h"

#include "TASTntuRaw.hxx"

/*!
  \class TASTntuRaw TASTntuRaw.hxx "TASTntuRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TASTntuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTntuHit::TASTntuHit()
 : TAGobject(),
   fChannel(0),
   fCharge(0.),
   fTime(0.)
{
   
}

//------------------------------------------+-----------------------------------
//! Constructor
TASTntuHit::TASTntuHit(Int_t channel, Double_t charge, Double_t time)
 : TAGobject(),
   fChannel(channel),
   fCharge(charge),
   fTime(time)
{
   
}

//------------------------------------------+-----------------------------------
//! Destructor.

TASTntuHit::~TASTntuHit()
{
}

//------------------------------------------+-----------------------------------
void TASTntuHit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

//------------------------------------------+-----------------------------------
void TASTntuHit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}


//##############################################################################

ClassImp(TASTntuRaw);

TString TASTntuRaw::fgkBranchName   = "strh.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTntuRaw::TASTntuRaw()
: TAGdata(),
 fListOfHits(0)
{
  m_Charge=-1000;
  m_TrigTime=-1000;
  m_TrigType=-1000;
  SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TASTntuRaw::~TASTntuRaw()
{
  delete fListOfHits;
}

//------------------------------------------+-----------------------------------
Int_t TASTntuRaw::GetHitsN() const
{
   return fListOfHits->GetEntries();
}

//______________________________________________________________________________
//
TASTntuHit* TASTntuRaw::NewHit(int channel, double charge, double time)
{
   TClonesArray &pixelArray = *fListOfHits;
   
   TASTntuHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTntuHit(channel, charge, time);
   
   return hit;
   
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TASTntuHit* TASTntuRaw::Hit(Int_t i)
{
   return (TASTntuHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TASTntuHit* TASTntuRaw::Hit(Int_t i) const
{
   return (const TASTntuHit*) ((*fListOfHits)[i]);;
}

   
//------------------------------------------+-----------------------------------
//! Setup clones.

void TASTntuRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TASTntuHit");
   
  return;
}
   

//------------------------------------------+-----------------------------------
//! Clear event.

void TASTntuRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  if (fListOfHits) fListOfHits->Clear("C");

  return;
}


