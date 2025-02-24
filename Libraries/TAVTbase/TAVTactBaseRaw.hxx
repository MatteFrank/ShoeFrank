#ifndef _TAVTactBaseRaw_HXX
#define _TAVTactBaseRaw_HXX

#include "TAVTmi26Type.hxx"
#include "TAVTparameters.hxx"

#include "TAGactionFile.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include <vector>


class TAVTntuHit;
class TH2F;
class TH1F;

/*!
 \file TAVTactBaseRaw.hxx
 \brief   Declaration of TAVTactBaseRaw.
 */
/*------------------------------------------+---------------------------------*/

using namespace std;

class TAVTactBaseRaw : public TAGactionFile {
public:
   
   explicit TAVTactBaseRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactBaseRaw();
   
   // Base creation of histogram
   virtual void CreateHistogram();
   
public:
   //! Get sensor header
   static  UInt_t  GetSensorKey(Int_t idx)                  { return fgkSensorKey[idx];   }
   //! Get header size
   static  Int_t   GetHeaderSize()                          { return fgkFrameHeaderSize;  }
   //! Get key tail
   static  UInt_t  GetSensorTail(Int_t idx)                 { return fgkSensorTail[idx];  }
   //! Get line width
   static  Int_t   GetLineWidth()                           { return fgkLineWidth;        }
   //! Get frame header
   static  UInt_t  GetFrameHeader()                         { return fgkFrameHeader;      }
   //! Get frame tail
   static  UInt_t  GetFrameTail()                           { return fgkFrameTail;        }

protected:
      
   TAGdataDsc*       fpNtuRaw;		         ///< output data dsc
   TAGparaDsc*       fpGeoMap;		         ///< geo para dsc
   TAGparaDsc*       fpConfig;		         ///< config para dsc
   TAGparaDsc*       fpParMap;		         ///< map para dsc
   
   vector<UInt_t>    fData;                  ///< data array to fill
   UInt_t*           fDataEvent;             ///< data array to fill the whol eevent
   UInt_t            fDataLink;              ///< data link

   Int_t             fEventNumber;           ///< number of the event
   Int_t             fPrevEventNumber[MaxSens];   ///< previous number of the event
   Int_t             fTriggerNumber;         ///< number of the trigger
   Int_t             fPrevTriggerNumber[MaxSens]; ///< previous number of the trigger
   ULong64_t         fTimeStamp;             ///< time stamp per frame
   Int_t             fPrevTimeStamp[MaxSens];     ///< time stamp per frame
   Int_t             fFrameCount;            ///< number of frame
   Int_t             fTriggerNumberFrame;    ///< number of the trigger
   Int_t             fTimeStampFrame;        ///< time stamp per frame
   Int_t             fFirstFrame;            ///< first frame flag
   Bool_t            fFrameOk;               ///< first frame flag
   
   Int_t             fIndex;                 ///< Data index
   UInt_t            fCurrentTriggerCnt;     ///< Current trigger count
   Int_t             fEventSize;             ///< Event size
   Int_t             fDataSize;              ///< Data size
   Bool_t            fReadingEvent;          ///< Reading flag
   Bool_t            fOverflow;              ///< Overflow flag
   
   Int_t             fEventsOverflow;        ///< Number of event in overflow
   Int_t             fNStatesInLine;         ///< Number of states in line
   
   TString           fPrefix;                ///< prefix of histogram
   TString           fTitleDev;              ///< device name for histogram title
   
   TH2F*             fpHisPixelMap[MaxSens];      ///< pixel map per sensor histogram
   TH1F*             fpHisRateMap[MaxSens];       ///< pixel rate per sensor histogram
   TH1F*             fpHisRateMapQ[MaxSens];      ///< pixel rate per sensor quadrant histogram
   TH1F*             fpHisEvtLength[MaxSens];     ///< event data length for each sensor (all 3 frames) histogram
   TH1F*             fpHisEvtNumber[MaxSens];     ///< event number for each sensor (all 3 frames) histogram
   TH1F*             fpHisTriggerEvt[MaxSens];    ///< trigger number for each sensor (all 3 frames) histogram
   TH1F*             fpHisTimeStampEvt[MaxSens];  ///< time stamp for each sensor (all 3 frames) histogram
   TH1F*             fpHisTriggerFrame[MaxSens];  ///< trigger number for each frame histogram
   TH1F*             fpHisTimeStampFrame[MaxSens];///< time stamp number for each frame histogram
   TH1F*             fpHisFrameCnt[MaxSens];      ///< frame counter histogram
   TH1F*             fpHisFrameErrors[MaxSens];   ///< frame error counter histogram
   TH1F*             fpHisBCOofTrigger;           ///< Bunch crossing trigger histogram
   TH1F*             fpHisSensorHit;              ///< Hits per sensor

protected:
   //! Header key
   static const UInt_t  fgkSensorKey[];      ///< key header array per sensor
   //! Frame size
   static const Int_t   fgkFrameHeaderSize;  ///< Frame header size
   //! Key tail
   static const UInt_t  fgkSensorTail[];     ///< key tailer array per sensor
   //! Line width
   static const Int_t   fgkLineWidth;        ///< Size of a line
   //! Frame header
   static const UInt_t  fgkFrameHeader;      ///< Frame header word
   //! Frame tail
   static const UInt_t  fgkFrameTail;        ///< Frame tailer word
   
protected:
   // Reset frame counters
   virtual void  ResetFrames();
   
   // Add pixel to list
   virtual void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn, Int_t frameNumber = 0);
   
   // Get Sensor number
   virtual  Int_t  GetSensor(UInt_t key);

   // decode frame
   virtual Bool_t DecodeFrame(Int_t iSensor, MI26_FrameRaw *frame = 0x0, Int_t iFrame=0);

   // Fill histogram frame
   virtual void FillHistoFrame(Int_t iSensor, MI26_FrameRaw* data = 0x0);
   
   // Fill histogram frame
   virtual void FillHistoEvt(Int_t iSensor);
   
   // Fill histogram pixel
   virtual void FillHistoPixel(Int_t planeId, Int_t aLine, Int_t aColumn, Float_t value = 1.);
   
   //! Fill DAQ event
   virtual void FillDaqEvent() { return; }

   ClassDef(TAVTactBaseRaw,0)

};

#endif
