/*!
 \file TAVTactBaseRaw.cxx
 \brief    Base class to decode vertex raw data
 */

#include "DECardEvent.hh"

#include "TH2F.h"

#include "TAGrecoManager.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparMap.hxx"

#include "TAVTntuHit.hxx"
#include "TAVTactBaseRaw.hxx"
#include "TAVTmi26Type.hxx"

/*!
 \class TAVTactBaseRaw
 \brief Base class to decode vertex raw data
 */

ClassImp(TAVTactBaseRaw);

const UInt_t TAVTactBaseRaw::fgkKeyHeader[]     = {0x80008000, 0x80018001, 0x80028002, 0x80038003};
const Int_t  TAVTactBaseRaw::fgkFrameHeaderSize =  6;
const Int_t  TAVTactBaseRaw::fgkLineWidth       =  9;
const UInt_t TAVTactBaseRaw::fgkFrameHeader     =  0x80088007;
const UInt_t TAVTactBaseRaw::fgkFrameTail       =  0xaaa8aaa7;
const UInt_t TAVTactBaseRaw::fgkKeyTail[]       = {0x8bb08bb0, 0x8bb18bb1, 0x8bb28bb2, 0x8bb38bb3};

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw input hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAVTactBaseRaw::TAVTactBaseRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAGactionFile(name, "TAVTactBaseRaw - Base action for unpack vertex raw data"),
  fpNtuRaw(pNtuRaw),
  fpGeoMap(pGeoMap),
  fpConfig(pConfig),
  fpParMap(pParMap),
  fData(0x0),
  fDataEvent(0x0),
  fDataLink(0),
  fEventNumber(0),
  fTriggerNumber(0),
  fTimeStamp(0),
  fFrameCount(0),
  fTriggerNumberFrame(0),
  fTimeStampFrame(0),
  fFirstFrame(-1),
  fFrameOk(true),
  fNSensors(-1),
  fIndex(0),
  fCurrentTriggerCnt(0),
  fEventSize(0),
  fDataSize(0),
  fReadingEvent(0),
  fOverflow(0),
  fEventsOverflow(0), 
  fNStatesInLine(0),
  fPrefix("vt"),
  fTitleDev("Vertex")
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseRaw::~TAVTactBaseRaw()
{
   if (fDataEvent)
      delete [] fDataEvent;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseRaw::CreateHistogram()
{
   DeleteHistogram();
   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisPixelMap[i] = new TH2F(Form("%sPixelMap%d", fPrefix.Data(), i+1), Form("%s - pixel map for sensor %d", fTitleDev.Data(), i+1),
                                  pGeoMap->GetPixelsNy(), 0, pGeoMap->GetPixelsNy(),
                                  pGeoMap->GetPixelsNx(), 0, pGeoMap->GetPixelsNx());
      fpHisPixelMap[i]->SetStats(kFALSE);
      AddHistogram(fpHisPixelMap[i]);
   
      
      fpHisRateMap[i] = new TH1F(Form("%sRateMap%d", fPrefix.Data(), i+1), Form("%s - rate per line for sensor %d", fTitleDev.Data(), i+1),
                                 pGeoMap->GetPixelsNx(), 0, pGeoMap->GetPixelsNx());
      AddHistogram(fpHisRateMap[i]);
      
      fpHisRateMapQ[i] = new TH1F(Form("%sRateMapQ%d", fPrefix.Data(), i+1), Form("%s - rate per quadrant for sensor %d", fTitleDev.Data(), i+1), 10, 0, 5);
      AddHistogram(fpHisRateMapQ[i]);
	  
      fpHisEvtLength[i] = new TH1F(Form("%sEvtLength%d", fPrefix.Data(), i+1), Form("%s - event length sensor %d", fTitleDev.Data(), i+1), 1000, 0, 10000);
      AddHistogram(fpHisEvtLength[i]);
      
      fpHisTriggerEvt[i] = new TH1F(Form("%sTriggerEvt%d", fPrefix.Data(), i+1), Form("%s - Trigger difference in event sensor %d vs sensor 1",
                                                                                      fTitleDev.Data(), i+1),  40, -19.5, 20.5);
      AddHistogram(fpHisTriggerEvt[i]);
      
      fpHisEvtNumber[i] = new TH1F(Form("%sNumberEvt%d", fPrefix.Data(), i+1), Form("%s -  Event number difference per event sensor %d", fTitleDev.Data(),
                                                                                    i+1), 40, -19.5, 20.5);
      AddHistogram(fpHisEvtNumber[i]);
      
      fpHisTimeStampEvt[i] = new TH1F(Form("%sTimeStampEvt%d", fPrefix.Data(), i+1), Form("%s -  Time stamp difference per event sensor %d", fTitleDev.Data(),
                                                                                          i+1), 1000, -200, 200);
      AddHistogram(fpHisTimeStampEvt[i]);
  
      fpHisTriggerFrame[i] = new TH1F(Form("%sTriggerFrame%d", fPrefix.Data(), i+1), Form("%s - Trigger difference in sensor %d", fTitleDev.Data(), i+1),  20,
                                      -9.5, 10.5);
      AddHistogram(fpHisTriggerFrame[i]);
      
      fpHisTimeStampFrame[i] = new TH1F(Form("%sTimeStampFrame%d", fPrefix.Data(), i+1), Form("%s - Time stamp difference in sensor% d", fTitleDev.Data(),
                                                                                              i+1),  1000, -20000, 20000);
      AddHistogram(fpHisTimeStampFrame[i]);
      
      fpHisFrameCnt[i] = new TH1F(Form("%sFrameCnt%d", fPrefix.Data(), i+1), Form("%s - Frame cnt difference in sensor %d", fTitleDev.Data(), i+1),  510,
                                  -9.5, 500.5);
      AddHistogram(fpHisFrameCnt[i]);
   
      fpHisFrameErrors[i] = new TH1F(Form("%sErrorsFrame%d", fPrefix.Data(), i+1), Form("%s - Frame errors in sensor %d", fTitleDev.Data(), i+1), 4, 0.5, 4.5);
      AddHistogram(fpHisFrameErrors[i]);
   }
   
   fpHisBCOofTrigger = new TH1F(Form("%sBCOofTrigger", fPrefix.Data()), Form("%s - BCOofTrigger difference", fTitleDev.Data()), 200000, 0, 200000);
   AddHistogram(fpHisBCOofTrigger);

   SetValidHistogram(kTRUE);
   return;
}

