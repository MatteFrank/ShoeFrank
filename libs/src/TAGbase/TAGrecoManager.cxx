/*!
 \file TAGrecoManager.cxx
 \brief   Implementation of TAGrecoManager.
 */

#include "cxxabi.h"

#include "Varargs.h"
#include "TNamed.h"
#include "Riostream.h"

#include "TAGparTools.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAGrecoManager
 \brief Reconstruction manager. **
 */

map<TString, TString> TAGrecoManager::fgkDectFullName = {{"ST", "Start Counter"}, {"BM", "Beam Monitor"}, {"DI", "Dipole"}, {"TG", "Target"},
                                                         {"VT", "Vertex"}, {"IT", "Inner Tracker"}, {"MSD", "MicroStrip Detector"}, {"TW", "ToF Wall"},
                                                         {"CA", "Calorimeter"}};

const TString TAGrecoManager::fgkDefParName = "FootGlobal.par";

//! Class Imp
ClassImp(TAGrecoManager);

//_____________________________________________________________________________
//! Global static pointer used to ensure a single instance of the class.
TAGrecoManager* TAGrecoManager::fgInstance = NULL;


//_____________________________________________________________________________
//! Instance
//!
//! \param[in] expName experiment name
TAGrecoManager* TAGrecoManager::Instance( const TString expName )
{
   if (!fgInstance)   // Only allow one instance of class to be generated, only true for multi-thread.
      fgInstance = new TAGrecoManager( expName );

   return fgInstance;
}

//_____________________________________________________________________________
//! Retrieve instance
TAGrecoManager* TAGrecoManager::GetPar()
{
    if (!fgInstance)
       cout << "ERROR::TAGrecoManager::GetPar()  -->  called a get before TAGrecoManager object instance." << endl, exit(0);

    return fgInstance;
}

//_____________________________________________________________________________
//! Destructor
TAGrecoManager::~TAGrecoManager()
{
  fClassDebugLevels.Delete();
  for (Int_t i = 0; i < fClassDebugLevels.GetEntriesFast(); i++) {
      if (fClassDebugLevels[i])
         fClassDebugLevels[i]->Delete();
   }
}

//_____________________________________________________________________________
//! Private constructor
//!
//! \param[in] expName experiment name
TAGrecoManager::TAGrecoManager( const TString expName )
: TObject(),
  fParFileName(""),        fDebugLevel(0),       fChi2(-1),				    fMeasureN(11),			 fSkipN(-1),
  fKalmanMode(""),         fKalReverse(false),   fVerFLUKA(false),
  fEnableLocalReco(false), fEnableTree(false),   fEnableHisto(false),    fEnableSaveHits(false), fEnableTracking(false), fEnableRootObject(false),
  fEnableTWZmc(false),     fEnableTWnoPU(false), fEnableTWZmatch(false), fEnableTWCalBar(false), fEnableTWRateSmearMC(false),
  fDoCalibTW(false),       fDoCalibBM(false),    fEnableRegionMc(false), fEnableMsdTrack(false), fEnableItrTrack(false),
  fIncludeST(false),       fIncludeBM(false),    fIncludeTG(false),      fIncludeDI(false),      fIncludeTW(false),      fIncludeMSD(false),
  fIncludeCA(false),       fIncludeIT(false),    fIncludeVT(false),
  fIncludeKalman(false),   fIncludeTOE(false),   fIncludeStraight(false)
{
    TString absName = Form("./config/%s/%s", expName.Data(), fgkDefParName.Data());
    fParFileName = absName.Data();

    fCopyInputFile.clear();
}

