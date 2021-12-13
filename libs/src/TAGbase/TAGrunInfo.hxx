#ifndef _TAGrunInfo_HXX
#define _TAGrunInfo_HXX
/*!
  \file TAGrunInfo.hxx
  \brief   Declaration of TAGrunInfo.
*/
/*------------------------------------------+---------------------------------*/
#include "TString.h"

#include "TAGobject.hxx"
#include <map>

/*!
 \struct GlobalParameter_t
 \brief   Global reconstruction parameter
 */
struct GlobalParameter_t : public  TObject {
   GlobalParameter_t() : TObject(),
   EnableLocalReco(false), EnableTree(false), EnableHisto(false), EnableSaveHits(false),
   EnableTracking(false), EnableRootObject(false), EnableTWZmc(false), EnableTWnoPU(false),EnableTWZmatch(false), EnableTWCalBar(false),
   EnableRegionMc(false), IncludeKalman(false), IncludeTOE(false), IncludeDI(false), IncludeST(false), IncludeBM(false),
   IncludeTG(false), IncludeVT(false), IncludeIT(false), IncludeMSD(false), IncludeTW(false),
   IncludeCA(false) { }

   Bool_t EnableLocalReco;  ///< Enable local reconstruction
   Bool_t EnableTree;       ///< Enable tree saving
   Bool_t EnableHisto;      ///< Enable histogram
   Bool_t EnableSaveHits;   ///< Enable saving hits
   Bool_t EnableTracking;   ///< Enable tracking
   Bool_t EnableRootObject; ///< Enable root object reading
   Bool_t EnableTWZmc;      ///< Enable TW true MC atomic charge Z
   Bool_t EnableTWnoPU;     ///< Enable TW no pileup
   Bool_t EnableTWZmatch;   ///< Enable TW atomic charge matching
   Bool_t EnableTWCalBar;   ///< Enable TW bar calibration
   Bool_t EnableRegionMc;   ///< Enable MC region
   Bool_t IncludeKalman;    ///< Include Kalman
   Bool_t IncludeTOE;       ///< Include TOE
   Bool_t IncludeDI;        ///< Include Dipole
   Bool_t IncludeST;        ///< Include STC
   Bool_t IncludeBM;        ///< Include BM
   Bool_t IncludeTG;        ///< Include Target
   Bool_t IncludeVT;        ///< Include VTX
   Bool_t IncludeIT;        ///< Include ITR
   Bool_t IncludeMSD;       ///< Include MSD
   Bool_t IncludeTW;        ///< Include TW
   Bool_t IncludeCA;        ///< Include CAL

   ClassDef(GlobalParameter_t,2)
};

class TAGrunInfo : public TAGobject {
private:
   TString            fsCam;        ///< campaign name
   Short_t            fiRun;        ///< run number

   GlobalParameter_t  fGlobalParameter; ///< global parameters
   map<TString,int>   fCrossMap;        ///< map for the MC crossing block

public:
   TAGrunInfo();
   TAGrunInfo(const TAGrunInfo& info);
   TAGrunInfo(TString s_cam, Short_t i_run);
   virtual       ~TAGrunInfo();

   //! Set campaign name
   void            SetCampaignName(TString s_cam)    { fsCam = s_cam;       }
   //! Set run number
   void            SetRunNumber(Short_t i_run)       { fiRun = i_run;       }

   //! Get campaign name
   const           Char_t* CampaignName()      const { return fsCam.Data(); }
   //! Get run number
   Short_t         RunNumber()                 const { return fiRun;        }

   //! Clear
   virtual void    Clear(Option_t* opt="");
   //! To stream
   virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

   //! operator==
   friend bool     operator==(const TAGrunInfo& lhs, const TAGrunInfo& rhs);
   //! operator=
   const TAGrunInfo& operator=(const TAGrunInfo &right);

   //! Get global parameter
   GlobalParameter_t& GetGlobalPar() { return fGlobalParameter; }

   //! crossing region:
   //! Add region
   void            AddRegion(TString &regName, Int_t regNum ){fCrossMap[regName] = regNum;}
   //! Get region
   Int_t           GetRegion(TString &regName) const {return (fCrossMap.find(regName)==fCrossMap.end()) ? -1 : fCrossMap.at(regName);}
   //! Import crossing map
   void            ImportCrossMap(const TAGrunInfo& right){fCrossMap=right.fCrossMap;};

private:
   static TString    fgkObjectName;    ///< name in TFile

public:
   //! Get Object name
   static const Char_t* GetObjectName()   { return fgkObjectName.Data();   }

   ClassDef(TAGrunInfo,4)
};

#endif
