#ifndef _TAGnameManager_HXX
#define _TAGnameManager_HXX
/*!
 \file TAGnameManager.hxx
 \brief   Declaration of TAGnameManager.
 */
/*------------------------------------------+---------------------------------*/
#include <map>
#include <TArrayI.h>
#include <TString.h>
#include <TMath.h>
#include <TGeoMatrix.h>

#include "TAGobject.hxx"
#include "TAGparTools.hxx"
#include "TAGparGeo.hxx"


using namespace std;
enum  DataMC {
   kST, kBM, kVTX, kITR, kMSD, kTW, kCAL, kWD, kEVT, kREG, kPART
};

class TAGnameManager : public TAGobject {

public:
   TAGnameManager();
   virtual ~TAGnameManager();

public:
   static const TString GetParaDscName(TString className);
   static const TString GetDataDscName(TString className);
   static const TString GetActionName(TString className,  Bool_t flagMc = false);
   static const TString GetDataDscMcName(Int_t idx = kST);

private:
   static vector<TString>        fgkParaDscMap; ///< Parameter descriptor vector
   static vector<TString>        fgkDataDscMap; ///< Data descriptor map
   static vector<TString>        fgkDataDscMapMC; ///< Data descriptor map
   static map<TString, TString>  fgkDetectorMap;  ///< Detector map

   ClassDef(TAGnameManager,1)
};

#endif
