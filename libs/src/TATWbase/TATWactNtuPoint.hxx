#ifndef _TATWactNtuPoint_HXX
#define _TATWactNtuPoint_HXX
/*!
 \file
 \version $Id: TATWactNtuPoint.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TATWactNtuPoint.
 */
/*------------------------------------------+---------------------------------*/
#include "TError.h"

#include "TAGaction.hxx"

#include "TATWparGeo.hxx"
#include "TATWparCal.hxx"

#include "TAGparTools.hxx"
#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"

class TAVTntuCluster;
class TAVTntuHit;
class TAVTparGeo;
class TH1F;

class TATWactNtuPoint : public TAGaction {
   
public:
   explicit  TATWactNtuPoint(const char* name       = 0,
                             TAGdataDsc* p_nturaw   = 0,
                             TAGdataDsc* p_ntupoint = 0,
                             TAGparaDsc* p_geomap   = 0,
                             TAGparaDsc* p_calmap   = 0,
                             Bool_t isZmatch   = false,
                             Bool_t isMC   = false);

   virtual ~TATWactNtuPoint();
   
   //! Action
   virtual  Bool_t Action();
   
   //! Find point
   virtual Bool_t  FindPoints();
  
   //! Get Hit position
   virtual Double_t  GetPositionFromDeltaTime(int layer, int bar, TATWntuHit* hit);
   //! Get Hit position
   virtual Double_t  GetPositionFromBarCenter(int layer, int bar, TATWntuHit* hit);
   //! Get Local Point position
   virtual TVector3  GetLocalPointPosition(int layer1, double pos1, int bar1, int bar2);
   //! Set TW Point
   virtual TATWpoint* SetTWPoint(TATWntuPoint* ntuPoint, int layer, TATWntuHit* hit1, TATWntuHit* hit2, TVector3 pos);
   // check if hits matched in the same TW point have same Z
   virtual Bool_t IsPointsWithMatchedZ(TATWntuHit* hit1, TATWntuHit* hitmin);

   //! Create histo
   void            CreateHistogram();
   
private:
   TAGdataDsc*     fpNtuRaw;		 // input data dsc
   TAGdataDsc*     fpNtuPoint;           // output data dsc
   TAGparaDsc*     fpGeoMap;		 // geometry para dsc
   TAGparaDsc*     fpCalMap;		 // calibration Z para dsc

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
   vector<TH1F*>   fpHisElossMean;
   vector<TH1F*>   fpHisTofMean;

   map<Int_t,TATWntuHit*> mapHitX;
   map<Int_t,TATWntuHit*> mapHitY;
   map<Int_t,TATWntuHit*> mapMoreHits;
   map<Int_t,TATWntuHit*> mapLessHits;


   ClassDef(TATWactNtuPoint,0)
};

#endif
