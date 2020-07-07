#include "TAGcampaignManager.hxx"
#include <fstream>

#include "TObjArray.h"

#include "GlobalPar.hxx"

//Class that manage the campaigns

ClassImp(TAGcampaignManager);

const TString TAGcampaignManager::fgkDefaultActName = "actCamMan";
const TString TAGcampaignManager::fgkDefaultFolder  = "./cammaps/";
const TString TAGcampaignManager::fgkDefaultExt     = ".cam";
const TString TAGcampaignManager::fgkStandardCamName = "STD";
const TString TAGcampaignManager::fgkDefaultCamName = "FOOT_cam.map";


//_____________________________________________________________________________
TAGcampaignManager::TAGcampaignManager(const TString exp)
 : TAGaction(fgkDefaultActName.Data(), "TAGcampaignManager - Campaign Manager"),
   fFileStream(new TAGparTools()),
   fCampaignsN(0),
   fCurCampaign(new TAGcampaign()),
   fCurCampaignName(exp),
   fCurCampaignNumber(-1)
{
   if (fCurCampaignName.IsNull())
      fCurCampaignName = fgkStandardCamName;
}

//_____________________________________________________________________________
TAGcampaignManager::~TAGcampaignManager()
{
   delete fFileStream;
   delete fCurCampaign;
}

//_____________________________________________________________________________
Bool_t TAGcampaignManager::FromFile(TString ifile)
{
  //Initialization of Geom Parameters
   if(FootDebugLevel(1))
      Info("FromFile()"," Init campaigns ");

   if (ifile == "")
      ifile = fgkDefaultFolder + fgkDefaultCamName;
   
   if (!fFileStream->Open(ifile)) {
      Error("FromFile()", "failed to open file '%s'", ifile.Data());
      return false;
   }
  
   Int_t     number;
   while(!fFileStream->Eof()) {
      TString   name;
      Int_t     flag;
      TString   date;
      TString   summary;

      fFileStream->ReadItem(number);
      fFileStream->ReadStrings(name);
      fFileStream->ReadItem(flag);
      fFileStream->ReadStrings(date);
      fFileStream->ReadStrings(summary);

      if (fFileStream->Eof()) break;
      fCampaignsN++;
      
      fCamParameter[number].Number = number;
      if(FootDebugLevel(1))
         cout  << "  Campaign number:  "<< fCamParameter[number].Number << endl;
      
      fCamParameter[number].Name = name;
      if(FootDebugLevel(1))
         cout  << "  Campaign name:  "<< fCamParameter[number].Name.Data() << endl;
      
      fCamParameter[number].McFlag = flag;
      if(FootDebugLevel(1))
         cout  << "  MC data flag:  "<< fCamParameter[number].McFlag << endl;
      
      fCamParameter[number].Date = date;
      if(FootDebugLevel(1))
         cout  << "  Campaign period:  "<< fCamParameter[number].Date.Data() << endl;
      
      fCamParameter[number].Summary = summary;
      if(FootDebugLevel(1))
         cout  << "  Campaign summary:  "<< fCamParameter[number].Summary.Data() << endl;
      
      if(FootDebugLevel(1))
         cout << endl;
      
   }
   fFileStream->Close();

   
   for (Int_t i = 0; i < fCampaignsN; ++i) {
      TString name = fCamParameter[i].Name;
      if (name.CompareTo(fCurCampaignName) == 0)
         fCurCampaignNumber = i;
   }

   if (fCurCampaignNumber == -1)
      Error("FromFile()", "No campaign with name %s found in database !", fCurCampaignName.Data());
      
   if(FootDebugLevel(1))
      cout  << "  Current Campaign number:  "<< fCurCampaignNumber << endl;

   
   TString curCampaignName =  fgkDefaultFolder + fCurCampaignName + fgkDefaultExt;

   if (!fCurCampaign->FromFile(curCampaignName))
      return false;
   
   return true;
}

