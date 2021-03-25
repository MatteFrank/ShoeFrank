#include "cxxabi.h"

#include "Varargs.h"
#include "TNamed.h"
#include "Riostream.h"

#include "TAGparTools.hxx"
#include "GlobalPar.hxx"

map<TString, TString> GlobalPar::m_dectFullName = {{"ST", "Start Counter"}, {"BM", "Beam Monitor"}, {"DI", "Dipole"}, {"TG", "Target"},
                                                   {"VT", "Vertex"}, {"IT", "Inner Tracker"}, {"MSD", "Multi-Strip Detector"}, {"TW", "ToF Wall"},
                                                   {"CA", "Calorimeter"}};

const TString GlobalPar::m_defParName = "FootGlobal.par";


//_____________________________________________________________________________
// Global static pointer used to ensure a single instance of the class.
GlobalPar* GlobalPar::m_pInstance = NULL;


//_____________________________________________________________________________
GlobalPar* GlobalPar::Instance( const TString expName )  {

    if (!m_pInstance)   // Only allow one instance of class to be generated, only true for multi-thread.
        m_pInstance = new GlobalPar( expName );

   return m_pInstance;
}

//_____________________________________________________________________________
GlobalPar* GlobalPar::GetPar()  {

    if (!m_pInstance) 
       cout << "ERROR::GlobalPar::GetPar()  -->  called a get before GlobalPar object istance." << endl, exit(0);

    return m_pInstance;
}

//_____________________________________________________________________________
GlobalPar::~GlobalPar()
{
  m_ClassDebugLevels.Delete();
  for (Int_t i = 0; i < m_ClassDebugLevels.GetEntriesFast(); i++) {
      if (m_ClassDebugLevels[i])
         m_ClassDebugLevels[i]->Delete();
   }
}

//_____________________________________________________________________________
// private constructor
GlobalPar::GlobalPar( const TString expName )
{
    TString absName = Form("./config/%s/%s", expName.Data(), m_defParName.Data());   
    m_parFileName = absName.Data();

    m_copyInputFile.clear();
    m_debug = 0;

    m_kalmanMode = -1;

    m_kalReverse = false;
    m_verFLUKA = false;

    m_doCalibTW = false;
    m_doCalibBM = false;
    ReadParamFile();
}

