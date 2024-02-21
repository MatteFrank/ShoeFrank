#ifndef _TANAactNtuMass_HXX
#define _TANAactNtuMass_HXX
/*!
 \file TANAactNtuMass.hxx
 \brief   Declaration of TANAactNtuMass.
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TANAactBaseNtu.hxx"

class TANAactNtuMass : public TANAactBaseNtu {
   
public:
   explicit  TANAactNtuMass(const char* name         = 0,
                            TAGdataDsc* p_irntutrack = 0,
                            TAGparaDsc* p_geomap     = 0);

   virtual ~TANAactNtuMass();
   
   // Create histograms
   virtual  void    CreateHistogram();
   
   // Action
   virtual  Bool_t  Action();

   
   ClassDef(TANAactNtuMass, 0)

};

#endif
