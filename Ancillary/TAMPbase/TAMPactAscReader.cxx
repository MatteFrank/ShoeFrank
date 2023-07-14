/*!
 \file TAMPactAscReader.cxx
 \brief    Read raw data from single VTX file ()ascii format) from new firmware
 */
#include "TH2F.h"

#include "TAGrecoManager.hxx"
#include "TAGroot.hxx"
#include "TAMPparGeo.hxx"
#include "TAMPparConf.hxx"
#include "TAMPparMap.hxx"

#include "TAVTntuHit.hxx"

#include "TAMPactAscReader.hxx"

/*!
 \class TAMPactAscReader TAMPactAscReader.hxx "TAMPactAscReader.hxx"
 \brief Read raw data from single VTX file ()ascii format) from new firmware
 */

//! Class Imp
ClassImp(TAMPactAscReader);

const TString TAMPactAscReader::fgkKeyEvent    = "===";
const TString TAMPactAscReader::fgkKeyDetector = "---";


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
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMPactAscReader::~TAMPactAscReader()
{   
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMPactAscReader::CreateHistogram()
{
   DeleteHistogram();
   TAMPparGeo* pGeoMap = (TAMPparGeo*) fpGeoMap->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      Int_t type       = pGeoMap->GetSensorPar(i).TypeIdx;
      TString typeName = pGeoMap->GetTypePar(type).TypeName;
      fpHisPixelMap[i] = new TH2F(Form("mpPixelMap%d", i+1), Form("%s - pixel map for sensor %d", typeName.Data(), i+1),
                                  pGeoMap->GetPixelsNy(type), 0, pGeoMap->GetPixelsNy(type),
                                  pGeoMap->GetPixelsNx(type), 0, pGeoMap->GetPixelsNx(type));
      fpHisPixelMap[i]->SetStats(kFALSE);
      AddHistogram(fpHisPixelMap[i]);
   }
   
   SetValidHistogram(kTRUE);
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
   Char_t tmp[255];
   fIndex = 0;

   // search beginning of event
   do {
      fRawFileAscii.getline(tmp, 255);
      TString line = tmp;
      
      if (line.Contains(fgkKeyEvent)) {
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
   
   fIndex++;
   
   ok = true;
   
   do {
      ok = DecodeSensor();
   } while(ok);
   
   if (fRawFileAscii.eof())
      return true;
   
   if (fRawFileAscii.eof()) return false;
   
   fEventSize = fIndex;
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Decode sensor
Bool_t TAMPactAscReader::DecodeSensor()
{
   TAMPparMap* pParMap = (TAMPparMap*)fpParMap->Object();

   UInt_t aCol, aLine;
   UInt_t le, fe;
   Float_t ts;

   streampos oldpos;
   
   // read sensor number
   Char_t tmp[255];
   fRawFileAscii.getline(tmp, 255);
 
   UInt_t sensorId = -1;
   TString line = tmp;
   if (line.Contains(fgkKeyDetector)) {
      TString line = TAGparTools::Remove(tmp, fgkKeyDetector[0], true);
      sensorId = pParMap->GetChannel(line);
   } else {
      return false;
   }
   
   do {
      oldpos = fRawFileAscii.tellg();  // stores the position
      fRawFileAscii.getline(tmp, 255);
      line = tmp;
      if (line.Contains(fgkKeyEvent)) return false;
      
      Int_t pos = line.First(' ');
      TString sPix(line(pos+1, line.Length()-pos));
      sscanf(sPix.Data(), "%u, %u, %u, %u, %f", &aCol, &aLine, &le, &fe, &ts);
      
      Int_t ToT = -1;
      if (fe-le < 1)
         ToT = le;
      else
         ToT = fe-le;
         
      if (ToT < 1) ToT = 1;
      
      AddPixel(sensorId, ToT, aLine, aCol);
               
   } while (!line.Contains(fgkKeyDetector));
      
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
//! Add pixel to container
//!
//! \param[in] iSensor sensor index
//! \param[in] value pixel value
//! \param[in] aLine line id
//! \param[in] aColumn column id
void TAMPactAscReader::AddPixel( Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn)
{
   // Add a pixel to the vector of pixels
   // require the following info
   // - input = number of the sensors
   // - value = analog value of this pixel
   // - line & column = position of the pixel in the matrix
   
   TAVTntuHit*  pNtuRaw = (TAVTntuHit*)  fpNtuRaw->Object();
   TAMPparGeo*  pGeoMap = (TAMPparGeo*)  fpGeoMap->Object();
   TAMPparConf* pConfig = (TAMPparConf*) fpConfig->Object();
   
   if (pConfig->IsDeadPixel(iSensor, aLine, aColumn)) return;
   
   TAVThit* pixel = (TAVThit*)pNtuRaw->NewPixel(iSensor, value, aLine, aColumn);
   Int_t type = pGeoMap->GetSensorPar(iSensor).TypeIdx;
   double v   = pGeoMap->GetPositionV(aLine, type);
   double u   = pGeoMap->GetPositionU(aColumn, type);
   TVector3 pos(u,v,0);
   pixel->SetPosition(pos);
   
   if (ValidHistogram())
      FillHistoPixel(iSensor, aLine, aColumn);
}

// --------------------------------------------------------------------------------------
//! Fill histogram related to pixel
//!
//! \param[in] planeId sensor index
//! \param[in] aLine line id
//! \param[in] aColumn column id
void TAMPactAscReader::FillHistoPixel(Int_t planeId, Int_t aLine, Int_t aColumn)
{
   fpHisPixelMap[planeId]->Fill(aLine, aColumn);
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
