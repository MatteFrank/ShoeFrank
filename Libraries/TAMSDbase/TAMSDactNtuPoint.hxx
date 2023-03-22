#ifndef _TAMSDactNtuPoint_HXX
#define _TAMSDactNtuPoint_HXX
/*!
 \file TAMSDactNtuPoint.hxx
 \brief   Declaration of TAMSDactNtuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAMSDparameters.hxx"

class TAMSDntuCluster;
class TAMSDhit;
class TAMSDparGeo;
class TH1F;
class TH2F;

class TAMSDactNtuPoint : public TAGaction {

public:
    explicit       TAMSDactNtuPoint(const char* name       = 0,
                                    TAGdataDsc* p_ntuclus  = 0,
                                    TAGdataDsc* p_ntupoint = 0,
                                    TAGparaDsc* p_geomap   = 0);

   virtual        ~TAMSDactNtuPoint();

   //! Action
   Bool_t          Action();

   //! Find point
   Bool_t          FindPoints();

   //! Create histo
   void            CreateHistogram();

private:
   TAGdataDsc*     fpNtuCluster;		    ///< input data dsc
   TAGdataDsc*     fpNtuPoint;	 	    ///< output data dsc
   TAGparaDsc*     fpGeoMap;		       ///< geometry para dsc

   TH2F*           fpHisPointMap[MaxRpPlane];    ///< cluster map per station
   TH1F*           fpHisPointCharge[MaxRpPlane]; ///< cluster charge per station
   TH1F*           fpHisPointChargeTot;          ///< Total cluster charge for all stations

   ClassDef(TAMSDactNtuPoint,0)
};

#endif
