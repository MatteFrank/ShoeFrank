/*!
 \file TAGnameManager.cxx
 \brief Implemented of TAGnameManager
 */

#include "TAGnameManager.hxx"
#include <fstream>
#include <unistd.h>
#include "TObjArray.h"
#include "TSystem.h"

#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

/*
\class TAGnameManager
\brief Class that manage the campaigns. **
*/

//! Class Imp
ClassImp(TAGnameManager);

vector<TString>  TAGnameManager::fgkParaDscMap = {"Map", "Geo", "Conf", "Cal", "Tim"};

vector<TString>  TAGnameManager::fgkDataDscMap = {"Raw", "Hit", "Clus", "Track", "Vertex", "Point", "Trigger", "Event", "Reader", "Part", "Writer"};

vector<TString>  TAGnameManager::fgkDataDscMapMC = {"st", "bm", "vt", "it", "ms", "tw", "ca", "wd", "evt", "reg", "eve"};

map<TString, TString>  TAGnameManager::fgkDetectorMap = {{"TAST", "st"}, {"TABM", "bm"}, {"TAVT", "vt"}, {"TAIT", "it"},
                                                         {"TAMSD", "ms"}, {"TATW", "tw"}, {"TACA", "ca"}, {"TAWD", "wd"},
                                                         {"TAGdaq", "daq"}, {"TAGntuEvent", "evt"}, {"actNtuEvent", "evt"},
                                                         {"TAGactDaq", "daq"}, {"actNtuPart", "eve"}, {"TAGactTree", "evt"}};

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
//!  \param[in] className name of paraDsc class
const TString TAGnameManager::GetDataDscName(TString className)
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
   
   TString name = prefix+dataType;
   
   return name;
}

//_____________________________________________________________________________
//! Get data
//!
//!  \param[in] idx index in the vector
const TString TAGnameManager::GetDataDscMcName(Int_t idx)
{
   TString prefix = fgkDataDscMapMC[idx];
   
   TString name = prefix+"Mc";
   
   return name;
}

//_____________________________________________________________________________
//! Get data
//!
//!  \param[in] className name of paraDsc class
//!  \param[in] flagMc flag for MC actions 
const TString TAGnameManager::GetActionName(TString className, Bool_t flagMc)
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
   
   TString name = prefix+TString("Act")+dataType;
   
   if (flagMc)
      name += "Mc";
   
   return name;
}
