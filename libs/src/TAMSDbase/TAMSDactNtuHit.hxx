#ifndef _TAMSDactNtuHit_HXX
#define _TAMSDactNtuHit_HXX
/*!
 \file
 \version $Id: TAMSDactNtuHit.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
 \brief   Declaration of TAMSDactNtuHit.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TAMSDparGeo.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDparCal.hxx"
#include "TAMSDcalibrationMap.hxx"

class TAMSDactNtuHit : public TAGaction {
   
public:
   
   explicit        TAMSDactNtuHit(const char* name=0,
                                  TAGdataDsc* p_datraw=0,
                                  TAGdataDsc* p_datdaq=0,
                                  TAGparaDsc* p_pargeo=0,
                                  TAGparaDsc* p_parconf=0,
                                  TAGparaDsc* p_parcal=0);
   virtual         ~TAMSDactNtuHit();
   
   Bool_t   Action();
   
   void     CreateHistogram();
   
private:
   TAGdataDsc*     fpDatRaw;		    // input data dsc
   TAGdataDsc*     fpNtuRaw;		    // output data dsc
   TAGparaDsc*     fpGeoMap;        // output data dsc
   TAGparaDsc*     fpParConf;       // output data dsc
   TAGparaDsc*     fpParCal;        // output data dsc

   ClassDef(TAMSDactNtuHit,0)
};

#endif