//_____________________________________________________________________________
void TAGcampaignManager::Print(Option_t* opt) const
{
   TString option(opt);
   
   if (option.Contains("all")) {
      cout << "Number of campaigns: " << fCampaignsN << endl;
      
      for (Int_t i = 0; i < fCampaignsN; ++i) {
         cout  << "  Campaign number:  " << fCamParameter[i].Number << endl;
         cout  << "  Campaign name:    " << fCamParameter[i].Name.Data() << endl;
         cout  << "  MC data flag:     " << fCamParameter[i].McFlag << endl;
         cout  << "  Campaign period:  " << fCamParameter[i].Date.Data() << endl;
         cout  << "  Campaign summary: " << fCamParameter[i].Summary.Data() << endl;
         cout  << endl;
      }
      cout << "  Current campaign number: " << fCurCampaignNumber << endl;
      cout  << endl;

   } else {
      Int_t i = fCurCampaignNumber;
      cout << "Current campaign number: " << fCurCampaignNumber << endl;
      cout  << "  Campaign number:      " << fCamParameter[i].Number << endl;
      cout  << "  Campaign name:        " << fCamParameter[i].Name.Data() << endl;
      cout  << "  MC data flag:         " << fCamParameter[i].McFlag << endl;
      cout  << "  Campaign period:      " << fCamParameter[i].Date.Data() << endl;
      cout  << "  Campaign summary:     " << fCamParameter[i].Summary.Data() << endl;
      cout  << endl;
   }
   
   fCurCampaign->Print(opt);
}

//##############################################################################

ClassImp(TAGcampaign);

//_____________________________________________________________________________
TAGcampaign::TAGcampaign()
 : TAGparTools(),
   fName(""),
   fRunArray(),
   fDevicesN(-1)
{
}

//_____________________________________________________________________________
TAGcampaign::~TAGcampaign()
{
}