//_____________________________________________________________________________
const TAGrunInfo GlobalPar::GetGlobalInfo()
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
void GlobalPar::ReadParamFile ()
{
  TAGparTools* parTools = new TAGparTools();
  if (!parTools->Open(m_parFileName.data())) {
    Error("ReadParamFile()", "Cannot open file %s", m_parFileName.c_str());
    exit(0);
  }
  
  TString key, item;
  while (!parTools->Eof()) {
    parTools->ReadItem(key, item);
    
    m_copyInputFile.push_back(Form("%s %s", key.Data(), item.Data()));

    if (key.Contains("Debug:")) {
      m_debug = item.Atoi();
      if (m_debug > 0)
        printf("Debug: %d\n", m_debug);
    }
    
    if (key.Contains("MC Particle Types:")) {
      m_mcParticles.clear();
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string tmpString = "";
      if (m_debug > 0)
        printf("MC Particle Types: ");
      while ( formulasStream >> tmpString ) {
        m_mcParticles.push_back(tmpString);
        if (m_debug > 0)
          printf(" %s ", tmpString.data());
      }
      if (m_debug > 0)
        printf("\n");
    }
    
    if (key.Contains("ClassDebugLevel:")) {
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string className = "";
      int    classLevel = -1;
      formulasStream >> className;
      formulasStream >> classLevel;
      m_map_debug[className] = classLevel;
      if (m_debug > 0)
        printf("ClassDebugLevel: %s %d\n", className.c_str(), classLevel);
    }
    
    
    if (key.Contains("IncludeKalman:")  ) {
      if ( item.Contains("y")) m_includeKalman = true;
      else                     m_includeKalman = false;
      if (m_debug > 0)
        printf("IncludeKalman: %d\n", m_includeKalman);
      
    }
    if (key.Contains("IncludeTOE:")) {
      if ( item.Contains("y")) m_includeTOE = true;
      else                     m_includeTOE = false;
      if (m_debug > 0)
        printf("IncludeTOE: %d\n", m_includeTOE);
    }
    
    if (key.Contains("EnableLocalReco:")  ) {
      if ( item.Contains("y"))  m_enableLocalReco = true;
      else                      m_enableLocalReco = false;
      if (m_debug > 0)
        printf("EnableLocalReco: %d\n", m_enableLocalReco);
    }
    
    
    if (key.Contains("Kalman Mode:")) {
      vector<TString> tmp_Modes = { "off", "on", "ref", "daf", "dafsimple" };
      istringstream sss(item.Data());
      
      TString inputMode;
      sss >> inputMode;
      
      inputMode.ToLower();
      for (unsigned int i=0; i<tmp_Modes.size(); i++) {
        
        if (inputMode.Contains(tmp_Modes[i]) ) {
          m_kalmanMode = i;
          break;
        }
      }
      if (m_debug > 0)
        printf("Kalman Mode: %d\n", m_kalmanMode);
    }
    
    if (key.Contains("Tracking Systems Considered:")) {
      m_trackingSystems.clear();
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string tmpString = "";
      if (m_debug > 0)
        printf("Tracking Systems Considered: ");
      while ( formulasStream >> tmpString ) {
        m_trackingSystems.push_back(tmpString);
        if (m_debug > 0)
          printf(" %s ", tmpString.data());
      }
      if (m_debug > 0)
        printf("\n");
    }
    
    if (key.Contains("Reverse Tracking:")) {
      if ( item == "true")
        m_kalReverse = true;
      else
        m_kalReverse = false;
      if (m_debug > 0)
        printf("Reverse Tracking: %d\n", m_kalReverse);
    }
    
    
    if (key.Contains("VT  Reso:") ) {
      m_VTreso = item.Atof();
      if (m_debug > 0)
        printf("VT  Reso: %g\n", m_VTreso);
    }
    
    if (key.Contains("IT  Reso:") ) {
      m_ITreso = item.Atof();
      if (m_debug > 0)
        printf("IT  Reso: %g\n", m_ITreso);
    }
    
    if (key.Contains("MSD Reso:")  ) {
      m_MSDreso = item.Atof();
      if (m_debug > 0)
        printf("MSD  Reso: %g\n", m_MSDreso);
    }
    
    if (key.Contains("TW  Reso:") ) {
      m_TWreso = item.Atof();
      if (m_debug > 0)
        printf("TW  Reso: %g\n", m_TWreso);
    }
    
    if (key.Contains("Kalman Particle Types:")) {
      m_kalParticles.clear();
      string formulasString = item.Data();
      istringstream formulasStream( formulasString );
      string tmpString = "";
      if (m_debug > 0)
        printf("Kalman Particle Types:");
      while ( formulasStream >> tmpString ) {
        m_kalParticles.push_back(tmpString);
        if (m_debug > 0)
          printf(" %s ", tmpString.data());
      }
      if (m_debug > 0)
        printf("\n");
    }
    
    if (key.Contains("EnableTree:") ) {
      if ( item.Contains("y"))  m_enableTree = true;
      else                      m_enableTree = false;
      if (m_debug > 0)
        printf("EnableTree: %d\n", m_enableTree);
    }
    
    if (key.Contains("EnableHisto:")) {
      if ( item.Contains("y") ) m_enableHisto = true;
      else                      m_enableHisto = false;
      if (m_debug > 0)
        printf("EnableHisto: %d\n", m_enableHisto);
    }
    
    if (key.Contains("EnableTracking:")) {
      if ( item.Contains("y") ) m_enableTracking = true;
      else                      m_enableTracking = false;
      if (m_debug > 0)
        printf("EnableTracking: %d\n", m_enableTracking);
    }
    
    if (key.Contains("EnableSaveHits:")  ) {
      if ( item.Contains("y"))  m_enableSaveHits = true;
      else                      m_enableSaveHits = false;
      if (m_debug > 0)
        printf("EnableSaveHits: %d\n", m_enableSaveHits);
    }
    
    if (key.Contains("EnableRootObject:") ) {
      if ( item.Contains("y"))  m_enableRootObject = true;
      else                      m_enableRootObject = false;
      if (m_debug > 0)
        printf("EnableRootObject: %d\n", m_enableRootObject);
    }
    
    if (key.Contains("EnableTWZmc:") ) {
      if ( item.Contains("y"))  m_enableTWZmc = true;
      else                      m_enableTWZmc = false;
      if (m_debug > 0)
        printf("EnableTWZmc: %d\n", m_enableTWZmc);
    }
    
    if (key.Contains("EnableTWnoPU:") ) {
      if ( item.Contains("y"))  m_enableTWnoPU = true;
      else                      m_enableTWnoPU = false;
      if (m_debug > 0)
        printf("EnableTWnoPU: %d\n", m_enableTWnoPU);
    }
    

    if (key.Contains("EnableTWZmatch:") ) {
      if ( item.Contains("y"))  m_enableTWZmatch = true;
      else                      m_enableTWZmatch = false;
      if (m_debug > 0)
        printf("EnableTWZmatch: %d\n", m_enableTWZmatch);
    }
    
    if (key.Contains("EnableTWCalBar:")  ) {
      if ( item.Contains("y"))  m_enableTWCalBar = true;
      else                      m_enableTWCalBar = false;
      if (m_debug > 0)
        printf("EnableTWCalBar: %d\n", m_enableTWCalBar);
    }
    
    if (key.Contains("EnableRegionMc:")  ) {
      if ( item.Contains("y"))  m_enableRegionMc = true;
      else                      m_enableRegionMc = false;
      if (m_debug > 0)
        printf("EnableRegionMc: %d\n", m_enableRegionMc);
    }
    
    if (key.Contains("IncludeDI:") ) {
      if ( item.Contains("y"))  m_includeDI = true;
      else                      m_includeDI = false;
      if (m_debug > 0)
        printf("IncludeDI: %d\n", m_includeDI);
      
      if (m_includeDI)
        m_dectInclude.push_back("DI");
    }
    
    if (key.Contains("IncludeST:")) {
      if ( item.Contains("y"))  m_includeST = true;
      else                      m_includeST = false;
      if (m_debug > 0)
        printf("IncludeST: %d\n", m_includeST);
      
      if (m_includeST)
        m_dectInclude.push_back("ST");
    }
    
    if (key.Contains("IncludeBM:") ) {
      if ( item.Contains("y") )  m_includeBM = true;
      else                      m_includeBM = false;
      if (m_debug > 0)
        printf("IncludeBM: %d\n", m_includeBM);
      
      if (m_includeBM)
        m_dectInclude.push_back("BM");
    }
    
    if (key.Contains("IncludeTG:")  ) {
      if ( item.Contains("y"))  m_includeTG = true;
      else                      m_includeTG = false;
      if (m_debug > 0)
        printf("IncludeTG: %d\n", m_includeTG);
      
      if (m_includeTG)
        m_dectInclude.push_back("TG");
    }
    
    if (key.Contains("IncludeVT:")  ) {
      if ( item.Contains("y"))  m_includeVT = true;
      else                      m_includeVT = false;
      if (m_debug > 0)
        printf("IncludeVT: %d\n", m_includeVT);
      
      if (m_includeVT)
        m_dectInclude.push_back("VT");
    }
    
    if (key.Contains("IncludeIT:") ) {
      if ( item.Contains("y"))  m_includeIT = true;
      else                      m_includeIT = false;
      if (m_debug > 0)
        printf("IncludeIT: %d\n", m_includeIT);
      
      if (m_includeIT)
        m_dectInclude.push_back("IT");
    }
    
    if (key.Contains("IncludeMSD:") ) {
      if ( item.Contains("y"))  m_includeMSD = true;
      else                      m_includeMSD = false;
      if (m_debug > 0)
        printf("IncludeMSD: %d\n", m_includeMSD);
      
      if (m_includeMSD)
        m_dectInclude.push_back("MSD");
    }
    
    if (key.Contains("IncludeTW:")  ) {
      if ( item.Contains("y"))  m_includeTW = true;
      else                      m_includeTW = false;
      if (m_debug > 0)
        printf("IncludeTW: %d\n", m_includeTW);
      
      if (m_includeTW)
        m_dectInclude.push_back("TW");
    }
    
    if (key.Contains("IncludeCA:") ) {
      if ( item.Contains("y"))  m_includeCA = true;
      else                      m_includeCA = false;
      if (m_debug > 0)
        printf("IncludeCA: %d\n", m_includeCA);
      
      if (m_includeCA)
        m_dectInclude.push_back("CA");
    }
    
    if (key.Contains("Print OutputFile:")) {
      if ( item == "true" )
        m_printoutfile = true;
      else
        m_printoutfile = false;
      if (m_debug > 0)
        printf("Print OutputFile: %d\n", m_printoutfile);
    }
    
    if (key.Contains("Output Filename:") ) {
      m_outputfilename = item.Data();
      if (m_debug > 0)
        printf("Output Filename: %s\n", m_outputfilename.data());
    }
    
    if (key.Contains("Print OutputNtuple:") ) {
      if ( item == "true" )
        m_printoutntuple = true;
      else
        m_printoutntuple = false;
      if (m_debug > 0)
        printf("Print OutputNtuple: %d\n", m_printoutntuple);
    }
    
    if (key.Contains("Output Ntuplename:") ) {
      m_outputntuplename = item.Data();
      if (m_debug > 0)
        printf("Output Ntuplename: %s\n", m_outputntuplename.data());
    }
    
    if (key.Contains("FLUKA version:")) {
      if ( item == "pro")
        m_verFLUKA = true;
      else if ( item == "dev" )
        m_verFLUKA = false;
      else
        m_verFLUKA = false;
      if (m_debug > 0)
        printf("FLUKA version: %d\n", m_verFLUKA);
    }
  }
  
  parTools->Close();
  
  delete parTools;
}

