/*!
  \file
 \brief Get vertex raw data from stand alone single file (ascii format)
  Keep VME name, though new card is no more VME standard
*/

#include <limits.h>

#include "DECardEvent.hh"

#include "TAGrecoManager.hxx"
#include "TAGroot.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTactVmeSingleReader.hxx"

/*!
  \class TAVTactVmeSingleReader
  \brief Reader action for ascii single file **
*/

ClassImp(TAVTactVmeSingleReader);

TString TAVTactVmeSingleReader::fgDefaultFolderName = "run_";
TString TAVTactVmeSingleReader::fgDefaultExtName    = ".ZS";
 Bool_t TAVTactVmeSingleReader::fgTrigJumpAuto      = true;

// set nil
map<pair<int, int>, int > TAVTactVmeSingleReader::fgTrigJumpMap = { {{0, 0}, 0}};


//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactVmeSingleReader::TAVTactVmeSingleReader(const char* name, TAGdataDsc* pDatRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
 : TAVTactBaseRaw(name, pDatRaw, pGeoMap, pConfig, pParMap),
   fRunNumber(-1),
   fTrigJumpStart(-1),
   fTrigReset(0)
{
   SetTitle("TAVTactVmeSingleReader - reader for VME reader");
   fBaseName ="data_FPGA_Mouser993P0160_V1_ch";
   
   Int_t size = (sizeof(MI26_FrameRaw)/4)*3 + 3; // 3 frame per event and 3 header word for each sensor
   fDataEvent = new UInt_t[size];
   fData.reserve(size);

   if (!fgTrigJumpAuto)
      fTrigJumpStart = INT_MAX;
   
   // run 2212
//   fTrigJumpStart = 210;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactVmeSingleReader::~TAVTactVmeSingleReader()
{
}

// -------------------------------------------------------------------------------------
void TAVTactVmeSingleReader::SetTrigJumpMap(Int_t iSensor, Int_t trigger, Int_t jump)
{
   pair<int, int> id{iSensor, trigger};
   fgTrigJumpMap[id] = jump;
}

//------------------------------------------+-----------------------------------
//! Open ascii data sources.
Int_t TAVTactVmeSingleReader::Open(const TString& name, Option_t* opt, const TString, Bool_t )
{
   TString inputFileName;
   
   static Bool_t first = true;
   if (first)
	  SetRunNumber(name);
   else 
	  first = false;
	
   Bool_t valid = true;
   Bool_t isOk  = false;
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Int_t i = 5;
   // Close any previous open file
   if( fRawFileAscii.is_open() && !fRawFileAscii.eof()) {
      isOk = true;
   } else {
      fRawFileAscii.close();
      inputFileName = Form("%s%04d/800%d_%s%d%s", fgDefaultFolderName.Data(), fRunNumber, i, fBaseName.Data(), i, fgDefaultExtName.Data());
      fRawFileAscii.open(inputFileName.Data());
      if( fRawFileAscii.fail() ) { // end the reading if file opening failed
         cout << endl << "TAVTactVmeSingleReader::Open(), cannot open file " << inputFileName.Data() << endl;
         isOk = false;
      } else {
         isOk = true;
      }
   }
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAVTactVmeSingleReader::Close()
{
	  fRawFileAscii.close();
}

//------------------------------------------+-----------------------------------
//! Process
Bool_t TAVTactVmeSingleReader::Process()
{
   Char_t tmp[255];

   Int_t size = (sizeof(MI26_FrameRaw)/4)*3 + 3;
   
   MI26_FrameRaw* data = new MI26_FrameRaw;
   
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Bool_t neof = false;
   
   size *= pGeoMap->GetSensorsN();
   // loop over boards
   fRawFileAscii >> tmp;

   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      Int_t planeStatus = pConfig->GetStatus(i);
      if (planeStatus == -1) continue;

      if (GetSensorEvent(i)) {
         neof |= true;
         ResetFrames();

         fIndex = 0;

         // loop over frame (3 max)
         for (Int_t k = 0; k < 3; ++k) {
            if (GetFrame(i, data))
               DecodeFrame(i, data);
         }

         memset(fDataEvent, 0, size);
      } else {
         SetBit(kEof);
         SetBitAllDataOut(kEof);
      }
   }

   delete data;

   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);

   
   if (neof)
      return true;
   else
      return false;
      
}

// private method

