#ifndef _TAGbaseWCpar_HXX_
#define _TAGbaseWCpar_HXX_

#pragma pack(1) // padding suppression

struct EventData
{
   Int_t    EventNumber;
   Double_t EpochTime;
   UInt_t   Year;
   UInt_t   Month;
   UInt_t   Day;
   UInt_t   Hour;
   UInt_t   Minute;
   UInt_t   Second;
   UInt_t   Millisecond;
   ULong_t  TDCsamIndex;
   Int_t    StoredChannelsN;
};

struct ChannelData
{
   Int_t   channel;
   Int_t   EventIDsamIndex;
   Int_t   FirstCellToPlotsamIndex;
   Short_t Waveform[1024];
};

struct ChannelDataMeas
{
   Int_t    channel;
   Int_t    EventIDsamIndex;
   Int_t    FirstCellToPlotsamIndex;
   Float_t  MeasuredBaseline;
   Float_t  AmplitudeValue;
   Float_t  ComputedCharge;
   Float_t  RiseTimeInstant;
   Float_t  FallTimeInstant;
   Float_t  RawTriggerRate;
   Short_t  Waveform[1024];
};

#pragma pack()

#endif