// --------------------------------------------------------------------------------------
//! Get sensor id for a given key
//!
//! \param[in] key a given key
Int_t TAVTactBaseRaw::GetSensor(UInt_t key)
{
   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*) fpGeoMap->Object();

   key = (key >> 16) & 0xFFFF;
   for (Int_t i = 0; i <  pGeoMap->GetSensPerDataLink(); ++i) {
      if (fgkKeyHeader[i] == key)
         return i;
   }
   return -1;
}

// --------------------------------------------------------------------------------------
//! Reset frame info
void TAVTactBaseRaw::ResetFrames()
{
   fFirstFrame = 0;
   fFrameOk = true;
}

// --------------------------------------------------------------------------------------
//! Fill histograms related to frame
//!
//! \param[in] iSensor sensor index
//! \param[in] data Mimosa sensor data structure
void TAVTactBaseRaw::FillHistoFrame(Int_t iSensor, MI26_FrameRaw* data)
{
   Bool_t ok = true;
   UInt_t trigger   = data->TriggerCnt;
   UInt_t timeStamp = data->TimeStamp;
   UInt_t frameCnt  = data->FrameCnt;
   
   if (fFirstFrame == 0) {
      fTriggerNumberFrame = trigger;
      fFrameCount         = frameCnt;
      fTimeStampFrame     = timeStamp;
      fFirstFrame++;
      
   } else if (fFirstFrame == 1){
      fpHisTriggerFrame[iSensor]->Fill(trigger - fTriggerNumberFrame);
      fpHisTimeStampFrame[iSensor]->Fill(timeStamp - fTimeStampFrame);
      fpHisFrameCnt[iSensor]->Fill(frameCnt - fFrameCount);
   } else
      fFirstFrame++;
   
   if (fFirstFrame == 1) {
   if( ((frameCnt - fFrameCount) == 0) || ((frameCnt - fFrameCount) == 1) || ((frameCnt - fFrameCount) == 2))
      ok = true;
   else
      ok = false;
   }
   
   fFrameOk *= ok;
   
 //  printf("frame %d framePrev %d ok %d\n",  frameCnt, fFrameCount, fFrameOk);

}

