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
   
   if (runNumber == -1) {
      Error("TAGrunManager()", "No run number set, please set the run number");
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

      
   // TYpe name
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
   typeParameter.BeamEnergy = energy.Atof();

   if(FootDebugLevel(1))
      printf("Energy: %.0f\n", energy.Atof());
   
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

   // Total number of events
   i++;
   p = 0;
   while (line[i] != '\"') {
      buf[p++] = line[i];
      i++;
   }
   buf[p-1] = '\0';
   
   TString evtTot = buf;
   typeParameter.TotalEvts = evtTot.Atoi();

   if(FootDebugLevel(1))
      printf("Total Events: %d\n", evtTot.Atoi());

   // Comments
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

//------------------------------------------+-----------------------------------
//! Print
//!
//! \param[in] opt option for printout
void TAGrunManager::Print(Option_t* opt) const
{
   TString option(opt);
   
   if (option.Contains("all")) {
      cout << "Number of types: " << fTypeParameter.size() << endl;

      for (Int_t i = 1; i < (int)fTypeParameter.size()+1; ++i) {
         cout  << "  Type index:    " << fTypeParameter.at(i).TypeId << endl;
         cout  << "  Type name:     " << fTypeParameter.at(i).TypeName.Data() << endl;
         cout  << "  Main Trigger:  " << fTypeParameter.at(i).Trigger.Data() << endl;
         cout  << "  Type Beam:     " << fTypeParameter.at(i).Beam.Data() << endl;
         cout  << "  Beam Energy:   " << fTypeParameter.at(i).BeamEnergy << endl;
         cout  << "  Type Target:   " << fTypeParameter.at(i).Target.Data() << endl;
         cout  << "  Total Events:  " << fTypeParameter.at(i).TotalEvts << endl;
         cout  << "  Comments:      " << fTypeParameter.at(i).Comments.Data() << endl;

         cout  << endl;
      }
      cout << "  Current campaign number: " << fRunNumber << endl;
      cout  << endl;

   } else {
      Int_t i = fRunNumber;
      cout  << "Current run number:     " << fRunNumber << endl;
      cout  << "  Daq events:           " << fRunParameter.at(i).DaqEvts << endl;
      cout  << "  Duration:             " << fRunParameter.at(i).Duration << " s" << endl;
      cout  << "  Daq Rate:             " << fRunParameter.at(i).DaqRate << " Hz" << endl;

      Int_t type = fRunParameter.at(i).RunType;
      cout  << "  Run Beam:             " << fTypeParameter.at(type).Beam.Data() << endl;
      cout  << "  Run Beam Energy:      " << fTypeParameter.at(type).BeamEnergy << " MeV/u" << endl;
      cout  << "  Run Target:           " << fTypeParameter.at(type).Target.Data() << endl;
      cout  << endl;
   }
}
