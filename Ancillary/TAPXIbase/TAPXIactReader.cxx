/*!
 \file
 \brief   Implementation of TAPXIactReader.
 */

#include "TH2F.h"

#include <vector>
#include <algorithm>

#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAPXIactReader.hxx"

/*!
 \class TAPXIactReader
 \brief Reader action for PXI format. **
 */

TString TAPXIactReader::fgkPrefixFileName               = "RUN_";
const Int_t TAPXIactReader::fgkMaxNofMappedIdx              = 10;

ClassImp(TAPXIactReader);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAPXIactReader::TAPXIactReader(const char* name, TAGdataDsc* pDatRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig,
                               TAGdataDsc* pDatRawBm, TAGparaDsc* pGeoMapBm, TAGparaDsc* pConfigBm)
 : TAGactionFile(name, "TAPXIactReader - PXI file reader", "READ"),
   fpDatRaw(pDatRaw),
   fpGeoMap(pGeoMap),
   fpConfig(pConfig),
   fpDatRawBm(pDatRawBm),
   fpGeoMapBm(pGeoMapBm),
   fpConfigBm(pConfigBm),
   fTriggerMode(-1),
   fFramesToSkipAtEndOfAcqForTrig(10), // JB 2011/06/30
   fTriggerLineOffset(4),
   fNextFirstLineToKeep(-1),
   fNextFirstFrame(-1),
   fFirstLineToKeep(-1),
   fFirstFrame(-1),
   fLastLineToKeep(-1),
   fLastFrame(-1),
   fRunNumber(-1),
   fFramesReadFromFile(0),
   fAcqsReadFromFile(0), // JB 2011/03/14
   fNFramesPerAcq(-1),
   fNacquisitions(0),
   fReadingEvent(false),
   fReadTwice(false),
   fOverflow(false),
   fEventReady(false),
   fReadBeamOff(false),
   fReadTriggerSpill(false),
   fCurrentAcqNumber(-1),
   fCurrentFrameNumber(0),
   fCurrentEventNumber(-1), // is increment to 0 at first event
   fAcqId(-1),
   fFrameId(-1),
   fEventsCount(0),
   fEventsOverflow(0),
   fDaqEventNumber(-1),
   fEventsAborted(0),
   fFramesReadTwice(0),
   fTriggerCount(0), // JB 2011/06/18
   fNStatesInLine(0),
   fNumberOfFiles(0),
   fTriggerN(0),
   fDebugLevel(0)
{
   AddPara(pGeoMap, "TAVTparGeo");
   AddPara(pConfig, "TAVTparConf");
   AddDataOut(fpDatRaw, "TAVTntuHit");
   if (pGeoMapBm) AddPara(pGeoMapBm, "TAVTparGeo");
   if (pConfigBm) AddPara(pConfigBm, "TAVTparConf");
   if (fpDatRawBm) AddDataOut(fpDatRawBm, "TAVTntuHit");
   
   // Trigger mode:
   // 0: mode A-0, all frame are a new event (triggerless)
   // 1: mode A-1, events are built from trigger position in frame
   // 2: mode B (not used yet)
   
   TAVTparGeo* geoMap = (TAVTparGeo*) fpGeoMap->Object();
   fNumberOfLines     = geoMap->GetPixelsNy();
   
   fpPxiDaq  = new TAGmi28PxiDaq();
   fpParFile = fpPxiDaq->GetParFile();
   
   for(Int_t ib = 0; ib < 100; ++ib)
      fNStatesInBlock[ib] = 0;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAPXIactReader::~TAPXIactReader()
{
   delete fpPxiDaq;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAPXIactReader::CreateHistogram()
{
   DeleteHistogram();
   TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisPixelMap[i] = new TH2F(Form("vtPixelMap%d", i+1), Form("Vertex - pixel map for sensor %d", i+1),
                                  pGeoMap->GetPixelsNy(), -0.5, pGeoMap->GetPixelsNy()-0.5,
                                  pGeoMap->GetPixelsNx(), -0.5, pGeoMap->GetPixelsNx()-0.5);
      fpHisPixelMap[i]->SetStats(kFALSE);
      AddHistogram(fpHisPixelMap[i]);
      fpHisEvtLength[i] = new TH1F(Form("vtEvtLength%d", i+1), Form("Vertex - event length sensor %d", i+1),
                                   50, -0.5, 99.5);
      AddHistogram(fpHisEvtLength[i]);
   }
   
   if (fpGeoMapBm) {
      TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMapBm->Object();
      for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
         fpHisPixelMapBm[i] = new TH2F(Form("vtPixelMapBm%d", i+1), Form("Vertex - pixel map for sensor %d", i+1),
                                       pGeoMap->GetPixelsNy(), -0.5, pGeoMap->GetPixelsNy()-0.5,
                                       pGeoMap->GetPixelsNx(), -0.5, pGeoMap->GetPixelsNx()-0.5);
         fpHisPixelMapBm[i]->SetStats(kFALSE);
         AddHistogram(fpHisPixelMapBm[i]);
         fpHisEvtLengthBm[i] = new TH1F(Form("vtEvtLengthBm%d", i+1), Form("Vertex - event length sensor %d", i+1),
                                        50, -0.5, 99.5);
         AddHistogram(fpHisEvtLengthBm[i]);
      }
   }
   
   SetValidHistogram(kTRUE);
   return;
}

