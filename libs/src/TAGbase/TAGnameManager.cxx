/*!
 \file TAGnameManager.cxx
 \brief Implemented of TAGnameManager
 */

#include "TAGnameManager.hxx"
#include "TAGroot.hxx"
#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

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

vector<TString>        TAGnameManager::fgkDataDscMap   = {"Raw", "Hit", "Clus", "Track", "Vertex", "Point", "Trigger", "Event", "Reader", "reader", "Part", "Writer"};

vector<TString>        TAGnameManager::fgkDataDscMapMC = {"st", "bm", "vt", "it", "ms", "tw", "ca"};

map<TString, TString>  TAGnameManager::fgkDetectorMap  = {{"TAST", "st"}, {"TABM", "bm"}, {"TAVT", "vt"}, {"TAIT", "it"}, {"TAGpar", "tg"},
                                                          {"TAMSD", "ms"}, {"TATW", "tw"}, {"TACA", "ca"}, {"WD", "wd"},
                                                          {"TAGdaq", "daq"}, {"TAGntuEvent", "evt"}, {"actNtuEvent", "evt"}, {"ntuPart", "mc"},  {"TAMCntuEvent", "mc"},
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

/*------------------------------------------+---------------------------------*/
//! Print
//!
//! \param[in] option printout option
void TAGnameManager::PrintNames(Option_t* option)
{
   if (!gTAGroot) return;
   
   TString opt(option);
   
   if (opt=="all") {
      TList* list = gTAGroot->ListOfAction();
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         TAGaction* action = (TAGaction*)list->At(i);
         TString name(action->ClassName());
         cout << setw(20) << left << action->ClassName() << " " << FootActionDscName(name) << endl;
      }
      cout << endl;
      
      list = gTAGroot->ListOfDataDsc();
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         TAGdataDsc* dsc = (TAGdataDsc*)list->At(i);
         TAGdata* obj = dsc->Object();
         TString name(obj->ClassName());
         cout << setw(20) << left << obj->ClassName() << " " << FootActionDscName(name) << endl;;
      }
      cout << endl;
      
      list = gTAGroot->ListOfParaDsc();
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         TAGparaDsc* dsc = (TAGparaDsc*)list->At(i);
         TAGpara* obj = dsc->Object();
         TString name(obj->ClassName());
         cout << setw(20) << left << obj->ClassName() << " " << FootParaDscName(name) << endl;;
      }
   }
}
