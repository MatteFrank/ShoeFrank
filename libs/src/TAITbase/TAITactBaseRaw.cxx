/*!
 \file
 \version $Id: TAITactBaseRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
 \brief   Implementation of TAITactBaseRaw.
 */

#include "DECardEvent.hh"

#include "TH2F.h"

#include "GlobalPar.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"

#include "TAITntuRaw.hxx"
#include "TAITactBaseRaw.hxx"
#include "TAVTmi26Type.hxx"

/*!
 \class TAITactBaseRaw TAITactBaseRaw.hxx "TAITactBaseRaw.hxx"
 \brief Base class to get vertex raw data. **
 */

ClassImp(TAITactBaseRaw);

const UInt_t TAITactBaseRaw::fgkKeyHeader[]      = {0x80008000, 0x80018001, 0x80028002, 0x80038003};
const Int_t  TAITactBaseRaw::fgkFrameHeaderSize  = 5;
const Int_t  TAITactBaseRaw::fgkLineWidth        = 9;
const UInt_t TAITactBaseRaw::fgkFrameHeader      = 0x80088007;
const UInt_t TAITactBaseRaw::fgkFrameTail        = 0xaaa8aaa7;
const UInt_t TAITactBaseRaw::fgkKeyTail[]        = {0x8bb08bb0, 0x8bb18bb1, 0x8bb28bb2, 0x8bb38bb3};

//------------------------------------------+-----------------------------------
//! Default constructor.

