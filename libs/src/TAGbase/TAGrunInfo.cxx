/*!
  \file
  \version $Id: TAGrunInfo.cxx
  \brief   Implementation of TAGrunInfo.
*/

#include "TBuffer.h"

#include "TAGrunInfo.hxx"

/*!
  \class TAGrunInfo TAGrunInfo.hxx "TAGrunInfo.hxx"
  \brief Run number info. **
*/

ClassImp(GlobalParameter_t)
ClassImp(TAGrunInfo);

TString TAGrunInfo::fgkObjectName = "runinfo";

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGrunInfo::TAGrunInfo()
 : fsCam(""),
   fiRun(-1)
{}

//------------------------------------------+-----------------------------------
//! Construct with campaign and run number.

TAGrunInfo::TAGrunInfo(TString s_cam, Short_t i_run)
 : fsCam(s_cam),
   fiRun(i_run)
{}

//------------------------------------------+-----------------------------------
//! copy cstr
TAGrunInfo::TAGrunInfo(const TAGrunInfo& right)
 : fsCam(right.fsCam),
   fiRun(right.fiRun)
{
   fGlobalParameter.EnableLocalReco  = right.fGlobalParameter.EnableLocalReco;
   fGlobalParameter.EnableTree       = right.fGlobalParameter.EnableTree;
   fGlobalParameter.EnableHisto      = right.fGlobalParameter.EnableHisto;
   fGlobalParameter.EnableSaveHits   = right.fGlobalParameter.EnableSaveHits;
   fGlobalParameter.EnableTracking   = right.fGlobalParameter.EnableTracking;
   fGlobalParameter.EnableRootObject = right.fGlobalParameter.EnableRootObject;
   fGlobalParameter.EnableTofZmc     = right.fGlobalParameter.EnableTofZmc;
   fGlobalParameter.IncludeKalman    = right.fGlobalParameter.IncludeKalman;
   fGlobalParameter.IncludeTOE       = right.fGlobalParameter.IncludeTOE;
   fGlobalParameter.IncludeDI        = right.fGlobalParameter.IncludeDI;
   fGlobalParameter.IncludeST        = right.fGlobalParameter.IncludeST;
   fGlobalParameter.IncludeBM        = right.fGlobalParameter.IncludeBM;
   fGlobalParameter.IncludeTG        = right.fGlobalParameter.IncludeTG;
   fGlobalParameter.IncludeVT        = right.fGlobalParameter.IncludeVT;
   fGlobalParameter.IncludeIT        = right.fGlobalParameter.IncludeIT;
   fGlobalParameter.IncludeMSD       = right.fGlobalParameter.IncludeMSD;
   fGlobalParameter.IncludeTW        = right.fGlobalParameter.IncludeTW;
   fGlobalParameter.IncludeCA        = right.fGlobalParameter.IncludeCA;
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGrunInfo::~TAGrunInfo()
{}

//------------------------------------------+-----------------------------------
//! operator =
const TAGrunInfo& TAGrunInfo::operator=(const TAGrunInfo &right)
{
   fiRun = right.fiRun;
   fsCam = right.fsCam;
   
   fGlobalParameter.EnableLocalReco  = right.fGlobalParameter.EnableLocalReco;
   fGlobalParameter.EnableTree       = right.fGlobalParameter.EnableTree;
   fGlobalParameter.EnableHisto      = right.fGlobalParameter.EnableHisto;
   fGlobalParameter.EnableSaveHits   = right.fGlobalParameter.EnableSaveHits;
   fGlobalParameter.EnableTracking   = right.fGlobalParameter.EnableTracking;
   fGlobalParameter.EnableRootObject = right.fGlobalParameter.EnableRootObject;
   fGlobalParameter.EnableTofZmc     = right.fGlobalParameter.EnableTofZmc;
   fGlobalParameter.IncludeKalman    = right.fGlobalParameter.IncludeKalman;
   fGlobalParameter.IncludeTOE       = right.fGlobalParameter.IncludeTOE;
   fGlobalParameter.IncludeDI        = right.fGlobalParameter.IncludeDI;
   fGlobalParameter.IncludeST        = right.fGlobalParameter.IncludeST;
   fGlobalParameter.IncludeBM        = right.fGlobalParameter.IncludeBM;
   fGlobalParameter.IncludeTG        = right.fGlobalParameter.IncludeTG;
   fGlobalParameter.IncludeVT        = right.fGlobalParameter.IncludeVT;
   fGlobalParameter.IncludeIT        = right.fGlobalParameter.IncludeIT;
   fGlobalParameter.IncludeMSD       = right.fGlobalParameter.IncludeMSD;
   fGlobalParameter.IncludeTW        = right.fGlobalParameter.IncludeTW;
   fGlobalParameter.IncludeCA        = right.fGlobalParameter.IncludeCA;
   
   return *this;
}

//------------------------------------------+-----------------------------------
//! Clear run info.

void TAGrunInfo::Clear(Option_t*)
{
  fsCam = "";
  fiRun = -1;
}

//------------------------------------------+-----------------------------------
//! ostream insertion.

void TAGrunInfo::ToStream(ostream& os, Option_t* option) const
{
  os << "Run info:     "
   << Form("  cam: %s", fsCam.Data())
   << Form("  run: %4d", fiRun)
   << endl;
   
   os << "Global info:     \n"
   << Form("  EnableLocalReco: %d\n", fGlobalParameter.EnableLocalReco)
   << Form("  EnableTree: %d\n", fGlobalParameter.EnableTree)
   << Form("  EnableHisto: %d\n", fGlobalParameter.EnableHisto)
   << Form("  EnableTracking: %d\n", fGlobalParameter.EnableTracking)
   << Form("  EnableSaveHits: %d\n", fGlobalParameter.EnableSaveHits)
   << Form("  EnableRootObject: %d\n", fGlobalParameter.EnableRootObject)
   << Form("  EnableTofZmc: %d\n\n", fGlobalParameter.EnableTofZmc)
   << Form("  IncludeKalman: %d\n", fGlobalParameter.IncludeKalman)
   << Form("  IncludeTOE: %d\n\n", fGlobalParameter.IncludeTOE)
   << Form("  IncludeDI: %d\n", fGlobalParameter.IncludeDI)
   << Form("  IncludeST: %d\n", fGlobalParameter.IncludeST)
   << Form("  IncludeBM: %d\n", fGlobalParameter.IncludeBM)
   << Form("  IncludeTG: %d\n", fGlobalParameter.IncludeTG)
   << Form("  IncludeVT: %d\n", fGlobalParameter.IncludeVT)
   << Form("  IncludeIT: %d\n", fGlobalParameter.IncludeIT)
   << Form("  IncludeMSD: %d\n", fGlobalParameter.IncludeMSD)
   << Form("  IncludeTW: %d\n", fGlobalParameter.IncludeTW)
   << Form("  IncludeCA: %d", fGlobalParameter.IncludeCA)
   << endl;
}

//------------------------------------------+-----------------------------------
/*!
 \relates TAGrunInfo
 \brief Returns true of event id's \a lhs and \a rhs are equal
 */

bool operator==(const TAGrunInfo& lhs, const TAGrunInfo& rhs)
{
   return lhs.fsCam==rhs.fsCam && lhs.fiRun==rhs.fiRun;
   }