//------------------------------------------+-----------------------------------
//! Process
Bool_t TAPXIactReader::Process()
{
   if (Valid()) return kTRUE;
   if (IsZombie()) return kFALSE;
   
   if (!FetchData()) {
      SetBit(kEof);
      SetBitAllDataOut(kEof);
      return false;
   } else {
      SetBit(kValid);
   }
   
   fpDatRaw->SetBit(kValid);
   if (fpDatRawBm) fpDatRawBm->SetBit(kValid);
   return true;
}

// --------------------------------------------------------------------------------------
Int_t TAPXIactReader::Open(const TString& path, Option_t* option, const TString /*treeName*/, Bool_t /*dscBranch*/)
{      
   SetRunNumber(path);
   
   TString dir = path;
   
   fNacquisitions = fpPxiDaq->LoadRun(dir, fgkPrefixFileName, fRunNumber);
   if( fNacquisitions < 0) {
      Error("Open()", "Cannot open file %s", path.Data());
      return fNacquisitions;
   }
   
   Info("Open()","Run %d loaded => Contains %d acquistions \n", fRunNumber, fNacquisitions );
   
   fNumberOfFiles = 1; // 1 required because tested later to check a file was associated
   ReadConfiguration();
   
   return fNacquisitions;
}

// --------------------------------------------------------------------------------------
Bool_t TAPXIactReader::ReadConfiguration()
{
   // Read the configuration file which contains info usefull to read data
   
   fNSensors          = fpParFile->GetFrioRunCont().ParMi28Nb;
   fRunNumber         = fpParFile->GetFrioRunCont().ParRunNo;
   fNFramesPerAcq     = fpParFile->GetFrioRunCont().ParFrameNbPerAcq;
   fTriggerMode       = fpParFile->GetFrioRunCont().ParTrigMode;
   //fTriggerMode = 4;
   
   Int_t parTotEvNb     = fpParFile->GetFrioRunCont().ParTotEvNb;
   Int_t parEvNbPerFile = fpParFile->GetFrioRunCont().ParEvNbPerFile;
   
   // Init overflow count per sensor
   // JB 2011/11/02
   fOverflowSensor = new int[fNSensors];
   for( int is=0; is<fNSensors; is++) {
      fOverflowSensor[is] = 0;
   }
   
   Info("ReadConfiguration()", "   Sensor name: %d",  fpParFile->GetFrioRunCont().ParMapsName);
   Info("ReadConfiguration()", "   # sensors: %d",    fNSensors);
   Info("ReadConfiguration()", "   Run number: %d",   fRunNumber);
   Info("ReadConfiguration()", "   Trigger mode: %d", fTriggerMode);
   Info("ReadConfiguration()", "   Total number of events: %d", parTotEvNb);
   Info("ReadConfiguration()", "   Number of events per file: %d", parEvNbPerFile);
   Info("ReadConfiguration()", "   # lines: %d",    fNumberOfLines);
   
   return true;
}

