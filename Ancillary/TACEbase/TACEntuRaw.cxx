/*!
 \file
 \version $Id: TACEntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TACEntuRaw.
 */

#include "TString.h"
#include "TACEntuRaw.hxx"

/*!
 \class TACEntuRaw TACEntuRaw.hxx "TACEntuRaw.hxx"
 \brief Mapping and Geometry parameters for IR detectors. **
 */

ClassImp(TACErawHit);

TString TACEntuRaw::fgkBranchName   = "twdat.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TACErawHit::TACErawHit()
: TAGwaveCatcher()
{
}

//------------------------------------------+-----------------------------------
//! constructor.
TACErawHit::TACErawHit(const TAGwaveCatcher& w)
: TAGwaveCatcher(w)
{
   
}


//------------------------------------------+-----------------------------------
//! Destructor.
TACErawHit::~TACErawHit(){
   
}


//##############################################################################

ClassImp(TACEntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACEntuRaw::TACEntuRaw()
 : TAGdata(),
   fHit(0),
   fSoftwareVersion(""),
   fSamplesN(-1),
   fChannelsN(-1),
   fPeriod(0.0),
   fTimeStamp(0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACEntuRaw::~TACEntuRaw()
{
   if(fHit)delete fHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TACEntuRaw::SetupClones()
{
   if (!fHit) fHit = new TACErawHit;
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TACEntuRaw::Clear(Option_t*)
{
   if (fHit) fHit->Clear();
}


//-----------------------------------------------------------------------------
//! access to the hit
TACErawHit* TACEntuRaw::GetHit()
{
   return fHit;
}


//------------------------------------------+-----------------------------------
//! Read-only access  hit
const TACErawHit* TACEntuRaw::GetHit() const
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TACErawHit* TACEntuRaw::NewHit(const TAGwaveCatcher& w)
{
   fHit->SetWave(w);
   return fHit;
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TACEntuRaw::ToStream(ostream& os, Option_t* option) const
{
   os << "TACEntuRaw " << GetName()
   << endl;
}




