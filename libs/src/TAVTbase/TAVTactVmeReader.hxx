#ifndef _TAVTactVmeReader_HXX
#define _TAVTactVmeReader_HXX
/*!
  \file
  \brief   Declaration of TAVTactVmeReader.
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
class TAVTactVmeReader : public TAVTactBaseRaw  {
public:
   explicit        TAVTactVmeReader(const char* name=0, TAGdataDsc* pDatRaw = 0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual         ~TAVTactVmeReader();
      
   //! Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = "");
   
   //! close files
   virtual void    Close();
   
   //! Process
   virtual Bool_t  Process();
   
   //! Set start trigger number for re-synchronization
   void    SetTrigJumpStart(Int_t start)                    { fTrigJumpStart = start;     }

 public:
   enum {kSize = 10};
   static void     SetDefaultFolderName(const Char_t* name) { fgDefaultFolderName = name; }
   static TString  GetDefaultFolderName()                   { return fgDefaultFolderName; }
   
   static void     SetTrigJumpMap(Int_t iSensor, Int_t trigger, Int_t jump);
   
private:
   ifstream          fRawFileAscii[kSize]; // file streamm
   Int_t             fRunNumber;           // run number

   TString           fPrefixName;          // prefix folder name
   TString           fBaseName;         // base file name
   
   Int_t             fTrigJumpStart;
   map<pair<int,int>,  int>  fTrigJumpFirst;
   
private:
   static       TString fgDefaultFolderName;
   static       TString fgDefaultExtName;
   static       map< pair<int, int>, int> fgTrigJumpMap;
   static       Bool_t fgTrigJumpAuto;
   
private:	
   //! Get the event
   Bool_t GetSensorEvent(Int_t iSensor);
   
   //! Get frame and returns frameRaw
   Bool_t GetFrame(Int_t iSensor, MI26_FrameRaw* data);
   
   //! Set run number
   void   SetRunNumber(const TString& name);
   
   ClassDef(TAVTactVmeReader,0)
};

#endif
