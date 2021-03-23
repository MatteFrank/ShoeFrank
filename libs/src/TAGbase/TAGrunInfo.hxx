#ifndef _TAGrunInfo_HXX
#define _TAGrunInfo_HXX
/*!
  \file
  \version $Id: TAGrunInfo.hxx
  \brief   Declaration of TAGrunInfo.
*/
/*------------------------------------------+---------------------------------*/
#include "TString.h"

#include "TAGobject.hxx"
#include <map>

struct GlobalParameter_t : public  TObject {
   GlobalParameter_t() : TObject(),
   EnableLocalReco(false), EnableTree(false), EnableHisto(false), EnableSaveHits(false),
   EnableTracking(false), EnableRootObject(false), EnableTWZmc(false), EnableTWnoPU(false),EnableTWZmatch(false), EnableTWCalBar(false),
   IncludeKalman(false), IncludeTOE(false), IncludeCross(false), IncludeDI(false), IncludeST(false), IncludeBM(false),
   IncludeTG(false), IncludeVT(false), IncludeIT(false), IncludeMSD(false), IncludeTW(false),
   IncludeCA(false) { }

   Bool_t EnableLocalReco;
   Bool_t EnableTree;
   Bool_t EnableHisto;
   Bool_t EnableSaveHits;
   Bool_t EnableTracking;
   Bool_t EnableRootObject;
   Bool_t EnableTWZmc;
   Bool_t EnableTWnoPU;
   Bool_t EnableTWZmatch;
   Bool_t EnableTWCalBar;
   Bool_t IncludeKalman;
   Bool_t IncludeTOE;
   Bool_t IncludeCross;
   Bool_t IncludeDI;
   Bool_t IncludeST;
   Bool_t IncludeBM;
   Bool_t IncludeTG;
   Bool_t IncludeVT;
   Bool_t IncludeIT;
   Bool_t IncludeMSD;
   Bool_t IncludeTW;
   Bool_t IncludeCA;

   ClassDef(GlobalParameter_t,2)
};

class TAGrunInfo : public TAGobject {
private:
   TString            fsCam;        // campaign name
   Short_t            fiRun;        // run number

   GlobalParameter_t  fGlobalParameter; // global parameters
   map<TString,int>   fCrossMap;        // map for the MC crossing block

public:
   TAGrunInfo();
   TAGrunInfo(const TAGrunInfo& info);
   TAGrunInfo(TString s_cam, Short_t i_run);
   virtual       ~TAGrunInfo();

   void            SetCampaignName(TString s_cam)    { fsCam = s_cam;       }
   void            SetRunNumber(Short_t i_run)       { fiRun = i_run;       }

   const           Char_t* CampaignName()      const { return fsCam.Data(); }
   Short_t         RunNumber()                 const { return fiRun;        }

   virtual void    Clear(Option_t* opt="");

   virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

   friend bool     operator==(const TAGrunInfo& lhs, const TAGrunInfo& rhs);
   const TAGrunInfo& operator=(const TAGrunInfo &right);

   GlobalParameter_t& GetGlobalPar() { return fGlobalParameter; }

   //crossing region:
   void            AddRegion(TString &regName, Int_t regNum ){fCrossMap[regName] = regNum;};
   Int_t           GetRegion(TString &regName){return (fCrossMap.find(regName)==fCrossMap.end()) ? -1 : fCrossMap.at(regName);};

private:
   static TString    fgkObjectName;    // name in TFile

public:
   static const Char_t* GetObjectName()   { return fgkObjectName.Data();   }

   ClassDef(TAGrunInfo,3)
};

#endif
