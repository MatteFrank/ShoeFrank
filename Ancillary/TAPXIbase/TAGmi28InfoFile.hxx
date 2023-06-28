#ifndef _TAGmi28InfoFile_HXX
#define _TAGmi28InfoFile_HXX

#include <fstream>
#include <vector>
#include "TObject.h"
#include "TString.h"

class TAGmi28InfoFile : public TObject {

public:
  TAGmi28InfoFile();
  virtual ~TAGmi28InfoFile();
  
private:
  static const Int_t fgkBlocInfoMaxSize;

private:
  std::ifstream fInfoFile;
  UInt_t        fTotalBlocSize;
  Int_t         fRefPosFile;
  std::vector<UInt_t> fListOfPosInfoFile;

  typedef union {  
    Char_t  Year;
    Char_t  Month;
    Char_t  Day;
    Char_t  NotUsed;
  } OpenFileDate;
  
  typedef union {  
    Char_t  Year;
    Char_t  Month;
    Char_t  Day;
    Char_t  NotUsed;
  } CloseFileDate;

  typedef struct {  
    Char_t  Cent;
    Char_t  Sec;
    Char_t  Min;  
    Char_t  Hour;
  } OpenFileTime;
  
  typedef struct {  
    Char_t  Cent;
    Char_t  Sec;
    Char_t  Min;  
    Char_t  Hour;
  } CloseFileTime;

  typedef struct {
    ULong64_t Offset;
    UInt_t    Sz;
    Int_t     SpareW32InfoFormat; // Number which identifies the meaning ASpareW32Info
    Int_t     SpareW32InfoNb;     // Number of spare parameters
    std::vector<Int_t> ASpareW32Info; // Array for user parameters
    UInt_t    Dummy; // For alignment 
  } StreamFileBlocInf;
  StreamFileBlocInf& fStreamFileBlocInf;

  typedef struct {
    Int_t               Version;
    OpenFileDate        DateCreate;
    OpenFileTime        TimeCreate;
    CloseFileDate       DateClose;
    CloseFileTime       TimeClose;
    Int_t               FixedBlocSz;
    UInt_t              MaxBlocSz;
    UInt_t              BlocSz;
    UInt_t       	BlocNb;
    UInt_t	        Dummy; // For alignment 
    StreamFileBlocInf   ABlocInf;  
  } StreamFileRecInfFile;
  StreamFileRecInfFile fStreamFileRecInfFile;
  
public:
  StreamFileRecInfFile GetHeaderInfoFile()             {return fStreamFileRecInfFile;       }
  StreamFileBlocInf    GetBlocInfoFile()               {return fStreamFileBlocInf;          }
  UInt_t               GetTotalBlocNbr()               {return fStreamFileRecInfFile.BlocNb;}
  UInt_t               GetBlocSize()                   {return fStreamFileBlocInf.Sz;       }
  UInt_t               GetTotalBlocSize()              {return fTotalBlocSize;              }
  Int_t                GetRefPosFile()                 {return fRefPosFile;                 }
  UInt_t               GetListOfPosInfoFile(Int_t acq) {return fListOfPosInfoFile.at(acq);  }
  
public:
  Bool_t OpenRecInfoFile(const char* fileName);
  void   ReadHeaderInfoFile();
  void   ReadBlocInfoFile();
  void   SetTotalBlocSize(UInt_t size);
  void   FillListOfPosInfoFile(Int_t acqNbr);
  
  ClassDef(TAGmi28InfoFile,1)
};

#endif