//_____________________________________________________________________________
//! Get Global information
const TAGrunInfo TAGrecoManager::GetGlobalInfo()
{
   TAGrunInfo runInfo;
  
   if (IsLocalReco())
      runInfo.GetGlobalPar().EnableLocalReco = true;
  
   if (IsSaveTree())
      runInfo.GetGlobalPar().EnableTree = true;
  
   if (IsSaveHisto())
      runInfo.GetGlobalPar().EnableHisto = true;
  
   if (IsSaveHits())
      runInfo.GetGlobalPar().EnableSaveHits = true;
  
   if (IsTracking())
      runInfo.GetGlobalPar().EnableTracking = true;

   if (IsReadRootObj())
      runInfo.GetGlobalPar().EnableRootObject = true;
  
   if (IsTWZmc())
      runInfo.GetGlobalPar().EnableTWZmc = true;
  
   if (IsRegionMc())
     runInfo.GetGlobalPar().EnableRegionMc = true;

   if (IsTWnoPU())
      runInfo.GetGlobalPar().EnableTWnoPU = true;
  
   if (IsTWZmatch())
      runInfo.GetGlobalPar().EnableTWZmatch = true;
  
   if (IncludeKalman())
      runInfo.GetGlobalPar().IncludeKalman = true;
  
   if (IncludeTOE())
      runInfo.GetGlobalPar().IncludeTOE = true;
  
   if (IncludeDI())
      runInfo.GetGlobalPar().IncludeDI = true;
  
   if (IncludeST())
      runInfo.GetGlobalPar().IncludeST = true;
  
   if (IncludeBM())
      runInfo.GetGlobalPar().IncludeBM = true;
  
   if (IncludeTG())
      runInfo.GetGlobalPar().IncludeTG = true;
  
   if (IncludeVT())
      runInfo.GetGlobalPar().IncludeVT = true;
  
   if (IncludeIT())
      runInfo.GetGlobalPar().IncludeIT = true;
  
   if (IncludeMSD())
      runInfo.GetGlobalPar().IncludeMSD = true;
  
   if (IncludeTW())
      runInfo.GetGlobalPar().IncludeTW = true;
  
   if (IncludeCA())
      runInfo.GetGlobalPar().IncludeCA = true;
  
   return runInfo;
}

