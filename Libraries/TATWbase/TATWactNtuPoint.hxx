#ifndef _TATWactNtuPoint_HXX
#define _TATWactNtuPoint_HXX
/*!
 \file TATWactNtuPoint.hxx
 \brief   Declaration of TATWactNtuPoint.
 */
/*------------------------------------------+---------------------------------*/
#include "TError.h"

#include "TAGaction.hxx"

#include "TATWparGeo.hxx"
#include "TATWparConf.hxx"
#include "TATWparCal.hxx"

#include "TAGparTools.hxx"
#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"

class TH1F;

class TATWactNtuPoint : public TAGaction {
   
public:
   explicit  TATWactNtuPoint(const char* name       = 0,
                             TAGdataDsc* p_nturaw   = 0,
                             TAGdataDsc* p_ntupoint = 0,
                             TAGparaDsc* p_geomap   = 0,
                             TAGparaDsc* p_conf     = 0,
                             TAGparaDsc* p_calmap   = 0);
   
   virtual ~TATWactNtuPoint();
   
   //! Action
   virtual  Bool_t Action();
   
   //! Find point
   virtual Bool_t  FindPoints();
  
   //! Get Hit position
   virtual Double_t  GetPositionFromDeltaTime(int layer, int bar, TATWhit* hit);
   //! Get Hit position
   virtual Double_t  GetPositionFromBarCenter(int layer, int bar, TATWhit* hit);
   //! Get Local Point position
   virtual TVector3  GetLocalPointPosition(int layer1, double pos1, int bar1, int bar2);
   virtual TVector3  GetLocalPosSingleBar(Int_t pos_alongBar, TATWhit *hit);
   //! Set TW Point
   virtual TATWpoint* SetTWPoint(TATWntuPoint* ntuPoint, int layer, TATWhit* hit1, TATWhit* hit2, TVector3 pos);
  //! function to remove hits in TW beyond the bars X-Y overlapped region
   virtual  Bool_t IsInsideTwOverlappedXYBars( Double_t pos_along_bar, Double_t half_bar_length );
   //! check if hits matched in the same TW point have same Z
   virtual Bool_t IsPointWithMatchedZ(TATWhit* hit1, TATWhit* hitmin);
   //! IsMultHit return true if a MC track has hit multiple bars in the same layer (multi-Hit)
   virtual Bool_t IsMultHit(TATWhit* hit);

   //! Create histo
   void            CreateHistogram();
   
private:
   TAGdataDsc*     fpNtuRaw;		 // input data dsc
   TAGdataDsc*     fpNtuPoint;           // output data dsc
   TAGparaDsc*     fpGeoMap;		 // geometry para dsc
   TAGparaDsc*     fpConfMap;       // configuration para dsc
   TAGparaDsc*     fpCalMap;       // calibration Z para dsc

   TAGparGeo*      fparGeo;
   TAGgeoTrafo*    fgeoTrafo;

   TATWparGeo*     fparGeoTW;
   TATWparCal*     fparCal;

   Int_t           fZbeam;
   Float_t         fDefPosErr;    // default position error
   Bool_t          fIsZmatch;
   Bool_t          fIsZMCtrue;

   vector<TH1F*>   fpHisDist;
   vector<TH1F*>   fpHisDeltaE;
   vector<TH1F*>   fpHisDeltaTof;
   vector<TH1F*>   fpHisDeltaPosX;
   vector<TH1F*>   fpHisDeltaPosY;
   vector<TH1F*>   fpHisElossMean;
   vector<TH1F*>   fpHisTofMean;
   TH2F*           fpHisPointMap;
   
   map<Int_t,TATWhit*> fmapHitX;
   map<Int_t,TATWhit*> fmapHitY;
   map<Int_t,TATWhit*> fmapMoreHits;
   map<Int_t,TATWhit*> fmapLessHits;

   map<int,vector<int>> fmapMultHit;

   ClassDef(TATWactNtuPoint,0)
};

#endif
