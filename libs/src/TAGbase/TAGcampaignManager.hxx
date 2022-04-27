#ifndef _TAGcampaignManager_HXX
#define _TAGcampaignManager_HXX
/*!
 \file TAGcampaignManager.hxx
 \brief   Declaration of TAGcampaignManager.
 */
/*------------------------------------------+---------------------------------*/
#include <map>
#include <TArrayI.h>
#include <TString.h>
#include <TMath.h>
#include <TGeoMatrix.h>

#include "TAGaction.hxx"
#include "TAGparTools.hxx"


using namespace std;

class TAGcampaign : public TAGparTools {
public:
   TAGcampaign();
   virtual ~TAGcampaign();
   
   //! From file
   Bool_t            FromFile(TString ifile = "");
   //! Get campaign name
   const Char_t*     GetName()     const { return fName.Data(); }
   //! Get run array of campaign
   const TArrayI&    GetRunArray() const { return fRunArray;    }
   //! Get number of devices
   Int_t             GetDevicesN() const { return fDevicesN;    }
   
   // Get geometry file name
   const Char_t*     GetGeoFile(const  TString& detName, Int_t runNumber);
   // Get configuration file name
   const Char_t*     GetConfFile(const TString& detName, Int_t runNumber, TString bName = "", Int_t bEnergy = -1);
   // Get mapping file name
   const Char_t*     GetMapFile(const  TString& detName, Int_t runNumber, Int_t item = 0);
   // Get region file name
   const Char_t*     GetRegFile(const  TString& detName, Int_t runNumber);
   // Get calibration file name
   const Char_t*     GetCalFile(const  TString& detName, Int_t runNumber, Bool_t isTofCalib = false,
                                Bool_t isTofBarCalib = false, Bool_t elossTuning = false, Bool_t rate=false);
   // Detector on flag
   Bool_t            IsDetectorOn(const TString& detName);
   // Print out information
   void              Print(Option_t* opt = "") const;
   //! Get list of detector
   const vector<TString>& GetDetVector() const { return fDetectorVec; }

private:
   // detector vector using std
   vector<TString>        fDetectorVec;        ///< List of detector
   
   // geometry file
   map<TString, TString> fFileGeoMap;          ///< Map of geometry file names for a given detector
   map<TString, TArrayI> fRunsGeoMap;          ///< Map of list of run for a given detector
   
   // configuration file
   map<TString, TString> fFileConfMap;         ///< Map of configuration file names for a given detect
   map<TString, TArrayI> fRunsConfMap;         ///< Map of list of run for a given detector
   
   // mapping file
   map<TString, vector<TString> > fFileMap;    ///< Map of mapping file names for a given detect
   map<TString, vector<TArrayI> > fRunsMap;    ///< Map of list of run for a given detector
  
   // region file
   map<TString, TString> fFileRegMap;          ///< Map of region file names for a given detect
   map<TString, TArrayI> fRunsRegMap;          ///< Map of list of run for a given detector
  
   // calibration file
   map<TString, vector<TString> > fFileCalMap; ///< Map of calibration file names for a given detect
   map<TString, vector<TArrayI> > fRunsCalMap; ///< Map of list of run for a given detector
   
   TString               fName;                ///< Campaign name
   TArrayI               fRunArray;            ///< Run array
   Int_t                 fDevicesN;            ///< Number of devices
   
private:
   //! Get file name
   const Char_t* GetFile(const TString& detName, Int_t runNumber, const TString& nameFile, TArrayI array);
   //! Get calibration items
  const Char_t* GetCalItem(const  TString& detName, Int_t runNumber, Int_t item, Bool_t isTofBarCalib = false, Bool_t israte=false);

private:
   static map<Int_t, TString> fgTWcalFileType; ///< Map of special TW calibration file name
   static map<Int_t, TString> fgTWmapFileType; ///< Map of special TW mapping file name
   static map<Int_t, TString> fgCAcalFileType; ///< Map of special CA calibration file name

   ClassDef(TAGcampaign,2)
};

//##############################################################################