//_____________________________________________________________________________
//! Read from file
void TAGrecoManager::FromFile()
{
  TAGparTools* parTools = new TAGparTools();
  if (!parTools->Open(fParFileName.data())) {
    Error("FromFile()", "Cannot open file %s", fParFileName.c_str());
    exit(0);
  }
  
  TString key, item;
  while (!parTools->Eof()) {
    parTools->ReadItem(key, item);
    
    fCopyInputFile.push_back(Form("%s %s", key.Data(), item.Data()));

    if (key.Contains("Debug:")) {
      fDebugLevel = item.Atoi();
      if (fDebugLevel > 0)
        printf("Debug: %d\n", fDebugLevel);
    }
    
    if (key.Contains("Chi2 cut:")) {
      fChi2 = item.Atof();
      if (fDebugLevel > 0)
        printf("Chi2 cut: %f\n", fChi2);
    }

    if (key.Contains("N measure in global tracking:")) {
      fMeasureN = item.Atoi();
      if (fDebugLevel > 0)
        printf("N measure in global tracking: %d\n", fMeasureN);
    }

    if (key.Contains("Skip n events:")) {
		fSkipN = item.Atoi();
		if (fDebugLevel > 0)
			printf("Skip n events: %d\n", fSkipN);
		if (  fSkipN <= 0 )	fSkipN = -1;
		else 					fSkipN -= 1;
	}
    
    if (key.Contains("MC Particle Types:")) {
      fMcParticles.clear();
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string tmpString = "";
      if (fDebugLevel > 0)
        printf("MC Particle Types: ");
      while ( formulasStream >> tmpString ) {
        fMcParticles.push_back(tmpString);
        if (fDebugLevel > 0)
          printf(" %s ", tmpString.data());
      }
      if (fDebugLevel > 0)
        printf("\n");
    }
    
    if (key.Contains("ClassDebugLevel:")) {
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string className = "";
      int    classLevel = -1;
      formulasStream >> className;
      formulasStream >> classLevel;
      fMapDebug[className] = classLevel;
      if (fDebugLevel > 0)
        printf("ClassDebugLevel: %s %d\n", className.c_str(), classLevel);
    }
    
    if (key.Contains("Genfit Event Display ON:")  ) {
      if ( item.Contains("y")) fEnableEventDisplay = true;
      else                     fEnableEventDisplay = false;
      if (fDebugLevel > 0)
        printf("Genfit Event Display ON: %d\n", fEnableEventDisplay);
    }
    
    if (key.Contains("IncludeKalman:")  ) {
      if ( item.Contains("y")) fIncludeKalman = true;
      else                     fIncludeKalman = false;
      if (fDebugLevel > 0)
        printf("IncludeKalman: %d\n", fIncludeKalman);
      
    }
    if (key.Contains("IncludeTOE:")) {
      if ( item.Contains("y")) fIncludeTOE = true;
      else                     fIncludeTOE = false;
      if (fDebugLevel > 0)
        printf("IncludeTOE: %d\n", fIncludeTOE);
    }
     
     if (key.Contains("IncludeStraight:")) {
        if ( item.Contains("y")) fIncludeStraight = true;
        else                     fIncludeStraight = false;
        if (fDebugLevel > 0)
           printf("IncludeStraight: %d\n", fIncludeStraight);
     }
    
    if (key.Contains("EnableLocalReco:")  ) {
      if ( item.Contains("y"))  fEnableLocalReco = true;
      else                      fEnableLocalReco = false;
      if (fDebugLevel > 0)
        printf("EnableLocalReco: %d\n", fEnableLocalReco);
    }
    
    
    if (key.Contains("Kalman Mode:")) {
      vector<TString> tmp_Modes = { "off", "on", "ref", "daf", "dafsimple" };
      istringstream sss(item.Data());
      
      TString inputMode;
      sss >> inputMode;
      
      inputMode.ToLower();
      for (unsigned int i=0; i<tmp_Modes.size(); i++) {
        
        if (inputMode.Contains(tmp_Modes[i]) ) {
          fKalmanMode = tmp_Modes[i];
          break;
        }
      }
      if (fDebugLevel > 0)
        cout<<"Kalman Mode:" << fKalmanMode<<endl;
    }
    
    if (key.Contains("Kalman preselection strategy:")) {
      vector<TString> tmp_Modes = { "TrueParticle", "Standard", "Linear", "Backtracking" };
      istringstream sss(item.Data());
      
      TString inputMode;
      sss >> inputMode;
      
      // inputMode.ToLower();
      for (unsigned int i=0; i<tmp_Modes.size(); i++) {
        
        if (inputMode.Contains(tmp_Modes[i]) ) {
          fKPreselectStrategy = tmp_Modes[i];
          break;
        }
      }
      if (fDebugLevel > 0)
        cout << "Kalman preselection strategy:" << fKPreselectStrategy << endl;
    }
    
    if (key.Contains("Tracking Systems Considered:")) {
      fTrackingSystems.clear();
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string tmpString = "";
      if (fDebugLevel > 0)
        printf("Tracking Systems Considered: ");
      while ( formulasStream >> tmpString ) {
        fTrackingSystems.push_back(tmpString);
        if (fDebugLevel > 0)
          printf(" %s ", tmpString.data());
      }
      if (fDebugLevel > 0)
        printf("\n");
    }
    
    if (key.Contains("Reverse Tracking:")) {
      if ( item == "true")
        fKalReverse = true;
      else
        fKalReverse = false;
      if (fDebugLevel > 0)
        printf("Reverse Tracking: %d\n", fKalReverse);
    }
    
    if (key.Contains("Kalman Particle Types:")) {
      fKalParticles.clear();
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string tmpString = "";
      if (fDebugLevel > 0)
        printf("Kalman Particle Types:");
      while ( formulasStream >> tmpString ) {
        fKalParticles.push_back(tmpString);
        if (fDebugLevel > 0)
          printf(" %s ", tmpString.data());
      }
      if (fDebugLevel > 0)
        printf("\n");
    }
    
     if (key.Contains("TGT Tag:")) {
        string tgtstring = item.Data();
        fTgtTag.push_back(tgtstring);

        parTools->ReadItem(key, item);
        fCopyInputFile.push_back(Form("%s %s", key.Data(), item.Data()));

        if (key.Contains("VTX Tag Cuts:")) {
           istringstream formulasStream( item.Data() );
           size_t tmp = 0;
          if (fDebugLevel > 0)
              printf("VTX Tag Cuts: ");
           while ( formulasStream >> tmp ) {
              fVtxTagCuts[tgtstring].push_back(tmp);
              if (fDebugLevel > 0)
                 printf(" %lu ", tmp);
           }
           if (fDebugLevel > 0)
              printf("\n");
        }
        
        parTools->ReadItem(key, item);
        fCopyInputFile.push_back(Form("%s %s", key.Data(), item.Data()));

        if (key.Contains("IT Tag Cuts:")) {
           istringstream formulasStream( item.Data() );
           size_t tmp = 0;
           if (fDebugLevel > 0)
              printf("IT Tag Cuts: ");
           while ( formulasStream >> tmp ) {
              fItrTagCuts[tgtstring].push_back(tmp);
              if (fDebugLevel > 0)
                 printf(" %lu ", tmp);
           }
           if (fDebugLevel > 0)
              printf("\n");
        }
        
        parTools->ReadItem(key, item);
        fCopyInputFile.push_back(Form("%s %s", key.Data(), item.Data()));

        if (key.Contains("MSD Tag Cuts:")) {
           istringstream formulasStream( item.Data() );
           size_t tmp = 0;
           if (fDebugLevel > 0)
              printf("MSD Tag Cuts: ");
           while ( formulasStream >> tmp ) {
              fMsdTagCuts[tgtstring].push_back(tmp);
              if (fDebugLevel > 0)
                 printf(" %lu ", tmp);
           }
           if (fDebugLevel > 0)
              printf("\n");
        }
        
        parTools->ReadItem(key, item);
        fCopyInputFile.push_back(Form("%s %s", key.Data(), item.Data()));

        if (key.Contains("MSD2 Tag Cuts:")) {
           istringstream formulasStream( item.Data() );
           size_t tmp = 0;
           if (fDebugLevel > 0)
              printf("MSD2 Tag Cuts: ");
           while ( formulasStream >> tmp ) {
              fMsd2TagCuts[tgtstring].push_back(tmp);
              if (fDebugLevel > 0)
                 printf(" %lu ", tmp);
           }
           if (fDebugLevel > 0)
              printf("\n");
        }
        
        parTools->ReadItem(key, item);
        fCopyInputFile.push_back(Form("%s %s", key.Data(), item.Data()));

        if (key.Contains("TOF Tag Cuts:")) {
           istringstream formulasStream( item.Data() );
           size_t tmp = 0;
           if (fDebugLevel > 0)
              printf("TOF Tag Cuts: ");
           while ( formulasStream >> tmp ) {
              fTwTagCuts[tgtstring].push_back(tmp);
              if (fDebugLevel > 0)
                 printf(" %lu ", tmp);
           }
           if (fDebugLevel > 0)
              printf("\n");
        }
     }
     
    if (key.Contains("EnableTree:") ) {
      if ( item.Contains("y"))  fEnableTree = true;
      else                      fEnableTree = false;
      if (fDebugLevel > 0)
        printf("EnableTree: %d\n", fEnableTree);
    }
    
    if (key.Contains("EnableHisto:")) {
      if ( item.Contains("y") ) fEnableHisto = true;
      else                      fEnableHisto = false;
      if (fDebugLevel > 0)
        printf("EnableHisto: %d\n", fEnableHisto);
    }
    
    if (key.Contains("EnableTracking:")) {
      if ( item.Contains("y") ) fEnableTracking = true;
      else                      fEnableTracking = false;
      if (fDebugLevel > 0)
        printf("EnableTracking: %d\n", fEnableTracking);
    }
    
    if (key.Contains("EnableSaveHits:")  ) {
      if ( item.Contains("y"))  fEnableSaveHits = true;
      else                      fEnableSaveHits = false;
      if (fDebugLevel > 0)
        printf("EnableSaveHits: %d\n", fEnableSaveHits);
    }
    
    if (key.Contains("EnableRootObject:") ) {
      if ( item.Contains("y"))  fEnableRootObject = true;
      else                      fEnableRootObject = false;
      if (fDebugLevel > 0)
        printf("EnableRootObject: %d\n", fEnableRootObject);
    }
    
    if (key.Contains("EnableTWZmc:") ) {
      if ( item.Contains("y"))  fEnableTWZmc = true;
      else                      fEnableTWZmc = false;
      if (fDebugLevel > 0)
        printf("EnableTWZmc: %d\n", fEnableTWZmc);
    }
    
    if (key.Contains("EnableTWnoPU:") ) {
      if ( item.Contains("y"))  fEnableTWnoPU = true;
      else                      fEnableTWnoPU = false;
      if (fDebugLevel > 0)
        printf("EnableTWnoPU: %d\n", fEnableTWnoPU);
    }
    

    if (key.Contains("EnableTWZmatch:") ) {
      if ( item.Contains("y"))  fEnableTWZmatch = true;
      else                      fEnableTWZmatch = false;
      if (fDebugLevel > 0)
        printf("EnableTWZmatch: %d\n", fEnableTWZmatch);
    }
    
    if (key.Contains("EnableTWCalBar:")  ) {
      if ( item.Contains("y"))  fEnableTWCalBar = true;
      else                      fEnableTWCalBar = false;
      if (fDebugLevel > 0)
        printf("EnableTWCalBar: %d\n", fEnableTWCalBar);
    }

    if (key.Contains("EnableTWRateSmearMC:")  ) {
      if ( item.Contains("y"))  fEnableTWRateSmearMC = true;
      else                      fEnableTWRateSmearMC = false;
      if (fDebugLevel > 0)
        printf("EnableTWRateSmearMC: %d\n", fEnableTWRateSmearMC);
    }
    
    if (key.Contains("EnableRegionMc:")  ) {
      if ( item.Contains("y"))  fEnableRegionMc = true;
      else                      fEnableRegionMc = false;
      if (fDebugLevel > 0)
        printf("EnableRegionMc: %d\n", fEnableRegionMc);
    }
    
    if (key.Contains("EnableMsdTrack:")  ) {
       if ( item.Contains("y"))  fEnableMsdTrack = true;
       else                      fEnableMsdTrack = false;
       if (fDebugLevel > 0)
          printf("EnableMsdTrack: %d\n", fEnableMsdTrack);
    }
     
    if (key.Contains("EnableItrTrack:")  ) {
       if ( item.Contains("y"))  fEnableItrTrack = true;
       else                      fEnableItrTrack = false;
       if (fDebugLevel > 0)
          printf("EnableItrTrack: %d\n", fEnableItrTrack);
    }
     
    if (key.Contains("IncludeDI:") ) {
      if ( item.Contains("y"))  fIncludeDI = true;
      else                      fIncludeDI = false;
      if (fDebugLevel > 0)
        printf("IncludeDI: %d\n", fIncludeDI);
      
      if (fIncludeDI)
        fDectInclude.push_back("DI");
    }
    
    if (key.Contains("IncludeST:")) {
      if ( item.Contains("y"))  fIncludeST = true;
      else                      fIncludeST = false;
      if (fDebugLevel > 0)
        printf("IncludeST: %d\n", fIncludeST);
      
      if (fIncludeST)
        fDectInclude.push_back("ST");
    }
    
    if (key.Contains("IncludeBM:") ) {
      if ( item.Contains("y") )  fIncludeBM = true;
      else                      fIncludeBM = false;
      if (fDebugLevel > 0)
        printf("IncludeBM: %d\n", fIncludeBM);
      
      if (fIncludeBM)
        fDectInclude.push_back("BM");
    }
    
    if (key.Contains("IncludeTG:")  ) {
      if ( item.Contains("y"))  fIncludeTG = true;
      else                      fIncludeTG = false;
      if (fDebugLevel > 0)
        printf("IncludeTG: %d\n", fIncludeTG);
      
      if (fIncludeTG)
        fDectInclude.push_back("TG");
    }
    
    if (key.Contains("IncludeVT:")  ) {
      if ( item.Contains("y"))  fIncludeVT = true;
      else                      fIncludeVT = false;
      if (fDebugLevel > 0)
        printf("IncludeVT: %d\n", fIncludeVT);
      
      if (fIncludeVT)
        fDectInclude.push_back("VT");
    }
    
    if (key.Contains("IncludeIT:") ) {
      if ( item.Contains("y"))  fIncludeIT = true;
      else                      fIncludeIT = false;
      if (fDebugLevel > 0)
        printf("IncludeIT: %d\n", fIncludeIT);
      
      if (fIncludeIT)
        fDectInclude.push_back("IT");
    }
    
    if (key.Contains("IncludeMSD:") ) {
      if ( item.Contains("y"))  fIncludeMSD = true;
      else                      fIncludeMSD = false;
      if (fDebugLevel > 0)
        printf("IncludeMSD: %d\n", fIncludeMSD);
      
      if (fIncludeMSD)
        fDectInclude.push_back("MSD");
    }
    
    if (key.Contains("IncludeTW:")  ) {
      if ( item.Contains("y"))  fIncludeTW = true;
      else                      fIncludeTW = false;
      if (fDebugLevel > 0)
        printf("IncludeTW: %d\n", fIncludeTW);
      
      if (fIncludeTW)
        fDectInclude.push_back("TW");
    }
    
    if (key.Contains("IncludeCA:") ) {
      if ( item.Contains("y"))  fIncludeCA = true;
      else                      fIncludeCA = false;
      if (fDebugLevel > 0)
        printf("IncludeCA: %d\n", fIncludeCA);
      
      if (fIncludeCA)
        fDectInclude.push_back("CA");
    }
    
    if (key.Contains("FLUKA version:")) {
      if ( item == "pro")
        fVerFLUKA = true;
      else if ( item == "dev" )
        fVerFLUKA = false;
      else
        fVerFLUKA = false;
      if (fDebugLevel > 0)
        printf("FLUKA version: %d\n", fVerFLUKA);
    }
  }
  
  parTools->Close();
  delete parTools;

  SetDebugLevels();
}

