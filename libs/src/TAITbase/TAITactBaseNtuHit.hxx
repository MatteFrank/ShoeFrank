#ifndef _TAITactBaseNtuHit_HXX
#define _TAITactBaseNtuHit_HXX

#include "TAVTactBaseRaw.hxx"

/*!
 \file TAITactBaseNtuHit.hxx
 \brief   Declaration of TAITactBaseNtuHit.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAITactBaseNtuHit : public TAVTactBaseRaw {
public:
   
   explicit TAITactBaseNtuHit(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactBaseNtuHit();
      
protected:
   // Find vertex data
   Bool_t DecodeEvent();
      
   // Get the starting point of each event
   Bool_t GetItrHeader();
   
   // Get the starting point of each frame
   Bool_t GetSensorHeader(Int_t iSensor);
   
   // Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, MI26_FrameRaw* data);
   
   ClassDef(TAITactBaseNtuHit,0)
};

#endif
