#ifndef GLOBALPAR_H
#define GLOBALPAR_H

/*!
 \file TAGrecoManager.hxx
 \brief   Declaration of TAGrecoManager.
 */
/*------------------------------------------+---------------------------------*/


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

class TAGrecoManager {

public:
   // singleton class of global foot parameters
	static TAGrecoManager* Instance( const TString expName = "" );
	static TAGrecoManager* GetPar();
   
public:
	~TAGrecoManager();

   // From file
	void  FromFile();
   // Print info
	void  Print(Option_t* opt = "");
   // Get run info
   const TAGrunInfo GetGlobalInfo();
   // Find MC particle
   Bool_t Find_MCParticle( string villain );

   //! Debug level
	Int_t  Debug()                const { return fDebugLevel;          }
   //! Get chi2 cut
	Float_t  Chi2Cut()            const { return fChi2;                }
   //! Get number of measurements
	Int_t  MeasureN()             const { return fMeasureN;            }
   //! Get skip events
	Int_t  SkipN()              	const { return fSkipN;               }

   //! Kalman Getter
   //! Kalman mode
   string KalMode()              const { return fKalmanMode;          }
   //! Preselect strategy
   string PreselectStrategy()    const { return fKPreselectStrategy;  }
   //! Kalman inverse flag
	Bool_t IsKalReverse()         const { return fKalReverse;          }
   //! Fluka version
	Bool_t verFLUKA()             const { return fVerFLUKA;            }
   //! Enable Kalman display
   Bool_t EnableEventDisplay()   const { return fEnableEventDisplay;  }

   //! Kalman system vectors
	vector<string>  KalSystems()        { return fTrackingSystems;     }
   //! Kalman particle vectors
	vector<string>  KalParticles()      { return fKalParticles;        }
   //! Kalman MC particle vectors
   vector<string>  MCParticles()       { return fMcParticles;         }
   //! Detector included
   vector<TString> DectIncluded()      { return fDectInclude;         }

    //! Reconstruction parameters Getter
    //! Local reconstruction flag

    //! Reconstruction parameters Getter
    Bool_t IsLocalReco()         const { return fEnableLocalReco;     }
    //! Saving tree flag
    Bool_t IsSaveTree()          const { return fEnableTree;          }
    //! Save histogram flag
    Bool_t IsSaveHisto()         const { return fEnableHisto;         }
    //! Save hits flag
    Bool_t IsSaveHits()          const { return fEnableSaveHits;      }
    //! Tracking flag
    Bool_t IsTracking()          const { return fEnableTracking;      }
    //! Root object flag
    Bool_t IsReadRootObj()       const { return fEnableRootObject;    }
    //! TW MC atomic charge flag
    Bool_t IsTWZmc()             const { return fEnableTWZmc;         }
    //! TW no pileup flag
    Bool_t IsTWnoPU()            const { return fEnableTWnoPU;        }
    //! TW atomic charge Z matching flag
    Bool_t IsTWZmatch()          const { return fEnableTWZmatch;      }
    //! TW calibration per bar flag
    Bool_t IsTWCalBar()          const { return fEnableTWCalBar;      }
    //! MC region flag
    Bool_t IsRegionMc()          const { return fEnableRegionMc;      }
    //! TW Calibration flag
    Bool_t CalibTW()             const { return fDoCalibTW;           }
    //! BM Calibration flag
    Bool_t CalibBM()             const { return fDoCalibBM;           }

    //! Include dipole flag
    Bool_t IncludeDI()           const { return fIncludeDI;           }
    //! Include STC flag
    Bool_t IncludeST()           const { return fIncludeST;           }
    //! Include BM flag
    Bool_t IncludeBM()           const { return fIncludeBM;           }
    //! Include TW flag
    Bool_t IncludeTW()           const { return fIncludeTW;           }
    //! Include MSD flag
    Bool_t IncludeMSD()          const { return fIncludeMSD;          }
    //! Include CA flag
    Bool_t IncludeCA()           const { return fIncludeCA;           }
    //! Include target flag
    Bool_t IncludeTG()           const { return fIncludeTG;           }
    //! Include VTX flag
    Bool_t IncludeVT()           const { return fIncludeVT;           }
    //! Include ITR flag
    Bool_t IncludeIT()           const { return fIncludeIT;           }
   
