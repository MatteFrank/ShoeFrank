#ifndef _TAGcampaignManager_HXX
#define _TAGcampaignManager_HXX

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
   
   Bool_t            FromFile(TString ifile = "");
   const Char_t*     GetName()     const { return fName.Data(); }
   const TArrayI&    GetRunArray() const { return fRunArray;    }
   Int_t             GetDevicesN() const { return fDevicesN;    }
   
   const Char_t*     GetGeoFile(const  TString& detName, Int_t runNumber);
   const Char_t*     GetConfFile(const TString& detName, Int_t runNumber, TString bName = "", Int_t bEnergy = -1);
   const Char_t*     GetMapFile(const  TString& detName, Int_t runNumber, Int_t item = 0);
   const Char_t*     GetRegFile(const  TString& detName, Int_t runNumber);
   const Char_t*     GetCalFile(const  TString& detName, Int_t runNumber, Bool_t isTofCalib = false,
                                Bool_t isTofBarCalib = false, Bool_t elossTuning = false);

   Bool_t            IsDetectorOn(const TString& detName);
   void              Print(Option_t* opt = "") const;

   const vector<TString>& GetDetVector() const { return fDetectorVec; }

private:
   // detector vector using std
   vector<TString>        fDetectorVec;
   
   // geometry file
   map<TString, TString> fFileGeoMap;
   map<TString, TArrayI> fRunsGeoMap;
   
   // configuration file
   map<TString, TString> fFileConfMap;
   map<TString, TArrayI> fRunsConfMap;
   
   // mapping file
   map<TString, vector<TString> > fFileMap;
   map<TString, vector<TArrayI> > fRunsMap;
  
   // region file
   map<TString, TString> fFileRegMap;
   map<TString, TArrayI> fRunsRegMap;
  
   // calibration file
   map<TString, vector<TString> > fFileCalMap;
   map<TString, vector<TArrayI> > fRunsCalMap;
   
   TString               fName;
   TArrayI               fRunArray;
   Int_t                 fDevicesN;
   
private:
   const Char_t* GetFile(const TString& detName, Int_t runNumber, const TString& nameFile, TArrayI array);
   const Char_t* GetCalItem(const  TString& detName, Int_t runNumber, Int_t item, Bool_t isTofBarCalib = false);

private:
   static map<Int_t, TString> fgTWcalFileType;
   static map<Int_t, TString> fgTWmapFileType;
   static map<Int_t, TString> fgCAcalFileType;

   ClassDef(TAGcampaign,2)
};

//##############################################################################

class TAGcampaignManager : public TAGaction {
private:
   struct CamParameter_t : public  TNamed {
      TString   Name;      // Campaign name
      Int_t     Number;    // Campaign number
      Bool_t    McFlag;    // Flag for MC data (0 for real data)
      TString   Date;      // Date of data taking or production
      TString   Summary;   // Summary of the campaign
   };
   
   TAGparTools*   fFileStream;
   Int_t          fCampaignsN;
   TAGcampaign*   fCurCampaign;
   TString        fCurCampaignName;
   Int_t          fCurCampaignNumber;

   CamParameter_t fCamParameter[128];
   
public:
   TAGcampaignManager(const TString exp = "");
   virtual ~TAGcampaignManager();
   
   Bool_t               FromFile(TString ifile = "");
   
   Int_t                GetCampaignsN()             const { return fCampaignsN;        }
   Int_t                GetCurrentCamNumber()       const { return fCurCampaignNumber; }
   CamParameter_t&      GetCampaignPar(Int_t idx)         { return fCamParameter[idx]; }
   CamParameter_t&      GetCurCampaignPar()               { return fCamParameter[fCurCampaignNumber]; }
   const TAGcampaign*   GetCurCampaign()                  { return fCurCampaign;       }
   
   const TArrayI&       GetCurRunArray() const                                       { return fCurCampaign->GetRunArray();                   }
   const Char_t*        GetCurGeoFile(const TString& detName, Int_t runNumber = -1)  { return fCurCampaign->GetGeoFile(detName, runNumber);  }
   const Char_t*        GetCurConfFile(const TString& detName, Int_t runNumber = -1,
                                       TString bName = "", Int_t bEnergy = -1)
   { return fCurCampaign->GetConfFile(detName, runNumber, bName, bEnergy); }
   const Char_t*        GetCurMapFile(const TString& detName, Int_t runNumber = -1, Int_t item = 0)  { return fCurCampaign->GetMapFile(detName, runNumber, item);  }
  
   const Char_t*        GetCurRegFile(const TString& detName, Int_t runNumber = -1)  { return fCurCampaign->GetRegFile(detName, runNumber);  }
  
   const Char_t*        GetCurCalFile(const TString& detName, Int_t runNumber = -1,
                                      Bool_t isTofCalib = false, Bool_t isTofBarCalib = false,
                                      Bool_t elossTuning = false)
   { return fCurCampaign->GetCalFile(detName, runNumber, isTofCalib, isTofBarCalib, elossTuning);  }
   Bool_t               IsDetectorOn(const TString& detName)                         { return fCurCampaign->IsDetectorOn(detName);           }

   void                 Print(Option_t* opt = "") const;
   
private:
   static const TString fgkDefaultActName;
   static const TString fgkDefaultCamName;
   static const TString fgkDefaultFolder;
   static const TString fgkDefaultExt;

public:
   static TString       GetDefaultActName() { return fgkDefaultActName;        }
   static const Char_t* GetDefCamName()     { return fgkDefaultCamName.Data(); }
   static const Char_t* GetDefaultFolder()  { return fgkDefaultFolder.Data();  }
   static const Char_t* GetDefaultExt()     { return fgkDefaultExt.Data();     }

   
   ClassDef(TAGcampaignManager,1)
};

#endif
