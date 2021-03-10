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

#include "TATWntuHit.hxx"
#include "TATWparGeo.hxx"

#include "TAGgeoTrafo.hxx"
#include "EventStruct.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "Parameters.h"

class TAMCflukaParser;
class TATWdigitizer;

class TATWactNtuHitMC : public TAGaction {
public:
   explicit TATWactNtuHitMC(const char* name=0,
                            TAGdataDsc* p_ntuMC=0,
                            TAGdataDsc* p_ntuStMC=0,
                            TAGdataDsc* p_ntuEve=0,
                            TAGdataDsc* p_hitraw=0,
                            TAGparaDsc* p_parcal=0,
                            TAGparaDsc *p_pargeoG=0,
                            Bool_t isZmc = false,
			    Bool_t isZrecPUoff = false,
                            EVENT_STRUCT* evStr = 0);
  
   virtual  ~TATWactNtuHitMC();
   
   virtual bool  Action();
   
   void          CreateHistogram();
   
private:
   TAGdataDsc*     fpNtuMC;     // input mc hit TW
   TAGdataDsc*     fpNtuStMC;   // input mc hit ST
   TAGdataDsc*     fpNtuEve;    // input eve track dsc
   TAGdataDsc*     fpNtuRaw;    // output data dsc
   TAGparaDsc*     fpCalPar;    // calibration parameter dsc
   TAGparaDsc*     fParGeoG;    // beam parameter dsc
   TATWdigitizer*  fDigitizer;  // digitizer
   
   TATWparCal*     f_parcal;

   TAGparGeo*      f_pargeo;
   TAGgeoTrafo*    f_geoTrafo;
   EVENT_STRUCT*   fEventStruct;

   Int_t fZbeam;
   Int_t fCnt;
   Int_t fCntWrong;
   
   Bool_t fIsZtrueMC;
   Bool_t fIsZrecPUoff;

   TH1F* fpHisHitCol;
   TH1F* fpHisHitLine;
   TH1F* fpHisRecPos;
   TH1F* fpHisRecPosMc;
   TH1F* fpHisRecTof;
   TH1F* fpHisRecTofMc;
   TH2I* fpHisZID_f;
   TH2I* fpHisZID_r;
   TH2I* fpHisZID_MCtrue_f;
   TH2I* fpHisZID_MCtrue_r;
   TH2D* fpHisElossTof_MCrec[nLayers];
   TH2D* fpHisElossTof_MCtrue[nLayers];
   // vector of histo with the same size of the ion beam atomic number
   vector<TH2D*> fpHisElossTof_MC;
   vector<TH2D*> fpHisElossTof;
   vector<TH1F*> fpHisDistZ_MC;
   vector<TH1F*> fpHisDistZ;
   vector<TH1D*> fpHisResPos;
   
   map<int, TATWhit*> fMapPU; //! map for pilepup
   vector<TATWhit*> fVecPuOff; //! vector for pilepup Off
   
private:
   void          CreateDigitizer();
   
   ClassDef(TATWactNtuHitMC,0)
};

#endif
