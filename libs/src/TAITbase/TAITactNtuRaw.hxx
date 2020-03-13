#ifndef _TAITactNtuRaw_HXX
#define _TAITactNtuRaw_HXX

#include "TAITactBaseRaw.hxx"

/*!
 \file
 \version $Id: TAITactNtuRaw.hxx $
 \brief   Declaration of TAITactNtuRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAITactNtuRaw : public TAITactBaseRaw {
public:
   
   explicit TAITactNtuRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactNtuRaw();
   
   virtual Bool_t  Action();
   
private:
   TAGdataDsc*     fpDatDaq;		    // input data dsc
   
private:   
   //! Find vertex data
   Bool_t DecodeEvent(const DECardEvent* evt);
      
   //! Get the starting point of each event
   Bool_t GetVtxHeader();
   
   //! Get the starting point of each frame
   Bool_t GetSensorHeader(Int_t iSensor);
   
   //! Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, MI26_FrameRaw* data);
   
};

#endif
