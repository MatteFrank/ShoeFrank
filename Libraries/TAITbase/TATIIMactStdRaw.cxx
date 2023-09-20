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
: TATIIMactBaseNtuHit(name, pNtuRaw, pGeoMap, pConfig, pParMap),
  fDaqFileIndex(-1),
  fDaqFileChain(false)
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
   

   if (fRawFileAscii.eof()) {
      if (fDaqFileChain) {
         Close();
         if(Open(fCurFileName, "chain") == -1) return false;
      } else
         return false;
   }
   
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
   
   // TS
   fRawFileAscii.getline(tmp, 255, '\n');
   TString line1 = tmp;
   fTimeStamp = line1.Atoll();
   
   // Trigger
   fRawFileAscii.getline(tmp, 255, '\n');
   line1 = tmp;
   fTriggerNumber = line1.Atof();
   
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
      
      sscanf(line.Data(), "%d %d %d", &aCol, &aLine, &value);
      
      fData.push_back(aCol);
      fIndex++;
      fData.push_back(aLine);
      fIndex++;
      fData.push_back(value);
      fIndex++;

      if(FootDebugLevel(3))
         printf("Trig#: %d TS#: %llu col: %u line: %u value: %u\n", fTriggerNumber, fTimeStamp, aCol, aLine, value);

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
Int_t TATIIMactStdRaw::Open(const TString& name, Option_t* option, const TString /*treeName*/, Bool_t /*dscBranch*/)
{
   fCurFileName = name;
   
   TString opt(option);
   opt.ToLower();
   
   if (opt.Contains("chain")) {
      fDaqFileChain = true;
      fDaqFileIndex++;
   }
      
   // all hard coded for the moment
   if (fDaqFileChain) {
      if (name.EndsWith(".dat")) {
         Int_t pos = name.Last('.');
         pos -= 1;
         TString tmp = name(0, pos);
         fCurFileName = tmp + Form("%d", fDaqFileIndex) + ".dat";
      } else
         Error("Open()", "wrong file extension file, must be .dat");
   }
   
   Int_t b_bad = 0;
   
   Info("Open","File: %s with option::%s", fCurFileName.Data(), opt.Data());
   
   fRawFileAscii.open(fCurFileName.Data());
   if( fRawFileAscii.fail() ) { // end the reading if file opening failed
      Warning("Open()", "Cannot open next file %s, stop reading", name.Data());
      b_bad = -1;
   } else {
      b_bad = 0;
   }
   
   return b_bad;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TATIIMactStdRaw::Close()
{
      fRawFileAscii.close();
}
