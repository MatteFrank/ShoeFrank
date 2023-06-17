/*!
  \file TAGactWCreader.cxx
  \brief   Implementation of TAGactWCreader.
*/

#include "TAGgeoTrafo.hxx"
#include "TAGbaseWCparMap.hxx"
#include "TAGwaveCatcher.hxx"
#include "TAPLntuRaw.hxx"
#include "TACEntuRaw.hxx"
#include "TAPWntuRaw.hxx"

#include "TAGactWCreader.hxx"

/*!
  \class TAGactWCreader
  \brief Get ST/TW raw data from WC. **
*/

const Float_t TAGactWCreader::fgkAdc2mv    = (250./4096.);
const Float_t TAGactWCreader::fgkSize2Evts = 500;

ClassImp(TAGactWCreader);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactWCreader::TAGactWCreader(const char* name,
                               TAGparaDsc* p_WCmap,
                               TAGdataDsc* p_stwd,
                               TAGdataDsc* p_twwd,
                               TAGdataDsc* p_pwwd)
 : TAGactionFile(name, "TAGactWCreader - Unpack WC raw data"),
   fpdatRawSt(p_stwd),
   fpdatRawTw(p_twwd),
   fpdatRawPw(p_pwwd),
   fpParMapWC(p_WCmap),
   fEventsN(0),
   fDebugLevel(0),
   fSoftwareVersion(""),
   fSamplesN(-1),
   fChannelsN(-1),
   fPeriod(0.0),
   fTimeStamp(0),
   fHasMeasurement(false),
   fFileSize(0),
   fFrequency(1)
{
   if (fpdatRawSt)
      AddDataOut(fpdatRawSt, "TAPLntuRaw");
   if (fpdatRawTw)
      AddDataOut(fpdatRawTw, "TACEntuRaw");
   if (fpdatRawPw)
      AddDataOut(fpdatRawPw, "TAPWntuRaw");
}


