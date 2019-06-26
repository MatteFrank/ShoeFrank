/*!
  \file
  \brief   Implementation of TAVTactVmeWriter.
*/

#include <limits.h>

#include "DECardEvent.hh"

#include "GlobalPar.hxx"
#include "TAGroot.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTactVmeWriter.hxx"

/*!
  \class TAVTactVmeWriter 
  \brief Read stand alone files (ascii format) and write them in a single DAQ file (binary format) **
*/

ClassImp(TAVTactVmeWriter);

TString TAVTactVmeWriter::fgDefaultFolderName = "run_";
TString TAVTactVmeWriter::fgDefaultExtName    = ".ZS";
 Bool_t TAVTactVmeWriter::fgTrigJumpAuto      = true;


// set nil
map<pair<int, int>, int > TAVTactVmeWriter::fgTrigJumpMap = { {{0, 0}, 0}};


//run 2211
//map<pair<int, int>, int > TAVTactVmeWriter::fgTrigJumpMap = { {{1,18564}, 4}, {{1,18674}, 2}, {{1,18715}, 1},  {{1,37425}, 1}, {{1,37482}, 5},  {{1,37599}, 5}, {{1,38721}, 1},
//                                                              {{1,38834}, 1}, {{1,38838}, 1}, {{1,39847}, 15}, {{1,39956}, 1}, {{1,40990}, 13}, {{1,40993}, 1}, {{1,41008}, 1},
//                                                              {{1,41105}, 4}, {{1,41123}, 1},  {{1,41145}, 1} };


// run 2212
//map<pair<int, int>, int > TAVTactVmeWriter::fgTrigJumpMap = { {{1, 350}, 151}, {{2, 214}, 15}, {{3, 210}, 11}};


// run 2242
//map<pair<int, int>, int > TAVTactVmeWriter::fgTrigJumpMap = { {{1, 60}, 58}};

// run 2251
//map<pair<int, int>, int > TAVTactVmeWriter::fgTrigJumpMap = { {{1, 60}, 58}};


//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactVmeWriter::TAVTactVmeWriter(const char* name, TAGdataDsc* pDatRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
 : TAVTactBaseRaw(name, pDatRaw, pGeoMap, pConfig, pParMap),
   fRunNumber(-1),
   fTrigJumpStart(-1)
{
   SetTitle("TAVTactVmeWriter - reader for VME reader");
   fBaseName ="data_FPGA_Mouser993P0160_V1_ch";
   
   Int_t size = (sizeof(MI26_FrameRaw)/4)*3 + 3; // 3 frame per event and 3 header word for each sensor
   fDataEvent = new UInt_t[size];

   fDaqEvent.reserve(size*4);
   if (!fgTrigJumpAuto)
      fTrigJumpStart = INT_MAX;
   
   // run 2212
   //fTrigJumpStart = 210;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactVmeWriter::~TAVTactVmeWriter()
{
}

// --------------------------------------------------------------------------------------
void TAVTactVmeWriter::SetTrigJumpMap(Int_t iSensor, Int_t trigger, Int_t jump)
{
   pair<int, int> id{iSensor, trigger};
   fgTrigJumpMap[id] = jump;
}

//------------------------------------------+-----------------------------------
//! Open ascii data sources.
Int_t TAVTactVmeWriter::Open(const TString& name, Option_t* opt)
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

   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
     Int_t planeStatus = pConfig->GetStatus(i);
	  if (planeStatus == -1) continue;
	  
	  // Close any previous open file
	  if( fRawFileAscii[i].is_open() && !fRawFileAscii[i].eof()) {
		 isOk = true;
	  } else {
		 fRawFileAscii[i].close();
//		 inputFileName = Form("%s/%s%04d/800%d_%s%d%s", fPrefixName.Data(), fgDefaultFolderName.Data(), fRunNumber, i, fBaseName.Data(), i, fgDefaultExtName.Data());
        inputFileName = Form("%s/%s%04d/192.168.1.11_%s%d%s", fPrefixName.Data(), fgDefaultFolderName.Data(), fRunNumber, fBaseName.Data(), i, fgDefaultExtName.Data());

		 fRawFileAscii[i].open(inputFileName.Data());
		 if( fRawFileAscii[i].fail() ) { // end the reading if file opening failed
			cout << endl << "TAVTactVmeWriter::Open(), cannot open file " << inputFileName.Data() << endl;
			isOk = false;
		 } else {
			isOk = true;
		 }
	  }
	  valid &= isOk;
   }
   
   fDaqFile.open(Form("data_built.%d.vtx.daq.VTX.3.dat", fRunNumber), ios::out|ios::binary);
   
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAVTactVmeWriter::Close()
{
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i)
	  fRawFileAscii[i].close();
   
   fDaqFile.close();
}

//------------------------------------------+-----------------------------------
//! Process
Bool_t TAVTactVmeWriter::Process()
{
   Int_t size = (sizeof(MI26_FrameRaw)/4)*3 + 3;
   
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Bool_t neof = false;
   
   fDaqEvent.clear();
   fDaqSize = 0;
   
   // loop over boards
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
      Int_t planeStatus = pConfig->GetStatus(i);
      if (planeStatus == -1) continue;
      
      if (GetSensorEvent(i)) {
         neof |= true;
         ResetFrames();
      
         fIndex    = 0;
         memset(fDataEvent, 0, size);
      } else {
         SetBit(kEof);
         SetBitAllDataOut(kEof);
      }
   }

   // Fill DAQ event
   FillDaqEvent();
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);

   
   if (neof)
      return true;
   else
      return false;
      
}