//_____________________________________________________________________________
void GlobalPar::SetDebugLevels()
{
   for ( map< string, int >::iterator it = m_map_debug.begin(); it != m_map_debug.end(); ++it) {
      string name = it->first;
      int level   = m_map_debug[it->first];
      SetClassDebugLevel(name.c_str(), level);
   }
}

//_____________________________________________________________________________
void GlobalPar::SetClassDebugLevel(const char* className, Int_t level)
{
   // set the debug level for the given class
   
   if (!className) return;

   TObject* obj = Instance()->m_ClassDebugLevels.FindObject(className);
   if (!obj) {
      obj = new TNamed(className, className);
      Instance()->m_ClassDebugLevels.Add(obj);
   }

   obj->SetUniqueID(level);
}

//_____________________________________________________________________________
void GlobalPar::ClearClassDebugLevel(const char* className)
{
   // remove the setting of the debug level for the given class
   
   if (!className) return;
     TObject* obj = Instance()->m_ClassDebugLevels.FindObject(className);
   if (obj) delete Instance()->m_ClassDebugLevels.Remove(obj);
}

//_____________________________________________________________________________
Bool_t GlobalPar::GetMcDebugLevel(Int_t level, const char* className)
{
   // get the logging level for the given MC class
   // need to remove compiler index
   
   Int_t status;
   char output_buffer[255];

   const char* name = abi::__cxa_demangle(className,  output_buffer, 0,&status);

   return GetDebugLevel(level, name);
}

