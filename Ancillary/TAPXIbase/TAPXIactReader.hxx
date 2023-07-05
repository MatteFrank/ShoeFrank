#ifndef _TAPXIactReader_HXX
#define _TAPXIactReader_HXX
/*!
  \file
  \brief   Declaration of TAPXIactReader. New class based on PXIeBoardReader class (© JB & GC)
 
  \author J. Baudot, adpt. Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <fstream>
#include "TString.h"

#include "TAGmi28DataFile.hxx"
#include "TAGmi28ParFile.hxx"
#include "TAGmi28PxiDaq.hxx"
#include "TAGactionFile.hxx"

class TH2F;
class TH1F;

//##############################################################################
class TAPXIactReader : public TAGactionFile  {
public:
   explicit        TAPXIactReader(const char* name=0, TAGdataDsc* pDatRaw = 0, TAGparaDsc* pGeoMap = 0, TAGparaDsc* pConfig = 0,
                                    TAGdataDsc* pDatRawBm = 0, TAGparaDsc* pGeoMapBm = 0, TAGparaDsc* pConfigBm = 0);
   virtual         ~TAPXIactReader();
      
   //! Process
   virtual Bool_t  Process();
   
   //! Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& path, Option_t* option=0, const TString treeName="tree", Bool_t dscBranch = true);

   //! read configuration file
   Bool_t       ReadConfiguration();
   
   //! Base creation of histogram
   void         CreateHistogram();

   Bool_t       GetNextDaqEvent();
   Bool_t       ReadTriggersFromFrame();
   void	        StartEventReading(); 
   void         DefineEventEnd(); 
   void         CheckSecondReading(); 
   Bool_t       ReadPixelsFromFrame(); 
   Bool_t       FetchData();
   Int_t        GetBoardNumber()                 const { return fBoardNumber;        }
   Int_t        GetNSensors()                    const { return fNSensors;           }
   Int_t        GetTriggerMode()                 const { return fTriggerMode;        }
   void         SetTriggerMode(Int_t trig)             { fTriggerMode = trig;        } 
   Int_t        GetRunNumber()                   const { return fRunNumber;          }
   Int_t        GetEventNumber()                 const { return fCurrentEventNumber; }

   void         SetDaqEventNumber(Int_t daqEvent);
   Int_t        GetDaqEventNumber()              const {return  fDaqEventNumber;     }

   void         SetBeamOffStatus(Bool_t beamStatus)    { fReadBeamOff = beamStatus;  };
   Bool_t       GetBeamOffStatus()               const {return  fReadBeamOff;        }

   void         SetReadTriggerSpill(Bool_t spillStatus){ fReadTriggerSpill = spillStatus;  };
   Bool_t       GetReadTriggerSpill()            const {return  fReadTriggerSpill;         }
   
   Int_t        GetTriggerNumber()               const {return  fTriggerN;           }
   vector<Float_t> GetTriggerLine()                    {return  fTriggerLine;        }
   vector<Float_t> GetListOfTriggerTime()              {return  fTriggerTime;        }
   
   void         SetDebugLevel( Int_t level)            { fDebugLevel = level;        }
   void         PrintStatistics(ostream &stream=cout); 
   
protected:
   //! Add pixel to list
   void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn);
   
   //! Set run number
   void SetRunNumber(const TString& path);
   
private:
   static TString fgkPrefixFileName;
   static const Int_t fgkMaxNofMappedIdx;
   
private:
   TAGmi28DataFile::FrioFrame* fPtFrame;
   TAGmi28PxiDaq*    fpPxiDaq;
   TAGmi28ParFile*   fpParFile;
  
   TAGdataDsc*       fpDatRaw;        // pxi raw
   TAGparaDsc*       fpGeoMap;		  // geo para dsc
   TAGparaDsc*       fpConfig;		  // config para dsc
   
   TAGdataDsc*       fpDatRawBm;      // pxi raw for BM if ever
   TAGparaDsc*       fpGeoMapBm;		  // geo para dsc  for BM
   TAGparaDsc*       fpConfigBm;		  // config para dsc for BM

   Int_t             fTriggerN;
   vector<Float_t>   fTriggerLine;
   vector<Float_t>   fTriggerTime;
   
   Int_t             fBoardNumber;
   Int_t             fNSensors;
   Int_t             fTriggerMode;
   Int_t             fFramesToSkipAtEndOfAcqForTrig; 
   Int_t	     fTriggerLineOffset;
   Int_t             fNextFirstLineToKeep;
   Int_t             fNextFirstFrame;
   Int_t             fNextLastLineToKeep; 
   Int_t             fFirstLineToKeep;
   Int_t             fFirstFrame;
   Int_t             fLastLineToKeep;
   Int_t             fLastFrame;
   Int_t             fNumberOfLines;
   Int_t             fRunNumber;
   Int_t             fFramesReadFromFile;
   Int_t             fAcqsReadFromFile; 
   Int_t             fNFramesPerAcq; 
   Int_t             fNacquisitions; 
   
   Bool_t            fReadingEvent;
   Bool_t            fReadTwice;
   Bool_t            fOverflow; 
   Bool_t            fEventReady;
  
   Bool_t            fReadTriggerSpill;
   Bool_t            fReadBeamOff;

   Int_t             fCurrentAcqNumber;
   Int_t             fCurrentFrameNumber;
   Int_t             fAcqId;
   Int_t             fFrameId;
   Int_t             fCurrentEventNumber;
   vector<Int_t>     fListOfTriggerPos; 
   vector<Int_t>     fListOfNextTriggerPos; 
   vector<Int_t>     fListOfFrames; 
   
   Int_t             fEventsCount; 
   Int_t             fEventsOverflow; 
   Int_t             fEventsAborted; 
   Int_t             fFramesReadTwice; 
   Int_t             fTriggerCount; 
   Int_t             fNStatesInBlock[100]; 
   Int_t             fNStatesInLine; 
   Int_t*            fOverflowSensor; 

   Int_t             fDaqEventNumber;
   
   Int_t             fNumberOfFiles;
   Int_t             fDebugLevel;     // debug level
  
   TH2F*             fpHisPixelMap[32];  // pixel map per sensor
   TH1F*             fpHisEvtLength[32]; // event data length for each sensor
   
   TH2F*             fpHisPixelMapBm[32];  // pixel map per sensor
   TH1F*             fpHisEvtLengthBm[32]; // event data length for each sensor
   
   ClassDef(TAPXIactReader,0)
};

#endif
