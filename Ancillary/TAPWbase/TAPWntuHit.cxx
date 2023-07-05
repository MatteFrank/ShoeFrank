/*!
 \file
 \version $Id: TAPWntuHit.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TAPWntuHit.
 */

#include "TString.h"
#include "TAPWntuHit.hxx"

/*!
 \class TAPWntuHit TAPWntuHit.hxx "TAPWntuHit.hxx"
 \brief Mapping and Geometry parameters for IR detectors. **
 */

ClassImp(TAPWhit);

TString TAPWntuHit::fgkBranchName   = "pwntu.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TAPWhit::TAPWhit()
: TAGbaseWC()
{
}

//------------------------------------------+-----------------------------------
//! constructor.
TAPWhit::TAPWhit(TAGwaveCatcher* w, Bool_t pw)
: TAGbaseWC(w, pw)
{
   
}


//------------------------------------------+-----------------------------------
//! Destructor.
TAPWhit::~TAPWhit(){
   
}


//##############################################################################

ClassImp(TAPWntuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAPWntuHit::TAPWntuHit()
 : TAGdata(),
   fHit(0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAPWntuHit::~TAPWntuHit()
{
   if(fHit)delete fHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TAPWntuHit::SetupClones()
{
   if (!fHit) fHit = new TAPWhit();
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TAPWntuHit::Clear(Option_t*)
{
   if (fHit) fHit->Clear();
}

//------------------------------------------+-----------------------------------
//! New hit
TAPWhit* TAPWntuHit::NewHit()
{
   return fHit;
}


//-----------------------------------------------------------------------------
//! access to the hit
TAPWhit* TAPWntuHit::GetHit()
{
   return fHit;
}


//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit
const TAPWhit* TAPWntuHit::GetHit() const
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TAPWhit* TAPWntuHit::NewHit(TAGwaveCatcher *W, Bool_t pwFlag)
{
   fHit->SetHit(W, pwFlag);
   
   return fHit;
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAPWntuHit::ToStream(ostream& os, Option_t* option) const
{
   os << "TAPWntuHit " << GetName()
   << endl;
}




