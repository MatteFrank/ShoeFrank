/*!
 \file TAGactDaqReader.cxx
 \brief  Interface for DAQ file reader
 */

#include "TAGactDaqReader.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAGactDaqReader
 \brief  Interface for DAQ file reader
 */

ClassImp(TAGactDaqReader);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactDaqReader::TAGactDaqReader(const char* name, TAGdataDsc* p_datdaq)
  : TAGactionFile(name, "TAGactDaqReader - DAQ file reader", "READ"),
   fDaqFileReader(new EventReader()),
   fDaqEvent(p_datdaq),
   fDaqFileIndex(0),
   fDaqFileChain(false)
{
   AddDataIn(p_datdaq, "TAGdaqEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactDaqReader::~TAGactDaqReader()
{
   delete fDaqFileReader;
}

//------------------------------------------+-----------------------------------
// Open data source.
Int_t TAGactDaqReader::Open(const TString& name, Option_t* option, const TString, Bool_t )
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
      if (name.EndsWith(".data")) {
        Int_t pos = name.Last('.');
        pos -= 4;
        TString tmp = name(0, pos);
        fCurFileName = tmp + Form("%04d", fDaqFileIndex) + ".data";
      } else if(name.EndsWith(".data.moved")) {
        Int_t pos = name.Last('.');
        pos -= 9;
        TString tmp = name(0, pos);
        fCurFileName = tmp + Form("%04d", fDaqFileIndex) + ".data.moved";
      } else
        Error("Open()", "wrong file extension file, must be .data or .data.moved");
   }

   Int_t b_bad = 0;

   std::string filename ( fCurFileName.Data() );
   fDaqFileReader->openFile( filename );
   if ( !fDaqFileReader->getIsOpened() ) {
      Warning("Open()", "Cannot open next file %s, stop reading", name.Data());
      b_bad = -1;
   }

   if( !fDaqFileReader->endOfFileReached() ) {
      fDaqFileReader->readFileHeader();
      fDaqFileHeader = fDaqFileReader->getFileHeader();
      if (fDaqFileHeader)
         fDaqFileHeader->printData();
   }


   return b_bad;
}

//------------------------------------------+-----------------------------------
// Close input file.
void TAGactDaqReader::Close()
{
   fDaqFileReader->closeFile();
}

//------------------------------------------+-----------------------------------
// Returns \a true if an input file or connection is open.
Bool_t TAGactDaqReader::IsOpen() const
{
   return fDaqFileReader->getIsOpened();
}

//------------------------------------------+-----------------------------------
// Reset
void TAGactDaqReader::SkipEvents(Int_t nEvents)
{
   for (Int_t i = 0; i < nEvents; ++i)
      fDaqFileReader->skipEvent();
}

//------------------------------------------+-----------------------------------
// Process Reader.
Bool_t TAGactDaqReader::Process()
{
  if (Valid()) return kTRUE;
  if (IsZombie()) return kFALSE;

   if (!fDaqFileReader->getIsOpened())
      return false;
   
   TAGdaqEvent* datDaq = (TAGdaqEvent*)  fDaqEvent->Object();

   datDaq->Clear();

   fDaqFileReader->getNextEvent();

   // Global event information
   InfoEvent* evInfo = fDaqFileReader->getInfoEvent();
   datDaq->SetInfoEvent(evInfo);

   // Trigger data
   TrgEvent*  evTrg  = fDaqFileReader->getTriggerEvent();
   datDaq->SetTrgEvent(evTrg);

   if(FootDebugLevel(1))
      printf("DAQ trigger %u\n", evTrg->triggerCounter);

   // TDC # 0 and # 1 for BM
   const TDCEvent* evTDC0 = static_cast<const TDCEvent*>(fDaqFileReader->getFragmentID(dataV1190 | 0x30));
   if (evTDC0)
      datDaq->AddFragment(evTDC0);

   const TDCEvent* evTDC1 = static_cast<const TDCEvent*>(fDaqFileReader->getFragmentID(dataV1190 | 0x31));
   if (evTDC1)
      datDaq->AddFragment(evTDC1);

   // vertex
   const DECardEvent* evVTX = static_cast<const DECardEvent*>(fDaqFileReader->getFragmentID(dataVTX | 0x30));
   if (evVTX)
      datDaq->AddFragment(evVTX);

   //MSD 1st station
   const DEMSDEvent* evMSD0 = static_cast<const DEMSDEvent*>(fDaqFileReader->getFragmentID(dataMSD | 0x30));
   if (evMSD0)
      datDaq->AddFragment(evMSD0);

   //MSD 2nd station
   const DEMSDEvent* evMSD1 =static_cast<const DEMSDEvent*>(fDaqFileReader->getFragmentID(dataMSD | 0x31));
   if (evMSD1)
      datDaq->AddFragment(evMSD1);

   //MSD 3rd station
   const DEMSDEvent* evMSD2 = static_cast<const DEMSDEvent*>(fDaqFileReader->getFragmentID(dataMSD | 0x32));
   if (evMSD2)
      datDaq->AddFragment(evMSD2);

   // WD for ST, TW and CA
   const WDEvent* evWD = static_cast<const WDEvent*>(fDaqFileReader->getFragmentID(dataWD | 0x30));
   if (evWD)
      datDaq->AddFragment(evWD);

   if (fDaqFileReader->endOfFileReached()) {
      if (fDaqFileChain) {
         Close();
        if(Open(fCurFileName, "chain") ==  -1) return false;
      } else
         return false;
   }

   if( fDaqFileReader->check()) {
      fDaqEvent->SetBit(kValid);
      return true;
   }

   fDaqEvent->SetBit(kValid);

  return true;
}