//_____________________________________________________________________________
//! Set debug level
void TAGrecoManager::SetDebugLevels()
{
   for ( map< string, int >::iterator it = fMapDebug.begin(); it != fMapDebug.end(); ++it) {
      string name = it->first;
      int level   = fMapDebug[it->first];
      SetClassDebugLevel(name.c_str(), level);
   }
}

//_____________________________________________________________________________
//! Set debug level for a given class
//!
//! \param[in] className class name
//! \param[in] level debug level
void TAGrecoManager::SetClassDebugLevel(const char* className, Int_t level)
{
   // set the debug level for the given class
   
   if (!className) return;

   TObject* obj = Instance()->fClassDebugLevels.FindObject(className);
   if (!obj) {
      obj = new TNamed(className, className);
      Instance()->fClassDebugLevels.Add(obj);
   }

   obj->SetUniqueID(level);
}

//_____________________________________________________________________________
//! Clear debug level for a given class
//!
//! \param[in] className class name
void TAGrecoManager::ClearClassDebugLevel(const char* className)
{
   // remove the setting of the debug level for the given class
   
   if (!className) return;
     TObject* obj = Instance()->fClassDebugLevels.FindObject(className);
   if (obj) delete Instance()->fClassDebugLevels.Remove(obj);
}

//_____________________________________________________________________________
//! Set debug level for a given MC class
//!
//! \param[in] level debug level
//! \param[in] className class name
Bool_t TAGrecoManager::GetMcDebugLevel(Int_t level, const char* className)
{
   // get the logging level for the given MC class
   // need to remove compiler index
   
   Int_t status;
   std::size_t sz = 255;
   char output_buffer[255];

   const char* name = abi::__cxa_demangle(className,  output_buffer, &sz, &status);

   return GetDebugLevel(level, name);
}