// private method

// --------------------------------------------------------------------------------------
void TAVTactVmeWriter::FillDaqEvent()
{
   vector<UInt_t> data;
   data.resize(fDaqSize + 3);
   data[0] = fDaqSize + 3;
   data[1] = DECardEvent::GetVertexHeader();
   
   memcpy(&data[2], &fDaqEvent[0], fDaqSize*sizeof(uint32_t));
   
   int words = fDaqSize+3;
   
   data[words-1] = DECardEvent::GetVertexTail();
   
   
   if(FootDebugLevel(3)) {
      for (Int_t i = 0; i < words; ++i) {
         if (i == 10) {
            printf("\n");
         } else {
            if (i % 10 == 0) printf("\n");
         }
         printf("%08x ", data[i]);
      }
      printf("\n");
   }

   fDaqFile.write(reinterpret_cast<char *>(&data[0]), sizeof(UInt_t)*words);
}


// --------------------------------------------------------------------------------------
Bool_t TAVTactVmeWriter::GetSensorEvent(Int_t iSensor)
{
   Char_t tmp[255];
   
   fIndex = 0;
   UInt_t timestamp = 0;
   UInt_t dataPrev = 0;
   
   // lokking for header
   TString key  = Form("%x", GetKeyHeader(iSensor));
   TString tail = Form("%x", GetKeyTail(iSensor));

   do {
      fRawFileAscii[iSensor] >> tmp;
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
            printf("sensor header %s %d\n", tmp, (int) fRawFileAscii[iSensor].tellg()/9+1);
         
         fDataEvent[fIndex++] = GetKeyHeader(iSensor);
         
         fRawFileAscii[iSensor] >> tmp;
         sscanf(tmp, "%x", &fEventNumber);
         fDataEvent[fIndex++] = fEventNumber;
         
         // trigger
         fRawFileAscii[iSensor] >> tmp;
         sscanf(tmp, "%x", &fTriggerNumber);
         fDataEvent[fIndex++] = fTriggerNumber;
         
         pair<int, int> id(iSensor, fTriggerNumber);

         if (fPrevTriggerNumber[iSensor] != fTriggerNumber-1) {
            if(FootDebugLevel(1))
               printf("Jump sensor %d %d %d\n", iSensor, fPrevTriggerNumber[iSensor], fTriggerNumber);
            
            if (fTrigJumpFirst[id] == 0 && fTriggerNumber > fTrigJumpStart) {
               fgTrigJumpMap[id] = fTriggerNumber - fPrevTriggerNumber[iSensor] -1;
               fTrigJumpFirst[id] = 1;
            }
         }
         
         if (fgTrigJumpMap[id] > 0) {
            Int_t pos =  (int) fRawFileAscii[iSensor].tellg();
            fRawFileAscii[iSensor].seekg(pos-3*(8+1));
            fgTrigJumpMap[id]--;
            if(FootDebugLevel(1))
               printf("%d\n", fgTrigJumpMap[id]);
            return false;
         }


         if(FootDebugLevel(3))
            printf("sensor %d: %d %d\n", iSensor, fTriggerNumber, fEventNumber);
         
         // fake time stamp
         fRawFileAscii[iSensor] >> tmp;
         sscanf(tmp, "%x", &timestamp);
         fDataEvent[fIndex++] = timestamp;
         
         FillHistoEvt(iSensor);
         
         fPrevEventNumber[iSensor]   = fEventNumber;
         fPrevTriggerNumber[iSensor] = fTriggerNumber;
         fPrevTimeStamp[iSensor]     = fTimeStamp;
         break;
      }
   } while (!fRawFileAscii[iSensor].eof());
   
   if (fRawFileAscii[iSensor].eof()) return false;
   
   // look for trailer
   UInt_t data;
   
   do {
      fRawFileAscii[iSensor] >> tmp;
      TString line = tmp;
      sscanf(tmp, "%x", &data);
      fDataEvent[fIndex++] = data;
      
      if (line.Contains(tail)) {
         break;
      }
      
      if (line.Contains(key)) {
         Int_t pos =  (int) fRawFileAscii[iSensor].tellg();
         fRawFileAscii[iSensor].seekg(pos-1);
         fIndex--;
         if(FootDebugLevel(1))
            printf("Find unexpected key for sensor %d %s\n", iSensor, line.Data());
      }
      
   } while (!fRawFileAscii[iSensor].eof());
   
   if (fRawFileAscii[iSensor].eof()) return false;

   fEventSize = fIndex;
   
   if(FootDebugLevel(3)) {
      for (Int_t i = 0; i < fEventSize; ++i)
         printf("Data %08x\n", fDataEvent[i]);
      printf("\n");
   }
   
   
   // Fill DAQ
   fDaqSize += fEventSize;
   fDaqEvent.insert(fDaqEvent.end(), fDataEvent, fDataEvent+fEventSize);

   return true;
}

// --------------------------------------------------------------------------------------
void TAVTactVmeWriter::SetRunNumber(const TString& filename)
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
