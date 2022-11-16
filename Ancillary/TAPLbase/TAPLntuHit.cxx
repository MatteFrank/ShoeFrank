/*!
 \file
 \version $Id: TAPLntuHit.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TAPLntuHit.
 */

#include "TString.h"
#include "TAPLntuHit.hxx"

/*!
 \class TAPLntuHit TAPLntuHit.hxx "TAPLntuHit.hxx"
 \brief Mapping and Geometry parameters for IR detectors. **
 */

ClassImp(TAPLhit);

TString TAPLntuHit::fgkBranchName   = "stntu.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TAPLhit::TAPLhit()
: TAGbaseWC()
{
}

//------------------------------------------+-----------------------------------
//! constructor.
TAPLhit::TAPLhit(TAGwaveCatcher* w)
: TAGbaseWC(w)
{
   
}


//------------------------------------------+-----------------------------------
//! Destructor.
TAPLhit::~TAPLhit(){
   
}


//##############################################################################

ClassImp(TAPLntuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAPLntuHit::TAPLntuHit()
 : TAGdata(),
   fHit(0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAPLntuHit::~TAPLntuHit()
{
   if(fHit)delete fHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TAPLntuHit::SetupClones()
{
   if (!fHit) fHit = new TAPLhit();
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TAPLntuHit::Clear(Option_t*)
{
   if (fHit) fHit->Clear();
}

//-----------------------------------------------------------------------------
//! access to the hit
TAPLhit* TAPLntuHit::GetHit()
{
   return fHit;
}


//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TAPLhit* TAPLntuHit::GetHit() const
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TAPLhit* TAPLntuHit::NewHit(TAGwaveCatcher *W)
{
   fHit->SetHit(W);
   
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TAPLhit* TAPLntuHit::NewHit()
{
   return fHit;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAPLntuHit::ToStream(ostream& os, Option_t* option) const
{
   os << "TAPLntuHit " << GetName()
   << endl;
}




