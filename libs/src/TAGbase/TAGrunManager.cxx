/*!
 \file TAGrunManager.cxx
 \brief Implemented of TAGrunManager
 */

#include "TAGrunManager.hxx"
#include <fstream>
#include <unistd.h>
#include "TObjArray.h"
#include "TSystem.h"

#include "TAGrecoManager.hxx"

/*
\class TAGrunManager
\brief Class that manage the campaigns. **
*/

//! Class Imp
ClassImp(TAGrunManager);

const TString TAGrunManager::fgkDefaultActName = "actRunMan";
const TString TAGrunManager::fgkDefaultFolder  = "./cammaps/";
const TString TAGrunManager::fgkDefaultExt     = ".run";


//_____________________________________________________________________________
//! Constructor
//!
//! \param[in] exp experiment name
TAGrunManager::TAGrunManager(const TString exp, Int_t runNumber)
: TAGaction(fgkDefaultActName.Data(), "TAGrunManager - Campaign Manager"),
   fFileStream(new TAGparTools()),
   fCampaignName(exp),
   fRunNumber(runNumber)
{
   if (fCampaignName.IsNull()) {
      Error("TAGrunManager()", "No campaign name set, please set the campaign");
   }
   
   fEvtCounter.clear();
}

//_____________________________________________________________________________
//! Destructor
TAGrunManager::~TAGrunManager()
{
   delete fFileStream;
}

