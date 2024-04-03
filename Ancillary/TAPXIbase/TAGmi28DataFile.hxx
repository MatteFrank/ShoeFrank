#ifndef _TAGmi28DataFile_HXX
#define _TAGmi28DataFile_HXX

#include "TObject.h"
#include "TString.h"
#include <fstream>
#include <vector>

class TAGmi28DataFile : public TObject {
   
public:
   TAGmi28DataFile();
   virtual ~TAGmi28DataFile();
   
public:
   static const Int_t fgkAsicTrigResNbr;
   static const Int_t fgkMaxFrameRawNbr;
   static const Int_t fgkFilePath;
   static const Int_t fgkMaxStateNbrPerState;
   static const Int_t fgkMaxStateNbr;
   static const Int_t fgkMaxAsicNbr;
   static const Int_t fgkMaxTriggerNbr;
   static const Int_t fgkMaxFramePerAq;
   
private:
   std::ifstream fDataFile;
   Int_t         fCurrentPosFile;
   Int_t         fStartFrame;
   
private:
   union Mi28StatesLine {
      UShort_t W16;
       struct F{
         UShort_t StateNb  :  4;
         UShort_t LineAddr : 11;
         UShort_t Ovf      :  1;
      };
   };
   
   union Mi28State{
      UShort_t W16;
       struct F {
         UShort_t HitNb   :  2;
         UShort_t ColAddr : 11;
         UShort_t NotUsed :  3;
      } ;
   } ;
   
   struct FrioFrameHeader {
      UInt_t    Tag;                                    // EFRIO__FRAME_TAG_HEADER
      Int_t     AcqStatus;                              // Status of acquistion board for this acquisition
      Int_t     TrigStatus;                             // No meaning now = reserved for future use
      UShort_t  AcqId;                                  // Index of acquisition containing this frame
      UShort_t  FrameIdInAcq;                           // Index of frame IN the CURRENT acquisition
      UShort_t  MapsName;                               // MAPS name as a 16 bits code
      UShort_t  MapsNb;                                 // Total number of MAPS in data
      std::vector<UInt_t> AMapsHeader;        // Mimosa 28 header field
      std::vector<UInt_t> AMapsFrameCnt;      // Mimosa 28 frame counter field
      std::vector<UShort_t> AMapsDataLength;    // Mimosa 28 data length in BYTES -> It's final result NOT the DataLength FIELD from data stream
      std::vector<UInt_t> AMapsTrailer;       // Mimosa 28 trailer field
      Short_t   TriggerNb;                              // Total triggers number during this frame
      std::vector<UShort_t> AMapsTrigInfo;   // First 3 "Mi28 trigger info" -> Line of Mi26 read during which trigger occurs
      // if more than 3 trigger => look in trigger info block where all trigger are stored
   } ;
   
   struct FrioFrameData{
      UInt_t  Tag;                     // EFRIO__FRAME_TAG_DATA
      UInt_t  TotSz;                   // Total size of data bloc
      UInt_t  OneMapsSz;               // Size of data of one MAPS
      std::vector<UShort_t> ADataW32;             // Beginning of data space
   } ;
   
   union FrioTluTrigger {
      UInt_t W32;
      struct F{
         UInt_t TrigCnt         : 16; // Trigger counter read from TLU
         UInt_t FrameIdInAcq    : 11; // Index of frame in current acquisition during which trigger occurs ( 0 - 2407 )
         UInt_t EventTakenByDut :  1; // For future use : Flag at 1 if DUT has taken the event
         UInt_t Reserved        :  3;
         UInt_t InvalidInfo     :  1; // If 1 this field is not valid
      } ;
   } ;
   
   struct  FrioTriggerRec {
      UInt_t   Tag;          // EFRIO__FRAME_TAG_TRIG
      UInt_t   TotSz;        // Total size of trigger info bloc
      UShort_t TrigNb;       // Total trigger nb
      UShort_t TrigType;     // Type of trigger info stored
      std::vector<UInt_t> ATrig;
   } ;
   
