/*!
 \file
 \brief   Implementation of TAVTactStdRaw.
 */

#include "DECardEvent.hh"

#include "GlobalPar.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAVTactStdRaw.hxx"

/*!
 \class TAVTactStdRaw TAVTactStdRaw.hxx "TAVTactStdRaw.hxx"
 \brief Read raw data from single VTX file ()ascii format) from new firmware **
 */


TString TAVTactStdRaw::fgDefaultFolderName = "run_";
TString TAVTactStdRaw::fgDefaultExtName    = ".ZS";

//------------------------------------------+-----------------------------------
//! Default constructor.

TAVTactStdRaw::TAVTactStdRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseNtuRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{
   TAVTparGeo*  pGeoPar = (TAVTparGeo*)  fpGeoMap->Object();

   fBaseName ="data_FPGA_Mouser993P0160_V1_ch";

   Int_t size = ((sizeof(MI26_FrameRaw)/4)*3 + 3)*pGeoPar->GetSensorsN(); // 3 frame per event and 3 header word for each sensor
   fData.reserve(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactStdRaw::~TAVTactStdRaw()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAVTactStdRaw::Action()
{
   if (GetEvent())
      DecodeEvent();
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Get next event.
Bool_t TAVTactStdRaw::GetEvent()
{
   fData.clear();
   Char_t tmp[255];
   fIndex = 0;

   // lokking for header
   TString key  = Form("%x", DECardEvent::GetVertexHeader());
   TString tail = Form("%x", DECardEvent::GetVertexTail());

   do {
      fRawFileAscii >> tmp;
      TString line = tmp;
      
      if (line.Contains(key)) {
         if(FootDebugLevel(1))
            printf("sensor header %s %d\n", tmp, (int) fRawFileAscii.tellg()/9+1);
         
         fData.push_back(DECardEvent::GetVertexHeader());
         fIndex++;
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
      fData.push_back(data);
      fIndex++;
      
      if (line.Contains(tail)) {
         break;
      }
      
      if (line.Contains(key)) {
         Int_t pos =  (int) fRawFileAscii.tellg();
         fRawFileAscii.seekg(pos-1);
         fIndex--;
         if(FootDebugLevel(1))
            printf("Find unexpected key %s\n", line.Data());
         return false;
      }

   } while (!fRawFileAscii.eof());
   
   if (fRawFileAscii.eof()) return false;
   
   fEventSize = fIndex;
   
   if(FootDebugLevel(3)) {
      for (Int_t i = 0; i < fEventSize; ++i)
         printf("Data %08x\n", fData[i]);
      printf("\n");
   }
   
   return true;

}

//------------------------------------------+-----------------------------------
//! Open ascii data sources.
Int_t TAVTactStdRaw::Open(const TString& name, Option_t* opt)
{
   TString inputFileName;
   
   static Bool_t first = true;
   if (first)
      SetRunNumber(name);
   else
      first = false;
   
   Bool_t valid = false;
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Int_t i = 5;
   // Close any previous open file
   if( fRawFileAscii.is_open() && !fRawFileAscii.eof()) {
      valid = true;
   } else {
      fRawFileAscii.close();
      inputFileName = Form("%s/%s%04d/800%d_%s%d%s", fPrefixName.Data(), fgDefaultFolderName.Data(), fRunNumber, i, fBaseName.Data(), i, fgDefaultExtName.Data());
      
      fRawFileAscii.open(inputFileName.Data());
      if( fRawFileAscii.fail() ) { // end the reading if file opening failed
         cout << endl << "TAVTactStdRaw::Open(), cannot open file " << inputFileName.Data() << endl;
         valid = false;
      } else {
         valid = true;
      }
   }
   
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAVTactStdRaw::Close()
{
      fRawFileAscii.close();
}

// --------------------------------------------------------------------------------------
void TAVTactStdRaw::SetRunNumber(const TString& filename)
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
