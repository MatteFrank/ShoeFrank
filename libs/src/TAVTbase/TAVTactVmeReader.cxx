/*!
  \file
  \brief   Implementation of TAVTactVmeReader.
*/

#include "DECardEvent.hh"

#include "GlobalPar.hxx"
#include "TAGroot.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTactVmeReader.hxx"

/*!
  \class TAVTactVmeReader 
  \brief Reader action for ascii files M26-28 (Catania Test). **
   (binary file never been used)
*/

ClassImp(TAVTactVmeReader);

TString TAVTactVmeReader::fgDefaultFolderName = "run_";
TString TAVTactVmeReader::fgDefaultExtName    = ".ZS";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactVmeReader::TAVTactVmeReader(const char* name, TAGdataDsc* pDatRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseRaw(name, pDatRaw, pGeoMap, pConfig, pParMap),
  fRunNumber(-1)
{
   SetTitle("TAVTactVmeReader - reader for VME reader");
   fBaseName ="data_FPGA_Mouser993P0160_V1_ch";
   
   Int_t size = (sizeof(MI26_FrameRaw)/4)*3 + 3; // 3 frame per event and 3 header word for each sensor
   fDataEvent = new UInt_t[size];

}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactVmeReader::~TAVTactVmeReader()
{
   delete [] fDataEvent;
}

//------------------------------------------+-----------------------------------
//! Open ascii data sources.
Int_t TAVTactVmeReader::Open(const TString& name, Option_t* opt)
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
			cout << endl << "TAVTactVmeReader::Open(), cannot open file " << inputFileName.Data() << endl;
			isOk = false;
		 } else {
			isOk = true;
		 }
	  }
	  valid &= isOk;
   }
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAVTactVmeReader::Close()
{
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i)
	  fRawFileAscii[i].close();
}

//------------------------------------------+-----------------------------------
//! Process
Bool_t TAVTactVmeReader::Process()
{
   Int_t size = (sizeof(MI26_FrameRaw)/4)*3 + 3;
   
   MI26_FrameRaw* data = new MI26_FrameRaw;
   
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Bool_t neof = false;
   // loop over boards
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
      Int_t planeStatus = pConfig->GetStatus(i);
      if (planeStatus == -1) continue;
      
      if (GetSensorEvent(i)) {
         neof |= true;
         ResetFrames();
      
         fIndex    = 0;

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
Bool_t TAVTactVmeReader::GetSensorEvent(Int_t iSensor)
{
   Char_t tmp[255];
   
   fIndex = 0;
   UInt_t timestamp = 0;
   
   // lokking for header
   TString key  = Form("%x", GetKeyHeader(iSensor));
   
   do {
      fRawFileAscii[iSensor] >> tmp;
      TString line = tmp;
      
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
   TString tail  = Form("%x", GetKeyTail(iSensor));
   
   do {
      fRawFileAscii[iSensor] >> tmp;
      TString line = tmp;
      sscanf(tmp, "%x", &data);
      fDataEvent[fIndex++] = data;
      
      if (line.Contains(tail))
         break;
      
   } while (!fRawFileAscii[iSensor].eof());
   
   if (fRawFileAscii[iSensor].eof()) return false;

   fEventSize = fIndex;
   
   if(FootDebugLevel(3)) {
      for (Int_t i = 0; i < fEventSize; ++i)
         printf("Data %08x\n", fDataEvent[i]);
      printf("\n");
   }
   
   return true;
}


// --------------------------------------------------------------------------------------
Bool_t TAVTactVmeReader::GetFrame(Int_t iSensor, MI26_FrameRaw* data)
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
void TAVTactVmeReader::SetRunNumber(const TString& filename)
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
