/*!
  \file
  \version $Id: TASTntuHit.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TASTntuHit.
*/

#include "TString.h"

#include "TASTntuHit.hxx"

/*!
  \class TASTntuHit TASTntuHit.hxx "TASTntuHit.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

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

ClassImp(TASTntuHit);

TString TASTntuHit::fgkBranchName   = "strh.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTntuHit::TASTntuHit()
: TAGdata(),
 m_ListOfHits(0)
{
  m_Charge=-1000;
  m_TrigTime=-1000;
  m_TrigTime_oth=-1000;
  m_TrigType=-1000;
  SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TASTntuHit::~TASTntuHit()
{
  delete m_ListOfHits;
}

//------------------------------------------+-----------------------------------
Int_t TASTntuHit::GetHitsN() const
{
   return m_ListOfHits->GetEntries();
}

//______________________________________________________________________________
//
TASThit* TASTntuHit::NewHit(double charge, double de, double time)
{
   TClonesArray &pixelArray = *m_ListOfHits;
   TASThit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASThit(charge, de, time);
   
   return hit;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TASThit* TASTntuHit::GetHit(Int_t i)
{
   return (TASThit*) ((*m_ListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TASThit* TASTntuHit::GetHit(Int_t i) const
{
   return (const TASThit*) ((*m_ListOfHits)[i]);;
}

   
//------------------------------------------+-----------------------------------
//! Setup clones.

void TASTntuHit::SetupClones()
{
  if (!m_ListOfHits) m_ListOfHits = new TClonesArray("TASThit");
   
  return;
}
   

//------------------------------------------+-----------------------------------
//! Clear event.

void TASTntuHit::Clear(Option_t*)
{
  TAGdata::Clear();
  if (m_ListOfHits) m_ListOfHits->Clear("C");

  return;
}