TAITactBaseRaw::TAITactBaseRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAGactionFile(name, "TAITactBaseRaw - Base action for unpack vertex raw data"),
  fpNtuRaw(pNtuRaw),
  fpGeoMap(pGeoMap),
  fpConfig(pConfig),
  fpParMap(pParMap),
  fData(0x0),
  fEventNumber(0),
  fPrevEventNumber(0),
  fTriggerNumber(0),
  fPrevTriggerNumber(0),
  fTimeStamp(0),
  fPrevTimeStamp(0),
  fFrameCount(0),
  fTriggerNumberFrame(0),
  fTimeStampFrame(0),
  fFirstFrame(-1),
  fNSensors(-1),
  fIndex(0),
  fCurrentTriggerCnt(0),
  fEventSize(0),
  fReadingEvent(0),
  fOverflow(0),
  fEventsOverflow(0), 
  fNStatesInLine(0)
{
   AddDataOut(pNtuRaw, "TAITntuRaw");
   AddPara(pGeoMap, "TAITparGeo");
   AddPara(pConfig, "TAITparConf");
   
   TAITparGeo* parGeo = (TAITparGeo*) fpGeoMap->Object();
   fNSensors = parGeo->GetSensorsN();
   
   Int_t size = parGeo->GetSensorsN()*sizeof(MI26_FrameRaw)*4;
   fData.resize(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactBaseRaw::~TAITactBaseRaw()
{
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAITactBaseRaw::CreateHistogram()
{
   DeleteHistogram();
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	  if (TAITparConf::IsMapHistOn()) {
		 fpHisPixelMap[i] = new TH2F(Form("itPixelMap%d", i+1), Form("Vertex - pixel map for sensor %d", i+1), 
									 pGeoMap->GetPixelsNy(), 0, pGeoMap->GetPixelsNy(), 
									 pGeoMap->GetPixelsNx(), 0, pGeoMap->GetPixelsNx());
		 fpHisPixelMap[i]->SetStats(kFALSE);
		 AddHistogram(fpHisPixelMap[i]);
	  }
      
	  fpHisRateMap[i] = new TH1F(Form("itRateMap%d", i+1), Form("Vertex - rate per line for sensor %d", i+1), 
								 pGeoMap->GetPixelsNx(), 0, pGeoMap->GetPixelsNx());
	  AddHistogram(fpHisRateMap[i]);

	  fpHisRateMapQ[i] = new TH1F(Form("itRateMapQ%d", i+1), Form("Vertex - rate per quadrant for sensor %d", i+1), 10, 0, 5);
	  AddHistogram(fpHisRateMapQ[i]);
	  
	  fpHisEvtLength[i] = new TH1F(Form("itEvtLength%d", i+1), Form("Vertex - event length sensor %d", i+1), 1000, 0, 1000);
	  AddHistogram(fpHisEvtLength[i]);
      
     fpHisTriggerEvt[i] = new TH1F(Form("itTriggerEvt%d", i+1), Form("Vertex - Trigger difference in event sensor %d", i+1),  20, -9.5, 10.5);
     AddHistogram(fpHisTriggerEvt[i]);
      
     fpHisEvtNumber[i] = new TH1F(Form("itNumberEvt%d", i+1), Form("Vertex -  Event number difference per event sensor %d", i+1), 20, -9.5, 10.5);
     AddHistogram(fpHisEvtNumber[i]);
      
     fpHisTimeStampEvt[i] = new TH1F(Form("itTimeStampEvt%d", i+1), Form("Vertex -  Time stamp difference per event sensor %d", i+1), 1000, -200, 200);
     AddHistogram(fpHisTimeStampEvt[i]);
  
      fpHisTriggerFrame[i] = new TH1F(Form("itTriggerFrame%d", i+1), Form("Vertex - Trigger difference in sensor %d", i+1),  20, -9.5, 10.5);
      AddHistogram(fpHisTriggerFrame[i]);
      
      fpHisTimeStampFrame[i] = new TH1F(Form("itTimeStampFrame%d", i+1), Form("Vertex - Time stamp difference in sensor% d", i+1),  1000, -20000, 20000);
      AddHistogram(fpHisTimeStampFrame[i]);
      
      fpHisFrameCnt[i] = new TH1F(Form("itFrameCnt%d", i+1), Form("Vertex - Frame cnt difference in sensor %d", i+1),  510, -9.5, 500.5);
      AddHistogram(fpHisFrameCnt[i]);

   }

   SetValidHistogram(kTRUE);
   return;
}

// --------------------------------------------------------------------------------------
Int_t TAITactBaseRaw::GetSensor(UInt_t key)
{
   TAITparGeo*  pGeoMap = (TAITparGeo*)  fpGeoMap->Object();
   
   key = (key >> 16) & 0xFFFF;
   for (Int_t i = 0; i <  pGeoMap->GetSensorsN(); ++i) {
      if (fgkKeyHeader[i] == key)
         return i;
   }
   return -1;
}

// --------------------------------------------------------------------------------------
void TAITactBaseRaw::FillHistoFrame(Int_t iSensor, MI26_FrameRaw* data)
{
   UInt_t trigger   = data->TriggerCnt;
   UInt_t timeStamp = data->TimeStamp;
   UInt_t frameCnt  = data->FrameCnt;
   
   //printf("%u\n", frameCnt);
   if (fFirstFrame == 0) {
      fTriggerNumberFrame = trigger;
      fFrameCount         = frameCnt;
      fTimeStampFrame     = timeStamp;
      fFirstFrame++;
      
   } else if (fFirstFrame == 2){
      fpHisTriggerFrame[iSensor]->Fill(trigger - fTriggerNumberFrame);
      fpHisTimeStampFrame[iSensor]->Fill(timeStamp - fTimeStampFrame);
      fpHisFrameCnt[iSensor]->Fill(frameCnt - fFrameCount);
   } else
      fFirstFrame++;
}

// --------------------------------------------------------------------------------------
void TAITactBaseRaw::FillHistoEvt(Int_t iSensor)
{
   //if (fEventNumber - fPrevEventNumber > 1) printf("trig %d evt %d diffevt %d\n", fEventNumber, fTriggerNumber, fEventNumber - fPrevEventNumber );
   fpHisEvtNumber[iSensor]->Fill(fEventNumber - fPrevEventNumber);
   fpHisTriggerEvt[iSensor]->Fill(fTriggerNumber - fPrevTriggerNumber);
   fpHisTimeStampEvt[iSensor]->Fill(fTimeStamp - fPrevTimeStamp);
}

// --------------------------------------------------------------------------------------
Bool_t TAITactBaseRaw::DecodeFrame(Int_t iSensor, MI26_FrameRaw *frame)
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
    6) ADataW16[140]; //data MI26
    7) Trailer;
    */
   
   TAITntuRaw*  pNtuRaw = (TAITntuRaw*)  fpNtuRaw->Object();
   TAITparConf* pConfig = (TAITparConf*) fpConfig->Object();
   TAITparGeo*  pGeoPar = (TAITparGeo*)  fpGeoMap->Object();

   Int_t dataLength    = ((frame->DataLength & 0xFFFF0000)>>16);
   if (dataLength > 140) return false;
   
   UShort_t *frameData = (UShort_t*)frame->ADataW16;
   dataLength         *= 2; // go to short
   
   if (ValidHistogram()) {
      fpHisEvtLength[iSensor]->Fill(frame->TriggerCnt % 1000, dataLength/2);
      if (frame->TriggerCnt % 1000 == 0) fpHisEvtLength[iSensor]->Reset();
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
void TAITactBaseRaw::AddPixel( Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn)
{
   // Add a pixel to the vector of pixels
   // require the following info
   // - input = number of the sensors
   // - value = analog value of this pixel
   // - line & column = position of the pixel in the matrix
   
   TAITntuRaw* pNtuRaw = (TAITntuRaw*) fpNtuRaw->Object();
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   
   TAITntuHit* pixel   = (TAITntuHit*)pNtuRaw->NewPixel(iSensor, value, aLine, aColumn);
   
   double v = pGeoMap->GetPositionV(aLine);
   double u = pGeoMap->GetPositionU(aColumn);
   TVector3 pos(u,v,0);
   pixel->SetPosition(pos);

   
   if (ValidHistogram()) {
	  if (TAITparConf::IsMapHistOn()) 
		 fpHisPixelMap[iSensor]->Fill(aLine, aColumn);
	  
	  fpHisRateMap[iSensor]->Fill(aColumn);
	  
	  for (Int_t i = 0; i < 4; ++i) {
		 if (aColumn >= 258*i && aColumn < (i+1)*258)
			fpHisRateMapQ[iSensor]->Fill(i+1);
	  }
   }
}

