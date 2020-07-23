#ifndef _TATWactNtuHitMC_HXX
#define _TATWactNtuHitMC_HXX
/*!
 \file
 \version $Id: TATWactNtuHitMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TATWactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/
#include <vector>
#include <map>

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TATWntuRaw.hxx"
#include "TATWparGeo.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

class TATWdigitizer;

class TATWactNtuHitMC : public TAGaction {
public:
   explicit TATWactNtuHitMC(const char* name=0,
                            TAGdataDsc* p_ntuMC=0,
                            TAGdataDsc* p_ntuStMC=0,
                            TAGdataDsc* p_ntuEve=0,
                            TAGdataDsc* p_hitraw=0,
                            TAGparaDsc* p_parcal=0,
                            TAGparaDsc *p_pargeoG=0);
   virtual  ~TATWactNtuHitMC();
   
   virtual bool  Action();
   
   void          CreateHistogram();
   
private:
   TAGdataDsc*     fpNtuMC;     // input mc hit TW
   TAGdataDsc*     fpNtuStMC;   // input mc hit ST
   TAGdataDsc*     fpNtuEve;    // input eve track dsc
   TAGdataDsc*     fpNtuRaw;	  // output data dsc
   TAGparaDsc*     fpCalPar;    // calibration parameter dsc
   TAGparaDsc*     fParGeoG;    // beam parameter dsc
   TATWdigitizer*  fDigitizer;  // digitizer
   
   Int_t fZbeam;
   Int_t fCnt;
   Int_t fCntWrong;
   
   TH1F* fpHisHitCol;
   TH1F* fpHisHitLine;
   TH1F* fpHisHitMap;
   TH1F* fpHisRecPos;
   TH1F* fpHisRecPosMc;
   TH1F* fpHisRecTof;
   TH1F* fpHisRecTofMc;
   TH2I* fpHisZID;
   TH2I* fpHisZID_MCtrue;
   TH2D* fpHisElossTof_MCrec[nLayers];
   TH2D* fpHisElossTof_MCtrue[nLayers];
   // vector of histo with the same size of the ion beam atomic number
   vector<TH2D*> fpHisElossTof_MC;
   vector<TH2D*> fpHisElossTof;
   vector<TH1F*> fpHisDistZ_MC;
   vector<TH1F*> fpHisDistZ;
   
   map<int, TATWntuHit*> fMapPU; //! map for pilepup
   vector<TATWntuHit*> fVecPuOff; //! vector for pilepup Off
   
private:
   void          CreateDigitizer();
   
   ClassDef(TATWactNtuHitMC,0)
};

#endif
