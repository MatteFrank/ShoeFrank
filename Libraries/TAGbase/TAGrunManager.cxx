/*!
 \file TAGrunManager.cxx
 \brief Implemented of TAGrunManager
 */

#include "TAGrunManager.hxx"
#include <fstream>
#include <unistd.h>
#include "TObjArray.h"
#include "TSystem.h"

#include "TAGgeoTrafo.hxx"
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
: TAGaction(fgkDefaultActName.Data(), "TAGrunManager - Run Manager"),
   fFileStream(new TAGparTools()),
   fpParGeo(0x0),
   fCampaignName(exp),
   fRunNumber(runNumber),
   fIsFileRead(false)
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
Bool_t TAGrunManager::ConditionChecks(Int_t runNumber, TAGparGeo* parGeo)
{
   if (runNumber != -1)
      fRunNumber = runNumber;
   
   if (parGeo != 0x0)
      fpParGeo = parGeo;
   
   if (fRunNumber == -1)
      return true;

   if (FromFile()) {
      Print();
      
      if (fpParGeo != 0x0) {
         Int_t Abeam       = fpParGeo->GetBeamPar().AtomicMass;
         TString beamName  = fpParGeo->GetBeamPar().Material;
         TString beam      = Form("%d%s", Abeam, beamName.Data());
         Int_t energyBeam  = int(fpParGeo->GetBeamPar().Energy*TAGgeoTrafo::GevToMev());
         TString target    = fpParGeo->GetTargetPar().Material;
         Float_t tgtSize   = fpParGeo->GetTargetPar().Size[2];
         
         TString beamType    = GetCurrentType().Beam;
         Int_t energyType    = (int)GetCurrentType().BeamEnergy;
         TString targetType  = GetCurrentType().Target;
         Float_t tgtSizeType = GetCurrentType().TargetSize;
         TString comType     = GetCurrentType().Comments;
         
         if (energyBeam != energyType && energyType != 0)
            Error("Checks()", "Beam energy in TAGdetector file (%d) different as given by run manager (%d)", energyBeam, energyType);
         
         beamType.ToUpper();
         if (beam != beamType && beamType != "NONE")
            Error("Checks()", "Beam name in TAGdetector file (%s) different as given by run manager (%s)", beam.Data(), beamType.Data());
         
         
         if (strncmp(target.Data(), targetType.Data(), min(targetType.Length(), target.Length()))  && targetType != "None")
            Error("Checks()", "Target name in TAGdetector file (%s) different as given by run manager (%s)", target.Data(), targetType.Data());
         
         if (tgtSize != tgtSizeType && targetType != "None")
            Error("Checks()", "Target size in TAGdetector file (%.1f) different as given by run manager (%.1f)", tgtSize, tgtSizeType);
      }
      
      // Check if a detetcor is off in a given run
      vector<TString> list = TAGrecoManager::GetPar()->DectIncluded();
      for (vector<TString>::const_iterator it = list.begin(); it != list.end(); ++it) {
         TString str = *it;
         
         if (IsDetectorOff(str)) {
            Warning("Checks()", "the detector %s is NOT referenced in this run, Disable it", str.Data());
            if (str == "ST")
               TAGrecoManager::GetPar()->IncludeST(false);
            if (str == "BM")
               TAGrecoManager::GetPar()->IncludeBM(false);
            if (str == "VT")
               TAGrecoManager::GetPar()->IncludeVT(false);
            if (str == "IT")
               TAGrecoManager::GetPar()->IncludeIT(false);
            if (str == "MSD")
               TAGrecoManager::GetPar()->IncludeMSD(false);
            if (str == "TW")
               TAGrecoManager::GetPar()->IncludeTW(false);
            if (str == "CA")
               TAGrecoManager::GetPar()->IncludeCA(false);
         }
      }
      printf("\n");
   }
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Read global experiment file
//!
//! \param[in] ifile file name of the list of experiments
Bool_t TAGrunManager::FromFile(TString ifile)
{
   // check file already read
   if (fIsFileRead)
      return true;
   
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
   
   fFileStream->ReadStrings(fFileCamName);
   if(FootDebugLevel(1))
      cout  << "  Current Campaign name:  "<< fFileCamName.Data() << endl;
   
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
   
   fIsFileRead = true;
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Decode type line
//!
//! \param[in] line line to decode
void TAGrunManager::DecodeTypeLine(TString& line)
{
   TypeParameter_t typeParameter;
   typeParameter.MagnetFlag = "no";
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
            
         case 9:
            typeParameter.MagnetFlag = list[i];
            if(FootDebugLevel(1))
               printf("Magnet ON: %s\n", typeParameter.MagnetFlag.Data());
            break;
            
         default:
            break;
      }
   }
   
   fTypeParameter[idx] = typeParameter;
}