// --------------------------------------------------------------------------------------
Bool_t TAPXIactReader::GetNextDaqEvent( )
{
   
   // Read one Daq Frame from the raw data file
   
   bool searchFrame = true;
   fPtFrame = nullptr;
   
   while (searchFrame){ // search for a frame
      //      printf( "fCurrentAcqNumber %d \n", fCurrentAcqNumber);
      //      printf( "fNacquisitions-1 %d \n", fNacquisitions-1);
      //
      //      printf( "fCurrentFrameNumber %d \n", fCurrentFrameNumber);
      //      printf( "fNFramesPerAcq-1 %d \n", fNFramesPerAcq-1);
      // try the next frame in the current acquisition
      if ( fCurrentAcqNumber != -1 && fCurrentFrameNumber < fNFramesPerAcq-1 ){
         fCurrentFrameNumber++;
         // 	    printf( "here \n");
      }
      // try the first frame in the next acquisition
      else if (fCurrentAcqNumber < fNacquisitions-1){
         fAcqsReadFromFile++;
         fCurrentAcqNumber++;
         fCurrentFrameNumber = 0;
         if (!fpPxiDaq->GoToAcq(fCurrentAcqNumber)){
            Error("GetNextDaqEvent()", "Can't get acquisition %d", fCurrentAcqNumber);
            fCurrentFrameNumber = fNFramesPerAcq-1; // trick to force incrementation to next acq
         }
      }
      // no more acquisition neither frame
      else {
         // 	    printf( "there \n");
         Warning("GetNextDaqEvent()","No more data to read, last frame %d of last acquisition %d in file, STOPPING!\n", fCurrentFrameNumber, fCurrentAcqNumber);
         searchFrame = false;
      }
      if (searchFrame) { // test the found frame if any
         fpPxiDaq->SetFramePos(fCurrentFrameNumber);
         fPtFrame = fpPxiDaq->GetFrame(fCurrentFrameNumber);
         if (fPtFrame == NULL) {
            if (fDebugLevel > 1)
               Warning("GetNextDaqEvent()", "can't get frame %d from acquisition %d",  fCurrentFrameNumber, fCurrentAcqNumber);
         }
         else { // success
            fFramesReadFromFile++;
            searchFrame = false;
            if (fDebugLevel > 2){
               Info( "GetNextDaqEvent()","  Got frame %d in acquisition %d.\n", fCurrentFrameNumber, fCurrentAcqNumber);
            }
         }
      } // end test the found frame if any
      
   } // end search for a frame
   
   return (fPtFrame != NULL);
}

