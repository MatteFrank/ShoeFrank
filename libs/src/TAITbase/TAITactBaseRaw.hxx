#ifndef _TAITactBaseRaw_HXX
#define _TAITactBaseRaw_HXX

#include "TAVTmi26Type.hxx"

#include "TAGactionFile.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include <vector>


class TAITntuRaw;
class TH2F;
class TH1F;

/*!
 \file
 \version $Id: TAITactBaseRaw.hxx $
 \brief   Declaration of TAITactBaseRaw.
 */
/*------------------------------------------+---------------------------------*/

using namespace std;

class TAITactBaseRaw : public TAGactionFile {
public:
   
   explicit TAITactBaseRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactBaseRaw();
   
   //! Get number of sensors
   Int_t GetSensorsN() const { return fNSensors; }
   
   //! Base creation of histogram
   void CreateHistogram();
   
public:
   static  UInt_t  GetKeyHeader(Int_t idx)                  { return fgkKeyHeader[idx];   }
   static  Int_t   GetHeaderSize()                          { return fgkFrameHeaderSize;  }
   static  UInt_t  GetKeyTail(Int_t idx)                    { return fgkKeyTail[idx];     }
   static  Int_t   GetLineWidth()                           { return fgkLineWidth;        }

   static  UInt_t  GetFrameHeader()                         { return fgkFrameHeader;      }
   static  UInt_t  GetFrameTail()                           { return fgkFrameTail;        }

protected:
      
   TAGdataDsc*       fpNtuRaw;		    // output data dsc
   TAGparaDsc*       fpGeoMap;		    // geo para dsc
   TAGparaDsc*       fpConfig;		    // config para dsc
   TAGparaDsc*       fpParMap;		    // map para dsc
   
   vector<UInt_t>    fData;             // data array to fill

   Int_t             fEventNumber;      // number of the event
   Int_t             fPrevEventNumber;      // previous number of the event
   Int_t             fTriggerNumber;    // number of the trigger
   Int_t             fPrevTriggerNumber;    // previous number of the trigger
   Int_t             fTimeStamp;        // time stamp per frame
   Int_t             fPrevTimeStamp;        // time stamp per frame
   Int_t             fFrameCount;       // number of frame
   Int_t             fTriggerNumberFrame;    // number of the trigger
   Int_t             fTimeStampFrame;        // time stamp per frame
   Int_t             fFirstFrame;

   Int_t             fNSensors;
   Int_t             fIndex;
   UInt_t            fCurrentTriggerCnt;
   Int_t             fEventSize;
   Bool_t            fReadingEvent;
   Bool_t            fOverflow;
   
   Int_t             fEventsOverflow; 
   Int_t             fNStatesInLine; 
      
   TH2F*             fpHisPixelMap[8];  // pixel map per sensor
   TH1F*             fpHisRateMap[8];   // pixel map per sensor
   TH1F*             fpHisRateMapQ[8];  // pixel map per sensor quadrant
   TH1F*             fpHisEvtLength[8]; // event data length for each sensor (all 3 frames)
   TH1F*             fpHisEvtNumber[8]; //
   TH1F*             fpHisTriggerEvt[8]; //
   TH1F*             fpHisTimeStampEvt[8]; //
   TH1F*             fpHisTriggerFrame[8];
   TH1F*             fpHisTimeStampFrame[8];
   TH1F*             fpHisFrameCnt[8];
   
protected:
   static const UInt_t  fgkKeyHeader[];
   static const Int_t   fgkFrameHeaderSize;
   static const UInt_t  fgkKeyTail[];
   static const Int_t   fgkLineWidth;
   static const UInt_t  fgkFrameHeader;
   static const UInt_t  fgkFrameTail;
   
protected:
   
   //! Add pixel to list
   void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn);
   
   //! Get Sensor number
   Int_t  GetSensor(UInt_t key);

   //! decode frame
   Bool_t DecodeFrame(Int_t iSensor, MI26_FrameRaw *frame);

   //! Fill histogram frame
   void FillHistoFrame(Int_t iSensor, MI26_FrameRaw* data);
   
   //! Fill histogram frame
   void FillHistoEvt(Int_t iSensor);

   ClassDef(TAITactBaseRaw,0)

};

#endif
