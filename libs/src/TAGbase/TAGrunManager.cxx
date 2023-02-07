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

   RunParameter_t runPar = GetRunPar(fRunNumber);
   Int_t type            = runPar.RunType;
   fCurType              = fTypeParameter.at(type);
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Decode type line
//!
//! \param[in] line line to decode
void TAGrunManager::DecodeTypeLine(TString& line)
{
   TypeParameter_t typeParameter;
   
   // Index
   Int_t pos = line.First("\"");
   TString tmp(line(0, pos));
   Int_t idx = tmp.Atoi();
   typeParameter.TypeId = idx;

   if(FootDebugLevel(1))
      printf("Index: %d\n", idx);

      
   // Type name
   Int_t i = pos+1;
   Int_t p = 0;
   char buf[255];
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p] = '\0';
   
   TString type = buf;
   typeParameter.TypeName = type;

   if(FootDebugLevel(1))
      printf("Type: %s\n", type.Data());

   // Main Trigger
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
   
   TString trigger = buf;
   typeParameter.Trigger = trigger;

   if(FootDebugLevel(1))
      printf("Trigger: %s\n", trigger.Data());

   // Beam
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
   
   TString beam = buf;
   typeParameter.Beam = beam;

   if(FootDebugLevel(1))
      printf("Beam %s\n", beam.Data());
   
   //Beam energy
   i++;
   p = 0;
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p] = '\0';
   
   TString energy = buf;
   if (!energy.Contains("-")) {
      typeParameter.BeamEnergy = energy.Atof();
      typeParameter.BeamEnergy2 = 0.;
      if(FootDebugLevel(1))
         printf("Energy: %.0f\n", energy.Atof());
   } else {
      pos = energy.First("-");
      TString energy1(energy(0, pos));
      typeParameter.BeamEnergy = energy1.Atof();
      TString energy2(energy(pos+1, energy.Length()-pos));
      typeParameter.BeamEnergy2 = energy2.Atof();
      if(FootDebugLevel(1))
         printf("Energy: %.0f %.0f\n",  energy1.Atof(), energy2.Atof());
   }

   // Target
   i++;
   p = 0;
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p] = '\0';
   
   TString target = buf;
   typeParameter.Target = target;

   if(FootDebugLevel(1))
      printf("Target: %s\n", target.Data());

   // Target size & total number of events
   i++;
   p = 0;
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p-1] = '\0';
   
   Float_t size;
   Int_t evtTot;
   sscanf(buf, "%f %d", &size, &evtTot);
   
   typeParameter.TargetSize = size;
   typeParameter.TotalEvts = evtTot;

   if(FootDebugLevel(1)) {
      printf("Target size: %.1f\n", size);
      printf("Total Events: %d\n", evtTot);
   }
   
   // detector out
   i++;
   p = 0;
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p] = '\0';
   
   TString detectorOut = buf;
   typeParameter.DetectorOut =  TAGparTools::Tokenize(detectorOut.Data(), " " );

   if(FootDebugLevel(1))
      printf("DetectorOut: %s\n", detectorOut.Data());
      
   // Comments
   
   i++;
   p = 0;
   while (line[i] != '\"') {
      i++;
   }
   
   i++;
   p = 0;
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p] = '\0';
   
   TString comment = buf;
   typeParameter.Comments = comment;

   if(FootDebugLevel(1))
      printf("Comments: %s\n", comment.Data());

   fTypeParameter[idx] = typeParameter;
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

   runParameter.DaqEvts  = daqEvts;
   runParameter.Duration = duration;
   runParameter.DaqRate = daqRate;
   runParameter.RunType = runType;
   
   if(FootDebugLevel(1))
      printf("daqEvts: %d duration: %d daqRate: %d runType: %d\n", daqEvts, duration, daqRate, runType);
   
   fRunParameter[idx] = runParameter;
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

      for (  map<int, TAGrunManager::TypeParameter_t>::const_iterator it = fTypeParameter.begin(); it != fTypeParameter.end(); ++it) {

         cout  << "  Type index:    " << it->second.TypeId << endl;
         cout  << "  Type name:     " << it->second.TypeName.Data() << endl;
         cout  << "  Main Trigger:  " << it->second.Trigger.Data() << endl;
         cout  << "  Type Beam:     " << it->second.Beam.Data() << endl;
         cout  << "  Beam Energy:   " << it->second.BeamEnergy ;
         if (it->second.BeamEnergy2 > 0)
            cout  << " - " << it->second.BeamEnergy2 << endl;
         else
            cout << endl;

         cout  << "  Type Target:   " << it->second.Target.Data() << endl;
         cout  << "  Target Size:   " << it->second.TargetSize << endl;
         cout  << "  Total Events:  " << it->second.TotalEvts << endl;
         cout  << "  DetectorOut:  ";
         for (Int_t j = 0; j < (int) it->second.DetectorOut.size(); ++j)
            cout  << " " << it->second.DetectorOut[j].data();
         
         cout << endl;
         cout  << "  Comments:      " << it->second.Comments.Data() << endl;

         cout  << endl;
      }
      cout << "  Current campaign number: " << fRunNumber << endl;
      cout  << endl;

   } else {
      RunParameter_t runPar = GetRunPar(fRunNumber);
      Int_t duration = runPar.Duration;
      Int_t minutes  = duration / 60;
      Int_t seconds  = duration % 60;
      Int_t hours    = minutes  / 60;
      minutes        = minutes  % 60;
      
      printf("\nCurrent run number:     %d\n", fRunNumber);
      printf("  Daq events:           %d %03d\n", runPar.DaqEvts/1000, runPar.DaqEvts % 1000);
      printf("  Duration:             %d s [%02dh:%02dmin:%02ds]\n", duration, hours, minutes, seconds);
      printf("  Daq Rate:             %d Hz\n", runPar.DaqRate);

      printf("  Run Beam:             %s\n",         fCurType.Beam.Data());
      printf("  Run Beam Energy:      %.1f", fCurType.BeamEnergy);
      if (fCurType.BeamEnergy2 > 0)
         printf(" - %.1f MeV/u\n", fCurType.BeamEnergy2);
      else
         printf(" MeV/u\n");
      printf("  Run Target:           %s\n",         fCurType.Target.Data());
      printf("  Run Target Size:      %.1f cm\n\n",  fCurType.TargetSize);
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