//_____________________________________________________________________________
//! Check debug level for a given level and class name
//!
//! \param[in] level debug level
//! \param[in] className class name
Bool_t TAGrecoManager::GetDebugLevel(Int_t level, const char* className)
{
   // get the logging level for the given module and class
   
   if (className) {
      Int_t classLevel = -1;
      TObject* obj = Instance()->fClassDebugLevels.FindObject(className);
      if (obj) classLevel = obj->GetUniqueID();

      if ( level <= classLevel)
         return true;
   }
   
   // check global debug level
   if (level <= Instance()->Debug())
      return true;
   
   return false;
}

//_____________________________________________________________________________
//! Get debug level for a given class name
//!
//! \param[in] className class name
Int_t TAGrecoManager::GetDebugLevel(const char* className)
{
   // get the logging level for the given module and class
   
   if (className) {
      TObject* obj = Instance()->fClassDebugLevels.FindObject(className);
      if (obj) return obj->GetUniqueID();
   }
   
   // return global debug level
   return Instance()->Debug();
}

//_____________________________________________________________________________
//! Debug line message
//!
//! \param[in] level debug level
//! \param[in] className class name
//! \param[in] funcName method name
//! \param[in] format output format
//! \param[in] file file name
//! \param[in] line line number
void TAGrecoManager::DebugLine(Int_t level, const char* className, const char* funcName, const char* format, const char* file, Int_t line)
{
   // print the message
   if (level <= Instance()->GetDebugLevel(className)) {
      if (funcName)
         fprintf(stdout, "Debug in <%s::%s>: ", className, funcName);
   
      fprintf(stdout, "%s\n", format);

      fprintf(stdout, " in file %s at line %d\n", file, line);
   }
}