// --------------------------------------------------------------------------------------
Bool_t TAPXIactReader::ReadTriggersFromFrame()
{   
   // Read the frame information related to the triggers
   // to decide when to start building or not an event.
   // This "building" is called "reading" in the code and is marked by the flag ReadingEvent
   // which is returned by the function
   //
   // The decision depends on the trigger mode (TriggerMode).
   //
   // The decision is reprensented by the following limits for storing pixels:
   // o first line of first frame,
   // o last line of last frame, defined in method DefineEventEnd().
   //
   // Note that the decision to end the event reading or building is taken
   // in the method ReadPixelsFromFrame.
   //
   // IMPORTANT NOTICE
   // do not call for the next frame by indicating a number,
   // just check the next frame is the expected one.
   //
   // First part of the original old "DecodeFrame" method,
   // now based on the DAQlib methods.
   //
   // -+-+- Pointers to useful frame information
   fAcqId = fpPxiDaq->GetAcqIdFrHeader();
   fFrameId = fpPxiDaq->GetFrameIdFrHeader();
   std::vector<UInt_t> vTrW32 = fpPxiDaq->GetTriggerList();
   
   if (fDebugLevel > 1)
      printf("       frame %d: EventReading: %d from frame %d - line %d to frame %d - line %d with #Triggers %d\n",
             fAcqId, fReadingEvent, fFirstFrame, fFirstLineToKeep, fLastFrame, fLastLineToKeep, (int)fListOfTriggerPos.size());
   
   // ===============================================
   if( fReadTwice) { // this is the second reading, so we re-init the flag
      fReadTwice = false;
      if(fDebugLevel > 1)
         printf("                  ** Second and last reading!\n");
   }
   
   
   /* if (fCurrentEventNumber == 2000) {
    //printf("here");
    fReadBeamOff = !fReadBeamOff;
    }
    if (fCurrentEventNumber == 6000) fReadBeamOff = !fReadBeamOff;*/
   if (fReadBeamOff){
      // fCurrentEventNumber++;
      // return true;
   }
   
   // ===============================================
   // In trigger mode A-0:
   //  any frame is considered as a single event,
   // start the event right away.
   // ===============================================
   
   fTriggerTime.clear();
   fTriggerLine.clear();
   fTriggerN = 0;
   
   if( fTriggerMode == 0 ) { // trigger mode A-0
      fTriggerN = fpPxiDaq->GetTrigNbr();
      
      if (fTriggerN  > 0) {
         if (fTriggerN > 3){
            fTriggerTime.resize(3);
            fTriggerLine.resize(3);
            for (Int_t i = 0; i < 3; i++){
               fpPxiDaq->SetTluW32(vTrW32.at(i*2));
               fpPxiDaq->SetTriggerW32(vTrW32.at(i*2+1));
               fTriggerLine.at(i) = fpPxiDaq->GetTriggerLine();
               fTriggerTime.at(i) = ((1.0/(75.*1.e6)*16.)*fNumberOfLines*(fAcqId*fNFramesPerAcq + fFrameId)) + ((1.0/(75.*1.e6)*16.)*fTriggerLine.at(i));
            }
         }
         else{
            fTriggerTime.resize(fTriggerN);
            fTriggerLine.resize(fTriggerN);
            for (Int_t i = 0; i < fTriggerN; i++){
               fpPxiDaq->SetTluW32(vTrW32.at(i*2));
               fpPxiDaq->SetTriggerW32(vTrW32.at(i*2+1));
               fTriggerLine.at(i) = fpPxiDaq->GetTriggerLine();
               fTriggerTime.at(i) = ((1.0/(75.*1.e6)*16.)*fNumberOfLines*(fAcqId*fNFramesPerAcq + fFrameId)) + ((1.0/(75.*1.e6)*16.)*fTriggerLine.at(i));
            }
         }
         
         std::sort (fTriggerLine.begin(), fTriggerLine.end());
         std::sort (fTriggerTime.begin(), fTriggerTime.end());
      }
      
      fNextFirstFrame      =  fFrameId;
      fNextFirstLineToKeep =  0;
      fNextLastLineToKeep  = -1; // JB 2011/07/25
      fListOfFrames.push_back(fFirstFrame);
      fListOfTriggerPos.push_back(fFirstLineToKeep);
      
      
      StartEventReading();
   } // end trigger mode A-0
   
   // ===============================================
   // In trigger mode A-1:
   //   first test if the frame is the one expected to start an event,
   //   if we are already reading, store the current frame in the list attached to the event.
   //   then look for a potential new trigger position information.
   // ===============================================
   else if (fTriggerMode == 1) { // trigger mode A-1 with trigger info
      if (!fReadingEvent && fFrameId == fNextFirstFrame) {
         fListOfTriggerPos.insert(fListOfTriggerPos.begin(), fListOfNextTriggerPos.begin(), fListOfNextTriggerPos.end()); // JB 2011/07/18
         fListOfFrames.push_back(fNextFirstFrame);
         StartEventReading();
      }
      else if (fReadingEvent) { // if reading
         fListOfFrames.push_back(fFrameId);
         if (fLastFrame == fAcqId) CheckSecondReading();
      }
      if (!fReadTwice && fpPxiDaq->GetTrigNbr() > 0) { // if trigger info
         bool trigCheck = true;
         if (fAcqId > fNFramesPerAcq-fFramesToSkipAtEndOfAcqForTrig) {
            trigCheck = false;
         }
         for (Int_t iTrig = 0; iTrig < fpPxiDaq->GetTrigNbr(); iTrig++) {
            if( fpPxiDaq->GetTrigInfo(iTrig) == 0 ) {
               if(fDebugLevel>1)
                  printf("trigger[%d] at line %d found in fpPxiDaq->GetTrigInfo(iTrig) -> ignoring all trigger info for this event\n", iTrig, fpPxiDaq->GetTrigInfo(iTrig));
               trigCheck = false;
               break;
            }
         }
         if (trigCheck) { // if no triggers at line 0 (bug pxi_daq_V1.0 - 22/06/2011)
            fTriggerCount += fpPxiDaq->GetTrigNbr(); // JB, 2011/06/18
            fListOfNextTriggerPos.clear(); // JB 2011/07/18
            fNextFirstFrame = (fAcqId + 1)%fNFramesPerAcq;
            if( fNextFirstLineToKeep >= fNumberOfLines ) {
               fNextFirstFrame        = (fNextFirstFrame + 1)%fNFramesPerAcq;
            }
            fNextFirstLineToKeep = fNextFirstLineToKeep%fNumberOfLines; // SS 2011.11.04 - cutting line number > NumberOfLines when passing to the next to next frame
            fFirstLineToKeep     = fNextFirstLineToKeep; //SS 2011.11.07 - I think it is useless here. FirstLineToKeep is anyway defined in StartEventReading()
            if (fDebugLevel > 1)
               printf("Next event will start at frame %d - line %d\n", fNextFirstFrame, fNextFirstLineToKeep);
         } // end if no triggers at line 0 (bug pxi_daq_V1.0 - 22/06/2011)
      } // end if trigger info
   } // end trigger mode A-1 with trigger info
   
   // ===============================================
   // trigger mode not recognized
   // ===============================================
   else {
      Error("ReadTriggersFromFrame()", "trigger mode %d not managed!", fTriggerMode);
      return false;
   }
   
   return true; // if you got there, it is safe!
}

