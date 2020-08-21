#include "TAGcampaignManager.hxx"
#include <fstream>
#include <unistd.h>
#include "TObjArray.h"

#include "GlobalPar.hxx"

//Class that manage the campaigns

ClassImp(TAGcampaignManager);

const TString TAGcampaignManager::fgkDefaultActName = "actCamMan";
const TString TAGcampaignManager::fgkDefaultFolder  = "./cammaps/";
const TString TAGcampaignManager::fgkDefaultExt     = ".cam";
const TString TAGcampaignManager::fgkStandardCamName = "STD";
const TString TAGcampaignManager::fgkDefaultCamName = "FOOT.cam";


//_____________________________________________________________________________
TAGcampaignManager::TAGcampaignManager(const TString exp)
 : TAGaction(fgkDefaultActName.Data(), "TAGcampaignManager - Campaign Manager"),
   fFileStream(new TAGparTools()),
   fCampaignsN(0),
   fCurCampaign(new TAGcampaign()),
   fCurCampaignName(exp),
   fCurCampaignNumber(-1)
{
   if (fCurCampaignName.IsNull()) {
      fCurCampaignName = fgkStandardCamName;
      Warning("TAGcampaignManager()", "No campaign name set, using %s, should be used only be expert", fgkStandardCamName.Data());
   }
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

   if (fCurCampaignNumber == -1) {
      Error("FromFile()", "No campaign with name %s found in database !", fCurCampaignName.Data());
      exit(0);
   }
      
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

map<Int_t, TString> TAGcampaign::fgTWcalFileType = {{0, "TATW_Energy"},{1, "TATW_Tof"}, {2, "TATWEnergy"} };
map<Int_t, TString> TAGcampaign::fgTWmapFileType = {{0, "TATWChannel"},{1, "TATWbars"} };

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
      exit(0);
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
         if (fileName.Contains("geomaps") && fileName.EndsWith(".geo")) {
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
         if (fileName.Contains("config") && (fileName.EndsWith(".map"))) {
            
            // check order in TW mapping files
            if (fileName.Contains(fgTWmapFileType[0]) && fFileMap[detName].size() != 0 ) {
               Error("FromFile()", "File %s must appears in first position in TW mapping list in campaign file %s\n", fileName.Data(), fName.Data());
               exit(0);
            }
            
            fFileMap[detName].push_back(fileName);
            fRunsMap[detName].push_back(array);
            if(FootDebugLevel(1))
               cout << "Device: " << detName << " mapping file: " << fileName << endl;

         }

         // calib
         if ((fileName.Contains("calib") && (fileName.EndsWith(".cal") || fileName.EndsWith(".dat"))) || fileName.Contains("T0"))  { // needed for ST
            
            // check order in TW calibration files
            if (fileName.Contains(fgTWcalFileType[0]) && fFileCalMap[detName].size() != 0 ) {
               Error("FromFile()", "File %s must appears in first position in TW calibration list in campaign file %s\n", fileName.Data(), fName.Data());
               exit(0);
            }
            
            if (fileName.Contains(fgTWcalFileType[1]) && fFileCalMap[detName].size() != 1 ) {
               Error("FromFile()", "File %s must appears in first position in TW calibration list in campaign file %s\n", fileName.Data(), fName.Data());
               exit(0);
            }
            
            if (fileName.Contains(fgTWcalFileType[2]) && fFileCalMap[detName].size() != 2 ) {
               Error("FromFile()", "File %s must appears in first position in TW calibration list in campaign file %s\n", fileName.Data(), fName.Data());
               exit(0);
            }
            
            fFileCalMap[detName].push_back(fileName);
            fRunsCalMap[detName].push_back(array);
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
const Char_t* TAGcampaign::GetConfFile(const TString& detName, Int_t runNumber, TString bName, Int_t bEnergy)
{
   TString nameFile = fFileConfMap[detName];
   TArrayI arrayRun = fRunsConfMap[detName];
   
   if (!bName.IsNull() && !nameFile.IsNull()) {
      Int_t pos = nameFile.Last('.');
      nameFile.Insert(pos, Form("_%s_%d", bName.Data(), bEnergy));
   }

   return GetFile(detName, runNumber, nameFile, arrayRun);
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetMapFile(const TString& detName, Int_t runNumber, Int_t item)
{
   vector<TString> vecFile = fFileMap[detName];
   TString nameFile = vecFile[item];
   
   vector<TArrayI> vecRun = fRunsMap[detName];
   TArrayI arrayRun = vecRun[item];
   
   return GetFile(detName, runNumber, nameFile, arrayRun);
}


//_____________________________________________________________________________
const Char_t* TAGcampaign::GetCalFile(const  TString& detName, Int_t runNumber, Bool_t isTofCalib,
                                      Bool_t isTofBarCalib, Bool_t elossTuning)
{
   Int_t item = -1;
   if (!isTofCalib)
      item = 0;
   
   if (isTofCalib)
      item = 1;

   if (elossTuning)
      item = 2;
   
   return GetCalItem(detName, runNumber, item, isTofBarCalib);
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetCalItem(const TString& detName, Int_t runNumber, Int_t item, Bool_t isTofBarCalib)
{
   vector<TString> vecFile = fFileCalMap[detName];
   if (vecFile.size() == 0)
      return Form("");

   TString nameFile = vecFile[item];
   
   if (isTofBarCalib) {
      Int_t pos = nameFile.Last('.');
      nameFile.Insert(pos, "_perBar");
   }
   
   vector<TArrayI> vecRun = fRunsCalMap[detName];
   TArrayI arrayRun = vecRun[item];
   
   return GetFile(detName, runNumber, nameFile, arrayRun);
}

//_____________________________________________________________________________
const Char_t* TAGcampaign::GetFile(const TString& detName, Int_t runNumber, const TString& nameFile, TArrayI array)
{
   if (nameFile.IsNull()) {
      Warning("GetFile()", "Empty file for detector %s and run %d\n", detName.Data(), runNumber);
      return Form("");
   }
   
   if (array.GetSize() == 0) {
      if( access(nameFile.Data(), F_OK) == -1 ) {
         Warning("GetFile()", "File %s not found !", nameFile.Data());
         exit(0);
      }
      return Form("%s", nameFile.Data());
   }
   
   if (array[0] == -1 || runNumber == -1) {
      if( access(nameFile.Data(), F_OK) == -1 ) {
         Warning("GetFile()", "File %s not found !", nameFile.Data());
         exit(0);
      }
      return Form("%s", nameFile.Data());
   }

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
   
   if( access(name.Data(), F_OK) == -1 ) {
      Warning("GetFile()", "File %s not found !", name.Data());
      exit(0);
   }
   
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
   for ( map<TString, vector<TString> >::const_iterator it = fFileMap.begin(); it != fFileMap.end(); ++it) {
      
      vector<TString> vec = it->second;
      vector<TString>::const_iterator iter;
      for (iter = vec.begin(); iter != vec.end(); ++iter)
         cout << "  Device name: " << it->first << " with file: " << iter->Data() << endl;
      
      if (option.Contains("all")) {
         for ( map<TString, vector<TArrayI> >::const_iterator it = fRunsMap.begin(); it != fRunsMap.end(); ++it) {
            
            vector<TArrayI> vecRun = it->second;
            vector<TArrayI>::const_iterator iterRun;
            
            for (iterRun = vecRun.begin(); iterRun != vecRun.end(); ++iterRun) {
               
               const TArrayI array = *iterRun;
               cout << "  Device name: " << it->first << "  Run number:";
               for (Int_t i = 0; i <array.GetSize(); ++i) {
                  cout << array[i] << " ";
               }
               cout << endl;
            }
         }
      }
   }
   
   cout << "Calibration files for " << fName << ":" << endl;
   for ( map<TString, vector<TString> >::const_iterator it = fFileCalMap.begin(); it != fFileCalMap.end(); ++it) {
   
      vector<TString> vec = it->second;
      vector<TString>::const_iterator iter;
      for (iter = vec.begin(); iter != vec.end(); ++iter)
         cout << "  Device name: " << it->first << " with file: " << iter->Data() << endl;

      if (option.Contains("all")) {
         for ( map<TString, vector<TArrayI> >::const_iterator it = fRunsCalMap.begin(); it != fRunsCalMap.end(); ++it) {
         
            vector<TArrayI> vecRun = it->second;
            vector<TArrayI>::const_iterator iterRun;
            
            for (iterRun = vecRun.begin(); iterRun != vecRun.end(); ++iterRun) {
               
               const TArrayI array = *iterRun;
               cout << "  Device name: " << it->first << "  Run number:";
               for (Int_t i = 0; i <array.GetSize(); ++i) {
                  cout << array[i] << " ";
               }
               cout << endl;
            }
         }
      }
   }
}
