#ifndef _TATWactNtuPoint_HXX
#define _TATWactNtuPoint_HXX
/*!
 \file
 \version $Id: TATWactNtuPoint.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TATWactNtuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"

#include "TATWparGeo.hxx"
#include "TATWparCal.hxx"

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
                             TAGparaDsc* p_calmap   = 0);

   virtual ~TATWactNtuPoint();
   
   //! Action
   virtual  Bool_t Action();
   
   //! Find point
   virtual Bool_t  FindPoints();
   
   //! Create histo
   void            CreateHistogram();
   
private:
   TAGdataDsc*     fpNtuRaw;		 // input data dsc
   TAGdataDsc*     fpNtuPoint;           // output data dsc
   TAGparaDsc*     fpGeoMap;		 // geometry para dsc
   TAGparaDsc*     fpCalMap;		 // calibration Z para dsc


   TATWparGeo*     fparGeo;
   TATWparCal*     fparCal;

   Float_t         fDefPosErr;    // default position error

   TH1F*           fpHisDist;
   TH1F*           fpHisCharge1;
   TH1F*           fpHisCharge2;
   TH1F*           fpHisChargeTot;
   

   ClassDef(TATWactNtuPoint,0)
};

#endif
