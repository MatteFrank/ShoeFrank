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
#include "TString.h"

#include "TObjArray.h"

#include "TAGrunInfo.hxx"
using namespace std;

// singleton class of global foot parameters
class GlobalPar {

public:
	static GlobalPar* Instance( string aparFileName = "FootGlobal.par" );
	static GlobalPar* GetPar();
   
public:
	~GlobalPar();

	void ReadParamFile();
	void Print(Option_t* opt = "");
   const TAGrunInfo GetGlobalInfo();

	int  Debug()                const { return m_debug;               }

	int  KalMode()              const { return m_kalmanMode;          }
	bool IsKalReverse()         const { return m_kalReverse;          }
	bool verFLUKA()             const { return m_verFLUKA;            }
   
	vector<string> KalSystems()       { return m_trackingSystems;     }
	vector<string> KalParticles()     { return m_kalParticles;        }
   vector<string> MCParticles()      { return m_mcParticles;         }
   vector<TString> DectIncluded()    { return m_dectInclude;         }

	 double VTReso() 	          const { return m_VTreso;              }
    double ITReso() 	          const { return m_ITreso;              }
    double MSDReso()           const { return m_MSDreso;             }
    double TWReso()            const { return m_TWreso;              }

    bool IsPrintOutputFile()   const { return m_printoutfile;        }
    string OutputFile()        const { return m_outputfilename;      }
 
    bool IsPrintOutputNtuple() const { return m_printoutntuple;      }
    string OutputNtuple()      const { return m_outputntuplename;    }
	
    bool IsLocalReco()         const { return m_enableLocalReco;     }
    bool IsSaveTree()          const { return m_enableTree;          }
    bool IsSaveHisto()         const { return m_enableHisto;         }
    bool IsSaveHits()          const { return m_enableSaveHits;      }
    bool IsTracking()          const { return m_enableTracking;      }
    bool IsReadRootObj()       const { return m_enableRootObject;    }
    bool IsTofZmc()            const { return m_enableTofZmc;        }
   
    bool IncludeDI()           const { return m_includeDI;           }
    bool IncludeST()           const { return m_includeST;           }
    bool IncludeBM()           const { return m_includeBM;           }
    bool IncludeTW()           const { return m_includeTW;           }
    bool IncludeMSD()          const { return m_includeMSD;          }
    bool IncludeCA()           const { return m_includeCA;           }
    bool IncludeTG()           const { return m_includeTG;           }
    bool IncludeVT()           const { return m_includeVT;           }
    bool IncludeIT()           const { return m_includeIT;           }
   
    bool IncludeTOE()          const { return m_includeTOE;          }
    bool IncludeKalman()       const { return m_includeKalman;       }
   
    void IncludeTOE(bool t)          {  m_includeTOE = t;            }
    void IncludeKalman(bool t)       {  m_includeKalman = t;         }

    void IncludeDI(bool t)           {  m_includeDI = t;             }
    void IncludeST(bool t)           {  m_includeST = t;             }
    void IncludeBM(bool t)           {  m_includeBM = t;             }
    void IncludeTW(bool t)           {  m_includeTW = t;             }
    void IncludeMSD(bool t)          {  m_includeMSD = t;            }
    void IncludeCA(bool t)           {  m_includeCA = t;             }
    void IncludeTG(bool t)           {  m_includeTG = t;             }
    void IncludeVT(bool t)           {  m_includeVT = t;             }
    void IncludeIT(bool t)           {  m_includeIT = t;             }
   
    void SetDebugLevels();
    void RemoveSpace( string* s );
    bool IEquals(const string& a, const string& b);
    bool frankFind( string what, string where );
    string StrReplace(string original, string erase, string add);

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
   
private:
	static GlobalPar* m_pInstance;
   static map<TString, TString> m_dectFullName; // full name

private:
	vector<string> m_copyInputFile;

	map< string, pair< double, double > > m_map_range;
	map< string, int > m_nBin_map;
	map< string, string > m_map_saveDir;
	map< string, string > m_map_xTitles;
	map< string, string > m_map_yTitles;
   map< string, int >    m_map_debug;
	string m_parFileName;

	int m_debug;

	vector<string> m_mcParticles;
	
	int m_kalmanMode;
	bool m_kalReverse;
	bool m_verFLUKA;
	vector<string> m_trackingSystems;
   vector<string> m_kalParticles;
   vector<TString> m_dectInclude;

	double m_VTreso;
   double m_ITreso;
   double m_MSDreso;
   double m_TWreso;

   string  m_outputfilename;
   bool m_printoutfile;
       
   string  m_outputntuplename;
   bool m_printoutntuple;
   
   Bool_t m_enableLocalReco;
   Bool_t m_enableTree;
   Bool_t m_enableHisto;
   Bool_t m_enableSaveHits;
   Bool_t m_enableTracking;
   Bool_t m_enableRootObject;
   Bool_t m_enableTofZmc;
   
   bool m_includeST;
   bool m_includeBM;
   bool m_includeTG;
   bool m_includeDI;

   bool m_includeTW;
   bool m_includeMSD;
   bool m_includeCA;
   bool m_includeIT;
   bool m_includeVT;
    
   bool m_includeKalman;
   bool m_includeTOE;

   TObjArray  m_ClassDebugLevels;          // debug levels for classes

public:
   static void   Debug(Int_t level, const char* className = "", const char* funcName = "", const char* format = "", const char* file = "", Int_t line = -1);
   static Int_t  GetDebugLevel(const char* className);
   static Bool_t GetDebugLevel(Int_t level, const char* className);
   static Bool_t GetMcDebugLevel(Int_t level, const char* className);

   static void   SetClassDebugLevel(const char* className, Int_t level);
   static void   ClearClassDebugLevel(const char* className);


};

#define FootDebug(level, func, message ) GlobalPar::Debug(level, ClassName(), func, message, __FILE__, __LINE__)
#define FootDebugLevel(level) GlobalPar::GetDebugLevel(level, ClassName())
#define FootMcDebugLevel(level) GlobalPar::GetMcDebugLevel(level, typeid(*this).name())

#endif

