class TAGcampaignManager : public TAGaction {
private:
   /*!
    \struct CamParameter_t
    \brief  Campaign parameters
    */
   struct CamParameter_t : public  TNamed {
      TString   Name;      ///< Campaign name
      Int_t     Number;    ///< Campaign number
      Bool_t    McFlag;    ///< Flag for MC data (0 for real data)
      TString   Date;      ///< Date of data taking or production
      TString   Summary;   ///< Summary of the campaign
   };
   
   TAGparTools*   fFileStream;        ///< File stream
   Int_t          fCampaignsN;        ///< Number of campaigns
   TAGcampaign*   fCurCampaign;       ///< Current campaign
   TString        fCurCampaignName;   ///< Current campaign name
   Int_t          fCurCampaignNumber; ///< Current campaign number

   CamParameter_t fCamParameter[128]; ///< Campaign parameter
   
public:
   TAGcampaignManager(const TString exp = "");
   virtual ~TAGcampaignManager();
   // From file
   Bool_t               FromFile(TString ifile = "");
   
   //! Get number of campaign
   Int_t                GetCampaignsN()             const { return fCampaignsN;        }
   //! Get current campaign number
   Int_t                GetCurrentCamNumber()       const { return fCurCampaignNumber; }
   //! Get parameter for a given campaign
   CamParameter_t&      GetCampaignPar(Int_t idx)         { return fCamParameter[idx]; }
   //! Get parameter for the current campaign
   CamParameter_t&      GetCurCampaignPar()               { return fCamParameter[fCurCampaignNumber]; }
   //! Get current campaign
   const TAGcampaign*   GetCurCampaign()                  { return fCurCampaign;       }
   
   //! Get current run number array
   const TArrayI&       GetCurRunArray() const                                       { return fCurCampaign->GetRunArray();                   }
   //! Get current geometry file name
   const Char_t*        GetCurGeoFile(const TString& detName, Int_t runNumber = -1)  { return fCurCampaign->GetGeoFile(detName, runNumber);  }
   //! Get current configuration file name
   const Char_t*        GetCurConfFile(const TString& detName, Int_t runNumber = -1,
                                       TString bName = "", Int_t bEnergy = -1)
   { return fCurCampaign->GetConfFile(detName, runNumber, bName, bEnergy); }
   //! Get current mapping file name
   const Char_t*        GetCurMapFile(const TString& detName, Int_t runNumber = -1, Int_t item = 0)  { return fCurCampaign->GetMapFile(detName, runNumber, item);  }
   //! Get current region file name
   const Char_t*        GetCurRegFile(const TString& detName, Int_t runNumber = -1)  { return fCurCampaign->GetRegFile(detName, runNumber);  }
   //! Get current calibration file name
   const Char_t*        GetCurCalFile(const TString& detName, Int_t runNumber = -1,
                                      Bool_t isTofCalib = false, Bool_t isTofBarCalib = false,
                                      Bool_t elossTuning = false, Bool_t rate=false)
  { return fCurCampaign->GetCalFile(detName, runNumber, isTofCalib, isTofBarCalib, elossTuning,rate);  }
   //! Get detector on flag
   Bool_t               IsDetectorOn(const TString& detName)                         { return fCurCampaign->IsDetectorOn(detName);           }
   // Print out informations
   void                 Print(Option_t* opt = "") const;
   
private:
   static const TString fgkDefaultActName;  ///< Default action name
   static const TString fgkDefaultCamName;  ///< Default campaign manager name
   static const TString fgkDefaultFolder;   ///< Default campaign folder
   static const TString fgkDefaultExt;      ///< Default extension name

public:
   //! Get default action name
   static TString       GetDefaultActName() { return fgkDefaultActName;        }
   //! Get default ampaign manager name
   static const Char_t* GetDefCamName()     { return fgkDefaultCamName.Data(); }
   //! Get default campaign folder
   static const Char_t* GetDefaultFolder()  { return fgkDefaultFolder.Data();  }
   //! Get default extension name
   static const Char_t* GetDefaultExt()     { return fgkDefaultExt.Data();     }

   ClassDef(TAGcampaignManager,1)
};

#endif
