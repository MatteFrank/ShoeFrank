#ifndef _TAGmi28ParFile_HXX
#define _TAGmi28ParFile_HXX

#include "TAGmi28DataFile.hxx"
#include "TObject.h"
#include <fstream>

class TAGmi28ParFile : public TObject {
  
public:
  TAGmi28ParFile();
  virtual ~TAGmi28ParFile();
  
private:
  std::ifstream fParamFile;
  
private:
  typedef struct {  
    Int_t    	      dParDaqVersion;                        // Version of DAQ system - New field 21/02/2011
    UShort_t	      ParMapsName;                          // Name of MAPS - New field 21/02/2011
    UShort_t 	      ParMi28Nb;                            // Mimosa 26 number - Moved from Char_t to UShort_t on 21/02/2011
    Int_t  	      ParFrameNbPerAcq;                     // Frames number per acquisition
    Int_t  	      ParRunNo;                             // Run no
    Int_t 	      ParTotEvNb;                           // Total event number of run
    Int_t  	      ParEvNbPerFile;                       // Event number per file
    char*  	      ParDestDir;         // Run file destination directory       
    char*   	      ParFileNamePrefix;  // Prefix of run file name, eg : RUN_666 => "RUN" is the prefix
    char*   	      ParJtagFileName;    // JTAG configuration file (*.mcf) -> New field 03/02/2011
    Char_t  	      ParDataTransferMode;                  // Transfer mode see enum EFRIO__TRF_MODE in *.def file
    Char_t   	      ParTrigMode;                          // Trigger mode -> Future use
    Char_t   	      ParSaveOnDisk;                        // Save data on disk
    Char_t   	      ParSendOnEth;                         // Send data on Ethernet
    Char_t  	      ParSendOnEthPCent;                    // % of data sent on Ethernet
    Char_t  	      ParMeasDataRate;                      // Enable data rate measurement, hard coded in EFRIO__FConfRun (...)
    Char_t   	      ParAcqNbToMeasDataRate;               // Acq number used to measure data rate, hard coded in EFRIO__FConfRun (...)
    Int_t   	      InfZsFFrameRawBuffSz;                 // If data ParDataTransferMode = IPHC        => Size of acquisition frames buffer
    Int_t   	      InfFrameBuffSz;                       // If data ParDataTransferMode = EUDET 1,2,3 => Size of acquisition frames buffer
    char* 	      InfConfFileName;    // Run configuration file ( save EFRIO__TRunCont to disk ) name built form ParRunNo, ParDestDir
    char* 	      InfDataFileName;    // Run data file name built from ParRunNo, ParFileNamePrefix, ParDestDir
    Int_t  	      InfDataRateMeasTotalSz;               // Total size acquired during ParAcqNbToMeasDataRate acquisitions
    Int_t  	      InfDataRateMeasStartTimeMs;           // Start time of measurement
    Int_t   	      InfDataRateMeasStopTimeMs;            // Stop time of measurement
    Int_t 	      InfDataRateMeasTotalTimeMs;           // Total time of measurement
    Char_t	      InfSaveDataOnDiskRunning;             // Add on 15/02/2011  
    Int_t 	      CmdRun;                              // Add on 21/12/2010 for interface / EUDET DAQ  
    Int_t   	      ResAcqFunctRetCode;                   // Return code of Acq function
    Int_t    	      ResAcqCnt;                            // Acquisitions counter
    Int_t    	      ResFrameCnt;                          // Frames counter
    Int_t   	      ResEventCnt;                          // Events counter -> By default events counter = frames counter
    Float_t  	      ResDataRateMBytesPerSec;
  } FrioRunCont;
  FrioRunCont fFrioRunCont;
  
public:
  FrioRunCont& GetFrioRunCont() {return fFrioRunCont;}
  Int_t       GetFramePerAcq() { return fFrioRunCont.ParFrameNbPerAcq;}
  
public:
  Bool_t OpenParFile(const char* fileName);
  void ReadParFile();
  
  ClassDef(TAGmi28ParFile,1)

};

#endif