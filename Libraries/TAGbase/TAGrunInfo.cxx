/*!
  \file TAGrunInfo.cxx
  \brief   Implementation of TAGrunInfo.
*/

#include "TBuffer.h"

#include "TAGrunInfo.hxx"

/*!
  \class TAGrunInfo
  \brief Run number info. **
*/

//! Class Imp
ClassImp(GlobalParameter_t)
//! Class Imp
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
//!
//! \param[in] s_cam campaign name
//! \param[in] i_run run number
TAGrunInfo::TAGrunInfo(TString s_cam, Short_t i_run)
 : fsCam(s_cam),
   fiRun(i_run)
{}

//------------------------------------------+-----------------------------------
//! copy cstr
//!
//! \param[in] right run info to copy
TAGrunInfo::TAGrunInfo(const TAGrunInfo& right)
 : fsCam(right.fsCam),
   fiRun(right.fiRun),
   fCrossMap(right.fCrossMap)
{
   fGlobalParameter.FromLocalReco  = right.fGlobalParameter.FromLocalReco;
   fGlobalParameter.EnableTree       = right.fGlobalParameter.EnableTree;
   fGlobalParameter.EnableHisto      = right.fGlobalParameter.EnableHisto;
   fGlobalParameter.EnableSaveHits   = right.fGlobalParameter.EnableSaveHits;
   fGlobalParameter.EnableTracking   = right.fGlobalParameter.EnableTracking;
   fGlobalParameter.EnableRootObject = right.fGlobalParameter.EnableRootObject;
   fGlobalParameter.EnableRegionMc   = right.fGlobalParameter.EnableRegionMc;
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
//!
//! \param[in] right run info to equal
const TAGrunInfo& TAGrunInfo::operator=(const TAGrunInfo &right)
{
   fiRun = right.fiRun;
   fsCam = right.fsCam;
  
   fCrossMap.insert(right.fCrossMap.begin(), right.fCrossMap.end());
  
   fGlobalParameter.FromLocalReco  = right.fGlobalParameter.FromLocalReco;
   fGlobalParameter.EnableTree       = right.fGlobalParameter.EnableTree;
   fGlobalParameter.EnableHisto      = right.fGlobalParameter.EnableHisto;
   fGlobalParameter.EnableSaveHits   = right.fGlobalParameter.EnableSaveHits;
   fGlobalParameter.EnableTracking   = right.fGlobalParameter.EnableTracking;
   fGlobalParameter.EnableRootObject = right.fGlobalParameter.EnableRootObject;
   fGlobalParameter.EnableRegionMc   = right.fGlobalParameter.EnableRegionMc;
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
//! Clear event.
//!
//! \param[in] opt option for clearing (not used)
void TAGrunInfo::Clear(Option_t*)
{
  fsCam = "";
  fiRun = -1;
  fCrossMap.clear();
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAGrunInfo::ToStream(ostream& os, Option_t* option) const
{
  os << "Run info:     "
   << Form("  cam: %s", fsCam.Data())
   << Form("  run: %4d", fiRun)
   << endl;

   os << "Global info:     \n"
   << Form("  FromLocalReco: %d\n", fGlobalParameter.FromLocalReco)
   << Form("  EnableTree: %d\n", fGlobalParameter.EnableTree)
   << Form("  EnableHisto: %d\n", fGlobalParameter.EnableHisto)
   << Form("  EnableTracking: %d\n", fGlobalParameter.EnableTracking)
   << Form("  EnableSaveHits: %d\n", fGlobalParameter.EnableSaveHits)
   << Form("  EnableRootObject: %d\n", fGlobalParameter.EnableRootObject)
   << Form("  EnableRegionMc: %d\n\n", fGlobalParameter.EnableRegionMc)
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
   << Form("  IncludeCA: %d\n", fGlobalParameter.IncludeCA)
   << endl;

   if(fGlobalParameter.EnableRegionMc){
     os << "Crossing Map:  \n"
     <<Form("  Total number of regions: %ld\n",fCrossMap.size());
     if(strcmp(option,"v")==0 || strcmp(option,"verbose")==0)
       for (auto& x: fCrossMap)
           os<<"  "<<x.first.Data()<<"  " << x.second <<endl;
   }
}

//------------------------------------------+-----------------------------------
//! Returns true of run info lhs and rhs are equal
//!
//! \param[in] lhs left item
//! \param[in] rhs right item
bool operator==(const TAGrunInfo& lhs, const TAGrunInfo& rhs)
{
   return lhs.fsCam==rhs.fsCam && lhs.fiRun==rhs.fiRun;
   }
