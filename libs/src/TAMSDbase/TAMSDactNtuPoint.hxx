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

   TClonesArray*   m_listOfStripsCol;      // list of column strip
   TClonesArray*   m_listOfStripsRow;      // list of row strip 

   TH2F*           fpSens[3];

   ClassDef(TAMSDactNtuPoint,0)
};

#endif
