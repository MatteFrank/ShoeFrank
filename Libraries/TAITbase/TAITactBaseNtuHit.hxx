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
 
public:
   //! Get board header
   static  UInt_t  GetDataLinkKey(Int_t idx)    { return fgkDataLinkKey[idx];  }
   //! Get board tail
   static  UInt_t  GetDataLinkTail(Int_t idx)   { return fgkDataLinkTail[idx]; }
   
protected:
   Int_t  fDataLinkTrigger;             ///< DataLink size

protected:
   // Find vertex data
   Bool_t DecodeEvent();
      
   // Get the starting point of each event
   Bool_t GetItrHeader();
   
   // Get the starting point of each board
   Int_t GetDataLinkHeader();

   // Get the starting point of each frame
   Bool_t GetSensorHeader( Int_t iSensor,Int_t datalink);
   
   // Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, Int_t datalink, MI26_FrameRaw* data);
   
   // Add pixel to list
   void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn, Int_t frameNumber = 0);
   
protected:
   //! DataLink key
   static const UInt_t fgkDataLinkKey[];     ///< Key header array per data link
   static const UInt_t fgkDataLinkTail[];    ///< Key tailer array per data link
   
   ClassDef(TAITactBaseNtuHit,0)
};

#endif
