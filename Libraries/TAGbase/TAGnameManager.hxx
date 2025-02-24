#ifndef _TAGnameManager_HXX
#define _TAGnameManager_HXX
/*!
 \file TAGnameManager.hxx
 \brief   Declaration of TAGnameManager.
 */
/*------------------------------------------+---------------------------------*/
#include <map>
#include <TString.h>

#include "TAGobject.hxx"
#include "TAGparTools.hxx"
#include "TAGparGeo.hxx"


using namespace std;
enum  DetectorMC {
   kST, kBM, kVTX, kITR, kMSD, kTW, kCAL
};

class TAGnameManager : public TAGobject {

public:
   TAGnameManager();
   virtual ~TAGnameManager();

public:
   static const TString GetParaDscName(TString className);
   static const TString GetActionDscName(TString className);
   static const TString GetBranchName(TString className);
   static const TString GetBaseName(TString className);
   static const TString GetDataDscMcName(Int_t idx = kST);
   static const TString GetBranchMcName(Int_t idx = kST);

private:
   static vector<TString>        fgkParaDscMap;   ///< Parameter descriptor vector
   static vector<TString>        fgkDataDscMap;   ///< Data descriptor map
   static vector<TString>        fgkDataDscMapMC; ///< Data descriptor map
   static map<TString, TString>  fgkDetectorMap;  ///< Detector map
   static map<TString, TString>  fgkBranchMap;    ///< Branch map
   static map<TString, TString>  fgkBaseNameMap;  ///< Base name map

   static TString                fgkMcSuffix;     ///< MC suffix
   static TString                fgkActPrefix;    /// Action prefix
   static TString                fgkNtuHitMc;     ///< Special case for MC Hit container
   ClassDef(TAGnameManager,1)
};

#define FootParaDscName(className)   TAGnameManager::GetParaDscName(className).Data()
#define FootActionDscName(className) TAGnameManager::GetActionDscName(className).Data()
#define FootBranchName(className)    TAGnameManager::GetBranchName(className).Data()
#define FootBaseName(className)      TAGnameManager::GetBaseName(className).Data()
#define FootDataDscMcName(idx)       TAGnameManager::GetDataDscMcName(idx).Data()
#define FootBranchMcName(idx)        TAGnameManager::GetBranchMcName(idx).Data()

#endif
