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
   static  UInt_t  GetBoardKey(Int_t idx)    { return fgkBoardKey[idx];  }
   //! Get board tail
   static  UInt_t  GetBoardTail(Int_t idx)   { return fgkBoardTail[idx]; }
   
protected:
   Int_t  fBoardTrigger;             ///< Board size

protected:
   // Find vertex data
   Bool_t DecodeEvent();
      
   // Get the starting point of each event
   Bool_t GetItrHeader();
   
   // Get the starting point of each board
   Bool_t GetBoardHeader(Int_t iBoard);

   // Get the starting point of each frame
   Bool_t GetSensorHeader(Int_t iSensor, Int_t datalink);
   
   // Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, Int_t datalink, MI26_FrameRaw* data);
   
protected:
   //! Board key
   static const UInt_t fgkBoardKey[];     ///< Key header array per board
   static const UInt_t fgkBoardTail[];    ///< Key tailer array per board
   
   ClassDef(TAITactBaseNtuHit,0)
};

#endif
