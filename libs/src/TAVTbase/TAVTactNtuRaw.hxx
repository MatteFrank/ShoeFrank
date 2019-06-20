#ifndef _TAVTactNtuRaw_HXX
#define _TAVTactNtuRaw_HXX

#include "TAVTactBaseNtuRaw.hxx"

/*!
 \file
 \version $Id: TAVTactNtuRaw.hxx $
 \brief   Declaration of TAVTactNtuRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAVTactNtuRaw : public TAVTactBaseNtuRaw {
public:
   
   explicit TAVTactNtuRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactNtuRaw();
   
   virtual Bool_t  Action();
   
protected:
   TAGdataDsc*     fpDatDaq;		    // input data dsc
   
   
   ClassDef(TAVTactNtuRaw,0)
};

#endif