// --------------------------------------------------------------------------------------
Bool_t TAVTactVmeSingleReader::GetSensorEvent(Int_t iSensor)
{
   Char_t tmp[255];
   
   fIndex = 0;
   UInt_t timestamp = 0;
   UInt_t dataPrev = 0;
   
   // lokking for header
   TString key  = Form("%x", GetKeyHeader(iSensor));
   TString tail = Form("%x", GetKeyTail(iSensor));

   do {
      fRawFileAscii >> tmp;
      TString line = tmp;
      
      UInt_t data = 0;
      sscanf(tmp, "%x", &data);
      
      if (line.Contains(tail)){
         if(FootDebugLevel(1))
            printf("unexpected trailer for sensor %d previous data %08x\n", iSensor, dataPrev);
      }

      dataPrev = data;
      
      if (line.Contains(key)) {
         if(FootDebugLevel(1))
            printf("sensor header %s %d\n", tmp, (int) fRawFileAscii.tellg()/9+1);
         
         fDataEvent[fIndex++] = GetKeyHeader(iSensor);
         
         // event number
         fRawFileAscii >> tmp;
         sscanf(tmp, "%x", &fEventNumber);
         
         // trigger number
         fRawFileAscii >> tmp;
         sscanf(tmp, "%x", &fTriggerNumber);

         if (fTriggerNumber == 1) {
            fTrigReset = fPrevTriggerNumber[iSensor];
         }
         
         fDataEvent[fIndex++] = fEventNumber;
         fDataEvent[fIndex++] = fTriggerNumber;

         pair<int, int> id(iSensor, fTriggerNumber);

         
//         if (fPrevTriggerNumber[iSensor] != fTriggerNumber-1) {
//
//            if(FootDebugLevel(1))
//               printf("Jump sensor %d %d %d\n", iSensor, fPrevTriggerNumber[iSensor], fTriggerNumber);
//
//            if (fTrigJumpFirst[id] == 0 && fTriggerNumber > fTrigJumpStart) {
//               fgTrigJumpMap[id] = fTriggerNumber - fPrevTriggerNumber[iSensor] -1;
//               fTrigJumpFirst[id] = 1;
//            }
//         }
         
//         if (fgTrigJumpMap[id] > 0) {
//            Int_t pos =  (int) fRawFileAscii.tellg();
//            fRawFileAscii.seekg(pos-3*(8+1));
//            fgTrigJumpMap[id]--;
//            if(FootDebugLevel(1))
//               printf("%d\n", fgTrigJumpMap[id]);
//            return false;
//         }


         if(FootDebugLevel(3))
            printf("sensor %d: %d %d\n", iSensor, fTriggerNumber+fTrigReset, fEventNumber+fTrigReset);
         
         // fake time stamp
         fRawFileAscii >> tmp;
         sscanf(tmp, "%x", &timestamp);
         fDataEvent[fIndex++] = timestamp;
         
         if(FootDebugLevel(1))
            printf("Sensor %d Event %d trigger %d timestamp %x\n", iSensor, fEventNumber, fTriggerNumber, timestamp);
         
         if(ValidHistogram())
            FillHistoEvt(iSensor);
         
         fPrevEventNumber[iSensor]   = fEventNumber;
         fPrevTriggerNumber[iSensor] = fTriggerNumber;
         fPrevTimeStamp[iSensor]     = fTimeStamp;
         break;
      }
   } while (!fRawFileAscii.eof());
   
   if (fRawFileAscii.eof()) return false;
   
   // look for trailer
   UInt_t data;
   
   do {
      fRawFileAscii >> tmp;
      TString line = tmp;
      sscanf(tmp, "%x", &data);
      fDataEvent[fIndex++] = data;
      
      if (line.Contains(tail)) {
         break;
      }
      
      if (line.Contains(key)) {
         Int_t pos =  (int) fRawFileAscii.tellg();
         fRawFileAscii.seekg(pos-1);
         fIndex--;
         if(FootDebugLevel(1))
            printf("Find unexpected key for sensor %d %s\n", iSensor, line.Data());
      }
      
   } while (!fRawFileAscii.eof());
   
   if (fRawFileAscii.eof()) return false;

   fEventSize = fIndex;
   
   if(FootDebugLevel(3)) {
      for (Int_t i = 0; i < fEventSize; ++i)
         printf("Data %08x\n", fDataEvent[i]);
      printf("\n");
   }
   
   FillDaqEvent();

   return true;
}


// --------------------------------------------------------------------------------------
Bool_t TAVTactVmeSingleReader::GetFrame(Int_t iSensor, MI26_FrameRaw* data)
{
   Char_t tmp[255];
   fDataSize = 0;
   
 if (fIndex >= fEventSize -2) return false;

   // find header
   do {
      if (fDataEvent[fIndex] == GetFrameHeader()) {
         fData[fDataSize++] = fDataEvent[fIndex];
         break;
      }
   } while (fIndex++ < fEventSize);
   
   if (fIndex >= fEventSize -2) return false;

   fIndex++;
   
   // find trailer
   UInt_t key1  =  GetFrameTail() & 0xFFFF;
   UInt_t key2  = (GetFrameTail() & 0xFFFF0000) >> 16;
   
   do {
      fData[fDataSize++] = fDataEvent[fIndex];
      if (( (fDataEvent[fIndex] & 0xFFFF) == key1) || ( (fDataEvent[fIndex] & 0xFFFF0000) >> 16) == key2) {
         break;
      }
      
   } while (fIndex++ < fEventSize);
   
   memcpy(data, &fData[0], sizeof(MI26_FrameRaw));
   FillHistoFrame(iSensor, data);

   fDataSize -= fgkFrameHeaderSize; // removing header
   
   if(FootDebugLevel(2)) {
      for (Int_t i = 0; i < fDataSize+fgkFrameHeaderSize; ++i)
         printf("Data %08x\n", fData[i]);
      printf("\n");
   }

   if (fIndex >= fEventSize -2) return false;
   
   return true;
}

// --------------------------------------------------------------------------------------
void TAVTactVmeSingleReader::SetRunNumber(const TString& filename)
{
   TString name(filename);
   Int_t pos1 = name.Last('/');
   Int_t pos2 = name.First("_");
   Int_t len  = name.Length();
   
   fPrefixName = name(0, pos1);
   
   TString tmp = name(pos2+1, len-pos2);
   fRunNumber = tmp.Atoi();
   gTAGroot->SetRunNumber(fRunNumber);
}
