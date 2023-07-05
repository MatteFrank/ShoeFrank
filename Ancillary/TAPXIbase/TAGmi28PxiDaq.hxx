#ifndef _TAGmi28PxiDaq_HXX
#define _TAGmi28PxiDaq_HXX

#include "TAGmi28DataFile.hxx"
#include "TAGmi28InfoFile.hxx"
#include "TAGmi28ParFile.hxx"

#include "TString.h"
#include "TObject.h"

class TAGmi28PxiDaq : public TAGmi28DataFile {
  
public:
  TAGmi28PxiDaq();
  virtual ~TAGmi28PxiDaq();

private:
  TAGmi28DataFile* fDataFile;
  TAGmi28InfoFile* fInfoFile;
  TAGmi28ParFile*  fParFile;
  
  Int_t fAcqNbr;
  Int_t fFramePerAcq;
  
private:
  typedef struct {   
    TString 	      ParRunDir;       // Run directory
    TString 	      ParFileNamePrefix;             // Run file prefix, eg : RUN_666 => RUN_ is the prefix
    Int_t 	      ParRunNo;
    Int_t  	      ParFrameNbPerAcq;                  // Nb of frame in the acquisition ( get from run file )
    Int_t  	      ParAcqNo;                          // Index of acquisition to select ( get from GUI )
    Int_t  	      ParFrameNo;                        // Index of frame to get
    TString 	      InfRunParFile;   // File which contains run parameters *.par
    TString 	      InfRunInfoFile;  // File which contains run info *.bin.inf
    TString 	      InfRunDataFile;  // File which contains run data *.bin
    Int_t  	      InfMaxFrameSz;                     // Maximal size of one frame = total size for N Mimosa 26
    Char_t 	      ResRunLoaded;                      // Flag indicates run state : -1 not loaded, +1 loaded
  } RunContext;
  RunContext fRunContext;

public:
  RunContext GetFrioRunCont() {return fRunContext;}
  TAGmi28ParFile* GetParFile() {return fParFile;}
  
public:
  Int_t      IsLittleEndian();
  Int_t      LoadRun(TString& RunDir, TString& RunPrefix, Int_t RunNo);
  Bool_t     BuildFrameListFromAcq(Int_t FrameNb, FrioFrameList& list);
  Bool_t     GoToAcq(Int_t AcqNo);
  
  ClassDef(TAGmi28PxiDaq,1)

};

#endif