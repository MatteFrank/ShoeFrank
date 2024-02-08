#ifndef _TATWactNtuHitMC_HXX
#define _TATWactNtuHitMC_HXX
/*!
 \file TATWactNtuHitMC.hxx
 \brief   Declaration of TATWactNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/
#include <vector>
#include <map>

#include "TVector3.h"
#include "TH1.h"
#include "TH2.h"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAGgeoTrafo.hxx"

#include "TAMCflukaStruct.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

#include "TATWntuHit.hxx"
#include "TATWparGeo.hxx"
#include "TATWparConf.hxx"

#include "TATWparameters.hxx"

class TAMCflukaParser;
class TATWdigitizer;

class TATWactNtuHitMC : public TAGaction {
public:
   explicit TATWactNtuHitMC(const char* name=0,
                            TAGdataDsc* p_ntuMC=0,
                            TAGdataDsc* p_ntuStMC=0,
                            TAGdataDsc* p_ntuStHit=0,
                            TAGdataDsc* p_ntuEve=0,
                            TAGdataDsc* p_hitraw=0,
                            TAGparaDsc* p_parconf=0,
                            TAGparaDsc* p_parcal=0,
                            TAGparaDsc *p_pargeoG=0,
                            EVENT_STRUCT* evStr = 0);
  
   virtual  ~TATWactNtuHitMC();
   
   virtual bool  Action();
   
   void          CreateHistogram();
   
private:
   TAGdataDsc*     fpNtuTwMC;   // input mc hit TW
   TAGdataDsc*     fpNtuStMC;   // input mc hit ST
   TAGdataDsc*     fpNtuStHit;  // input reco MC hit ST
   TAGdataDsc*     fpNtuEve;    // input eve track dsc
   TAGdataDsc*     fpNtuRaw;    // output data dsc
   TAGparaDsc*     fpParConf;   // configuration parameter dsc
   TAGparaDsc*     fpCalPar;    // calibration parameter dsc
   TAGparaDsc*     fParGeoG;    // beam parameter dsc

   TATWdigitizer*  fDigitizer;  // digitizer
   
   TATWparCal*     f_parcal;
   TATWparConf*    f_parconf;

   TAGparGeo*      f_pargeo;
   TAGgeoTrafo*    f_geoTrafo;

   EVENT_STRUCT*   fEventStruct;

   Int_t           fZbeam;
   Int_t           fCnt;
   Int_t           fCntWrong;
   
   Bool_t          fIsZtrueMC;
   Bool_t          fIsZrecPUoff;
   Bool_t          fIsEnergyThrEnabled;
   Bool_t          fIsRateSmear;

   TH1I*           fpHisBarsID[nLayers];
   TH2I*           fpHisZID_MCrec[nLayers];
   TH2I*           fpHisZID_MCtrue[nLayers];
   TH2D*           fpHisElossTof_MCrec[nLayers];
   TH2D*           fpHisElossTof_MCtrue[nLayers];
   TH1D*           fpHisRate;

   // vector of histo with the same size of the ion beam atomic number
   vector<TH1D*>   fpHisResPos;
   vector<TH1D*>   fpHisResPos_2;
   vector<TH1D*>   fpHisResTof;
   vector<TH1D*>   fpHisResEloss;
   // vector<TH2D*>   fpHisElossTof_MC;
   vector<TH2D*>   fpHisElossTof;
   vector<TH2D*>   fpHisElossTof_MCtrue_Z[nLayers];  //! Eloss vs Tof for each Z and each TW layer for ZID tuning
   vector<TH2D*>   fpHisElossTof_MCrec_Z[nLayers];  //! Eloss vs Tof for each Z rec with no PU for each TW layer for ZID tuning
   vector<TH1F*>   fpHisDistZ_MC;
   vector<TH1F*>   fpHisDistZ;
  
   map<int, TATWhit*> fMapPU; //! map for pilepup: store TW hits in the same bar in the same event
   vector<TATWhit*>   fVecPuOff; //! vector for pilepup Off: store all the TW hits without taking care of multi hits in the same bar 
   
private:
   void          CreateDigitizer();
   void          StudyPerformancesZID(TAMChit *hitTW, TAMCntuHit *hitst, TAMCntuPart *part);
   void          ClearContainers();
   void          AssignZchargeAndToF(TATWhit *hittw, TAMCntuHit *hitst);     
   void          PrintTrueMcTWquantities(TAMChit *twhitmc, Int_t idtwhit);
   void          FlagUnderEnergyThresholdHits(TATWhit *hittw);
   void          PlotRecMcTWquantities(TATWhit *twhit, TAMCntuHit *hitst, Int_t zrec, Int_t zmc);
   void          PrintRecTWquantities(TATWhit *tatwhit, Int_t zrec, Int_t zmc);
  
   Int_t         GetZmcTrue(TAMCntuPart *mcPart, Int_t trkid);

   Double_t      GetTimeST(TAGdataDsc *ntuStHit, TATWhit *twhit);
   Double_t      GetTruePosAlongBar(TAMChit *twhitmc);

   ClassDef(TATWactNtuHitMC,0)
};

#endif
