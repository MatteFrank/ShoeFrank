/*!
 \file
 \version $Id: TAPWntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TAPWntuRaw.
 */

#include "TString.h"
#include "TAPWntuRaw.hxx"

/*!
 \class TAPWntuRaw TAPWntuRaw.hxx "TAPWntuRaw.hxx"
 \brief Mapping and Geometry parameters for IR detectors. **
 */

ClassImp(TAPWrawHit);

TString TAPWntuRaw::fgkBranchName   = "pwdat.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TAPWrawHit::TAPWrawHit()
: TAGwaveCatcher()
{
}

//------------------------------------------+-----------------------------------
//! constructor.
TAPWrawHit::TAPWrawHit(const TAGwaveCatcher& w)
: TAGwaveCatcher(w)
{
   
}


//------------------------------------------+-----------------------------------
//! Destructor.
TAPWrawHit::~TAPWrawHit(){
   
}

//##############################################################################

ClassImp(TAPWntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAPWntuRaw::TAPWntuRaw()
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

TAPWntuRaw::~TAPWntuRaw()
{
   if(fHit)delete fHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TAPWntuRaw::SetupClones()
{
   if (!fHit) fHit = new TAPWrawHit();
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TAPWntuRaw::Clear(Option_t*)
{
   if (fHit) fHit->Clear();
}

//-----------------------------------------------------------------------------
//! access to the hit
TAPWrawHit* TAPWntuRaw::GetHit()
{
   return fHit;
}


//------------------------------------------+-----------------------------------
//! Read-only access hit
const TAPWrawHit* TAPWntuRaw::GetHit() const
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TAPWrawHit* TAPWntuRaw::NewHit(const TAGwaveCatcher& w)
{
   fHit->SetWave(w);
   return fHit;
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAPWntuRaw::ToStream(ostream& os, Option_t* option) const
{
   os << "TAPWntuRaw " << GetName()
   << endl;
}




