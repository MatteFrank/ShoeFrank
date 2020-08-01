#include "cxxabi.h"

#include "Varargs.h"
#include "TNamed.h"
#include "TArrayC.h"
#include "Riostream.h"

#include "GlobalPar.hxx"


//_____________________________________________________________________________
// Global static pointer used to ensure a single instance of the class.
GlobalPar* GlobalPar::m_pInstance = NULL;


//_____________________________________________________________________________
GlobalPar* GlobalPar::Instance( string aparFileName )  {

    if (!m_pInstance)   // Only allow one instance of class to be generated, only true for multi-thread.
        m_pInstance = new GlobalPar( aparFileName );

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
GlobalPar::GlobalPar( string aparFileName ) {

    m_parFileName = aparFileName;

    m_copyInputFile.clear();
    m_debug = 0;

    m_kalmanMode = -1;

    m_kalReverse = false;
    m_verFLUKA = false;

    ReadParamFile();
}

//_____________________________________________________________________________
void GlobalPar::ReadParamFile () {

    ifstream ifile;

    ifile.open( ((string)"./config/"+m_parFileName).c_str() );
    if ( !ifile.is_open() )        cout<< "ERROR  -->  wrong input in GlobalPar::ReadParamFile file "<< endl, exit(0);

    string line = "";
    while( getline( ifile, line ) ) {  

        if (line == "")  continue;
        if ( line.find("#") != string::npos || line.find("//") != string::npos )
            continue;

        m_copyInputFile.push_back(line);

        // remove spaces, not mandatory
        if ( line.find("Debug:") != string::npos ) {
            m_debug = atoi ( StrReplace( line, "Debug:", "" ).c_str() );
           
        } else if ( line.find("MC Particle Types:") != string::npos ) {
            m_mcParticles.clear();
            string formulasString = StrReplace( line, "MC Particle Types:", "" );
            istringstream formulasStream( formulasString );
            string tmpString = "";
            while ( formulasStream >> tmpString )
                m_mcParticles.push_back(tmpString);
        } else if ( line.find("ClassDebugLevel:") != string::npos ) {
           string formulasString = StrReplace( line, "ClassDebugLevel:", "" );
           istringstream formulasStream( formulasString );
           string className = "";
          int    classLevel = -1;
           formulasStream >> className;
           formulasStream >> classLevel;
           m_map_debug[className] = classLevel;
       }
       
        if ( line.find("Kalman Mode:") != string::npos ) {
            vector<string> tmp_Modes = { "OFF", "ON", "ref", "daf", "dafsimple" };
            istringstream sss(  StrReplace( line, "Kalman Mode:", "" ) );
            
            string inputMode;
            sss >> inputMode;
            for (unsigned int i=0; i<tmp_Modes.size(); i++) {

                if ( IEquals( inputMode, tmp_Modes[i] ) ) {
                    m_kalmanMode = i;
                    break;
                }
            }
            if (m_kalmanMode == -1)         cout<< "ERROR  -->  Kalman Mode parameter "<< endl, exit(0);
        } 

        else if ( line.find("Tracking Systems Considered:") != string::npos ) {
            m_trackingSystems.clear();
            string formulasString = StrReplace( line, "Tracking Systems Considered:", "" );
            istringstream formulasStream( formulasString );
            string tmpString = "";
            while ( formulasStream >> tmpString )
                m_trackingSystems.push_back(tmpString);
        } 


        else if ( line.find("Reverse Tracking:") != string::npos ) {
            string rev =StrReplace( line, "Reverse Tracking:", "" );
            RemoveSpace( &rev );
            if ( rev == "true" )
                m_kalReverse = true;
            else
                m_kalReverse = false;
        } 

        else if ( line.find("FLUKA version:") != string::npos ) {
            string rev =StrReplace( line, "FLUKA version:", "" );
            RemoveSpace( &rev );
            if ( rev == "pro" )
                m_verFLUKA = true;
            else if ( rev == "dev" )
                m_verFLUKA = false;
        } 


        else if ( line.find("Kalman Particle Types:") != string::npos ) {
            m_kalParticles.clear();
            string formulasString = StrReplace( line, "Kalman Particle Types:", "" );
            istringstream formulasStream( formulasString );
            string tmpString = "";
            while ( formulasStream >> tmpString )
                m_kalParticles.push_back(tmpString);
        } 
         

        else if ( line.find("VT  Reso") != string::npos ) {
            m_VTreso = atof ( StrReplace( line, "VT  Reso:", "" ).c_str() );
        } 
        else if ( line.find("IT  Reso") != string::npos ) {
            m_ITreso = atof ( StrReplace( line, "IT  Reso:", "" ).c_str() );
        }
        else if ( line.find("MSD Reso") != string::npos ) {
            m_MSDreso = atof ( StrReplace( line, "MSD Reso:", "" ).c_str() );   
        }
        else if ( line.find("TW  Reso:") != string::npos ) {
            m_TWreso = atof ( StrReplace( line, "TW  Reso:", "" ).c_str() );   
        }
        else if ( line.find("Print OutputFile") != string::npos ) {
            string rev =StrReplace( line, "Print OutputFile:", "" );
            RemoveSpace( &rev );
            if ( rev == "true" )
                m_printoutfile = true;
             else
                m_printoutfile = false;
        }
        else if ( line.find("Output Filename:") != string::npos ) {
            m_outputfilename = StrReplace( line, "Output Filename:", "" );
            RemoveSpace( &m_outputfilename );
        }
        else if ( line.find("Print OutputNtuple") != string::npos ) {
            string rev =StrReplace( line, "Print OutputNtuple:", "" );
            RemoveSpace( &rev );
                if ( rev == "true" )
                    m_printoutntuple = true;
                else 
                    m_printoutntuple = false;
        }
        else if ( line.find("Output Ntuplename:") != string::npos ) {
            m_outputntuplename = StrReplace( line, "Output Ntuplename:", "" );
            RemoveSpace( &m_outputntuplename );
        }

        else if ( line.find("IncludeDI:") != string::npos ) {
           string rev =StrReplace( line, "IncludeDI:", "" );
           RemoveSpace( &rev );
           if ( rev == "y" )        m_includeDI = true;
           else                     m_includeDI = false;
           if (m_includeDI)
              m_dectInclude.push_back("DI");
        }

        else if ( line.find("IncludeST:") != string::npos ) {
           string rev =StrReplace( line, "IncludeST:", "" );
           RemoveSpace( &rev );
           if ( rev == "y" )        m_includeST = true;
           else                     m_includeST = false;
           if (m_includeST)
              m_dectInclude.push_back("ST");
        }

        else if ( line.find("IncludeBM:") != string::npos ) {
            string rev =StrReplace( line, "IncludeBM:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeBM = true;
            else                     m_includeBM = false;
           if (m_includeBM)
              m_dectInclude.push_back("BM");
        }
        else if ( line.find("IncludeTW:") != string::npos ) {
            string rev =StrReplace( line, "IncludeTW:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeTW = true;
            else                     m_includeTW = false;
           if (m_includeTW)
              m_dectInclude.push_back("TW");
        }
        else if ( line.find("IncludeMSD:") != string::npos ) {
            string rev =StrReplace( line, "IncludeMSD:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeMSD = true;
            else                     m_includeMSD = false;
           if (m_includeMSD)
              m_dectInclude.push_back("MSD");
        }
        else if ( line.find("IncludeCA:") != string::npos ) {
            string rev =StrReplace( line, "IncludeCA:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeCA = true;
            else                     m_includeCA = false;
           if (m_includeCA)
              m_dectInclude.push_back("CA");
        }
        else if ( line.find("IncludeTG:") != string::npos ) {
           string rev =StrReplace( line, "IncludeTG:", "" );
           RemoveSpace( &rev );
           if ( rev == "y" )        m_includeTG = true;
           else                     m_includeTG = false;
           if (m_includeTG)
              m_dectInclude.push_back("TG");
        }

        else if ( line.find("IncludeVT:") != string::npos ) {
            string rev =StrReplace( line, "IncludeVT:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeVT = true;
            else                     m_includeVT = false;
            if (m_includeVT)
               m_dectInclude.push_back("VT");
        }
        else if ( line.find("IncludeIT:") != string::npos ) {
            string rev =StrReplace( line, "IncludeIT:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeIT = true;
            else                     m_includeIT = false;
            if (m_includeIT)
               m_dectInclude.push_back("IT");
        }
        else if ( line.find("IncludeKalman:") != string::npos ) {
            string rev =StrReplace( line, "IncludeKalman:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeKalman = true;
            else                     m_includeKalman = false;
        }
        else if ( line.find("IncludeTOE:") != string::npos ) {
           string rev =StrReplace( line, "IncludeTOE:", "" );
           RemoveSpace( &rev );
           if ( rev == "y" )        m_includeTOE = true;
           else                     m_includeTOE = false;
        }
        else if ( line.find("IncludeEvent:") != string::npos ) {
            string rev =StrReplace( line, "IncludeEvent:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeEvent = true;
            else                     m_includeEvent = false;
        }

    }


    // Check mandatory parameters set
    if ( m_trackingSystems.size() < 1 )     cout<< "ERROR :: GlobalPar.cxx  -->  wrong parameters config setting: m_trackingSystems ize = 0"<<endl, exit(0);

 
    ifile.close();

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
   const char* name = abi::__cxa_demangle(className,0,0,&status);

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
   } else {
      cout << "Global debug level: " << m_debug << endl;
      cout << "Detectors included:" << endl;
      
      vector<TString> list = m_dectInclude;
      for (vector<TString>::const_iterator it = list.begin(); it != list.end(); ++it) {
         TString str = *it;
         printf(" %s\n", str.Data());
      }
      
      if (m_includeKalman)
         cout << "Using GenFit for Global Recontruction" << endl;
      
      if (m_includeTOE)
         cout << "Using TOE for Global Recontruction" << endl;

   }
   
   cout << endl <<  "===========================================================================" << endl<<endl;
   
}

//____________________________________________________________________________
void GlobalPar::RemoveSpace( string* s )
{
	  s->erase( ::remove_if(s->begin(), s->end(), ::isspace), s->end() );
}



//____________________________________________________________________________
//Replace part of "original" if it founds "erase" with "add". Otherwise return input string.
string GlobalPar::StrReplace(string original, string erase, string add) {
   
   int found = original.find(erase);
   if ( (size_t)found != string::npos ) {
      int cutLength = erase.size();
      original.replace( found, cutLength, add );
      return original;
   }
   else {
      cout<<"not found "<<erase<<" in "<<original<<endl;
      return original;
   }
}

//____________________________________________________________________________
bool GlobalPar::IEquals(const string& a, const string& b)	{
   
   if (b.size() != a.size())
      return false;
   
   for (unsigned int i = 0; i < a.size(); ++i) {
      if (tolower(a[i]) != tolower(b[i])) {
         return false;
      }
   }
   return true;
}

//____________________________________________________________________________
bool GlobalPar::frankFind( string what, string where )	{
   
   int wildcard_pos = what.find("*");
   
   if ( wildcard_pos == 0 )    {
	    	if( where.find( what.substr( wildcard_pos+1 ) ) != string::npos )
            return true;
   }
   else if( wildcard_pos == (int)what.size()-1 )    {
	    	if( where.find( what.substr( 0, wildcard_pos ) ) != string::npos )
            return true;
   }
   else if ( wildcard_pos != (int)string::npos )    {
      int pre = where.find( what.substr( 0, wildcard_pos ) );
      int post = where.find( what.substr( wildcard_pos+1 ) );
      if( pre!=(int)string::npos && post!=(int)string::npos )
         return true;
   }
   
   return false;
}