//_____________________________________________________________________________
//! Debug message
//!
//! \param[in] level debug level
//! \param[in] className class name
//! \param[in] funcName method name
//! \param[in] format output format
void TAGrecoManager::Debug(Int_t level, const char* className, const char* funcName, const char* format,...)
{
  // print the message
  if (level <= Instance()->GetDebugLevel(className)) {
    if (funcName)
      fprintf(stdout, "Debug in <%s::%s>: ", className, funcName);
    
    if (format==NULL) return;
    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    fprintf(stdout, "\n");
    va_end(ap);
  }
}

//_____________________________________________________________________________
//! Debug MC message
//!
//! \param[in] className class name
//! \param[in] funcName method name
//! \param[in] format output format
void TAGrecoManager::GetMcInfoMsg(const char* className, const char* funcName, const char* format)
{
  Int_t status;
  
  std::size_t sz = 255;
  char output_buffer[255];
  
  const char* name = abi::__cxa_demangle(className,  output_buffer, &sz, &status);
  
  if (funcName)
    fprintf(stdout, "Info in <%s::%s>: ", name, funcName);
  cout << format << endl;
  
}

//_____________________________________________________________________________
//! Debug MC message
//!
//! \param[in] className class name
//! \param[in] funcName method name
//! \param[in] format output format
void TAGrecoManager::GetMcInfo(const char* className, const char* funcName, const char* format,...)
{
  Int_t status;
  std::size_t sz = 255;
  char output_buffer[255];

  const char* name = abi::__cxa_demangle(className,  output_buffer, &sz, &status);

  if (funcName)
    fprintf(stdout, "Info in <%s::%s>: ", name, funcName);

    if (format==NULL) return;
    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    fprintf(stdout, "\n");
    va_end(ap);
}