// --------------------------------------------------------------------------------------
void TAPXIactReader::StartEventReading()
{   
   // Start the event reading:
   // - the flag EventReadingEvent is set to true,
   // - increment the event number,
   // - the information on the next frame to read is reset,
   // - define when the event should end.
   //
   fReadingEvent        = true;
   fCurrentEventNumber++;
   fFirstFrame          = fNextFirstFrame;
   fFirstLineToKeep     = fNextFirstLineToKeep;
   DefineEventEnd();
   fNextFirstFrame      = -1;
   fNextFirstLineToKeep = -1;
   fNextLastLineToKeep  = -1;
   
   if (fDebugLevel > 1)
      printf("** Reading event %d with %d triggers!\n", fCurrentEventNumber, (int)fListOfTriggerPos.size());
}

// --------------------------------------------------------------------------------------
void TAPXIactReader::DefineEventEnd()
{   
   // Define the event end with two options:
   // (1): force the start at line 0 and end at line NumberOfLines-1 on the next frame,
   // (2=default): start at trigger position and end at trigger position+NumberOfLines on the next frame
   // ===============================================
   // default option (2)
   // event limits depend on the trigger position
   
   // Set the last frame according to the last line number
   //     if the position is < 0, last frame is also the first one,
   //     if the position is >= NumberOfLines, last frame is the next to next one after the first frame,
   //     if the position is < NumberOfLines, last frame is the next one after the first frame.
   if( fNextLastLineToKeep < 0 ) {
      fLastLineToKeep = fNumberOfLines-1;
      fLastFrame      = fNextFirstFrame; // SS 2011.11.08 fNextFirstFrame is used in place of FirstFrame. FirstFrame always remains the very first frame of the event
   }  else {
      fLastLineToKeep = fNextLastLineToKeep % fNumberOfLines; // modulo added to take into account limits, JB 2011/10/31
      fLastFrame      = (fNextFirstFrame+1) % fNFramesPerAcq; //SS 2011.11.08 fNextFirstFrame is used in place of FirstFrame. FirstFrame always remains the very first frame of the event
   }
   // end option (2)
   
   // option (1)
   // event limits are always first and last frame line
   // but last frame does not change
   
   if (fDebugLevel > 1)
      printf("** the event starting at frame %d and line %d will stop at frame %d and line %d!\n",
             fFirstFrame, fFirstLineToKeep, fLastFrame, fLastLineToKeep);
}

