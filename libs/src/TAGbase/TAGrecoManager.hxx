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

	void             FromFile();
	void             Print(Option_t* opt = "");
   const TAGrunInfo GetGlobalInfo();
   Bool_t           Find_MCParticle( string villain );

	Int_t  Debug()                const { return fDebugLevel;          }

    // Kalman Getter
   string KalMode()              const { return fKalmanMode;          }
   string PreselectStrategy()    const { return fKPreselectStrategy;  }
	Bool_t IsKalReverse()         const { return fKalReverse;          }
	Bool_t verFLUKA()             const { return fVerFLUKA;            }
   Bool_t EnableEventDisplay()   const { return fEnableEventDisplay;  }

	vector<string>  KalSystems()        { return fTrackingSystems;     }
	vector<string>  KalParticles()      { return fKalParticles;        }
   vector<string>  MCParticles()       { return fMcParticles;         }
   vector<TString> DectIncluded()      { return fDectInclude;         }

	 Double_t VTReso() 	         const { return fVTreso;              }
    Double_t ITReso() 	         const { return fITreso;              }
    Double_t MSDReso()           const { return fMSDreso;             }
    Double_t TWReso()            const { return fTWreso;              }

    // Reconstruction parameters Getter
    Bool_t IsLocalReco()         const { return fEnableLocalReco;     }
    Bool_t IsSaveTree()          const { return fEnableTree;          }
    Bool_t IsSaveHisto()         const { return fEnableHisto;         }
    Bool_t IsSaveHits()          const { return fEnableSaveHits;      }
    Bool_t IsTracking()          const { return fEnableTracking;      }
    Bool_t IsReadRootObj()       const { return fEnableRootObject;    }
    Bool_t IsTWZmc()             const { return fEnableTWZmc;         }
    Bool_t IsTWnoPU()            const { return fEnableTWnoPU;        }
    Bool_t IsTWZmatch()          const { return fEnableTWZmatch;      }
    Bool_t IsTWCalBar()          const { return fEnableTWCalBar;      }
    Bool_t IsRegionMc()          const { return fEnableRegionMc;      }
    Bool_t CalibTW()             const { return fDoCalibTW;           }
    Bool_t CalibBM()             const { return fDoCalibBM;           }

    Bool_t IncludeDI()           const { return fIncludeDI;           }
    Bool_t IncludeST()           const { return fIncludeST;           }
    Bool_t IncludeBM()           const { return fIncludeBM;           }
    Bool_t IncludeTW()           const { return fIncludeTW;           }
    Bool_t IncludeMSD()          const { return fIncludeMSD;          }
    Bool_t IncludeCA()           const { return fIncludeCA;           }
    Bool_t IncludeTG()           const { return fIncludeTG;           }
    Bool_t IncludeVT()           const { return fIncludeVT;           }
    Bool_t IncludeIT()           const { return fIncludeIT;           }
   
    Bool_t IncludeTOE()          const { return fIncludeTOE;          }
    Bool_t IncludeKalman()       const { return fIncludeKalman;       }
  
    // Reconstruction parameters Setter
    void IncludeTOE(Bool_t t)          {  fIncludeTOE = t;            }
    void IncludeKalman(Bool_t t)       {  fIncludeKalman = t;         }
    void EnableLocalReco()             {  fEnableLocalReco = true;    }
    void DisableLocalReco()            {  fEnableLocalReco = false;   }

    void EnableRootObject()            {  fEnableRootObject = true;   }
    void DisableRootObject()           {  fEnableRootObject = false;  }

    void EnableRegionMc()              {  fEnableRegionMc = true;     }
    void DisableRegionMc()             {  fEnableRegionMc = false;    }
   
    void EnableTree()                  {  fEnableTree = true;         }
    void DisableTree()                 {  fEnableTree = false;        }
   
    void EnableHisto()                 {  fEnableHisto = true;        }
    void DisableHisto()                {  fEnableHisto = false;       }

    void IncludeDI(Bool_t t)           {  fIncludeDI = t;             }
    void IncludeST(Bool_t t)           {  fIncludeST = t;             }
    void IncludeBM(Bool_t t)           {  fIncludeBM = t;             }
    void IncludeTW(Bool_t t)           {  fIncludeTW = t;             }
    void IncludeMSD(Bool_t t)          {  fIncludeMSD = t;            }
    void IncludeCA(Bool_t t)           {  fIncludeCA = t;             }
    void IncludeTG(Bool_t t)           {  fIncludeTG = t;             }
    void IncludeVT(Bool_t t)           {  fIncludeVT = t;             }
    void IncludeIT(Bool_t t)           {  fIncludeIT = t;             }
    void CalibTW(Bool_t t)             {  fDoCalibTW = t;             }
    void CalibBM(bool t)               {  fDoCalibBM = t;             }
  
    void SetDebugLevels();
  
private:
	TAGrecoManager();
	TAGrecoManager( const TString expName );
   
private:
	static TAGrecoManager*       fgInstance;
   static map<TString, TString> fgkDectFullName; // full name
   static const TString         fgkDefParName;

private:
   // file parameters
	vector<string>       fCopyInputFile;
   map< string, Int_t > fMapDebug;
	string               fParFileName;
	Int_t                fDebugLevel;

   // debug levels for classes
   TObjArray fClassDebugLevels;
   
   // Kalman parameters
	vector<string>  fMcParticles;
   string          fKalmanMode;
   string          fKPreselectStrategy;
   Bool_t          fEnableEventDisplay;
	Bool_t          fKalReverse;
	Bool_t          fVerFLUKA;
	vector<string>  fTrackingSystems;
   vector<string>  fKalParticles;
   vector<TString> fDectInclude;

	Double_t        fVTreso;
   Double_t        fITreso;
   Double_t        fMSDreso;
   Double_t        fTWreso;
  
   // reconstruction parameter
   Bool_t fEnableLocalReco;
   Bool_t fEnableTree;
   Bool_t fEnableHisto;
   Bool_t fEnableSaveHits;
   Bool_t fEnableTracking;
   Bool_t fEnableRootObject;
   Bool_t fEnableTWZmc;
   Bool_t fEnableTWnoPU;
   Bool_t fEnableTWZmatch;
   Bool_t fEnableTWCalBar;
   Bool_t fDoCalibTW;
   Bool_t fDoCalibBM;
   Bool_t fEnableRegionMc;

   Bool_t fIncludeST;
   Bool_t fIncludeBM;
   Bool_t fIncludeTG;
   Bool_t fIncludeDI;

   Bool_t fIncludeTW;
   Bool_t fIncludeMSD;
   Bool_t fIncludeCA;
   Bool_t fIncludeIT;
   Bool_t fIncludeVT;
    
   Bool_t fIncludeKalman;
   Bool_t fIncludeTOE;

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
