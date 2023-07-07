/*!
 \file TAMPactAscReader.cxx
 \brief    Read raw data from single VTX file ()ascii format) from new firmware
 */

#include "TAGrecoManager.hxx"
#include "TAGroot.hxx"
#include "TAMPparGeo.hxx"
#include "TAMPparConf.hxx"

#include "TAMPactAscReader.hxx"

/*!
 \class TAMPactAscReader TAMPactAscReader.hxx "TAMPactAscReader.hxx"
 \brief Read raw data from single VTX file ()ascii format) from new firmware
 */

//! Class Imp
ClassImp(TAMPactAscReader);


//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAMPactAscReader::TAMPactAscReader(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAGactionFile(name, "Monopix2 - raw data reader"),
   fpNtuRaw(pNtuRaw),
   fpGeoMap(pGeoMap),
   fpConfig(pConfig),
   fpParMap(pParMap)
{
   Int_t size = 5000;
   fData.reserve(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMPactAscReader::~TAMPactAscReader()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAMPactAscReader::Action()
{
   GetEvent();
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Get next event.
Bool_t TAMPactAscReader::GetEvent()
{
   fData.clear();
   Char_t tmp[255];
   fIndex = 0;

   // lokking for header
   TString keyEvent("=== ");
   TString keyDetector("--- ");

   // search beginning of event
   do {
      fRawFileAscii.getline(tmp, 255);
      TString line = tmp;
      
      if (line.Contains(keyEvent)) {
         fIndex++;
         break;
      }
   } while (!fRawFileAscii.eof());
   
   if (fRawFileAscii.eof()) return false;
   
   // look for trailer
   UInt_t data;
   TString line;
   
   Bool_t ok = true;


      //3 lines skipped
      fRawFileAscii.getline(tmp, 255);
      fRawFileAscii.getline(tmp, 255);
      fRawFileAscii.getline(tmp, 255);
      
      // event number
      fRawFileAscii.getline(tmp, 255);
      line = tmp;
      Int_t pos = line.First(":");
      
      TString sEventNb(line(0, pos-1));
      data = sEventNb.Atoi();

      fData.push_back(data);
      fIndex++;
      // sensor number
     
      ok = true;

      do {
         ok = DecodeSensor();
         printf("%d\n", ok);
      } while(ok);
         
      if (fRawFileAscii.eof())
         return true;

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
//! Decode sensor
Bool_t TAMPactAscReader::DecodeSensor()
{
   TString keyDetector("--- ");
   TString keyEvent("=== ");
   struct Pixel_t{
      UInt_t col, line;
      UInt_t le, fe;
      Float_t ts;
   };
   
   Pixel_t pixel;
   vector<Pixel_t> vec;
   streampos oldpos;
   
   // read sensor number
   Char_t tmp[255];
   //oldpos = fRawFileAscii.tellg();  // stores the position
   fRawFileAscii.getline(tmp, 255);
   TString line = tmp;
   if (line.Contains("Adenium_") || line.Contains("Monopix2_")) {
      Int_t pos = line.First("_");
      TString name(pos+1, 3);
      UInt_t sensorId = name.Atoi();
      fData.push_back(sensorId);
   } else {
    //  fRawFileAscii.seekg(oldpos);
      return false;
   }
   
   do {
      oldpos = fRawFileAscii.tellg();  // stores the position
      fRawFileAscii.getline(tmp, 255);
      line = tmp;
      printf("%s\n", tmp);
      if (line.Contains(keyEvent)) return false;
      
      Int_t pos = line.First(' ');
      TString sPix(line(pos+1, line.Length()-pos));
      sscanf(sPix.Data(), "%u, %u, %u, %u, %f", &pixel.col, &pixel.line, &pixel.le, &pixel.fe, &pixel.ts);
      //   vec.push_back(pixel);
   } while (!line.Contains(keyDetector));
   
//   UInt_t size = vec.size();
//   if (size > 0) {
//      fData.push_back(size);
//
//      for (auto & pix : vec) {
//         fData.push_back(pix.col);
//         fData.push_back(pix.line);
//         fData.push_back(pix.le);
//         fData.push_back(pix.fe);
//      }
//   }
   
   // go back one line
   fRawFileAscii.seekg(oldpos);

   return true;
}

//------------------------------------------+-----------------------------------
//! Open ascii data sources.
//!
//! \param[in] name action name
//! \param[in] opt open file options
//! \param[in] treeName name of tree in file
//! \param[in] dscBranch flag for object descriptor
Int_t TAMPactAscReader::Open(const TString& name, Option_t* opt, const TString /*treeName*/, Bool_t /*dscBranch*/)
{
   static Bool_t first = true;
   if (first)
      SetRunNumber(name);
   else
      first = false;
   
   Bool_t valid = false;

   
   // Close any previous open file
   if( fRawFileAscii.is_open() && !fRawFileAscii.eof()) {
      valid = true;
   } else {
      fRawFileAscii.close();
      fRawFileAscii.open(name.Data());
      if( fRawFileAscii.fail() ) { // end the reading if file opening failed
         cout << endl << "TAMPactAscReader::Open(), cannot open file " << name.Data() << endl;
         valid = false;
      } else {
         valid = true;
      }
   }
   
   return valid;
}

//------------------------------------------+-----------------------------------
//! Close input file.
void TAMPactAscReader::Close()
{
      fRawFileAscii.close();
}

// --------------------------------------------------------------------------------------
//! Set run number from file
//!
//! \param[in] filename input daq file name
void TAMPactAscReader::SetRunNumber(const TString& filename)
{
   TString name(filename);
   Int_t pos1 = name.Last('/');
   Int_t pos2 = name.First("_");
   Int_t len  = name.Length();
   
  // fPrefixName = name(0, pos1);
   
   TString tmp = name(pos2+1, len-pos2);
   fRunNumber = tmp.Atoi();
   gTAGroot->SetRunNumber(fRunNumber);
}
