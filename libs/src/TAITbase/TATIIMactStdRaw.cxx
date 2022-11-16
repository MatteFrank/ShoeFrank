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
: TAITactBaseNtuHit(name, pNtuRaw, pGeoMap, pConfig, pParMap)
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

   // lokking for header
   TString key  = Form("%x", DEITREvent::GetItrHeader());
   TString tail = Form("%x", DEITREvent::GetItrTail());

   do {
      fRawFileAscii >> tmp;
      TString line = tmp;
      line.ToLower();
      
      if (line.Contains(key)) {
         if(FootDebugLevel(1))
            printf("sensor header %s\n", tmp);
         
         fData.push_back(DEITREvent::GetItrHeader());
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
      line.ToLower();
      sscanf(line.Data(), "%x", &data);
      fData.push_back(data);
      fIndex++;
      
      if (line.Contains(tail)) {
         if(FootDebugLevel(1))
            printf("sensor tail   %s\n", tmp);
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
