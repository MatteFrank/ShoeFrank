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
class TAGrecoManager {

public:
	static TAGrecoManager* Instance( const TString expName = "" );
	static TAGrecoManager* GetPar();
   
public:
	~TAGrecoManager();

	void FromFile();
	void Print(Option_t* opt = "");
   const TAGrunInfo GetGlobalInfo();

	Int_t  Debug()                const { return m_debug;               }

    // Kalman Getter
   string KalMode()              const { return m_kalmanMode;          }
   string PreselectStrategy()    const { return m_kPreselectStrategy;  }
	Bool_t IsKalReverse()         const { return m_kalReverse;          }
	Bool_t verFLUKA()             const { return m_verFLUKA;            }
   Bool_t EnableEventDisplay()   const { return m_enableEventDisplay;  }

	vector<string> KalSystems()         { return m_trackingSystems;     }
	vector<string> KalParticles()       { return m_kalParticles;        }
   vector<string> MCParticles()        { return m_mcParticles;         }
   vector<TString> DectIncluded()      { return m_dectInclude;         }

	 Double_t VTReso() 	         const { return m_VTreso;              }
    Double_t ITReso() 	         const { return m_ITreso;              }
    Double_t MSDReso()           const { return m_MSDreso;             }
    Double_t TWReso()            const { return m_TWreso;              }

    Bool_t Find_MCParticle( string villain );

    // Reconstruction parameters Getter
    Bool_t IsLocalReco()         const { return m_enableLocalReco;     }
    Bool_t IsSaveTree()          const { return m_enableTree;          }
    Bool_t IsSaveHisto()         const { return m_enableHisto;         }
    Bool_t IsSaveHits()          const { return m_enableSaveHits;      }
    Bool_t IsTracking()          const { return m_enableTracking;      }
    Bool_t IsReadRootObj()       const { return m_enableRootObject;    }
    Bool_t IsTWZmc()             const { return m_enableTWZmc;         }
    Bool_t IsTWnoPU()            const { return m_enableTWnoPU;        }
    Bool_t IsTWZmatch()          const { return m_enableTWZmatch;      }
    Bool_t IsTWCalBar()          const { return m_enableTWCalBar;      }
    Bool_t IsRegionMc()          const { return m_enableRegionMc;      }
    Bool_t CalibTW()             const { return m_doCalibTW;           }
    Bool_t CalibBM()             const { return m_doCalibBM;           }

    Bool_t IncludeDI()           const { return m_includeDI;           }
    Bool_t IncludeST()           const { return m_includeST;           }
    Bool_t IncludeBM()           const { return m_includeBM;           }
    Bool_t IncludeTW()           const { return m_includeTW;           }
    Bool_t IncludeMSD()          const { return m_includeMSD;          }
    Bool_t IncludeCA()           const { return m_includeCA;           }
    Bool_t IncludeTG()           const { return m_includeTG;           }
    Bool_t IncludeVT()           const { return m_includeVT;           }
    Bool_t IncludeIT()           const { return m_includeIT;           }
   
    Bool_t IncludeTOE()          const { return m_includeTOE;          }
    Bool_t IncludeKalman()       const { return m_includeKalman;       }
  
    // Reconstruction parameters Setter
    void IncludeTOE(Bool_t t)          {  m_includeTOE = t;            }
    void IncludeKalman(Bool_t t)       {  m_includeKalman = t;         }
    void EnableLocalReco()             {  m_enableLocalReco = true;    }
    void DisableLocalReco()            {  m_enableLocalReco = false;   }

    void EnableRootObject()            {  m_enableRootObject = true;   }
    void DisableRootObject()           {  m_enableRootObject = false;  }

    void EnableRegionMc()              {  m_enableRegionMc = true;     }
    void DisableRegionMc()             {  m_enableRegionMc = false;    }
   
    void EnableTree()                  {  m_enableTree = true;         }
    void DisableTree()                 {  m_enableTree = false;        }
   
    void EnableHisto()                 {  m_enableHisto = true;        }
    void DisableHisto()                {  m_enableHisto = false;       }

