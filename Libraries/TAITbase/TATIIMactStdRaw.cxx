/*!
 \file TATIIMactStdRaw.cxx
 \brief  Implementation of TATIIMactStdRaw
 */

#include "DEITREvent.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGroot.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"

#include "TATIIMactStdRaw.hxx"

/*!
 \class TATIIMactStdRaw
 \brief    Read raw data from single ITR file (ascii format) from new firmware
 */

//! Class Imp
ClassImp(TATIIMactStdRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TATIIMactStdRaw::TATIIMactStdRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TATIIMactBaseNtuHit(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{

}

//------------------------------------------+-----------------------------------
//! Destructor.
TATIIMactStdRaw::~TATIIMactStdRaw()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TATIIMactStdRaw::Action()
{
   if (GetEvent())
      DecodeEvent();
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Get next event.
Bool_t TATIIMactStdRaw::GetEvent()
{
   fData.clear();
   Char_t tmp[255];
   fIndex = 0;

   UInt_t aCol = 0;
   UInt_t aLine = 0;
   UInt_t value = 0;
   ULong64_t timeStamp;
   
   // lokking for header
   TString key  = Form("%x", fgkEventKey);
   TString tail = Form("%x", fgkEventTail);

   do {
      fRawFileAscii.getline(tmp, 255, '\n');
      TString line = tmp;
      
      if (line.Contains(key)) {
         if(FootDebugLevel(1))
            printf("Event header %s\n", tmp);
         
         fData.push_back(fgkEventKey);
         fIndex++;
         break;
      }
   } while (!fRawFileAscii.eof());
   
   if (fRawFileAscii.eof()) return false;
   
   // look for trailer
   UInt_t data;

   do {
      fRawFileAscii.getline(tmp, 255, '\n');
      
      TString line = tmp;
      line.ToLower();

      if (line.Contains(tail)) {
         if(FootDebugLevel(1))
            printf("event tail   %s\n", tmp);
         fEventSize = fIndex;
         return true;
      }
      
      sscanf(line.Data(), "%llu %d %d %d %d", &timeStamp, &fTriggerNumber, &aCol, &aLine, &value);
      
      fData.push_back(timeStamp >> 32);
      fIndex++;
      fData.push_back(timeStamp & 0xFFFFFFFF);
      fIndex++;
      fData.push_back(fTriggerNumber);
      fIndex++;
      fData.push_back(aCol);
      fIndex++;
      fData.push_back(aLine);
      fIndex++;
      fData.push_back(value);
      fIndex++;

      if(FootDebugLevel(3))
         printf("Trig#: %d TS#: %llu line: %u col: %u value: %u\n", fTriggerNumber, timeStamp, aLine, aCol, value);

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
//!
//! \param[in] name action name
//! \param[in] opt open file options
//! \param[in] treeName name of tree in file
//! \param[in] dscBranch flag for object descriptor
Int_t TATIIMactStdRaw::Open(const TString& name, Option_t* opt, const TString /*treeName*/, Bool_t /*dscBranch*/)
{
   TString inputFileName;
      
   Bool_t valid = false;
   
   // Close any previous open file
   if( fRawFileAscii.is_open() && !fRawFileAscii.eof()) {
      valid = true;
   } else {
      fRawFileAscii.close();
      inputFileName = name;
      
      fRawFileAscii.open(inputFileName.Data());
      if( fRawFileAscii.fail() ) { // end the reading if file opening failed
         cout << endl << "TATIIMactStdRaw::Open(), cannot open file " << inputFileName.Data() << endl;
         valid = false;
      } else {
         valid = true;
      }
   }
   
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TATIIMactStdRaw::Close()
{
      fRawFileAscii.close();
}