// --------------------------------------------------------------------------------------
void TAPXIactReader::CheckSecondReading()
{   
   // CALL this function ONLY when reading the LAST FRAME of an event.
   //
   // Test the condition for reading twice the current frame
   // = we are reading the last frame of the current event
   //   AND this frame is also the beginning of the next exepected event.
   //
   // Then two situations may occur:
   // a) the last line of the current event lies before the first line of the next event,
   //   --> event information are NOT intricated in the same frame part,
   // b) the last line of the current event lies before the first line of the next event
   //   --> event information ARE intricated in the same frame part,
   //
   // (a) case, we simply read the frame twice.
   //
   // (b) case, we do not read the frame twice and we have several options:
   // - fEventBuildingMode/10 == 1: we abort the two events
   // - fEventBuildingMode/10 == 2: we abort the two events
   // - Otherwise(default): the current event is extended to the end of the next one => they are merged.
   //
   // JB 2011/03/14
   // Modified: JB 2011/07/18 correct multi-triggers storage
   // Modified: SS 2011/11/04 correct redefinition of event end
   
   if (fNextFirstFrame == fLastFrame ) { // test if reading twice
      fFramesReadTwice++;
      if (fDebugLevel > 2)
         Info("CheckSecondReading()", "  Board %d: event %d shares its last frame %d with next event first frame %d -> READ TWICE potential condition.\n",
              fBoardNumber, fCurrentEventNumber, fLastFrame, fNextFirstFrame);
      
      // -----------------
      // case (a)
      if (fNextFirstLineToKeep > fLastLineToKeep) { // test if events are NOT intricated
         fReadTwice = true;
         if(fDebugLevel > 1)
            printf("                  ** This frame %d will be read twice (for the two separated events starting at frame: %d and %d)!\n",
                   fLastFrame, fFirstFrame, fNextFirstFrame);
      }
      
      // -----------------
      // case (b)
      else { // test if events ARE intricated
         fReadTwice = false;
         fListOfTriggerPos.insert( fListOfTriggerPos.begin(), fListOfNextTriggerPos.begin(), fListOfNextTriggerPos.end()); // JB 2011/07/18
         DefineEventEnd();
         fNextFirstLineToKeep = -1;
         fNextFirstFrame      = -1;
         fNextLastLineToKeep = -1; //SS 2011.11.07 - to have correct definition of the last line of the multi-merged event.
      } // end test if events ARE intricated
      
   } // end test if reading twice
   
   else {
      if (fDebugLevel > 2)
         printf("  PXI board %d: event %d DOES NOT share its last frame %d with next event first frame %d.\n",
                fBoardNumber, fCurrentEventNumber, fLastFrame, fNextFirstFrame);
   }
}

