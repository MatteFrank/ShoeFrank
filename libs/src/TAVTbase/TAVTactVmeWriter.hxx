#ifndef _TAVTactVmeWriter_HXX
#define _TAVTactVmeWriter_HXX
/*!
  \file
  \brief   Declaration of TAVTactVmeWriter.
*/
/*------------------------------------------+---------------------------------*/
#include <fstream>
#include <map>

#include "TString.h"
#include "TH1F.h"

#include "TAVTactBaseRaw.hxx"

class TH2F;
class TH1F;
//##############################################################################
class TAVTactVmeWriter : public TAVTactBaseRaw  {
public:
   explicit        TAVTactVmeWriter(const char* name=0, TAGdataDsc* pDatRaw = 0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual         ~TAVTactVmeWriter();
      
   //! Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = "");
   
   //! close files
   virtual void    Close();
   
   //! Process
   virtual Bool_t  Process();
   
   //! Set start trigger number for re-synchronization
   void            SetTrigJumpStart(Int_t start)            { fTrigJumpStart = start;     }

 public:
   enum {kSize = 10};
   static void     SetDefaultFolderName(const Char_t* name) { fgDefaultFolderName = name; }
   static TString  GetDefaultFolderName()                   { return fgDefaultFolderName; }
   
   static void     SetTrigJumpMap(Int_t iSensor, Int_t trigger, Int_t jump);
   
private:
   ifstream        fRawFileAscii[kSize]; // file streamm
   ofstream        fDaqFile;

   Int_t           fRunNumber;           // run number

   TString         fPrefixName;          // prefix folder name
   TString         fBaseName;            // base file name
   
   vector<UInt_t>  fDaqEvent;
   Int_t           fDaqSize;
   
   Int_t           fTrigJumpStart;
   map<pair<int,int>,  int>  fTrigJumpFirst;
   
private:
   static           TString fgDefaultFolderName;
   static           TString fgDefaultExtName;
   static           map< pair<int, int>, int> fgTrigJumpMap;
   static           Bool_t fgTrigJumpAuto;
   
private:	
   //! Get the event
   Bool_t           GetSensorEvent(Int_t iSensor);
   
   //! Fill and write DAQ event
   void             FillDaqEvent();
   
   //! Set run number
   void             SetRunNumber(const TString& name);
   
   ClassDef(TAVTactVmeWriter,0)
};

#endif