//________________________________________________________________________________________
//! Print
//!
//! \param[in] opt print out option
void TAGrecoManager::Print(Option_t* opt) const
{
   TString option(opt);
   
   cout << endl << "========================   Input Parameters  =============================" << endl<<endl;

   if (option.Contains("all")) {

      for ( unsigned int cl=0; cl<fCopyInputFile.size(); cl++ )
         cout << fCopyInputFile[cl] << endl;
      cout << endl <<  "===========================================================================" << endl<<endl;

   } else {
      cout << "Global debug level: " << fDebugLevel << endl;
      cout << "Detectors included:" << endl;
      
      printf(" - ");
      vector<TString> list = fDectInclude;
      for (vector<TString>::const_iterator it = list.begin(); it != list.end(); ++it) {
         TString str = fgkDectFullName[*it];
         printf("%s - ", str.Data());
      }
      
      printf("\n\n");

      if (fIncludeKalman)
         Info("Print()", "Using GenFit for Global Recontruction");
      
      if (fIncludeTOE)
         Info("Print()", "Using TOE for Global Recontruction");
      
      if (fIncludeStraight)
         Info("Print()", "Using straight line extrapolation for Global Recontruction");

      printf("\n");

   }
}

//____________________________________________________________________________
//! Check MC particle
//!
//! \param[in] villain MC particle
bool TAGrecoManager::Find_MCParticle( string villain )
{
   return ( find( fMcParticles.begin(), fMcParticles.end(), villain ) == fMcParticles.end() ? false : true);
}