//------------------------------------------+-----------------------------------
//! Read global experiment file
//!
//! \param[in] ifile file name of the list of experiments
Bool_t TAGrunManager::FromFile(TString ifile)
{
   //Initialization of Geom Parameters
   if(FootDebugLevel(1))
      Info("FromFile()"," Init campaigns ");
   
   if (ifile == "")
      ifile = fgkDefaultFolder + fCampaignName + fgkDefaultExt;

   if(gSystem->AccessPathName(ifile.Data())) {
      Warning("FromFile()", "Runs file not provided for campaign %s\n", fCampaignName.Data());
      return false;
   }
   
   if (!fFileStream->Open(ifile))
      return false;
   
   
   if(FootDebugLevel(1))
      cout << "\nReading Run File: " << ifile << endl;
   
   fFileStream->ReadStrings(fName);
   if(FootDebugLevel(1))
      cout  << "  Current Campaign name:  "<< fName << endl;
   
   fFileStream->ReadItem(fRunArray);
   if(FootDebugLevel(1)) {
      cout  << "  Run Array size:  "<< fRunArray.GetSize() << endl;
      cout << "   Run Number: ";
      for (Int_t i = 0; i < fRunArray.GetSize(); ++i) {
         cout << fRunArray[i] << " ";
      }
      cout << endl;
   }
   
   TString line;
   // Read run types
   while(!fFileStream->Eof()) {
    
      fFileStream->ReadLine(line);
      if (line[0] == '#' && line[1] == '#') break;
      
      if (line[0] == '#') continue;
      
      DecodeTypeLine(line);
   }
   
   // Read run parameters
   while(!fFileStream->Eof()) {

      fFileStream->ReadLine(line);
      if (line[0] == '#') continue;

      DecodeRunLine(line);
   }
   
   fFileStream->Close();

   fCurRun     = GetRunPar(fRunNumber);
   Int_t type  = fCurRun.RunType;
   fCurType    = fTypeParameter.at(type);
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Decode type line
//!
//! \param[in] line line to decode
void TAGrunManager::DecodeTypeLine(TString& line)
{
   TypeParameter_t typeParameter;
   
   vector<TString> list = TAGparTools::Tokenize(line);
   
   Int_t idx;
   for (Int_t i = 0; i < (int)list.size(); ++i ) {
      switch (i) {
         case 0:
            typeParameter.TypeId = list[i].Atoi();
            idx = typeParameter.TypeId;
            if(FootDebugLevel(1))
               printf("Index: %d\n", idx);
            break;
            
         case 1:
            typeParameter.TypeName = list[i];
            if(FootDebugLevel(1))
               printf("Type: %s\n", typeParameter.TypeName.Data());
            break;
            
         case 2:
            typeParameter.Trigger = list[i];
            if(FootDebugLevel(1))
               printf("Trigger: %s\n", typeParameter.Trigger.Data());
            break;
            
         case 3:
            typeParameter.Beam = list[i];
            if(FootDebugLevel(1))
               printf("Beam %s\n", typeParameter.Beam.Data());
            break;
            
         case 4:
            if (!list[i].Contains("-")) {
               typeParameter.BeamEnergy = list[i].Atof();
               typeParameter.BeamEnergy2 = 0.;
               if(FootDebugLevel(1))
                  printf("Energy: %.0f\n", list[i].Atof());
            } else {
               Int_t pos = list[i].First("-");
               TString energy1(list[i](0, pos));
               typeParameter.BeamEnergy = energy1.Atof();
               TString energy2(list[i](pos+1, list[i].Length()-pos));
               typeParameter.BeamEnergy2 = energy2.Atof();
               if(FootDebugLevel(1))
                  printf("Energy: %.0f %.0f\n",  energy1.Atof(), energy2.Atof());
            }
            break;
            
         case 5:
            typeParameter.Target = list[i];
            if(FootDebugLevel(1))
               printf("Target: %s\n", typeParameter.Target.Data());
            break;
            
         case 6:
            Float_t size;
            Int_t evtTot;
            sscanf(list[i].Data(), "%f %d", &size, &evtTot);
            typeParameter.TargetSize = size;
            typeParameter.TotalEvts = evtTot;
            if(FootDebugLevel(1)) {
               printf("Target size: %.1f\n", size);
               printf("Total Events: %d\n", evtTot);
            }
            break;
            
         case 7:
            typeParameter.DetectorOut =  TAGparTools::Tokenize(list[i].Data(), " " );
            if(FootDebugLevel(1))
               printf("DetectorOut: %s\n", list[i].Data());
            break;
            
         case 8:
            typeParameter.Comments = list[i];
            if(FootDebugLevel(1))
               printf("Comments: %s\n", typeParameter.Comments.Data());
            break;
            
         default:
            break;
      }
      fTypeParameter[idx] = typeParameter;
   }


}

//------------------------------------------+-----------------------------------
//! Decode run line
//!
//! \param[in] line line to decode
void TAGrunManager::DecodeRunLine(TString& line)
{
   RunParameter_t runParameter;
   
   // Index
   Int_t pos = line.First("\"");
   TString tmp(line(0, pos));
   Int_t idx = tmp.Atoi();

   // printf("%d\n", idx);
   runParameter.RunId = idx;
   
   // Start of run
   Int_t i = pos+1;
   Int_t p = 0;
   char buf[255];
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p] = '\0';
   
   TString start = buf;
   runParameter.StartTime = start;

   if(FootDebugLevel(1))
      printf("Start: %s\n", start.Data());
   
   // Stop of run
   i++;
   p = 0;
   while (line[i] != '\"') {
      i++;
   }
   
   i++;
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p] = '\0';
   
   TString stop = buf;
   runParameter.StopTime = stop;

   if(FootDebugLevel(1))
      printf("Stop: %s\n", stop.Data());
   
   //daqEvts, duration, daqRate & runType
   tmp = line(i+1, line.Length()-i);
   
   Int_t daqEvts;
   Int_t duration;
   Int_t daqRate;
   Int_t runType;
   
   sscanf(tmp.Data(), "%d %d %d %d", &daqEvts, &duration, &daqRate, &runType);

   fEvtCounter[runType] += daqEvts;
   runParameter.DaqEvts  = daqEvts;
   runParameter.Duration = duration;
   runParameter.DaqRate  = daqRate;
   runParameter.RunType  = runType;
   
   if(FootDebugLevel(1))
      printf("daqEvts: %d duration: %d daqRate: %d runType: %d\n", daqEvts, duration, daqRate, runType);
   
   fRunParameter[idx] = runParameter;
}


//------------------------------------------+-----------------------------------
//! Decode type line
//!
//! \param[in] nb number to print
TString TAGrunManager::SmartPrint(Int_t nb, Int_t sep)
{
   vector<int> separated;
   
   do {
      separated.push_back(nb % sep);
      nb /= sep;
   } while(nb > 0);
   
   TString tmp;
   for (Int_t i = (int)separated.size()-1; i >=0; --i) {
      if (i == (int)separated.size()-1)
         tmp +=  Form("%d ",separated[i]);
      else
         tmp +=  Form("%03d ",separated[i]);
   }

   return tmp;
}

