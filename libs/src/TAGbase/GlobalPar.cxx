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
    m_nLevelOfDebug = 4;    // from 0 to 4

    m_kalmanMode = -1;

    m_kalReverse = false;
    m_geoROOT = true;
    m_geoFLUKA = false;
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



        else if ( line.find("Create Reconstruction Geo:") != string::npos ) {
            string rev =StrReplace( line, "Create Reconstruction Geo:", "" );
            RemoveSpace( &rev );
            if ( rev == "y" )
                m_geoROOT = true;
            else
                m_geoROOT = false;
        } 


        else if ( line.find("Print FLUKA Geo input files:") != string::npos ) {
            string rev =StrReplace( line, "Print FLUKA Geo input files:", "" );
            RemoveSpace( &rev );
            if ( rev == "y" )
                m_geoFLUKA = true;
            else
                m_geoFLUKA = false;
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
        }

        else if ( line.find("IncludeST:") != string::npos ) {
           string rev =StrReplace( line, "IncludeST:", "" );
           RemoveSpace( &rev );
           if ( rev == "y" )        m_includeST = true;
           else                     m_includeST = false;
        }

        else if ( line.find("IncludeBM:") != string::npos ) {
            string rev =StrReplace( line, "IncludeBM:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeBM = true;
            else                     m_includeBM = false;
        }
        else if ( line.find("IncludeTW:") != string::npos ) {
            string rev =StrReplace( line, "IncludeTW:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeTW = true;
            else                     m_includeTW = false;
        }
        else if ( line.find("IncludeMSD:") != string::npos ) {
            string rev =StrReplace( line, "IncludeMSD:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeMSD = true;
            else                     m_includeMSD = false;
        }
        else if ( line.find("IncludeCA:") != string::npos ) {
            string rev =StrReplace( line, "IncludeCA:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeCA = true;
            else                     m_includeCA = false;
        }
        else if ( line.find("IncludeTG:") != string::npos ) {
           string rev =StrReplace( line, "IncludeTG:", "" );
           RemoveSpace( &rev );
           if ( rev == "y" )        m_includeTG = true;
           else                     m_includeTG = false;
        }

        else if ( line.find("IncludeVertex:") != string::npos ) {
            string rev =StrReplace( line, "IncludeVertex:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeVertex = true;
            else                     m_includeVertex = false;
        }
        else if ( line.find("IncludeInnerTracker:") != string::npos ) {
            string rev =StrReplace( line, "IncludeInnerTracker:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeInnerTracker = true;
            else                     m_includeInnerTracker = false;
        }
        else if ( line.find("IncludeKalman:") != string::npos ) {
            string rev =StrReplace( line, "IncludeKalman:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeKalman = true;
            else                     m_includeKalman = false;
        }
        else if ( line.find("IncludeEvent:") != string::npos ) {
            string rev =StrReplace( line, "IncludeEvent:", "" );   
            RemoveSpace( &rev );
            if ( rev == "y" )        m_includeEvent = true;
            else                     m_includeEvent = false;
        }

        else if ( line.find("Magnetic field map name:") != string::npos ) {
            m_magFieldMap = StrReplace( line, "Magnetic field map name:", "" );
            RemoveSpace( &m_magFieldMap );
        }


    }


    // Check mandatory parameters set
    if ( m_debug < 0 || m_debug > m_nLevelOfDebug )     cout<< "ERROR :: GlobalPar.cxx  -->  wrong parameters config setting: debug level = "<< m_debug <<endl, exit(0);
    if ( m_trackingSystems.size() < 1 )     cout<< "ERROR :: GlobalPar.cxx  -->  wrong parameters config setting: m_trackingSystems ize = 0"<<endl, exit(0);

 
    ifile.close();

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
bool GlobalPar::CheckAllowedHitOrigin( string origin ) {
   if ( find( m_originAllowed.begin(), m_originAllowed.end(), origin ) == m_originAllowed.end() )
      return false;
   
   return true;
}


//________________________________________________________________________________________
void GlobalPar::PrintAllowedHitOrigin() {
   cout << "m_originAllowed = ";
   for ( unsigned int i=0; i < m_originAllowed.size(); i++ ) {
      cout << m_originAllowed.at(i);
      if ( i == m_originAllowed.size()-1 )        cout << ".";
      else                                        cout << ", ";
   }
   cout << endl;
}

//________________________________________________________________________________________
void GlobalPar::Print() {
   
   cout << endl << "========================   Input Parameters  =============================" << endl<<endl;
   for ( unsigned int cl=0; cl<m_copyInputFile.size(); cl++ ) {
      cout << m_copyInputFile[cl] << endl;
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






















