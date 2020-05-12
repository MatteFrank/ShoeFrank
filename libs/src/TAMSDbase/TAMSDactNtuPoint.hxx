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
class TAMSDntuHit;
class TAMSDparGeo;
class TH1F;

class TAMSDactNtuPoint : public TAGaction {

public:
   explicit  TAMSDactNtuPoint(const char* name       = 0,
                             TAGdataDsc* p_nturaw   = 0,
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
   TAGdataDsc*     fpNtuRaw;		 // input data dsc
   TAGdataDsc*     fpNtuPoint;	 	// output data dsc
   TAGparaDsc*     fpGeoMap;		 // geometry para dsc

   TClonesArray*   m_listOfStripsCol;      // list of column strip
   TClonesArray*   m_listOfStripsRow;      // list of row strip 


   TH1F*           fpHisDist;
   TH1F*           fpHisCharge1;
   TH1F*           fpHisCharge2;
   TH1F*           fpHisChargeTot;

   ClassDef(TAMSDactNtuPoint,0)
};

#endif