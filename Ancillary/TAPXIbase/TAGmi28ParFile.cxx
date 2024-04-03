#include "TAGmi28ParFile.hxx"
#include "TAGmi28DataFile.hxx"
#include <Riostream.h>
ClassImp(TAGmi28ParFile);

//______________________________________________________________________________
TAGmi28ParFile::TAGmi28ParFile()
: TObject(),
  fFrioRunCont()
{
  fFrioRunCont.ParDestDir        = new Char_t[TAGmi28DataFile::fgkFilePath];
  fFrioRunCont.ParFileNamePrefix = new Char_t[TAGmi28DataFile::fgkFilePath];
  fFrioRunCont.ParJtagFileName   = new Char_t[TAGmi28DataFile::fgkFilePath];
  fFrioRunCont.InfConfFileName   = new Char_t[TAGmi28DataFile::fgkFilePath];
  fFrioRunCont.InfDataFileName   = new Char_t[TAGmi28DataFile::fgkFilePath];
}

//______________________________________________________________________________
TAGmi28ParFile::~TAGmi28ParFile()
{
  for(Int_t i = 0; i < TAGmi28DataFile::fgkFilePath; i++){
    delete[] fFrioRunCont.ParDestDir;
    delete[] fFrioRunCont.ParFileNamePrefix;
    delete[] fFrioRunCont.ParJtagFileName;
    delete[] fFrioRunCont.InfConfFileName;
    delete[] fFrioRunCont.InfDataFileName;
  }
}

//______________________________________________________________________________
Bool_t TAGmi28ParFile::OpenParFile(const char* fileName)
{
  fParamFile.open(fileName, std::ios::binary);
  if (fParamFile.fail()) return false;
  fParamFile.seekg(0);
  return true;
}

//______________________________________________________________________________
void TAGmi28ParFile::ReadParFile()
{
  fParamFile.read(reinterpret_cast<char *>(&fFrioRunCont), sizeof(fFrioRunCont));
   return;
}