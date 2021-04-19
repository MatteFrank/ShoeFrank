/*!
  \file
  \brief   Implementation of TAVTactVmeWriter.
*/

#include <limits.h>

#include "DECardEvent.hh"

#include "TAGrecoManager.hxx"
#include "TAGroot.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTactVmeWriter.hxx"

/*!
  \class TAVTactVmeWriter 
  \brief Read stand alone files (ascii format) and write them in a single DAQ file (binary format) **
*/

ClassImp(TAVTactVmeWriter);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactVmeWriter::TAVTactVmeWriter(const char* name, TAGdataDsc* pDatRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
 : TAVTactVmeReader(name, pDatRaw, pGeoMap, pConfig, pParMap),
   fDaqSize(0)
{
   SetTitle("TAVTactVmeWriter - writer for VME reader");
  
   Int_t size = (sizeof(MI26_FrameRaw)/4)*3 + 3; // 3 frame per event and 3 header word for each sensor
   fDaqEvent.reserve(size*4);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactVmeWriter::~TAVTactVmeWriter()
{
}

//------------------------------------------+-----------------------------------
//! Open ascii data sources.
Int_t TAVTactVmeWriter::Open(const TString& name, Option_t* opt)
{
   // open reading
   Bool_t valid = TAVTactVmeReader::Open(name, opt);
   
   // open writing
   fDaqFile.open(Form("data_built.%d.vtx.daq.VTX.3.dat", fRunNumber), ios::out|ios::binary);
   
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAVTactVmeWriter::Close()
{
   // Closing input files
   TAVTactVmeReader::Close();
   
   // Closing output file
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
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      Int_t planeStatus = pConfig->GetStatus(i);
      if (planeStatus == -1) continue;
      
      if (GetSensorEvent(i)) {
         neof |= true;
      
         // reset sensor array
         fIndex = 0;
         memset(fDataEvent, 0, size);
      } else {
         SetBit(kEof);
         SetBitAllDataOut(kEof);
      }
   }

   // Fill DAQ event
   WriteDaqEvent();
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);

   
   if (neof)
      return true;
   else
      return false;
      
}

// private method

// --------------------------------------------------------------------------------------
void TAVTactVmeWriter::WriteDaqEvent()
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
void TAVTactVmeWriter::FillDaqEvent()
{
   // Fill DAQ
   fDaqSize += fEventSize;
   fDaqEvent.insert(fDaqEvent.end(), fDataEvent, fDataEvent+fEventSize);
}