    //! Global reconstruction with TOE flag
    Bool_t IncludeTOE()          const { return fIncludeTOE;          }
    //! Global reconstruction with GenFit flag
    Bool_t IncludeKalman()       const { return fIncludeKalman;       }
    //! Global reconstruction with straight line flag
    Bool_t IncludeStraight()     const { return fIncludeStraight;     }
  
    //! Reconstruction parameters Setter
    //! Set global reconstruction with TOE flag
    void IncludeTOE(Bool_t t)          {  fIncludeTOE = t;            }
    //! Set global reconstruction with GenFit flag
    void IncludeKalman(Bool_t t)       {  fIncludeKalman = t;         }
    //! Set global reconstruction with straight line flag
    void IncludeStraight(Bool_t t)     { fIncludeStraight = t;        }
   
    //! Enable global reconstruction from local reconstruction tree
    void EnableLocalReco()             {  fEnableLocalReco = true;    }
    //! Disable global reconstruction from local reconstruction tree
    void DisableLocalReco()            {  fEnableLocalReco = false;   }

    //! Enable root object tree
    void EnableRootObject()            {  fEnableRootObject = true;   }
    //! Disable root object tree
    void DisableRootObject()           {  fEnableRootObject = false;  }

    //! Enable MC region reading
    void EnableRegionMc()              {  fEnableRegionMc = true;     }
    //! Disable MC region reading
    void DisableRegionMc()             {  fEnableRegionMc = false;    }
   
    //! Enable filling in tree
    void EnableTree()                  {  fEnableTree = true;         }
    //! Disable filling in tree
    void DisableTree()                 {  fEnableTree = false;        }
   
    //! Enable filling histograms
    void EnableHisto()                 {  fEnableHisto = true;        }
    //! Disable filling histograms
    void DisableHisto()                {  fEnableHisto = false;       }

    //! Set include dipole flag
    void IncludeDI(Bool_t t)           {  fIncludeDI = t;             }
    //! Set include STC flag
    void IncludeST(Bool_t t)           {  fIncludeST = t;             }
    //! Set include BM flag
    void IncludeBM(Bool_t t)           {  fIncludeBM = t;             }
    //! Set include TW flag
    void IncludeTW(Bool_t t)           {  fIncludeTW = t;             }
    //! Set include MSD flag
    void IncludeMSD(Bool_t t)          {  fIncludeMSD = t;            }
    //! Set include CA flag
    void IncludeCA(Bool_t t)           {  fIncludeCA = t;             }
    //! Set include target flag
    void IncludeTG(Bool_t t)           {  fIncludeTG = t;             }
    //! Set include VT flag
    void IncludeVT(Bool_t t)           {  fIncludeVT = t;             }
    //! Set include ITR flag
    void IncludeIT(Bool_t t)           {  fIncludeIT = t;             }
   
   
    //! Set calibration process for TW
    void CalibTW(Bool_t t)             {  fDoCalibTW = t;             }
    //! Set calibration process for BM
    void CalibBM(bool t)               {  fDoCalibBM = t;             }
  
    // Debug levels
    void SetDebugLevels();
  
private:
	TAGrecoManager();
	TAGrecoManager( const TString expName );
   
private:
	static TAGrecoManager*       fgInstance;      ///< Instance
   static map<TString, TString> fgkDectFullName; ///< full name detector map
   static const TString         fgkDefParName;   ///< Default parameter name

private:
   //! file parameters
	vector<string>       fCopyInputFile;         ///< Copy input file vector
   map< string, Int_t > fMapDebug;              ///< Debug map
	string               fParFileName;           ///< Reconstruction parameter file name
	Int_t                fDebugLevel;            ///< Global debug level
 
   //! debug levels for classes
   TObjArray            fClassDebugLevels;      ///< List of debug level per class
   
