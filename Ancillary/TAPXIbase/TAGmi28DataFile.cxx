#include "TAGmi28DataFile.hxx"

#include <vector>
#include <Riostream.h>

ClassImp(TAGmi28DataFile);

const Int_t TAGmi28DataFile::fgkAsicTrigResNbr      = 4;
const Int_t TAGmi28DataFile::fgkMaxFrameRawNbr      = 1140;
const Int_t TAGmi28DataFile::fgkFilePath            = 256;
const Int_t TAGmi28DataFile::fgkMaxStateNbrPerState = 9;
const Int_t TAGmi28DataFile::fgkMaxStateNbr         = 576;
const Int_t TAGmi28DataFile::fgkMaxAsicNbr          = 16;
const Int_t TAGmi28DataFile::fgkMaxTriggerNbr       = 3;
const Int_t TAGmi28DataFile::fgkMaxFramePerAq       = 1800;

//______________________________________________________________________________
TAGmi28DataFile::TAGmi28DataFile()
: TObject(),
  fCurrentPosFile(0),
  fStartFrame(0),
//   fAsicFrameStatus(),
//   fMi28FrameRaw(),
//   fMi28StatesRec(),
//   fMi28Frame(),
  fMi28StatesLine(),
  fFStatesLine(),
  fMi28State(),
  fStates(),
  fFrioFrame(),
  fFrioFrameHeader(fFrioFrame.Header),
  fFrioFrameData(fFrioFrame.Data),
  fFrioFrameList(),
  fFrioTrigger(fFrioFrame.Trigger)
{
//   fAsicFrameStatus.ATrigRes.resize(fgkAsicTrigResNbr); 
//   fMi28FrameRaw.ADataW16.resize(fgkMaxFrameRawNbr);   
//   fMi28StatesRec.AStates.resize(fgkMaxStateNbrPerState);
//   fMi28Frame.AStatesRec.resize(fgkMaxStateNbr);   
  fFrioFrame.Header.AMapsHeader.resize(fgkMaxAsicNbr);
  fFrioFrame.Header.AMapsFrameCnt.resize(fgkMaxAsicNbr);
  fFrioFrame.Header.AMapsDataLength.resize(fgkMaxAsicNbr);
  fFrioFrame.Header.AMapsTrailer.resize(fgkMaxAsicNbr); 
  fFrioFrame.Header.AMapsTrigInfo.resize(fgkMaxTriggerNbr); 
  fFrioFrameList.AFrame.resize(fgkMaxFramePerAq);  
  fFrioFrameList.AFrameSz.resize(fgkMaxFramePerAq);
  
//   printf("header %d \n", fFrioFrameHeader.AMapsHeader.size());
}

//______________________________________________________________________________
TAGmi28DataFile::~TAGmi28DataFile()
{
}

//______________________________________________________________________________
Bool_t TAGmi28DataFile::OpenDataFile(const char* fileName)
{
  fDataFile.open(fileName, std::ios::binary);
  if (fDataFile.fail()) return false;
  fDataFile.seekg(0);
  return true;
}

