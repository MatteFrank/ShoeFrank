#ifndef _TAVTactBaseRaw_HXX
#define _TAVTactBaseRaw_HXX

#include "TAVTmi26Type.hxx"

#include "TAGactionFile.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include <vector>


class TAVTntuHit;
class TH2F;
class TH1F;

/*!
 \file
 \version $Id: TAVTactBaseRaw.hxx $
 \brief   Declaration of TAVTactBaseRaw.
 */
/*------------------------------------------+---------------------------------*/

using namespace std;

class TAVTactBaseRaw : public TAGactionFile {
public:
   
   explicit TAVTactBaseRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactBaseRaw();
   
   //! Get number of sensors
   Int_t GetSensorsN() const { return fNSensors; }
   
   //! Base creation of histogram
   virtual void CreateHistogram();
   
public:
   static  UInt_t  GetKeyHeader(Int_t idx)                  { return fgkKeyHeader[idx];   }
   static  Int_t   GetHeaderSize()                          { return fgkFrameHeaderSize;  }
   static  UInt_t  GetKeyTail(Int_t idx)                    { return fgkKeyTail[idx];     }
   static  Int_t   GetLineWidth()                           { return fgkLineWidth;        }

   static  UInt_t  GetFrameHeader()                         { return fgkFrameHeader;      }
   static  UInt_t  GetFrameTail()                           { return fgkFrameTail;        }

protected:
      
   TAGdataDsc*       fpNtuRaw;		     // output data dsc
   TAGparaDsc*       fpGeoMap;		     // geo para dsc
   TAGparaDsc*       fpConfig;		     // config para dsc
   TAGparaDsc*       fpParMap;		     // map para dsc
   
   vector<UInt_t>    fData;              // data array to fill
   UInt_t*           fDataEvent;         // data array to fill the whol eevent
   UInt_t            fDataLink;          // data link

   Int_t             fEventNumber;        // number of the event
   Int_t             fPrevEventNumber[32];    // previous number of the event
   Int_t             fTriggerNumber;      // number of the trigger
   Int_t             fPrevTriggerNumber[32];  // previous number of the trigger
   Int_t             fTimeStamp;          // time stamp per frame
   Int_t             fPrevTimeStamp[32];      // time stamp per frame
   Int_t             fFrameCount;         // number of frame
   Int_t             fTriggerNumberFrame; // number of the trigger
   Int_t             fTimeStampFrame;     // time stamp per frame
   Int_t             fFirstFrame;         // first frame flag
   Bool_t            fFrameOk;         // first frame flag
   

   Int_t             fNSensors;
   Int_t             fIndex;
   UInt_t            fCurrentTriggerCnt;
   Int_t             fEventSize;
   Int_t             fDataSize;
   Bool_t            fReadingEvent;
   Bool_t            fOverflow;
   
   Int_t             fEventsOverflow; 
   Int_t             fNStatesInLine; 
   
   TString           fPrefix;            // prefix of histogram
   TString           fTitleDev;          // device name for histogram title
   
   TH2F*             fpHisPixelMap[32];  // pixel map per sensor
   TH1F*             fpHisRateMap[32];   // pixel map per sensor
   TH1F*             fpHisRateMapQ[32];  // pixel map per sensor quadrant
   TH1F*             fpHisEvtLength[32]; // event data length for each sensor (all 3 frames)
   TH1F*             fpHisEvtNumber[32]; //
   TH1F*             fpHisTriggerEvt[32]; //
   TH1F*             fpHisTimeStampEvt[32]; //
   TH1F*             fpHisTriggerFrame[32];
   TH1F*             fpHisTimeStampFrame[32];
   TH1F*             fpHisFrameCnt[32];
   TH1F*             fpHisFrameErrors[32];
   
protected:
   static const UInt_t  fgkKeyHeader[];
   static const Int_t   fgkFrameHeaderSize;
   static const UInt_t  fgkKeyTail[];
   static const Int_t   fgkLineWidth;
   static const UInt_t  fgkFrameHeader;
   static const UInt_t  fgkFrameTail;
   
protected:
   
   // Reset frame counters
   virtual void  ResetFrames();
   
   //! Add pixel to list
   virtual void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn);
   
   //! Get Sensor number
   virtual  Int_t  GetSensor(UInt_t key);

   //! decode frame
   virtual Bool_t DecodeFrame(Int_t iSensor, MI26_FrameRaw *frame);

   //! Fill histogram frame
   virtual void FillHistoFrame(Int_t iSensor, MI26_FrameRaw* data);
   
   //! Fill histogram frame
   virtual void FillHistoEvt(Int_t iSensor);
   
   //! Fill histogram pixel
   virtual void FillHistoPixel(Int_t planeId, Int_t aLine, Int_t aColumn);
   
   //! Fill DAQ event
   virtual void FillDaqEvent() { return; }

   ClassDef(TAVTactBaseRaw,0)

};

#endif
