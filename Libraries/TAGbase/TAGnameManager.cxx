/*!
 \file TAGnameManager.cxx
 \brief Implemented of TAGnameManager
 */

#include "TAGnameManager.hxx"

/*
\class TAGnameManager
\brief Class that manage the names. **
*/

//! Class Imp
ClassImp(TAGnameManager);

TString                TAGnameManager::fgkMcSuffix     = "Mc";

TString                TAGnameManager::fgkActPrefix    = "Act";

TString                TAGnameManager::fgkNtuHitMc     = "TAMCntuHit";

vector<TString>        TAGnameManager::fgkParaDscMap   = {"Map", "Geo", "Conf", "Cal", "Tim"};

vector<TString>        TAGnameManager::fgkDataDscMap   = {"Raw", "Hit", "Clus", "Track", "Vertex", "Point", "Trigger", "Event", "Reader", "reader", "Part", "Writer", "Region", "KFitter"};

vector<TString>        TAGnameManager::fgkDataDscMapMC = {"st", "bm", "vt", "it", "msd", "tw", "ca"};

map<TString, TString>  TAGnameManager::fgkDetectorMap  = {{"TAST", "st"},    {"TABM", "bm"},         {"TAVT", "vt"},         {"TAIT", "it"},           {"TADI", "di"},  {"TAGactKFitter", "glb"},
                                                          {"TAGpar", "tg"},  {"TAMSD", "ms"},        {"TATW", "tw"},         {"TACA", "ca"},           {"WD", "wd"},
                                                          {"TAGdaq", "daq"}, {"TAGntuEvent", "evt"}, {"actNtuEvent", "evt"}, {"ntuPart", "mc"},        {"tuGlb", "glb"},
                                                          {"Region", "mc"},  {"TAGactDaq", "daq"},   {"actNtuPart", "eve"},  {"TAGactDscTree", "evt"}, {"TAGactFlatTree", "conv"}};

map<TString, TString>  TAGnameManager::fgkBranchMap    = {{"Raw", "dat."},    {"Hit", "rh."},             {"Cluster", "clus."},    {"Point", "pt."},      {"Track", "track."},
                                                          {"Vertex", "vtx."}, {"TAMCntuEvent", "mcevt."}, {"TAGntuEvent", "evt."}, {"ntuPart", "track."}, {"ntuRegion", "reg."}, {"Trigger", "trig."}};

map<TString, TString>  TAGnameManager::fgkBaseNameMap  = {{"TAST", "ST"},    {"TABM", "BM"},         {"TAVT", "VT"},         {"TAIT", "IT"},           {"TADI", "DI"}, {"TAWD", "WD"},
                                                          {"TAGpar", "TG"},  {"TAMSD", "MSD"},       {"TATW", "TW"},         {"TACA", "CA"},           {"WD", "WD"},
                                                          {"TAGevent", "EVT"},    {"TAGgeoTrafo", "FOOT"}};
//_____________________________________________________________________________
//! Constructor
//!
//! \param[in] exp experiment name
TAGnameManager::TAGnameManager()
: TAGobject()
{
}

//_____________________________________________________________________________
//! Destructor
TAGnameManager::~TAGnameManager()
{
}

//_____________________________________________________________________________
//! Get para
//!
//!  \param[in] className name of paraDsc class
const TString TAGnameManager::GetParaDscName(TString className)
{
   TString prefix;
   for (auto const& it : fgkDetectorMap) {
      if (className.Contains(it.first)) {
         prefix = it.second;
         break;
      }
   }
   
   TString paraType;
   for (auto const& itv : fgkParaDscMap) {
      if (className.Contains(itv.Data())) {
         paraType = itv.Data();
         break;
      }
   }

   TString name = prefix+paraType;
   
   return name;
}

//_____________________________________________________________________________
//! Get data
//!
//!  \param[in] idx index in the vector
const TString TAGnameManager::GetDataDscMcName(Int_t idx)
{
   TString prefix = fgkDataDscMapMC[idx];
   
   TString name = prefix+fgkMcSuffix;
   
   return name;
}

//_____________________________________________________________________________
//! Get data
//!
//!  \param[in] className name of paraDsc class
//!  \param[in] flagMc flag for MC actions
const TString TAGnameManager::GetActionDscName(TString className)
{   
   TString prefix;
   for (auto const& it : fgkDetectorMap) {
      if (className.Contains(it.first)) {
         prefix = it.second;
         break;
      }
   }
   
   TString dataType;
   for (auto const& itv : fgkDataDscMap) {
      if (className.Contains(itv.Data())) {
         dataType = itv.Data();
         break;
      }
   }
   
   TString name;
   
   TString actPrefix = fgkActPrefix;
   actPrefix.ToLower();
   if (className.Contains(actPrefix)) {
      name = prefix+fgkActPrefix+dataType;
      TString suffix = fgkMcSuffix;
      suffix.ToUpper();
      if (className.Contains(suffix))
         name += fgkMcSuffix;
   } else
      name = prefix+dataType;
   
   return name;
}

//_____________________________________________________________________________
//! Get data
//!
//!  \param[in] className name of paraDsc class
//!  \param[in] flagMc flag for MC actions
const TString TAGnameManager::GetBranchName(TString className)
{
   TString prefix;
   for (auto const& it : fgkDetectorMap) {
      if (className.Contains(it.first)) {
         prefix = it.second;
         break;
      }
   }
   
   TString suffix;
   for (auto const& itv : fgkBranchMap) {
      if (className.Contains(itv.first)) {
         suffix = itv.second;
         break;
      }
   }
   
   TString  name = prefix+suffix;
   
   return name;
}

//_____________________________________________________________________________
//! Get data
//!
//!  \param[in] idx index in the vector
const TString TAGnameManager::GetBranchMcName(Int_t idx)
{
   TString suffix = fgkDataDscMapMC[idx];
   
   TString prefix = fgkMcSuffix;
   prefix.ToLower();
   
   TString name = prefix+suffix+".";
   
   return name;
}

//_____________________________________________________________________________
//! Get base name
//!
//!  \param[in] className name of paraDsc class
const TString TAGnameManager::GetBaseName(TString className)
{
   TString name;
   for (auto const& it : fgkBaseNameMap) {
      if (className.Contains(it.first)) {
         name = it.second;
         break;
      }
   }
   
   return name;
}
