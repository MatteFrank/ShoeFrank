#ifndef _TANEactNtuHit_HXX
#define _TANEactNtuHit_HXX
/*!
  \file TANEactNtuHit.hxx
  \brief   Declaration of TANEactNtuHit.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TANEparGeo.hxx"
#include "TANEntuRaw.hxx"
#include "TANEntuHit.hxx"

class TANEactNtuHit : public TAGaction {

public:

  explicit  TANEactNtuHit(const char* name=0,
                                TAGdataDsc* p_datraw=0,
                                TAGdataDsc* p_datdaq=0,
                                TAGparaDsc* p_parmap=0);
   virtual  ~TANEactNtuHit();

   Bool_t   Action();

   void     CreateHistogram();
 
private:
   TAGdataDsc*     fpDatRaw;		    ///< input raw data
   TAGdataDsc*     fpNtuRaw;         ///< output calibrated data
   TAGparaDsc*     fpParMap;         ///< mapping pointer

   TH1F*           fhArrivalTime[4];  ///< histograms for arrival time
   TH1F*           fhCharge[4];       ///< histograms for charge
   TH1F*           fhTrigTime;        ///< histogram for trigger time
   TH1F*           fhTotCharge;       ///< histogram for total charge
   TH1F*           fhEventTime;       ///< histogram for event type
   TH1F*           fhChannelMap;      ///< histogram for channel map
  
   ClassDef(TANEactNtuHit, 0)
};

#endif
