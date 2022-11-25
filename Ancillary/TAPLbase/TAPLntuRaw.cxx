/*!
 \file
 \version $Id: TAPLntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TAPLntuRaw.
 */

#include "TString.h"
#include "TAPLntuRaw.hxx"

/*!
 \class TAPLntuRaw TAPLntuRaw.hxx "TAPLntuRaw.hxx"
 \brief Mapping and Geometry parameters for IR detectors. **
 */

ClassImp(TAPLrawHit);

TString TAPLntuRaw::fgkBranchName   = "stdat.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TAPLrawHit::TAPLrawHit()
: TAGwaveCatcher()
{
}

//------------------------------------------+-----------------------------------
//! constructor.
TAPLrawHit::TAPLrawHit(const TAGwaveCatcher& w)
: TAGwaveCatcher(w)
{
   
}


//------------------------------------------+-----------------------------------
//! Destructor.
TAPLrawHit::~TAPLrawHit(){
   
}

//##############################################################################

ClassImp(TAPLntuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAPLntuRaw::TAPLntuRaw()
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

TAPLntuRaw::~TAPLntuRaw()
{
   if(fHit)delete fHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TAPLntuRaw::SetupClones()
{
   if (!fHit) fHit = new TAPLrawHit();
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TAPLntuRaw::Clear(Option_t*)
{
   if (fHit) fHit->Clear();
}

//-----------------------------------------------------------------------------
//! access to the hit
TAPLrawHit* TAPLntuRaw::GetHit()
{
   return fHit;
}


//------------------------------------------+-----------------------------------
//! Read-only access hit
const TAPLrawHit* TAPLntuRaw::GetHit() const
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TAPLrawHit* TAPLntuRaw::NewHit(const TAGwaveCatcher& w)
{
   fHit->SetWave(w);
   return fHit;
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAPLntuRaw::ToStream(ostream& os, Option_t* option) const
{
   os << "TAPLntuRaw " << GetName()
   << endl;
}