// --------------------------------------------------------------------------------------
bool TAPXIactReader::ReadPixelsFromFrame()
{   
   // Read the frame information related to the pixels
   // It expects the method ReadTriggersFromFrame() was call just before
   // to decide about event building
   //
   // Contains the line overflow management.
   //
   // Second part of the original old "DecodeFrame" method,
   // now based on the DAQlib methods.
   //
   // JB 2011/03/14
   // Modified JB 2011/10/31 condition to stop the pixel reading
   
   // -+-+- Pointers to useful frame information
   UShort_t oneMapsSzW16  = fpPxiDaq->GetDataOneMapSize() / 2;
   UShort_t dataW16Length = 0;
   UShort_t lastW16 = 0;
   Int_t iSrcW16 = 0;
   Int_t iStatesLine = 0;
   Int_t iState = 0;
   Int_t statesNbPerLine = 0;
   std::vector<UShort_t> vSrcW16 = fpPxiDaq->GetDataW32();
   
   fAcqId = fpPxiDaq->GetAcqIdFrHeader();
   fFrameId = fpPxiDaq->GetFrameIdFrHeader();
   
   if(fDebugLevel > 2)
      printf("  PXI board %d: Reading pixel from frame AcqId = %.4d - FrameIdInAcq = %.4d - Current event %d\n",
             fBoardNumber, fAcqId, fFrameId, fCurrentEventNumber );
   
   // -+-+- Start loop on sensors
   for (Int_t iSensor = 0; iSensor < fpPxiDaq->GetMapsNbr() ; iSensor++ ) { // loop on sensors
      dataW16Length  = fpPxiDaq->GetMapDataLength(iSensor) / 2;
      iSrcW16        = 0;
      // =======
      if (dataW16Length != 0) { // check there is some data to read
         lastW16 = dataW16Length - 1;
         iStatesLine  = 0;
         // ========
         while (iSrcW16 < lastW16) { // loop over words to read
            // -- StatesLine field
            
            fpPxiDaq->SetStLineW16(vSrcW16.at(iSrcW16 + (iSensor  * oneMapsSzW16)));
            statesNbPerLine = fpPxiDaq->GetStLineStateNbr();
            // 		   printf("********statesNbPerLine********** %d\n", statesNbPerLine);
            if (fpPxiDaq->GetStLineOvf() > 0) { // detect overflow
               fOverflow = true;
               fOverflowSensor[iSensor] += 1; // JB 2011/11/02
            } // end detect overflow
            
            if (fDebugLevel > 3)
               printf("                  sensor %2d line %4d, #states %d, overflow %d, reading event ? %d\n",
                      iSensor, fpPxiDaq->GetStLineAdrr(), fpPxiDaq->GetStLineStateNbr(), fpPxiDaq->GetStLineOvf(), fReadingEvent);
            
            ++iSrcW16;
            
            // -- States fields
            
            // Init some statistics for this line
            // JB 2009/09/10
            for(Int_t ib = 0; ib < 100; ib++) {
               fNStatesInBlock[ib] = 0;
            }
            fNStatesInLine = 0;
            
            for (iState = 0; iState < statesNbPerLine; iState++ ) { // loop on states
               fpPxiDaq->SetStW16(vSrcW16.at(iSrcW16 + (iSensor  * oneMapsSzW16)));
               fNStatesInLine++;
               fNStatesInBlock[fpPxiDaq->GetStLineAdrr()%64] += 1;
               
               // A state contains HitNb+1 pixels
               // the first pixel being on the left at the column ColAddr
               for (Int_t iPixel = 0; iPixel < fpPxiDaq->GetStHitNbr() + 1; iPixel++) { // loop on pixels in the state
                  
                  // create a new pixel only if we are reading an event
                  // and if the line is inside the proper limits
                  if(fReadingEvent && (
                                       (fFrameId == fFirstFrame && fpPxiDaq->GetStLineAdrr() >= fFirstLineToKeep)
                                       || (fFrameId == fLastFrame && fpPxiDaq->GetStLineAdrr() <= fLastLineToKeep)
                                       || (fFirstFrame < fFrameId && fFrameId < fLastFrame) // MG, 2010/07/06
                                       )
                     ) {
                     AddPixel(iSensor, 1, fpPxiDaq->GetStLineAdrr(), fpPxiDaq->GetStColAdrr() + iPixel);
                     
                  }
                  
               } // end loop on pixels in the state
               
               if(fDebugLevel>3) printf("State %d, #pixels %d, column %d\n", iState, fpPxiDaq->GetStHitNbr() + 1, fpPxiDaq->GetStColAdrr());
               
               ++iSrcW16;
            } // end loop on states
            
            ++iStatesLine;
            
         } // end loop over words to read
         
      }  // end check there is some data to read
      
      // Condition to stop the reading after the data of the last sensor
      //  ==LastFrame changed to >=LastFrame to prevent to loop for ever for an unexisting frame, JB 2011/10/31
      if (iSensor == (fpPxiDaq->GetMapsNbr() - 1) && fReadingEvent && fFrameId >= fLastFrame) {
         fReadingEvent   = false;
         fEventReady     = true;
         fLastLineToKeep = -1;
         fLastFrame      = -1;
         if (fDebugLevel > 1) printf("* Reading event %d stopped!\n", fCurrentEventNumber);
      }
      
   } // end loop on sensors
   
   return true; // if you got there, it is safe!
}

// --------------------------------------------------------------------------------------
Bool_t TAPXIactReader::FetchData( )
{
   // Fill a new event containing all the fired pixels of each sensors acquired,
   // return "true" if the event is OK, "false" otherwise
   //
   // To build the event, frame(s) is(are) read, the number depending on the trigger configuration.
   //
   // Modified: JB, 2010/06/16, manage several triggers
   
   // -+-+- Initialization
   bool readingOK = true; // flag to test the reading
   fOverflow       = false; // init value, JB 2009/09/08
   fListOfTriggerPos.clear(); //JB 2010/06/16
   fListOfFrames.clear(); //JB 2010/06/16
   
   // -+-+- Check files have been associated
   if (fNumberOfFiles == 0) {
      Error("FetchData()", " NO RAW DATA FILE WAS ASSOCIATED WITH BOARD %d STOP", fBoardNumber);
      return false;
   }
   
   // -+-+- Loop over Daq Events (or frames for all sensors)
   while (!fEventReady && readingOK) { // while some event has to be read & readout OK
      Bool_t goDecode = true;
      if( !fReadTwice ) {  // If not second reading of same frame, JB 2009/09/09
         goDecode = GetNextDaqEvent(); // Get the next Daq Event
      }
      if( goDecode ) { // If frame is OK
         readingOK &= ReadTriggersFromFrame();
         if( fReadingEvent) readingOK &= ReadPixelsFromFrame();
      } // end if frame is OK
      // wrong Daq frame
      else {
         readingOK = false;
         Warning("FetchData()", "Daq Event unreadable!");
         return readingOK;
      }
      
   } // while some events have to be read
   
   // -+-+- Create the event if event is complete and readout is OK
   if (fEventReady && readingOK) {
      if( fOverflow) {  // return no pixels if overflow, JB 2009/09/09, 2010/07/06, JB 2011/07/26
         fEventsOverflow++;
      }
      fEventsCount++;
      fEventReady  = false;
   }
   
   return readingOK;
}

