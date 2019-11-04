/*!
 \file
 \brief   Implementation of TAVTactStdDaqRaw.
 */

#include "DECardEvent.hh"

#include "GlobalPar.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAVTactStdDaqRaw.hxx"

/*!
 \class TAVTactStdDaqRaw TAVTactStdDaqRaw.hxx "TAVTactStdDaqRaw.hxx"
 \brief Get vertex raw data from re-synchronized sensors in single file (binary format)**
 */

ClassImp(TAVTactStdDaqRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAVTactStdDaqRaw::TAVTactStdDaqRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseNtuRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{
   TAVTparGeo*  pGeoPar = (TAVTparGeo*)  fpGeoMap->Object();
   Int_t size = ((sizeof(MI26_FrameRaw)/4)*3 + 3)*pGeoPar->GetSensorsN(); // 3 frame per event and 3 header word for each sensor
   fData.reserve(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactStdDaqRaw::~TAVTactStdDaqRaw()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAVTactStdDaqRaw::Action()
{
   if (GetEvent())
      DecodeEvent();
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Get next event.
Bool_t TAVTactStdDaqRaw::GetEvent()
{
   fData.clear();
   
   UInt_t words;
   
   fDaqFile.read((char*) &words, sizeof(UInt_t));
   fEventSize = words-1;
   
   fData.resize(fEventSize);
   fDaqFile.read( (char*) &fData[0], fEventSize*sizeof(UInt_t));
   
   if(FootDebugLevel(3)) {
      printf("size %d\n", fEventSize);
      for (Int_t i = 0; i < fEventSize; ++i) {
         if (i == 10) {
            printf("\n");
         } else {
            if (i % 10 == 0) printf("\n");
         }
         printf("%08x ", fData[i]);
      }
      printf("\n");
   }

   return true;

}

//------------------------------------------+-----------------------------------
//! Open ascii data sources.
Int_t TAVTactStdDaqRaw::Open(const TString& name, Option_t* opt)
{
   TString inputFileName;
   
   static Bool_t first = true;
   if (first)
      SetRunNumber(name);
   else
      first = false;
   
   Bool_t valid = false;
   
   // Close any previous open file
   if( fDaqFile.is_open() && !fDaqFile.eof()) {
      valid = true;
   } else {
      fDaqFile.close();
      inputFileName = name;
      
      fDaqFile.open(inputFileName.Data(), ios::binary);
      if( fDaqFile.fail() ) { // end the reading if file opening failed
         cout << endl << "TAVTactStdDaqRaw::Open(), cannot open file " << inputFileName.Data() << endl;
         valid = false;
      } else {
         valid = true;
      }
   }
   
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAVTactStdDaqRaw::Close()
{
      fDaqFile.close();
}

// --------------------------------------------------------------------------------------
void TAVTactStdDaqRaw::SetRunNumber(const TString& filename)
{
   TString name(filename);
   Int_t pos2 = name.First(".");
   Int_t len  = name.Length();
   
   TString tmp = name(pos2+1, len-4);
   fRunNumber = tmp.Atoi();
   gTAGroot->SetRunNumber(fRunNumber);
}