   //! Kalman parameters
   Float_t              fChi2;                  ///< Chi2 cut
   Int_t                fMeasureN;              ///< Number of measurements
   Int_t                fSkipN;                 ///< Number of events to skip
	vector<string>       fMcParticles;           ///< Kalman MC particles
   string               fKalmanMode;            ///< Kalman mode
   string               fKPreselectStrategy;    ///< Kalman preselection strategy
   Bool_t               fEnableEventDisplay;    ///< Enable event display
	Bool_t               fKalReverse;            ///< Kalman reverse flag
	Bool_t               fVerFLUKA;              ///< Fluka version
	vector<string>       fTrackingSystems;       ///< List of tracking systems
   vector<string>       fKalParticles;          ///< List of Kalmaan particles
   vector<TString>      fDectInclude;           ///< List of included detectors
   
   //! reconstruction parameter
   Bool_t               fEnableLocalReco;       ///< Enable global reconstruction from l0 reconstruction
   Bool_t               fEnableTree;            ///< Enbale tree filling
   Bool_t               fEnableHisto;           ///< Enable histogram filling
   Bool_t               fEnableSaveHits;        ///< Enable saving hits
   Bool_t               fEnableTracking;        ///< Enable tracking
   Bool_t               fEnableRootObject;      ///< Enable root object in tree
   Bool_t               fEnableTWZmc;           ///< Enable TW MC atomic charge Z
   Bool_t               fEnableTWnoPU;          ///< Enable TW no pileup
   Bool_t               fEnableTWZmatch;        ///< Enable TW MC atomic charge Z matching
   Bool_t               fEnableTWCalBar;        ///< Enable TW calibration per bar
   Bool_t               fDoCalibTW;             ///< Enable TW cliabration process
   Bool_t               fDoCalibBM;             ///< Enable BM cliabration process
   Bool_t               fEnableRegionMc;        ///< Enable MC region reading

   Bool_t               fIncludeST;             ///< Include STC
   Bool_t               fIncludeBM;             ///< Include BM
   Bool_t               fIncludeTG;             ///< Include target
   Bool_t               fIncludeDI;             ///< Include dipole
   Bool_t               fIncludeTW;             ///< Include TW
   Bool_t               fIncludeMSD;            ///< Include MSD
   Bool_t               fIncludeCA;             ///< Include CAL
   Bool_t               fIncludeIT;             ///< Include ITR
   Bool_t               fIncludeVT;             ///< Include VTX
    
   Bool_t               fIncludeKalman;         ///< Global reconstruction from GenFit
   Bool_t               fIncludeTOE;            ///< Global reconstruction from TOE
   Bool_t               fIncludeStraight;       ///< Global reconstruction from straight line extrapolation

public:
   // Debug with line number
   static void   DebugLine(Int_t level, const char* className = "", const char* funcName = "", const char* format = "", const char* file = "", Int_t line = -1);
   // Debug with variable output
   static void   Debug(Int_t level, const char* className = "", const char* funcName = "", const char* format = "", ...);
   // Get debug level per classname
   static Int_t  GetDebugLevel(const char* className);
   // Get debug level per level and classname
   static Bool_t GetDebugLevel(Int_t level, const char* className);
   // Get MC debug level per level and classname
   static Bool_t GetMcDebugLevel(Int_t level, const char* className);
   // Get MC dedug level per classname
   static void   GetMcInfo(const char* className = "", const char* funcName = "", const char* format = "", ...);
   // Get MC dedug level per classname
   static void   GetMcInfoMsg(const char* className = "", const char* funcName = "", const char* format = "");

   // Set debug level per classname
   static void   SetClassDebugLevel(const char* className, Int_t level);
   // Clear debug level per classname
   static void   ClearClassDebugLevel(const char* className);
};

#define FootDebug(level, func, message, ...) TAGrecoManager::Debug(level, ClassName(), func, message, __VA_ARGS__)
#define FootDebugLine(level, func, message ) TAGrecoManager::DebugLine(level, ClassName(), func, message, __FILE__, __LINE__)
#define FootDebugLevel(level) TAGrecoManager::GetDebugLevel(level, ClassName())
#define FootMcDebugLevel(level) TAGrecoManager::GetMcDebugLevel(level, typeid(*this).name())
#define InfoMc(func, message, ...) TAGrecoManager::GetMcInfo(typeid(*this).name(), func, message, __VA_ARGS__)
#define InfoMcMsg(func, message) TAGrecoManager::GetMcInfo(typeid(*this).name(), func, message)

#endif