    void IncludeDI(Bool_t t)           {  m_includeDI = t;             }
    void IncludeST(Bool_t t)           {  m_includeST = t;             }
    void IncludeBM(Bool_t t)           {  m_includeBM = t;             }
    void IncludeTW(Bool_t t)           {  m_includeTW = t;             }
    void IncludeMSD(Bool_t t)          {  m_includeMSD = t;            }
    void IncludeCA(Bool_t t)           {  m_includeCA = t;             }
    void IncludeTG(Bool_t t)           {  m_includeTG = t;             }
    void IncludeVT(Bool_t t)           {  m_includeVT = t;             }
    void IncludeIT(Bool_t t)           {  m_includeIT = t;             }
    void CalibTW(Bool_t t)             {  m_doCalibTW = t;             }
    void CalibBM(bool t)               {  m_doCalibBM = t;             }
  
    void SetDebugLevels();
  
private:
	TAGrecoManager();
	TAGrecoManager( const TString expName );
   
private:
	static TAGrecoManager* m_pInstance;
   static map<TString, TString> m_dectFullName; // full name
   static const TString m_defParName;

private:
	vector<string> m_copyInputFile;

   map< string, Int_t >    m_map_debug;
	string m_parFileName;

	Int_t m_debug;

   // Kalman parameters
	vector<string> m_mcParticles;
   string m_kalmanMode;
   string m_kPreselectStrategy;
   Bool_t m_enableEventDisplay;
	Bool_t m_kalReverse;
	Bool_t m_verFLUKA;
	vector<string> m_trackingSystems;
   vector<string> m_kalParticles;
   vector<TString> m_dectInclude;

	Double_t m_VTreso;
   Double_t m_ITreso;
   Double_t m_MSDreso;
   Double_t m_TWreso;
  
   // reconstruction parameter
   Bool_t m_enableLocalReco;
   Bool_t m_enableTree;
   Bool_t m_enableHisto;
   Bool_t m_enableSaveHits;
   Bool_t m_enableTracking;
   Bool_t m_enableRootObject;
   Bool_t m_enableTWZmc;
   Bool_t m_enableTWnoPU;
   Bool_t m_enableTWZmatch;
   Bool_t m_enableTWCalBar;
   Bool_t m_doCalibTW;
   Bool_t m_doCalibBM;
   Bool_t m_enableRegionMc;

   Bool_t m_includeST;
   Bool_t m_includeBM;
   Bool_t m_includeTG;
   Bool_t m_includeDI;

   Bool_t m_includeTW;
   Bool_t m_includeMSD;
   Bool_t m_includeCA;
   Bool_t m_includeIT;
   Bool_t m_includeVT;
    
   Bool_t m_includeKalman;
   Bool_t m_includeTOE;
  
   // debug levels for classes
   TObjArray  m_ClassDebugLevels;

public:
   static void   DebugLine(Int_t level, const char* className = "", const char* funcName = "", const char* format = "", const char* file = "", Int_t line = -1);
   static void   Debug(Int_t level, const char* className = "", const char* funcName = "", const char* format = "", ...);
   static Int_t  GetDebugLevel(const char* className);
   static Bool_t GetDebugLevel(Int_t level, const char* className);
   static Bool_t GetMcDebugLevel(Int_t level, const char* className);
   static void   GetMcInfo(const char* className = "", const char* funcName = "", const char* format = "", ...);
   static void   GetMcInfoMsg(const char* className = "", const char* funcName = "", const char* format = "");

   static void   SetClassDebugLevel(const char* className, Int_t level);
   static void   ClearClassDebugLevel(const char* className);
};

#define FootDebug(level, func, message, ...) TAGrecoManager::Debug(level, ClassName(), func, message, __VA_ARGS__)
#define FootDebugLine(level, func, message ) TAGrecoManager::DebugLine(level, ClassName(), func, message, __FILE__, __LINE__)
#define FootDebugLevel(level) TAGrecoManager::GetDebugLevel(level, ClassName())
#define FootMcDebugLevel(level) TAGrecoManager::GetMcDebugLevel(level, typeid(*this).name())
#define InfoMc(func, message, ...) TAGrecoManager::GetMcInfo(typeid(*this).name(), func, message, __VA_ARGS__)
#define InfoMcMsg(func, message) TAGrecoManager::GetMcInfo(typeid(*this).name(), func, message)

#endif
