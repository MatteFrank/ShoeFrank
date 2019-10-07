#ifndef GLOBALPAR_H
#define GLOBALPAR_H


#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <memory>
#include <map>

#include "TObjArray.h"

using namespace std;

// singleton class of global foot parameters
class GlobalPar {

public:

	static GlobalPar* Instance( string aparFileName = "FootGlobal.par" );
	static GlobalPar* GetPar();
	~GlobalPar();

	void ReadParamFile();
	void Print();

	int Debug() { return m_debug; };

	int KalMode() { return m_kalmanMode; };
	bool IsKalReverse() { return m_kalReverse; };
	bool geoROOT() { return m_geoROOT; };
	bool geoFLUKA() { return m_geoFLUKA; };
	bool verFLUKA() { return m_verFLUKA; };
	vector<string> KalSystems() { return m_trackingSystems; };
	vector<string> KalParticles() { return m_kalParticles; };
	vector<string> MCParticles() { return m_mcParticles; };

	double VTReso() 	{return m_VTreso; };
    double ITReso() 	{return m_ITreso; };
    double MSDReso()        {return m_MSDreso; };
    double TWReso()        {return m_TWreso; };

    bool IsPrintOutputFile() { return m_printoutfile; };
    string OutputFile() { return m_outputfilename; };
 
    bool IsPrintOutputNtuple() { return m_printoutntuple; };
    string OutputNtuple() { return m_outputntuplename; };
	
    bool IncludeDI() { return m_includeDI; };
    bool IncludeST() { return m_includeST; };
    bool IncludeBM() { return m_includeBM; };
    bool IncludeTW() { return m_includeTW; };
    bool IncludeMSD() { return m_includeMSD; };
    bool IncludeCA() { return m_includeCA; };
    bool IncludeTG() { return m_includeTG; };
    bool IncludeVertex() { return m_includeVertex; };
    bool IncludeInnerTracker() { return m_includeInnerTracker; };
    
    bool IncludeKalman() { return m_includeKalman; };
    bool IncludeEvent() { return m_includeEvent; };


  //    string MagFieldInputMapName() { return m_magFieldMap; };
    void RemoveSpace( string* s );
    bool IEquals(const string& a, const string& b);
    bool frankFind( string what, string where );
    string StrReplace(string original, string erase, string add);

    bool CheckAllowedHitOrigin( string origin );
    void PrintAllowedHitOrigin();

	bool Find_MCParticle( string villain ) 
		{ return ( find( m_mcParticles.begin(), m_mcParticles.end(), villain ) == m_mcParticles.end() ? false : true ); };
	
	double GetLowBinHisto( string villain ) 	{ 
		for ( map< string, pair< double, double > >::iterator it = m_map_range.begin(); it != m_map_range.end(); it++ ) {
			if ( frankFind( (*it).first, villain ) )
				return (*it).second.first; 
		}
		return -666;
	};
	double GetUpBinHisto( string villain ) 	{ 
		for ( map< string, pair< double, double > >::iterator it = m_map_range.begin(); it != m_map_range.end(); it++ ) {
			if ( frankFind( (*it).first, villain ) )
				return (*it).second.second; 
		}
		return -666;
	};
	int GetNBinHisto( string villain )  	{ 
		for ( map< string, int >::iterator it = m_nBin_map.begin(); it != m_nBin_map.end(); it++ ) {
			if ( frankFind( (*it).first, villain ) )
				return (*it).second; 
		}
		return -666;
	};
	string GetSaveDirHisto( string villain )  	{ 
		for ( map< string, string >::iterator it = m_map_saveDir.begin(); it != m_map_saveDir.end(); it++ ) {
			if ( frankFind( (*it).first, villain ) )
				return (*it).second; 
		}
		return "default";
	};
	string GetXTitlesHisto( string villain )  	{ 
		for ( map< string, string >::iterator it = m_map_xTitles.begin(); it != m_map_xTitles.end(); it++ ) {
			if ( frankFind( (*it).first, villain ) )
				return (*it).second; 
		}
		return "default";
	};
	string GetYTitlesHisto( string villain )  	{ 
		for ( map< string, string >::iterator it = m_map_yTitles.begin(); it != m_map_yTitles.end(); it++ ) {
			if ( frankFind( (*it).first, villain ) )
				return (*it).second; 
		}
		return "default";
	};

private:
	GlobalPar();
	GlobalPar( string aparFileName );
	static GlobalPar* m_pInstance;

	vector<string> m_copyInputFile;

	vector<string> m_originAllowed;

	map< string, pair< double, double > > m_map_range;
	map< string, int > m_nBin_map;
	map< string, string > m_map_saveDir;
	map< string, string > m_map_xTitles;
	map< string, string > m_map_yTitles;

	string m_parFileName;

	int m_debug;
	int m_nLevelOfDebug;

	vector<string> m_mcParticles;
	
	int m_kalmanMode;
	bool m_kalReverse;
	bool m_geoROOT;
	bool m_geoFLUKA;
	bool m_verFLUKA;
	vector<string> m_trackingSystems;
	vector<string> m_kalParticles;

	double m_VTreso;
    double m_ITreso;
    double m_MSDreso; 
    double m_TWreso; 

    string  m_outputfilename;
    bool m_printoutfile;
       
    string  m_outputntuplename;
    bool m_printoutntuple;

  //    string m_magFieldMap;

   bool m_includeST;
   bool m_includeBM;
   bool m_includeTG;
   bool m_includeDI;

    bool m_includeTW;
    bool m_includeMSD;
    bool m_includeCA;
    bool m_includeInnerTracker;
    bool m_includeVertex;
    
    bool m_includeEvent;
    bool m_includeKalman;
   
   TObjArray  m_ClassDebugLevels;          // debug levels for classes

   
public:
   static void   Debug(Int_t level, const char* className = "", const char* funcName = "", const char* format = "", const char* file = "", Int_t line = -1);
   static Int_t  GetDebugLevel(const char* className);
   static Bool_t GetDebugLevel(Int_t level, const char* className);

   static void   SetClassDebugLevel(const char* className, Int_t level);
   static void   ClearClassDebugLevel(const char* className);


};

#define FootDebug(level, func, message ) GlobalPar::Debug(level, ClassName(), func, message, __FILE__, __LINE__)
#define FootDebugLevel(level) GlobalPar::GetDebugLevel(level, ClassName())

// extern GlobalPar* gPar; 
// R__EXTERN GlobalPar  *gPar; 


#endif

























