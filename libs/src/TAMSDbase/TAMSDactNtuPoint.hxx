#ifndef _TAMSDactNtuPoint_HXX
#define _TAMSDactNtuPoint_HXX
/*!
 \file
 \version $Id: TAMSDactNtuPoint.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactNtuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"

class TAMSDntuCluster;
class TAMSDhit;
class TAMSDparGeo;
class TH1F;
class TH2F;

class TAMSDactNtuPoint : public TAGaction {

public:
    explicit  TAMSDactNtuPoint(const char* name       = 0,
                             TAGdataDsc* p_ntuclus   = 0,
                             TAGdataDsc* p_ntupoint = 0,
                             TAGparaDsc* p_geomap   = 0);

   virtual ~TAMSDactNtuPoint();

   //! Action
   virtual  Bool_t Action();

   //! Find point
   virtual Bool_t  FindPoints();

   //! Create histo
   void            CreateHistogram();

private:
   TAGdataDsc*     fpNtuCluster;		 // input data dsc
   TAGdataDsc*     fpNtuPoint;	 	// output data dsc
   TAGparaDsc*     fpGeoMap;		 // geometry para dsc

   TH2F*           fpHisPointMap[8];    // cluster map per station
   TH1F*           fpHisPointCharge[8]; // cluster charge per station
   TH1F*           fpHisPointChargeTot; // Total cluster charge for all stations

   ClassDef(TAMSDactNtuPoint,0)
};

#endif