//______________________________________________________________________________
void TAGmi28DataFile::GoToPosFile(Int_t pos)
{
  fDataFile.seekg(pos);
  fCurrentPosFile = pos;
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::ReadBlocFrame()
{
  fDataFile.seekg(fCurrentPosFile);
  //fStartFrame = fCurrentPosFile;
  ReadFrameInfo();
  ReadFrameHeader();
//   fCurrentPosFile = fDataFile.tellg();
  ReadFrameData();
  //if (fFrioFrame.TrigRecOffset > 0) ReadFrameTrigger();
  
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::SetFramePos(Int_t frameId)
{
  fFrioFrame = fFrioFrameList.AFrame.at(frameId);
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::SetStLineW16(UShort_t state)
{
  fMi28StatesLine.W16 = state;
  fFStatesLine.StateNb = fMi28StatesLine.W16 & 0x000F;
  fFStatesLine.LineAddr = (fMi28StatesLine.W16 & 0x3FF0) >> 4;
  fFStatesLine.Ovf = (fMi28StatesLine.W16 & 0x8000) >> 15;
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::SetStW16(UShort_t state)
{
  fMi28State.W16 = state;
  fStates.HitNb = fMi28State.W16 & 0x0003;
  fStates.ColAddr = (fMi28State.W16 & 0x0FFC) >> 2;
  fStates.NotUsed = (fMi28State.W16 & 0xF000) >> 12;
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::SetTluW32(UInt_t info)
{
  fFrioTluInfo.W32 = info;
  fFrioTluTrigger.TrigCnt = fFrioTluInfo.W32 & 0x0000FFFF;
  fFrioTluTrigger.FrameIdInAcq = (fFrioTluInfo.W32 & 0x07FF0000) >> 16;
  fFrioTluTrigger.EventTakenByDut = (fFrioTluInfo.W32 & 0x08000000) >> 27;
  fFrioTluTrigger.Reserved = (fFrioTluInfo.W32 & 0x70000000) >> 28;
  fFrioTluTrigger.InvalidInfo = (fFrioTluInfo.W32 & 0x80000000) >> 31;
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::SetTriggerW32(UInt_t info)
{
  fTriggerInfo.W32 = info;
  fTrigger.Mi28Line = fTriggerInfo.W32 & 0x000003FF;
  fTrigger.Mi28Frame = (fTriggerInfo.W32 & 0x7FFFFC00) >> 10;
  fTrigger.InvalidInfo = (fTriggerInfo.W32 & 0x80000000) >> 31;
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::ReadFrameInfo()
{
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Tag), sizeof(fFrioFrame.Tag));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.DaqVersion), sizeof(fFrioFrame.DaqVersion));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.TotSz), sizeof(fFrioFrame.TotSz));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.TrigRecOffset), sizeof(fFrioFrame.TrigRecOffset));
  
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::ReadFrameHeader()
{
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.Tag), sizeof(fFrioFrame.Header.Tag));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.AcqStatus), sizeof(fFrioFrame.Header.AcqStatus));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.TrigStatus), sizeof(fFrioFrame.Header.TrigStatus));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.AcqId), sizeof(fFrioFrame.Header.AcqId));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.FrameIdInAcq), sizeof(fFrioFrame.Header.FrameIdInAcq));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.MapsName), sizeof(fFrioFrame.Header.MapsName));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.MapsNb), sizeof(fFrioFrame.Header.MapsNb));
  fDataFile.read(reinterpret_cast<char *>(fFrioFrame.Header.AMapsHeader.data()), fFrioFrame.Header.AMapsHeader.size()*sizeof(fFrioFrame.Header.AMapsHeader.at(0)));
  fDataFile.read(reinterpret_cast<char *>(fFrioFrame.Header.AMapsFrameCnt.data()), fFrioFrame.Header.AMapsFrameCnt.size()*sizeof(fFrioFrame.Header.AMapsFrameCnt.at(0)));
  fDataFile.read(reinterpret_cast<char *>(fFrioFrame.Header.AMapsDataLength.data()), fFrioFrame.Header.AMapsDataLength.size()*sizeof(fFrioFrame.Header.AMapsDataLength.at(0)));
  fDataFile.read(reinterpret_cast<char *>(fFrioFrame.Header.AMapsTrailer.data()), fFrioFrame.Header.AMapsTrailer.size()*sizeof(fFrioFrame.Header.AMapsTrailer.at(0)));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Header.TriggerNb), sizeof(fFrioFrame.Header.TriggerNb));
  fDataFile.read(reinterpret_cast<char *>(fFrioFrame.Header.AMapsTrigInfo.data()), fFrioFrame.Header.AMapsTrigInfo.size()*sizeof(fFrioFrame.Header.AMapsTrigInfo.at(0)));
//   std::cout << "size" << fFrioFrame.Header.AMapsTrigInfo.size() << std::endl;
//   if (fFrioFrame.Header.TriggerNb > 0) std::cout << fFrioFrame.Header.AMapsTrigInfo.at(0) << std::endl;
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::ReadFrameData()
{
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Data.Tag), sizeof(fFrioFrame.Data.Tag));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Data.TotSz), sizeof(fFrioFrame.Data.TotSz));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Data.OneMapsSz), sizeof(fFrioFrame.Data.OneMapsSz));
  fFrioFrame.Data.ADataW32.clear();
  fFrioFrame.Data.ADataW32.resize(fFrioFrame.Data.TotSz/sizeof(fFrioFrame.Data.ADataW32.at(0)));
  fDataFile.read(reinterpret_cast<char *>(fFrioFrame.Data.ADataW32.data()), fFrioFrame.Data.ADataW32.size()*sizeof(fFrioFrame.Data.ADataW32.at(0)));
  //ReadFrameTrigger();
  fCurrentPosFile += fFrioFrame.TotSz;
  
  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::ReadFrameTrigger()
{
  //fDataFile.seekg(fStartFrame + fFrioFrame.TrigRecOffset);
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Trigger.Tag), sizeof(fFrioFrame.Trigger.Tag));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Trigger.TotSz), sizeof(fFrioFrame.Trigger.TotSz));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Trigger.TrigNb), sizeof(fFrioFrame.Trigger.TrigNb));
  fDataFile.read(reinterpret_cast<char *>(&fFrioFrame.Trigger.TrigType), sizeof(fFrioFrame.Trigger.TrigType));
  fFrioFrame.Trigger.ATrig.clear();
  //if (fFrioFrame.Trigger.TrigNb > fgkMaxTriggerNbr) fFrioFrame.Trigger.ATrig.resize(fgkMaxTriggerNbr);
  //else fFrioFrame.Trigger.ATrig.resize(fFrioFrame.Trigger.TrigNb*2);
  //fDataFile.read(reinterpret_cast<char *>(fFrioFrame.Trigger.ATrig.data()), fFrioFrame.Trigger.ATrig.size()*sizeof(fFrioFrame.Trigger.ATrig.at(0)));

  return;
}

//______________________________________________________________________________
void TAGmi28DataFile::ResetFrameList()
{
  fFrioFrameList.AFrame.clear();  
  fFrioFrameList.AFrameSz.clear();
  fFrioFrameList.AFrame.resize(fgkMaxFramePerAq);  
  fFrioFrameList.AFrameSz.resize(fgkMaxFramePerAq);
  
  return;
}