// --------------------------------------------------------------------------------------
//!
//! Fill histogram event related
//!
//! \param[in] iSensor sensor index
void TAVTactBaseRaw::FillHistoEvt(Int_t iSensor)
{
   //if (fEventNumber - fPrevEventNumber > 1)printf("trig %d  prev %d\n", fTriggerNumber, fPrevTriggerNumber);
   fpHisEvtNumber[iSensor]->Fill(fEventNumber - fPrevEventNumber[iSensor]);
   fpHisTriggerEvt[iSensor]->Fill(fTriggerNumber - fPrevTriggerNumber[iSensor]);
   if (fTriggerNumber - fPrevTriggerNumber[iSensor] != 1)
      fpHisFrameErrors[iSensor]->Fill(4);
   fpHisTimeStampEvt[iSensor]->Fill(fTimeStamp - fPrevTimeStamp[iSensor]);
}

// --------------------------------------------------------------------------------------
//! Fill histogram related to pixel
//!
//! \param[in] planeId sensor index
//! \param[in] aLine line id
//! \param[in] aColumn column id
void TAVTactBaseRaw::FillHistoPixel(Int_t planeId, Int_t aLine, Int_t aColumn)
{
   fpHisPixelMap[planeId]->Fill(aLine, aColumn);
   
   fpHisRateMap[planeId]->Fill(aColumn);
   
   for (Int_t i = 0; i < 4; ++i) {
      if (aColumn >= 258*i && aColumn < (i+1)*258)
         fpHisRateMapQ[planeId]->Fill(i+1);
   }
}

