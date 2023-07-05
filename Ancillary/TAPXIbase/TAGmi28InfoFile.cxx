#include "TAGmi28InfoFile.hxx"

ClassImp(TAGmi28InfoFile);

const Int_t TAGmi28InfoFile::fgkBlocInfoMaxSize   = 20;

//______________________________________________________________________________
TAGmi28InfoFile::TAGmi28InfoFile()
: TObject(),
  fTotalBlocSize(0),
  fRefPosFile(0),
  fStreamFileRecInfFile(),
  fStreamFileBlocInf(fStreamFileRecInfFile.ABlocInf)
{
  fStreamFileRecInfFile.ABlocInf.ASpareW32Info.resize(fgkBlocInfoMaxSize);
}

//______________________________________________________________________________
TAGmi28InfoFile::~TAGmi28InfoFile()
{
}

//______________________________________________________________________________
Bool_t TAGmi28InfoFile::OpenRecInfoFile(const char* fileName)
{
  fInfoFile.open(fileName, std::ios::binary);
  if (fInfoFile.fail()) return false;
  fInfoFile.seekg(0);
  return true;
}

//______________________________________________________________________________
void TAGmi28InfoFile::ReadHeaderInfoFile()
{
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.Version), sizeof(fStreamFileRecInfFile.Version));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateCreate.Year), sizeof(fStreamFileRecInfFile.DateCreate.Year));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateCreate.Month), sizeof(fStreamFileRecInfFile.DateCreate.Month));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateCreate.Day), sizeof(fStreamFileRecInfFile.DateCreate.Day));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateCreate.NotUsed), sizeof(fStreamFileRecInfFile.DateCreate.NotUsed));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeCreate.Cent), sizeof(fStreamFileRecInfFile.TimeCreate.Cent));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeCreate.Sec), sizeof(fStreamFileRecInfFile.TimeCreate.Sec));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeCreate.Min), sizeof(fStreamFileRecInfFile.TimeCreate.Min));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeCreate.Hour), sizeof(fStreamFileRecInfFile.TimeCreate.Hour));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateClose.Year), sizeof(fStreamFileRecInfFile.DateClose.Year));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateClose.Month), sizeof(fStreamFileRecInfFile.DateClose.Month));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateClose.Day), sizeof(fStreamFileRecInfFile.DateClose.Day));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.DateClose.NotUsed), sizeof(fStreamFileRecInfFile.DateClose.NotUsed));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeClose.Cent), sizeof(fStreamFileRecInfFile.TimeClose.Cent));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeClose.Sec), sizeof(fStreamFileRecInfFile.TimeClose.Sec));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeClose.Min), sizeof(fStreamFileRecInfFile.TimeClose.Min));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.TimeClose.Hour), sizeof(fStreamFileRecInfFile.TimeClose.Hour));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.FixedBlocSz), sizeof(fStreamFileRecInfFile.FixedBlocSz));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.MaxBlocSz), sizeof(fStreamFileRecInfFile.MaxBlocSz));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.BlocSz), sizeof(fStreamFileRecInfFile.BlocSz));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.BlocNb), sizeof(fStreamFileRecInfFile.BlocNb));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.Dummy), sizeof(fStreamFileRecInfFile.Dummy));
  
  fRefPosFile = fInfoFile.tellg();
  
  return;
}

//______________________________________________________________________________
void TAGmi28InfoFile::ReadBlocInfoFile()
{
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.ABlocInf.Offset), sizeof(fStreamFileRecInfFile.ABlocInf.Offset));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.ABlocInf.Sz), sizeof(fStreamFileRecInfFile.ABlocInf.Sz));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.ABlocInf.SpareW32InfoFormat), sizeof(fStreamFileRecInfFile.ABlocInf.SpareW32InfoFormat));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.ABlocInf.SpareW32InfoNb), sizeof(fStreamFileRecInfFile.ABlocInf.SpareW32InfoNb));
  fInfoFile.read(reinterpret_cast<char *>(fStreamFileRecInfFile.ABlocInf.ASpareW32Info.data()), fStreamFileRecInfFile.ABlocInf.ASpareW32Info.size()*sizeof(fStreamFileRecInfFile.ABlocInf.ASpareW32Info.at(0)));
  fInfoFile.read(reinterpret_cast<char *>(&fStreamFileRecInfFile.ABlocInf.Dummy), sizeof(fStreamFileRecInfFile.ABlocInf.Dummy));
  
  return;
}

//______________________________________________________________________________
void TAGmi28InfoFile::SetTotalBlocSize(UInt_t size)
{
  fTotalBlocSize += size;
  return;
}

//______________________________________________________________________________
void TAGmi28InfoFile::FillListOfPosInfoFile(Int_t acqNbr)
{
  fListOfPosInfoFile.resize(acqNbr);
  fListOfPosInfoFile.at(0) = 0;
  for (Int_t i = 1; i < acqNbr; i++){
    ReadBlocInfoFile();
    SetTotalBlocSize(fStreamFileRecInfFile.ABlocInf.Sz);
    fListOfPosInfoFile.at(i) = fTotalBlocSize;
  }
  return;
}