//_____________________________________________________________________________
Bool_t GlobalPar::GetDebugLevel(Int_t level, const char* className)
{
   // get the logging level for the given module and class
   
   if (className) {
      Int_t classLevel = -1;
      TObject* obj = Instance()->m_ClassDebugLevels.FindObject(className);
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
Int_t GlobalPar::GetDebugLevel(const char* className)
{
   // get the logging level for the given module and class
   
   if (className) {
      TObject* obj = Instance()->m_ClassDebugLevels.FindObject(className);
      if (obj) return obj->GetUniqueID();
   }
   
   // return global debug level
   return Instance()->Debug();
}

//_____________________________________________________________________________
void GlobalPar::Debug(Int_t level, const char* className, const char* funcName, const char* format, const char* file, Int_t line)
{
   // print the message
   if (level <= Instance()->GetDebugLevel(className)) {
      if (funcName)
         fprintf(stdout, "Debug in <%s:%s>: ", className, funcName);
   
      fprintf(stdout, "%s\n", format);

      fprintf(stdout, " in file %s at line %d\n", file, line);

//      if (format==NULL) return;
//      va_list ap;
//      va_start(ap, format);
//      vfprintf(stdout, format, ap);
//      fprintf(stdout, "\n");
//      
//      va_end(ap);
   }
}


//________________________________________________________________________________________
void GlobalPar::Print(Option_t* opt) {
   
   TString option(opt);
   
   cout << endl << "========================   Input Parameters  =============================" << endl<<endl;

   if (option.Contains("all")) {

      for ( unsigned int cl=0; cl<m_copyInputFile.size(); cl++ )
         cout << m_copyInputFile[cl] << endl;
      cout << endl <<  "===========================================================================" << endl<<endl;

   } else {
      cout << "Global debug level: " << m_debug << endl;
      cout << "Detectors included:" << endl;
      
      printf(" - ");
      vector<TString> list = m_dectInclude;
      for (vector<TString>::const_iterator it = list.begin(); it != list.end(); ++it) {
         TString str = m_dectFullName[*it];
         printf("%s - ", str.Data());
      }
      
      printf("\n");

      if (m_includeKalman)
         cout << "Using GenFit for Global Recontruction" << endl;
      
      if (m_includeTOE)
         cout << "Using TOE for Global Recontruction" << endl;

      printf("\n\n");

   }
}

//____________________________________________________________________________
bool GlobalPar::Find_MCParticle( string villain )
{
   return ( find( m_mcParticles.begin(), m_mcParticles.end(), villain ) == m_mcParticles.end() ? false : true);
}