   union FrioTimeStamp {
      UInt_t W32;
       struct F {
         UInt_t Mi28Line    : 10; // Line of Mi28 read during which trigger occurs
         UInt_t Mi28Frame   : 21; // Frame of Mi28 ( = frame counter field ) read during which trigger occurs
         UInt_t InvalidInfo :  1; // If 1 this field is not valid
      } ;
   } ;
   
public:
   struct FrioFrame {
      UInt_t          Tag;            // EFRIO__FRAME_TAG
      Int_t           DaqVersion;     // Version of DAQ - New field 21/02/2011
      Int_t           TotSz;          // Total size of this frame
      Int_t           TrigRecOffset;  // Offset ( in bytes ) from beginning of frame to trigger info part
      FrioFrameHeader Header;         // Frame header
      FrioFrameData   Data;           // Beginning of data part
      FrioTriggerRec  Trigger;
   } ;
   
protected:
   struct FrioFrameList {
      std::vector<FrioFrame> AFrame;   // Array of each frame
      std::vector<UInt_t> AFrameSz;   // Array of frames size
   } ;
   
protected:
   Mi28StatesLine fMi28StatesLine;
   Mi28StatesLine::F fFStatesLine;
   Mi28State fMi28State;
   Mi28State::F fStates;
   FrioFrameHeader& fFrioFrameHeader;
   FrioFrameData& fFrioFrameData;
   FrioFrameList fFrioFrameList;
   FrioFrame fFrioFrame;
   FrioTriggerRec& fFrioTrigger;
   FrioTluTrigger fFrioTluInfo;
   FrioTluTrigger::F fFrioTluTrigger;
   FrioTimeStamp fTriggerInfo;
   FrioTimeStamp::F fTrigger;
   
public: 
   Int_t      GetCurrentPosFile()     {return fCurrentPosFile;}
   
   /********* Get Frame from list **********/
   FrioFrame* GetFrame(Int_t frameId) {return &fFrioFrameList.AFrame.at(frameId);}
   void SetFramePos(Int_t frameId);
   
   /********* Get Header Info **********/
   UShort_t GetAcqIdFrHeader()          {return fFrioFrameHeader.AcqId;                  }
   UShort_t GetFrameIdFrHeader()        {return fFrioFrameHeader.FrameIdInAcq;           }
   UShort_t GetMapsName()               {return fFrioFrameHeader.MapsName;               }
   UShort_t GetMapsNbr()                {return fFrioFrameHeader.MapsNb;                 }
   UInt_t   GetMapHeader(Int_t idx)     {return fFrioFrameHeader.AMapsHeader[idx];    }
   UInt_t   GetMapDataLength(Int_t idx) {return fFrioFrameHeader.AMapsDataLength.at(idx);}
   Short_t  GetTriggerNbr()             {return fFrioFrameHeader.TriggerNb;              }
   UShort_t GetTrigInfo(Int_t idx)      {return fFrioFrameHeader.AMapsTrigInfo.at(idx);  }
   
   /********* Get Data Info **********/
   UInt_t  GetDataTotSize()           {return fFrioFrameData.TotSz;     }
   UInt_t  GetDataOneMapSize()        {return fFrioFrameData.OneMapsSz; }
   std::vector<UShort_t> GetDataW32() {return fFrioFrameData.ADataW32;  }
   
   /********* Set StatesLine Info **********/
   void  SetStLineW16(UShort_t state);
   
   /********* Get StatesLine Info **********/
   UShort_t  GetStLineW16()      {return fMi28StatesLine.W16;  }
   UShort_t  GetStLineStateNbr() {return fFStatesLine.StateNb; }
   UShort_t  GetStLineAdrr()     {return fFStatesLine.LineAddr;}
   UShort_t  GetStLineOvf()      {return fFStatesLine.Ovf;     }
   
   /********* Set StatesLine Info **********/
   void  SetStW16(UShort_t state);
   
   /********* Get States Info **********/
   UShort_t  GetStW16()     {return fMi28State.W16; }
   UShort_t  GetStHitNbr()  {return fStates.HitNb;  }
   UShort_t  GetStColAdrr() {return fStates.ColAddr;}
   UShort_t  GetStNotUsed() {return fStates.NotUsed;}
   
   
   /********* Get Trigger Number **********/
   UShort_t GetTrigNbr() {return fFrioTrigger.TrigNb; }
   
   /********* Set Trigger Info **********/
   void  SetTriggerW32(UInt_t info);
   void  SetTluW32(UInt_t info);
   
   /********* Set Trigger Info **********/
   std::vector<UInt_t> GetTriggerList()  {return fFrioTrigger.ATrig; };
   
   /********* Get trigger Info **********/
   UInt_t  GetTriggerW32()   {return fTriggerInfo.W32;        }
   UInt_t  GetTriggerLine()  {return fTrigger.Mi28Line;   }
   UInt_t  GetTriggerFrame() {return fTrigger.Mi28Frame;  }
   UInt_t  GetTriggerValid() {return fTrigger.InvalidInfo;}
   
   
public:
   Bool_t OpenDataFile(const char* fileName);
   void   GoToPosFile(Int_t pos);
   void   ReadBlocFrame();
   void   ReadFrameInfo();
   void   ReadFrameHeader();
   void   ReadFrameData();
   void   ReadFrameTrigger();
   void   ResetFrameList();
   
   
   ClassDef(TAGmi28DataFile,1)
   
};

#endif