// --------------------------------------------------------------------------------------
//! Decode Frame
//!
//! \param[in] iSensor sensor index
//! \param[in] frame Mimosa sensor data structure
Bool_t TAVTactBaseRaw::DecodeFrame(Int_t iSensor, MI26_FrameRaw* frame)
{
   // Read the information of a frame for a given sensor
   // We use extensively the structure definined by Gille Clauss
   /*
    Events Words:
    1) Header;
    2) TriggerCnt;
    3) TimeStamp;
    4) FrameCnt;
    5) DataLength;
    6) ADataW16[1140]; //data MI26
    7) Trailer;
    */
   
   Int_t dataLength    = ((frame->DataLength & 0xFFFF0000)>>16);
   if (ValidHistogram()) {
   if (dataLength != fDataSize)
      fpHisFrameErrors[iSensor]->Fill(3);
   }
   
   if (dataLength > MI26__FFRAME_RAW_MAX_W16) return false;
   
   Int_t dataSize = fDataSize*2;
   
   
   UShort_t *frameData = (UShort_t*)frame->ADataW16;
   dataLength         *= 2; // go to short
   
   if (ValidHistogram()) {
      fpHisEvtLength[iSensor]->Fill(frame->TriggerCnt % 10000, fDataSize);
      if (frame->TriggerCnt % 10000 == 0) fpHisEvtLength[iSensor]->Reset();
   }
   
   if (iSensor == -1) {
      Warning("DecodeFrame()", "Wrong header key %x\n", frame->Header);
      return false;
   }
   
   // -+-+- Pointers AND LOOP to usefull data, i.e. line and states
   MI26__TStatesLine* lineStatus;
   MI26__TState*      state;
   
   Int_t index = 0;
   while( index < dataLength) { // Loop over usefull data
      

      if (index >= dataSize) return false;

      // first 16 bits word is the Status/Line
      lineStatus = (MI26__TStatesLine*)frameData;
      
      if(FootDebugLevel(3))
         printf("frame %x %x #state %d Line add %d ovf %d\n", frameData[0], frameData[1], lineStatus->F.StateNb,
                lineStatus->F.LineAddr, lineStatus->F.Ovf);
      
      frameData += 1; // goto next word
      index += 2;
      
      if( lineStatus->F.Ovf > 0 ) { // Stop when overflow
         if(fEventsOverflow%1000==0 && fReadingEvent)
            printf("WARNING : overflow while reading %d at frame %d and sensor %d, total overflow number is %d\n",
                   fReadingEvent, frame->FrameCnt, iSensor, fEventsOverflow);
         fOverflow = true;
      }
      
      if(FootDebugLevel(3))
         printf("  line %d, #states %d, overflow %d, reading event  %d\n",
                lineStatus->F.LineAddr, lineStatus->F.StateNb, lineStatus->F.Ovf, fReadingEvent);
      
      fNStatesInLine = 0;
      // Next words are the states if any
      for( Int_t iState = 0; iState < lineStatus->F.StateNb; ++iState ) { // loop over states found on sensor
         state = (MI26__TState*)frameData;
         frameData += 1; // goto next word
         index++;
         if(FootDebugLevel(3))
            printf("                  number of states %d, number of hits %d,\n", lineStatus->F.StateNb, state->F.HitNb+1);
         
         fNStatesInLine++;
         
         // A state contains HitNb+1 pixels
         // the first pixel being on the left at the column ColAddr
         for( Int_t iPixel=0; iPixel < state->F.HitNb+1; iPixel++) { // loop on pixels in the state
            
            if(FootDebugLevel(3))
               printf("   line %3d, col %3d\n", lineStatus->F.LineAddr, state->F.ColAddr+iPixel);
            
            // create a new pixel only if we are reading an event
            // and if the line is in the proper limit
            if (!lineStatus->F.Ovf) {
               AddPixel(iSensor, 1, lineStatus->F.LineAddr, state->F.ColAddr+iPixel);
               if(FootDebugLevel(3))
                  printf("sensor %d, line %d, col %d\n", iSensor, lineStatus->F.LineAddr, state->F.ColAddr+iPixel);
            }
         }
         
         if(FootDebugLevel(3))
            printf("                  state %d, #pixels %d, column %d at mem.pos %ld\n",
                   iState, state->F.HitNb+1, state->F.ColAddr, (long int)state);
      } // end loop over states
      
   } // end loop over usefull data
   
   return true;
}

// --------------------------------------------------------------------------------------
//! Add pixel to container
//!
//! \param[in] iSensor sensor index
//! \param[in] value pixel value
//! \param[in] aLine line id
//! \param[in] aColumn column id
void TAVTactBaseRaw::AddPixel( Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn)
{
   // Add a pixel to the vector of pixels
   // require the following info
   // - input = number of the sensors
   // - value = analog value of this pixel
   // - line & column = position of the pixel in the matrix
   
   TAVTntuHit*  pNtuRaw = (TAVTntuHit*)  fpNtuRaw->Object();
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   TAVTparMap*  pParMap = (TAVTparMap*)  fpParMap->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Int_t planeId = pParMap->GetPlaneId(iSensor);
   
   std::pair<int, int> pair(aLine, aColumn);
   if (pConfig->GetSensorPar(planeId).DeadPixelMap[pair] == 1) return;
   
   TAVThit* pixel   = (TAVThit*)pNtuRaw->NewPixel(planeId, value, aLine, aColumn);
   
   double v = pGeoMap->GetPositionV(aLine);
   double u = pGeoMap->GetPositionU(aColumn);
   TVector3 pos(u,v,0);
   pixel->SetPosition(pos);
   pixel->SetValidFrames(fFrameOk);
   
   if (ValidHistogram())
    FillHistoPixel(planeId, aLine, aColumn);
}