//------------------------------------------+-----------------------------------
//! Decode run line
//!
//! \param[in] line line to decode
void TAGrunManager::DecodeRunLine(TString& line)
{
   RunParameter_t runParameter;
   
   runParameter.Comments = "None";
   
   vector<TString> list = TAGparTools::Tokenize(line);
   
   Int_t idx;
   for (Int_t i = 0; i < (int)list.size(); ++i ) {

      switch (i) {
         case 0:
            runParameter.RunId = list[i].Atoi();
            idx = runParameter.RunId;
            if(FootDebugLevel(1))
               printf("Index: %d\n", runParameter.RunId);
            break;
            
         case 1:
            runParameter.StartTime = list[i];
            if(FootDebugLevel(1))
               printf("Type: %s\n",  runParameter.StartTime.Data());
            break;
            
         case 2:
            runParameter.StopTime = list[i];
            if(FootDebugLevel(1))
               printf("Trigger: %s\n", runParameter.StopTime.Data());
            break;
            
         case 3:
            Int_t daqEvts;
            Int_t duration;
            Int_t daqRate;
            Int_t runType;
            
            sscanf(list[i].Data(), "%d %d %d %d", &daqEvts, &duration, &daqRate, &runType);
            
            fEvtCounter[runType] += daqEvts;
            runParameter.DaqEvts  = daqEvts;
            runParameter.Duration = duration;
            runParameter.DaqRate  = daqRate;
            runParameter.RunType  = runType;
            
            if(FootDebugLevel(1))
               printf("daqEvts: %d duration: %d daqRate: %d runType: %d\n", daqEvts, duration, daqRate, runType);
            
         case 4:
            runParameter.Comments = list[i];
            if(FootDebugLevel(1))
               printf("Comments: %s\n", runParameter.Comments.Data());
            break;
            
         case 5:
            runParameter.DetectorOut =  TAGparTools::Tokenize(list[i].Data(), " " );
            if(FootDebugLevel(1))
               printf("DetectorOut: %s\n", list[i].Data());
            
         default:
            break;
      }
   }// Index
         
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
      out  << "  Beam Energy:          " << Form("%5.1f", type.BeamEnergy);
      if (type.BeamEnergy2 > 0)
        cout  << " - "  <<  Form("%6.1f", type.BeamEnergy2) << endl;
      else
         out << " MeV/u" << endl;
      
      out  << "  Type Target:          " << type.Target.Data() << endl;
      out  << "  Magnet ON:            " << type.MagnetFlag.Data() << endl;
      out  << "  Target Size:          "  <<  Form("%4.2f", type.TargetSize) << " cm" << endl;
      out  << "  Total Events:         " << TAGrunManager::SmartPrint(type.TotalEvts) << endl;
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
   out << Form("  Type:                 %d\n", run.RunType);
   out << Form("  Daq events:           %s\n",TAGrunManager::SmartPrint(run.DaqEvts).Data());
   out << Form("  Duration:             %d s [%02dh:%02dm:%02ds]\n", duration, hours, minutes, seconds);
   out << Form("  Daq Rate:             %d Hz\n", run.DaqRate);
   out << Form("  Comments:             %s\n", run.Comments.Data());
   out  << "  DetectorOut:         ";
   for (Int_t j = 0; j < (int) run.DetectorOut.size(); ++j)
      out  << " " << run.DetectorOut[j].data();
   out << endl;

   return  out;
}


//_____________________________________________________________________________
//! Check if detector present
//!
//! \param[in] detName detector name
Bool_t TAGrunManager::IsDetectorOff(const TString& detName)
{
   for ( vector<string>::iterator it = fCurRun.DetectorOut.begin(); it != fCurRun.DetectorOut.end(); ++it) {
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
//! Print all runs
//!
//! \param[in] type type number
void TAGrunManager::PrintRuns() const
{

   for(auto iterator = fRunParameter.begin(); iterator != fRunParameter.end(); ++iterator ) {
      RunParameter_t run = iterator->second;
      
      TString out;
      for (auto& it: run.DetectorOut)
         out += Form("%s ", it.data());
      
      printf("%4d\t \"%s\"\t \"%s\"\t %6d\t %5d\t %4d\t %2d\t \"%s\"\t \"%s\"\n", run.RunId, run.StartTime.Data(), run.StopTime.Data(),
             run.DaqEvts, run.Duration, run.DaqRate, run.RunType, run.Comments.Data(), out.Data());
   }
}

//------------------------------------------+-----------------------------------
//! Return inof run line
//!
//! \param[in] RunId run number
TString TAGrunManager::GetRunLine(Int_t runId) const
{
   auto itr = fRunParameter.find(runId);
   if (itr == fRunParameter.end()) {
      return TString("");
   }

   RunParameter_t run = fRunParameter.at(runId);
   
   TString outDet;
   for (auto& it: run.DetectorOut)
      outDet += Form("%s ", it.data());
   
   TString out = Form("%4d\t \"%s\"\t \"%s\"\t %6d\t %5d\t %4d\t %2d\t \"%s\"\t  \"%s\"", run.RunId, run.StartTime.Data(), run.StopTime.Data(),
          run.DaqEvts, run.Duration, run.DaqRate, run.RunType, run.Comments.Data(), outDet.Data());
   
   return out;
}

//------------------------------------------+-----------------------------------
//! Compute rates and durarion when not present
//!
void TAGrunManager::ComputeRates()
{
   struct tm tm1;
   struct tm tm2;

   for(auto iterator = fRunParameter.begin(); iterator != fRunParameter.end(); ++iterator ) {
      RunParameter_t& run = iterator->second;
      const char *time_start = run.StartTime.Data();
      const char *time_stop = run.StopTime.Data();
      strptime(time_start, "%Y-%m-%d %H:%M:%S", &tm1);
      strptime(time_stop,  "%Y-%m-%d %H:%M:%S", &tm2);
      time_t t1 = mktime(&tm1);
      time_t t2 = mktime(&tm2);
      double diff = difftime(t2, t1);
      run.Duration = (int)diff;
      run.DaqRate = int(run.DaqEvts/diff);
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

//------------------------------------------+-----------------------------------
//! Print runs for a given type
//!
//! \param[in] type type number
void TAGrunManager::GetRunsPerType(Int_t type) const
{
   for(auto iterator = fRunParameter.begin(); iterator != fRunParameter.end(); ++iterator ) {
      if (iterator->second.RunType == type)
         cout << iterator->second;
   }
}