//_____________________________________________________________________________
//! iostream for TypeParameter_t
//!
//! \param[in] out output stream
//! \param[in] type type parameter to print
ostream& operator<< (std::ostream& out, const TAGrunManager::TypeParameter_t& type)
{
      out  << "  Type index:           " << type.TypeId << endl;
      out  << "  Type name:            " << type.TypeName.Data() << endl;
      out  << "  Main Trigger:         " << type.Trigger.Data() << endl;
      out  << "  Type Beam:            " << type.Beam.Data() << endl;
      out  << "  Beam Energy:          " << type.BeamEnergy ;
      if (type.BeamEnergy2 > 0)
        cout  << " - " << type.BeamEnergy2 << endl;
      else
         out << " MeV/u" << endl;
      
      out  << "  Type Target:          " << type.Target.Data() << endl;
      out  << "  Target Size:          " << type.TargetSize << " cm" << endl;
      out  << "  Total Events:         " << TAGrunManager::SmartPrint(type.TotalEvts) << endl;
      out  << "  DetectorOut:          ";
      for (Int_t j = 0; j < (int) type.DetectorOut.size(); ++j)
         out  << " " << type.DetectorOut[j].data();
      
      out << endl;
      out  << "  Comments:             " << type.Comments.Data() << endl;
      
      out  << endl;
   
   return  out;
}

//_____________________________________________________________________________
//! iostream for TypeParameter_t
//!
//! \param[in] out output stream
//! \param[in] run run  parameter to print
ostream& operator<< (ostream& out, const TAGrunManager::RunParameter_t& run)
{
   Int_t duration = run.Duration;
   Int_t minutes  = duration / 60;
   Int_t seconds  = duration % 60;
   Int_t hours    = minutes  / 60;
   minutes        = minutes  % 60;
   
   out << Form("\nCurrent run number:     %d\n", run.RunId);
   out << Form("  Daq events:           %s\n",TAGrunManager::SmartPrint(run.DaqEvts).Data());
   out << Form("  Duration:             %d s [%02dh:%02dm:%02ds]\n", duration, hours, minutes, seconds);
   out << Form("  Daq Rate:             %d Hz\n", run.DaqRate);
   
   return  out;
}


//_____________________________________________________________________________
//! Check if detector present
//!
//! \param[in] detName detector name
Bool_t TAGrunManager::IsDetectorOff(const TString& detName)
{
   for ( vector<string>::iterator it = fCurType.DetectorOut.begin(); it != fCurType.DetectorOut.end(); ++it) {
      string tmp = *it;
      TString name = tmp.data();
      if (name.Contains(detName.Data()))
         return true;
   }
   
   return false;
}

//------------------------------------------+-----------------------------------
//! Print
//!
//! \param[in] opt option for printout
void TAGrunManager::Print(Option_t* opt) const
{
   TString option(opt);
   
   if (option.Contains("all")) {
      cout << "Number of types: " << fTypeParameter.size() << endl;
      
      for (const auto &it : fTypeParameter)
         cout << it.second;
      
      cout << "  Current campaign number: " << fRunNumber << endl;
      cout  << endl;
      
   } else if (option.Contains("count")) {
      Int_t sum = 0;
      for (const auto &it : fEvtCounter) {
         cout << "  Type index:    " << setw(2) << it.first << "  Total Events:  " << setw(11) << SmartPrint(it.second).Data() << '\n';
         sum += it.second;
      }
      cout << "                     Total Events:  " << setw(11) <<  SmartPrint(sum).Data() << endl;
      
   }  else {
      cout << fCurRun << endl;
      
      printf("Current type:\n");
      cout << fCurType;
   }
}

//------------------------------------------+-----------------------------------
//! Get current run parameter
//!
//! \param[in] idx index in the run array
TAGrunManager::RunParameter_t& TAGrunManager::GetRunPar(Int_t idx)
{
   auto itr = fRunParameter.find(idx);
   if (itr == fRunParameter.end()) {
      Error("GetRunPar()", "Wrong run number %d taking %d instead", idx, fRunArray[0]);
      return fRunParameter.at(fRunArray[0]);
   }
   
   return fRunParameter.at(idx);
}

//------------------------------------------+-----------------------------------
//! Get current run parameter
//!
//! \param[in] idx index in the run array
const TAGrunManager::RunParameter_t& TAGrunManager::GetRunPar(Int_t idx) const
{
   auto itr = fRunParameter.find(idx);
   if (itr == fRunParameter.end()) {
      Error("GetRunPar()", "Wrong run number %d taking %d instead", idx, fRunArray[0]);
      return fRunParameter.at(fRunArray[0]);
   }
   
   return fRunParameter.at(idx);
}
