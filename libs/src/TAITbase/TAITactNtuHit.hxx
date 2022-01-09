#ifndef _TAITactNtuHit_HXX
#define _TAITactNtuHit_HXX

/*!
 \file TAITactNtuHit.hxx
 \brief   Declaration of TAITactNtuHit.
 */

/*------------------------------------------+---------------------------------*/

#include "TAITactBaseRaw.hxx"

class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAITactNtuHit : public TAITactBaseRaw {
public:
   
   explicit TAITactNtuHit(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactNtuHit();
   
   virtual Bool_t  Action();
   
private:
   TAGdataDsc*     fpDatDaq;		    /// input data dsc
   
private:   
   //! Find vertex data
   Bool_t DecodeEvent();
      
   //! Get the starting point of each event
   Bool_t GetVtxHeader();
   
   //! Get the starting point of each frame
   Bool_t GetSensorHeader(Int_t iSensor);
   
   //! Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, MI26_FrameRaw* data);
   
   ClassDef(TAITactNtuHit,0)
};

#endif
