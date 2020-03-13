#ifndef _TAVTactBaseNtuRaw_HXX
#define _TAVTactBaseNtuRaw_HXX

#include "TAVTactBaseRaw.hxx"

/*!
 \file
 \version $Id: TAVTactBaseNtuRaw.hxx $
 \brief   Declaration of TAVTactBaseNtuRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAVTactBaseNtuRaw : public TAVTactBaseRaw {
public:
   
   explicit TAVTactBaseNtuRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactBaseNtuRaw();
      
protected:
   //! Find vertex data
   Bool_t DecodeEvent();
      
   //! Get the starting point of each event
   Bool_t GetVtxHeader();
   
   //! Get the starting point of each frame
   Bool_t GetSensorHeader(Int_t iSensor);
   
   //! Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, MI26_FrameRaw* data);
   
};

#endif
