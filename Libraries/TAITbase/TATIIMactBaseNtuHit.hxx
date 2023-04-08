#ifndef _TATIIMactBaseNtuHit_HXX
#define _TATIIMactBaseNtuHit_HXX

#include "TAVTactBaseRaw.hxx"

/*!
 \file TATIIMactBaseNtuHit.hxx
 \brief   Declaration of TATIIMactBaseNtuHit.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TATIIMactBaseNtuHit : public TAVTactBaseRaw {
public:
   
   explicit TATIIMactBaseNtuHit(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TATIIMactBaseNtuHit();
 
public:
   //! Get board header
   static  UInt_t  GetBoardKey(Int_t idx)    { return fgkBoardKey[idx];  }
   //! Get board tail
   static  UInt_t  GetBoardTail(Int_t idx)   { return fgkBoardTail[idx]; }
   //! Get frame header
   static  UInt_t  GetEventKey()             { return fgkEventKey;       }
   //! Get frame tail
   static  UInt_t  GetEventTail()            { return fgkEventTail;      }

protected:
   Int_t  fBoardTrigger;             ///< Board size

protected:
   // Find vertex data
   Bool_t DecodeEvent();
      
   // Get the starting point of each event
   Bool_t GetEvtHeader();
   
   // Get the starting point of each board
   Bool_t GetBoardHeader(Int_t iBoard);

   // Get the starting point of each frame
   Bool_t GetSensorHeader(Int_t iSensor, Int_t datalink);
   
   // Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, Int_t datalink, MI26_FrameRaw* data = 0x0);
   
   // Add pixel to list
   void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn);
   
protected:
   //! Board key
   static const UInt_t fgkBoardKey[];     ///< Key header array per board
   static const UInt_t fgkBoardTail[];    ///< Key tailer array per board
   static const UInt_t fgkEventKey;       ///< Event header word
   static const UInt_t fgkEventTail;      ///< Event tailer word

   ClassDef(TATIIMactBaseNtuHit,0)
};

#endif