// --------------------------------------------------------------------------------------
void TAPXIactReader::PrintStatistics(ostream &stream)
{   
   // Print statistics on the events read by this board
   //
   stream << "***********************************************" << endl;
   stream << " Board PXI" << fBoardNumber << " with trigger mode " << fTriggerMode << " found:" << endl;
   stream << fTriggerCount << " triggers read, " << endl;
   stream << fEventsCount << " events in total," << endl;
   if( fEventsOverflow ) { // added, JB 2011/11/02
      for( int is=0; is<fNSensors; is++) {
         stream << " s" << is << ": " << fOverflowSensor[is];
      }
      stream << endl;
   }
   stream << fAcqsReadFromFile << " acquisitions read overall." << endl;
   stream << fFramesReadFromFile<< " frames read overall." << endl;
   stream << fFramesReadTwice << " frames read twice." << endl;
   stream << fEventsAborted << " events aborted." << endl;
   stream << "***********************************************" << endl;
}

// --------------------------------------------------------------------------------------
void TAPXIactReader::AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn)
{
   // Add a pixel to the vector of pixels
   // require the following info
   // - input = number of the sensors
   // - value = analog value of this pixel
   // - line & column = position of the pixel in the matrix
   //   printf("line: %d, column: %d \n", aLine, aColumn);
   //std::cout << "input" << input << endl;
   Int_t delta = 0;
   if (fpConfigBm != NULL) {
      TAVTparConf* pConfigBm = (TAVTparConf*) fpConfigBm->Object();
      delta = pConfigBm->GetSensorsN();
      Int_t status = pConfigBm->GetStatus(input);
      if (status == -1) return;
      if (input >= delta) status = -1;
      if (status != -1){
         if (pConfigBm->IsDeadPixel(input, aLine, aColumn)) return;

         TAVTntuHit* pDatRawBm  = (TAVTntuHit*) fpDatRawBm->Object();
         pDatRawBm->NewPixel(input, value, aLine, aColumn);
         if (ValidHistogram()) {
               fpHisPixelMapBm[input]->Fill(aColumn, aLine);
         }
         return;
      }
   }
   
   input -= delta;
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Int_t status = pConfig->GetStatus(input);
   if (status == -1) return;
   
   if (pConfig->IsDeadPixel(input, aLine, aColumn)) return;
   
   TAVTntuHit* pDatRaw  = (TAVTntuHit*) fpDatRaw->Object();
   pDatRaw->NewPixel(input, value, aLine, aColumn);
   if (ValidHistogram()) {
         fpHisPixelMap[input]->Fill(aColumn, aLine);
   }
}

// --------------------------------------------------------------------------------------
void TAPXIactReader::SetRunNumber(const TString& path)
{
   Int_t pos1 = path.Last('/');
   
   TString tmp = path(pos1+1, path.Length()-pos1);
   fRunNumber = tmp.Atoi();
   gTAGroot->SetRunNumber(fRunNumber);
}

// --------------------------------------------------------------------------------------
void TAPXIactReader::SetDaqEventNumber(Int_t daqEvent)
{
   fDaqEventNumber = daqEvent;
   Float_t aquisitions = (Float_t)(fDaqEventNumber) / (Float_t)(fNFramesPerAcq);
   if (aquisitions < 0) return;
   if (aquisitions > fNacquisitions) Error("SetDaqEventNumber()", " Can't access the set event number... ");
   fCurrentAcqNumber = floor(aquisitions);
   Int_t frames = (aquisitions - fCurrentAcqNumber)*fNFramesPerAcq - 1;
   fCurrentFrameNumber = frames;
   fCurrentEventNumber = fCurrentAcqNumber*fNFramesPerAcq + frames;
   if (!fpPxiDaq->GoToAcq(fCurrentAcqNumber)) printf("Go to acquisition failed...");
   printf("Start: acquisition number: %d and frame number: %d \n", fCurrentAcqNumber, frames);
}


