#include "TAGmi28PxiDaq.hxx"

#include "TAGmi28DataFile.hxx"
#include "TAGmi28DataFile.hxx"
#include <TSystem.h>
#include "TString.h"
#include "TROOT.h"

ClassImp(TAGmi28PxiDaq);

//______________________________________________________________________________
TAGmi28PxiDaq::TAGmi28PxiDaq()
: TAGmi28DataFile(),
  fAcqNbr(0),
  fFramePerAcq(0),
  fRunContext()
{
  fParFile  = new TAGmi28ParFile();
  fInfoFile = new TAGmi28InfoFile();
}

//______________________________________________________________________________
TAGmi28PxiDaq::~TAGmi28PxiDaq()
{
}

//______________________________________________________________________________
Int_t TAGmi28PxiDaq::IsLittleEndian()
{  
  Int_t testEndian = 0x11223344;
  char* ptEndian;
    
  ptEndian = (char*) &testEndian;
    
  if ( (ptEndian[0] == 0x11) && (ptEndian[1] == 0x22) && (ptEndian[2] == 0x33) && (ptEndian[3] == 0x44) ) {
    return (0);
  }
    
  else {
    return (1);
  }
}

//______________________________________________________________________________
Int_t TAGmi28PxiDaq::LoadRun(TString& RunDir, TString& RunPrefix, Int_t RunNo)
{
  
  fRunContext.ResRunLoaded = -1;
  
  fRunContext.ParRunDir = RunDir;
  fRunContext.ParFileNamePrefix = RunPrefix;
  fRunContext.ParRunNo = RunNo;
  
  if(!gSystem->cd(fRunContext.ParRunDir)) printf("%s: Current path does not exist...", fRunContext.ParRunDir.Data());
  
  fRunContext.InfRunDataFile = Form("%s/%s%d.bin", fRunContext.ParRunDir.Data(), fRunContext.ParFileNamePrefix.Data(), fRunContext.ParRunNo);
  fRunContext.InfRunInfoFile = Form("%s/%s%d.bin.inf", fRunContext.ParRunDir.Data(), fRunContext.ParFileNamePrefix.Data(), fRunContext.ParRunNo);
  fRunContext.InfRunParFile  = Form("%s/%s%d.par", fRunContext.ParRunDir.Data(), fRunContext.ParFileNamePrefix.Data(), fRunContext.ParRunNo);
  
  if (!fParFile->OpenParFile(fRunContext.InfRunParFile.Data())) printf("%s: File does not exist...", fRunContext.InfRunParFile.Data());
  if (!fInfoFile->OpenRecInfoFile(fRunContext.InfRunInfoFile.Data())) printf("%s: File does not exist...", fRunContext.InfRunInfoFile.Data());
  if (!OpenDataFile(fRunContext.InfRunDataFile.Data())) printf("%s: File does not exist...", fRunContext.InfRunDataFile.Data());
  
  
  fParFile->ReadParFile();
  fInfoFile->ReadHeaderInfoFile();
  
  fAcqNbr = fInfoFile->GetTotalBlocNbr();
  fFramePerAcq = fParFile->GetFramePerAcq();
  
  fInfoFile->FillListOfPosInfoFile(fAcqNbr);

  return fAcqNbr;
  
}

//______________________________________________________________________________
Bool_t TAGmi28PxiDaq::BuildFrameListFromAcq(Int_t FrameNb, FrioFrameList& list)
{

  for (Int_t i = 0; i < FrameNb; i++){
    ReadBlocFrame();
    list.AFrameSz.at(i) = fFrioFrame.TotSz;
    list.AFrame.at(i) = fFrioFrame;
  }
  
  return true;
    
}
//______________________________________________________________________________
Bool_t TAGmi28PxiDaq::GoToAcq(Int_t AcqNo)
{
  UInt_t dataPosFile = fInfoFile->GetListOfPosInfoFile(AcqNo);
  GoToPosFile(dataPosFile);
  BuildFrameListFromAcq(fFramePerAcq, fFrioFrameList);
  
  return true;
}