/*!
 \file  TAITactStdDaqRaw.cxx
 \brief Implementation of TAITactStdDaqRaw
 */

#include "DECardEvent.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGroot.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"

#include "TAITactStdDaqRaw.hxx"

/*!
 \class TAITactStdDaqRaw
 \brief Get ITR raw data from single file (binary format)
 */

//! Class Imp
ClassImp(TAITactStdDaqRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAITactStdDaqRaw::TAITactStdDaqRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAITactBaseNtuHit(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{
   TAITparGeo*  pGeoPar = (TAITparGeo*)  fpGeoMap->Object();
   Int_t size = ((sizeof(MI26_FrameRaw)/4)*3 + 3)*pGeoPar->GetSensorsN(); // 3 frame per event and 3 header word for each sensor
   fData.reserve(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactStdDaqRaw::~TAITactStdDaqRaw()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAITactStdDaqRaw::Action()
{
   if (GetEvent())
      DecodeEvent();
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Get next event.
Bool_t TAITactStdDaqRaw::GetEvent()
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
//! Open binary data sources.
//!
//! \param[in] name action name
//! \param[in] opt open file options
//! \param[in] treeName name of tree in file
//! \param[in] dscBranch flag for object descriptor 
Int_t TAITactStdDaqRaw::Open(const TString& name, Option_t* opt, const TString /*treeName*/,  Bool_t /*dscBranch*/)
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
         cout << endl << "TAITactStdDaqRaw::Open(), cannot open file " << inputFileName.Data() << endl;
         valid = false;
      } else {
         valid = true;
      }
   }
   
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAITactStdDaqRaw::Close()
{
      fDaqFile.close();
}

// --------------------------------------------------------------------------------------
//! Set run number from file
//!
//! \param[in] filename input daq file name
void TAITactStdDaqRaw::SetRunNumber(const TString& filename)
{
   TString name(filename);
   Int_t pos2 = name.First(".");
   Int_t len  = name.Length();
   
   TString tmp = name(pos2+1, len-4);
   fRunNumber = tmp.Atoi();
   gTAGroot->SetRunNumber(fRunNumber);
}