//_____________________________________________________________________________
bool TAGcampaign::FromFile(TString ifile)
{
   if (!Open(ifile)) {
      Error("FromCamFile()", "failed to open file '%s'", ifile.Data());
      return false;
   }
   
   if(FootDebugLevel(1))
      cout << "\nReading Campaign File: " << ifile << endl;
   
   ReadStrings(fName);
   if(FootDebugLevel(1))
      cout  << "  Current Campaign name:  "<< fName << endl;
   
   ReadItem(fRunArray);
   if(FootDebugLevel(1)) {
      cout  << "  Run Array size:  "<< fRunArray.GetSize() << endl;
      cout << "   Run Number: ";
      for (Int_t i = 0; i < fRunArray.GetSize(); ++i) {
         cout << fRunArray[i] << " ";
      }
      cout << endl;
   }
   
   ReadItem(fDevicesN);
   if(FootDebugLevel(1))
      cout  << "  Number of devices  "<< fDevicesN << endl;

   for (Int_t i = 0; i < fDevicesN; ++i) {
   
      TString detName;
      Int_t fileNumber;

      ReadStrings(detName);
      ReadItem(fileNumber);
   
      if(FootDebugLevel(1))
         cout << "Detector: " << detName << " with " <<  fileNumber << " files" <<endl;

      // fill det vector
      fDetectorVec.push_back(detName);
      
      for (Int_t f = 0; f < fileNumber; ++f) {
         TArrayI array;
         TString fileName;
         

         ReadStringsInts(fileName, array);
         // geomaps
         if (fileName.Contains("geomaps") && fileName.EndsWith(".map")) {
            fFileGeoMap[detName] = fileName;
            fRunsGeoMap[detName] = array;
            if(FootDebugLevel(1))
               cout << "Device: " << detName << " geomaps file: " << fileName << endl;
         }

         // config
         if (fileName.Contains("config") && fileName.EndsWith(".cfg") && !fileName.Contains("T0")) { // needed for BM
            fFileConfMap[detName] = fileName;
            fRunsConfMap[detName] = array;
           if(FootDebugLevel(1))
               cout << "Device: " << detName << " config file: " << fileName << endl;

         }

         // mapping
         if (fileName.Contains("config") && (fileName.EndsWith(".map") || fileName.EndsWith(".xml"))) { // needed for TW
            fFileMap[detName] = fileName;
            fRunsMap[detName] = array;
            if(FootDebugLevel(1))
               cout << "Device: " << detName << " mapping file: " << fileName << endl;

         }

         // calib
         if ((fileName.Contains("config") && (fileName.EndsWith(".cal") || fileName.EndsWith("/"))) || fileName.Contains("T0"))  { // needed for BM
            fFileCalMap[detName] = fileName;
            fRunsCalMap[detName] = array;
            if(FootDebugLevel(1))
               cout << "Device: " << detName << " calib file: " << fileName << endl;
         }
      }
   }
   
   return true;
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetGeoFile(const TString& detName, Int_t runNumber)
{
   TString nameFile = fFileGeoMap[detName];
   TArrayI arrayRun = fRunsGeoMap[detName];
   
   return GetFile(detName, runNumber, nameFile, arrayRun);
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetConfFile(const TString& detName, Int_t runNumber)
{
   TString nameFile = fFileConfMap[detName];
   TArrayI arrayRun = fRunsConfMap[detName];
   
   return GetFile(detName, runNumber, nameFile, arrayRun);
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetMapFile(const TString& detName, Int_t runNumber)
{
   TString nameFile = fFileMap[detName];
   TArrayI arrayRun = fRunsMap[detName];
   
   return GetFile(detName, runNumber, nameFile, arrayRun);
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetCalFile(const TString& detName, Int_t runNumber)
{
   TString nameFile = fFileCalMap[detName];
   TArrayI arrayRun = fRunsCalMap[detName];
   
   return GetFile(detName, runNumber, nameFile, arrayRun);
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetFile(const TString& detName, Int_t runNumber, const TString& nameFile, TArrayI array)
{
   if (nameFile.IsNull())
      return Form("");
   
   if (array.GetSize() == 0)
      return Form("%s", nameFile.Data());

   if (array[0] == -1 || runNumber == -1)
      return Form("%s", nameFile.Data());
   
   Int_t run = -1;
   Int_t n =  array.GetSize();
   for (Int_t i  = 0; i < n-1; ++i) {
      if (runNumber >= array[i] && runNumber < array[i+1]) {
         run = array[i];
         break;
      }
   }
   if (run == -1)
      run = array[n-1];
   
   Int_t pos = nameFile.Last('.');

   if (pos == 0) // for folders
      pos = nameFile.Length()-1;
   
   TString name = nameFile;
   name.Insert(pos, Form("_%d", run));
   
   return Form("%s", name.Data());
}

//_____________________________________________________________________________
Bool_t TAGcampaign::IsDetectorOn(const TString& detName)
{
   for ( vector<TString>::iterator it = fDetectorVec.begin(); it != fDetectorVec.end(); ++it) {
      TString name = *it;
      if (name.Contains(detName))
         return true;
   }
   
   return false;
}

//_____________________________________________________________________________
void TAGcampaign::Print(Option_t* opt) const
{
   TString option(opt);
   
   cout << "Geometry files for " << fName << ":" << endl;
   for ( map<TString, TString>::const_iterator it = fFileGeoMap.begin(); it != fFileGeoMap.end(); ++it)
      cout << "  Device name: " << it->first << " with file: " << it->second << endl;
   
   if (option.Contains("all")) {
      for ( map<TString, TArrayI>::const_iterator it = fRunsGeoMap.begin(); it != fRunsGeoMap.end(); ++it) {
         const TArrayI array = it->second;
         cout << "  Device name: " << it->first << "  Run number:";
         for (Int_t i = 0; i <array.GetSize(); ++i) {
            cout << array[i] << " ";
         }
         cout << endl;
      }
   }
   
   cout << "Configuration files for " << fName << ":" << endl;
   for ( map<TString, TString>::const_iterator it = fFileConfMap.begin(); it != fFileConfMap.end(); ++it)
      cout << "  Device name: " << it->first << " with file: " << it->second << endl;
   
   if (option.Contains("all")) {
      for ( map<TString, TArrayI>::const_iterator it = fRunsConfMap.begin(); it != fRunsConfMap.end(); ++it) {
         const TArrayI array = it->second;
         cout << "  Device name: " << it->first << "  Run number:";
         for (Int_t i = 0; i <array.GetSize(); ++i) {
            cout << array[i] << " ";
         }
         cout << endl;
      }
   }
   
   cout << "Mapping files for " << fName << ":" << endl;
   for ( map<TString, TString>::const_iterator it = fFileMap.begin(); it != fFileMap.end(); ++it)
      cout << "  Device name: " << it->first << " with file: " << it->second << endl;
   
   if (option.Contains("all")) {
      for ( map<TString, TArrayI>::const_iterator it = fRunsMap.begin(); it != fRunsMap.end(); ++it) {
         const TArrayI array = it->second;
         cout << "  Device name: " << it->first << "  Run number:";
         for (Int_t i = 0; i <array.GetSize(); ++i) {
            cout << array[i] << " ";
         }
         cout << endl;
      }
   }
   
   cout << "Calibration files for " << fName << ":" << endl;
   for ( map<TString, TString>::const_iterator it = fFileCalMap.begin(); it != fFileCalMap.end(); ++it)
      cout << "  Device name: " << it->first << " with file: " << it->second << endl;
   
   if (option.Contains("all")) {
      for ( map<TString, TArrayI>::const_iterator it = fRunsCalMap.begin(); it != fRunsCalMap.end(); ++it) {
         const TArrayI array = it->second;
         cout << "  Device name: " << it->first << "  Run number:";
         for (Int_t i = 0; i <array.GetSize(); ++i) {
            cout << array[i] << " ";
         }
         cout << endl;
      }
   }
}
