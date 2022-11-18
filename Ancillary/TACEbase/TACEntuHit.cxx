/*!
 \file
 \version $Id: TACEntuHit.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TACEntuHit.
 */

#include "TString.h"
#include "TACEntuHit.hxx"

/*!
 \class TACEntuHit TACEntuHit.hxx "TACEntuHit.hxx"
 \brief Mapping and Geometry parameters for IR detectors. **
 */

ClassImp(TACEhit);

TString TACEntuHit::fgkBranchName   = "twntu.";


//------------------------------------------+-----------------------------------
//! Default constructor.
TACEhit::TACEhit()
: TAGbaseWC()
{
}

//------------------------------------------+-----------------------------------
//! constructor.
TACEhit::TACEhit(TAGwaveCatcher* w)
: TAGbaseWC(w)
{
}


//------------------------------------------+-----------------------------------
//! Destructor.
TACEhit::~TACEhit(){
   
}


//##############################################################################

ClassImp(TACEntuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACEntuHit::TACEntuHit()
 : TAGdata(),
   fHit(0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACEntuHit::~TACEntuHit()
{
   if(fHit)delete fHit;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TACEntuHit::SetupClones()
{
   if (!fHit) fHit = new TACEhit;
}


//------------------------------------------+-----------------------------------
//! Clear event.
void TACEntuHit::Clear(Option_t*)
{
   if (fHit) fHit->Clear();
}

//-----------------------------------------------------------------------------
//! access to the hit
TACEhit* TACEntuHit::GetHit()
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TACEhit* TACEntuHit::NewHit()
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! Read-only access hit
const TACEhit* TACEntuHit::GetHit() const
{
   return fHit;
}

//------------------------------------------+-----------------------------------
//! New hit
TACEhit* TACEntuHit::NewHit(TAGwaveCatcher *W)
{
   fHit->SetHit(W);
   
   return fHit;
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TACEntuHit::ToStream(ostream& os, Option_t* option) const
{
   os << "TACEntuHit " << GetName()
   << endl;
}