//------------------------------------------+-----------------------------------
//! Destructor.
TAGactWCreader::~TAGactWCreader()
{}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactWCreader::Process()
{
   if(! DecodeWaveForms()){
      
      SetBitAllDataOut(kEof);
      return false;
   } else {
      SetBit(kValid);
   }
   
   if (fpdatRawSt)
      fpdatRawSt->SetBit(kValid);
   if (fpdatRawTw)
      fpdatRawTw->SetBit(kValid);
   if (fpdatRawPw)
      fpdatRawPw->SetBit(kValid);
 
  return kTRUE;
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAGactWCreader::CreateHistogram()
{
  DeleteHistogram();
  SetValidHistogram(kTRUE);
}

// --------------------------------------------------------------------------------------
Int_t TAGactWCreader::Open(const TString& inputDataFile, Option_t* /*option*/, const TString /*treeName*/, Bool_t /*dscBranch*/)
{
   fInputFile.open(inputDataFile.Data(), ios::binary|ios::in);
   
   if (!fInputFile.is_open()) {
      printf("*** failed to open '%s'\n", inputDataFile.Data());
      return 0;
   }

   fInputFile.seekg (0, ios::end);
   fFileSize = fInputFile.tellg()/(1024.*1024.) + 0.5;
   fInputFile.seekg (0, ios::beg);

   if (fDebugLevel)
      printf("+++ reading file = %s (Size: %lu Mb)...\n", inputDataFile.Data(), fFileSize);

   DecodeHeader();
   
   return 1;
}

//------------------------------------------+-----------------------------------
void TAGactWCreader::DecodeHeader()
{
   // reading
   TString header_line;
   string line;
   
   fHasMeasurement = false;
   
   // HEADER 1 //
   //
   // "=== DATA FILE SAVED WITH SOFTWARE VERSION: V?.??.? ==="
   //
   getline(fInputFile, line, '\n');
   header_line = line.data();
   if (fDebugLevel) printf("[%s]\n", header_line.Data());
   assert(header_line[0] == '=');
   
   Int_t header_version_first = header_line.Last('V');
   fSoftwareVersion = header_line(header_version_first, 7);
   printf("    - data version    = %s\n", fSoftwareVersion.Data());
   
   if (fSoftwareVersion != "V2.9.13" && fSoftwareVersion != "V2.9.15")
      printf("*** unsupported data version\n");
   
   // HEADER 2 //
   // "=== WAVECATCHER SYSTEM OF TYPE ?? WITH ?? CHANNELS AND GAIN: ??? ==="
   getline(fInputFile, line, '\n');
   header_line = line.data();
   if (fDebugLevel) printf("%s\n", header_line.Data());
   
   // HEADER 3 //
   // === Rate coincidence masks ... === Posttrig in ns for SamBlock ... ===
   getline(fInputFile, line, '\n');
   header_line = line.data();
   if (fDebugLevel) printf("%s\n", header_line.Data());
   
   // HEADER 4 //
   // V2.9.13: === DATA SAMPLES [1024] in Volts == NB OF CHANNELS ACQUIRED: 64 == Sampling Period: 312.5 ps == INL Correction: 1
   // V2.9.15: === DATA SAMPLES [1024] in Volts == NB OF CHANNELS ACQUIRED: 64 == Sampling Period: 312.5 ps == INL Correction: 1 == MEASUREMENTS: 0 ===
   getline(fInputFile, line, '\n');
   header_line = line.data();
   if (fDebugLevel) printf("%s\n", header_line.Data());
   
   Int_t nsamples_first = 1+header_line.First('[');
   Int_t nsamples_last = header_line.Last(']');
   TString nsamples_str = header_line(nsamples_first, nsamples_last-nsamples_first);
   
   fSamplesN = nsamples_str.Atoi();
   printf("    - data sample     = %d\n", fSamplesN);
   
   size_t nchannels_first = 10 + line.find("ACQUIRED: ", nsamples_first);
   size_t nchannels_last = line.find_first_of(' ', nchannels_first);
   std::string nchannels_str = line.substr(nchannels_first, nchannels_last-nchannels_first);
   
   fChannelsN = atoi(nchannels_str.data());
   printf("    - nchannels       = %d\n", fChannelsN);
   
   size_t period_first = 7 + line.find("Period: ", nchannels_first);
   size_t period_last = line.find_first_of(' ', nchannels_first);
   string period_str = line.substr(period_first, period_last-period_first);
   
   fPeriod = atof(period_str.data());
   printf("    - sampling Period = %.1f\n", fPeriod);
   
   if (fSoftwareVersion == "V2.9.13") {
      // V2.9.13 has always measurement stored
      // (everything is set to 0 when disabled!)
      fHasMeasurement = true;
   } else if (fSoftwareVersion == "V2.9.15") {
      Int_t has_measurement_first = 14 + header_line.First("MEASUREMENTS: ");
      TString has_measurement_str = header_line(has_measurement_first, 2);
      fHasMeasurement = has_measurement_str.Atoi();
   }
   
   printf("    - measurement     = %d\n", fHasMeasurement);

   fEventsN = fgkSize2Evts*(fFileSize-1)/float(fChannelsN);
   
   if (fEventsN > 100000)      fFrequency = 100000;
   else if (fEventsN > 10000)  fFrequency = 10000;
   else if (fEventsN > 1000)   fFrequency = 1000;
   else if (fEventsN > 100)    fFrequency = 100;
   else if (fEventsN > 10)     fFrequency = 10;
   
   if (fpdatRawSt) {
      TAPLntuRaw* datRawSt = (TAPLntuRaw*)fpdatRawSt->Object();
      datRawSt->SetSoftwareVersion(fSoftwareVersion);
      datRawSt->SetSamplesN(fSamplesN);
      datRawSt->SetChannelsN(fChannelsN);
      datRawSt->SetPeriod(fPeriod);
   }
   
   if (fpdatRawTw) {
      TACEntuRaw* datRawTw = (TACEntuRaw*)fpdatRawTw->Object();
      datRawTw->SetSoftwareVersion(fSoftwareVersion);
      datRawTw->SetSamplesN(fSamplesN);
      datRawTw->SetChannelsN(fChannelsN);
      datRawTw->SetPeriod(fPeriod);
   }
   
   if (fpdatRawPw) {
      TAPWntuRaw* datRawPw = (TAPWntuRaw*)fpdatRawTw->Object();
      datRawPw->SetSoftwareVersion(fSoftwareVersion);
      datRawPw->SetSamplesN(fSamplesN);
      datRawPw->SetChannelsN(fChannelsN);
      datRawPw->SetPeriod(fPeriod);
   }
   
   fWaves.Reserve(fSamplesN);
}

//------------------------------------------+-----------------------------------
Bool_t TAGactWCreader::DecodeWaveForms()
{
   Clear();
   
   TAGbaseWCparMap* parMapWC = (TAGbaseWCparMap*)fpParMapWC->Object();
   
   TAPLntuRaw* datRawSt = 0x0;
   if (fpdatRawSt)
      datRawSt = (TAPLntuRaw*)fpdatRawSt->Object();
   
   TACEntuRaw* datRawTw = 0x0;
   if (fpdatRawTw)
      datRawTw = (TACEntuRaw*)fpdatRawTw->Object();
   
   TAPWntuRaw* datRawPw = 0x0;
   if (fpdatRawPw)
      datRawPw = (TAPWntuRaw*)fpdatRawPw->Object();
   
   if (!fInputFile.read((char*)(&fEventData), sizeof(fEventData))) return false;
   
   if (fDebugLevel)
      printf("%03d has %d channels\n", fEventData.EventNumber, fEventData.StoredChannelsN);

   if (fEventData.EventNumber % fFrequency == 0) {
      printf("\n Event: %d\n", fEventData.EventNumber);
      printf("%04d/%02d/%02d %02d:%02d:%02d",
             fEventData.Year, fEventData.Month, fEventData.Day,
             fEventData.Hour, fEventData.Minute, fEventData.Second);
      printf(".%03d", fEventData.Millisecond);
      printf("\tTDC = %lu\n", fEventData.TDCsamIndex);
   }
      
   fEventsN   = fEventData.EventNumber;
   fTimeStamp = fEventData.TDCsamIndex;
   fChannelsN = fEventData.StoredChannelsN;
   
   if (fDebugLevel)
      printf("Number of channels %d\n", fChannelsN );
   
   for (int ch = 0; ch < fChannelsN; ++ch) {
      Clear();
      ChannelDataMeas dataMeas;
      
      if (fHasMeasurement) {
         // read with 'ChannelDataMeas' struct
         fInputFile.read((char*)(&dataMeas), sizeof(ChannelDataMeas));
         if (fEventData.EventNumber % fFrequency == 0)
            printf("\tEvent TimeStamp = %u\n", dataMeas.EventIDsamIndex);
         
      } else {
         // read with 'ChannelData' struct
         ChannelData data;
         fInputFile.read((char*)(&data), sizeof(ChannelData));
         
         // copy the content into 'ChannelDataMeas' struct
         dataMeas.channel = data.channel;
         dataMeas.EventIDsamIndex = data.EventIDsamIndex;
         dataMeas.FirstCellToPlotsamIndex = data.FirstCellToPlotsamIndex;
         memcpy(dataMeas.Waveform, data.Waveform, 1024*sizeof(short));
      }
      
      if (fDebugLevel)
         printf("- reading channel %d\n", ch);
      
      for (int s = 0; s < fSamplesN; ++s) {
         fWaves.GetVectA(s) = fgkAdc2mv * dataMeas.Waveform[s];
         fWaves.GetVectT(s) = (fPeriod * s)*TAGgeoTrafo::PsToNs(); // ns
      }
      
      if (parMapWC->GetChannelType(ch) == "PL") {
         TAPLrawHit* hit = datRawSt->NewHit(fWaves);
         hit->SetSamplesN(fSamplesN);
      }
      
      if (parMapWC->GetChannelType(ch) == "CE") {
         TACErawHit* hit =  datRawTw->NewHit(fWaves);
         hit->SetSamplesN(fSamplesN);
      }
      
      if (parMapWC->GetChannelType(ch) == "PW") {
         TAPWrawHit* hit =  datRawPw->NewHit(fWaves);
         hit->SetSamplesN(fSamplesN);
      }
   } // for ch
   
   return true;
}

//------------------------------------------+-----------------------------------
void TAGactWCreader::Clear(Option_t* /*opt*/)
{
   fWaves.Clear();
   fWaves.Reserve(fSamplesN);

  return;
}
