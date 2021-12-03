#ifndef _TAVTactVmeSingleReader_HXX
#define _TAVTactVmeSingleReader_HXX
/*!
  \file
  \brief   Declaration of TAVTactVmeSingleReader.
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
class TAVTactVmeSingleReader : public TAVTactBaseRaw  {
public:
   explicit        TAVTactVmeSingleReader(const char* name=0, TAGdataDsc* pDatRaw = 0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual         ~TAVTactVmeSingleReader();
      
   //! Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = "",  const TString treeName="", Bool_t dscBranch = true);
   
   //! close files
   virtual void    Close();
   
   //! Process
   virtual Bool_t  Process();
   
   //! Set start trigger number for re-synchronization
   void            SetTrigJumpStart(Int_t start)            { fTrigJumpStart = start;     }

 public:
   enum {kSize = 10};
   //! Set default folder name
   static void     SetDefaultFolderName(const Char_t* name) { fgDefaultFolderName = name; }
   //! Get default folder name
   static TString  GetDefaultFolderName()                   { return fgDefaultFolderName; }
   //! Set trigger jump map
   static void     SetTrigJumpMap(Int_t iSensor, Int_t trigger, Int_t jump);
   
protected:
   ifstream        fRawFileAscii;        /// file streamm
   Int_t           fRunNumber;           /// run number

   TString         fPrefixName;          // prefix folder name
   TString         fBaseName;            // base file name
   
   Int_t            fTrigJumpStart;      /// trigger jump
   Int_t            fTrigReset;          /// trigger reset
   map<pair<int,int>,  int>  fTrigJurst; /// map of trigger jumpmpFirst;
   
protected:
   static           TString fgDefaultFolderName;             /// default folder name
   static           TString fgDefaultExtName;                /// default extension name
   static           map< pair<int, int>, int> fgTrigJumpMap; /// map of trigger jump
   static           Bool_t fgTrigJumpAuto;                   /// flag for jump detection
   
protected:
   //! Get the event
   Bool_t           GetSensorEvent(Int_t iSensor);
   
   //! Get frame and returns frameRaw
   Bool_t           GetFrame(Int_t iSensor, MI26_FrameRaw* data);
   
   //! Set run number
   void             SetRunNumber(const TString& name);
   
   ClassDef(TAVTactVmeSingleReader,0)
};

#endif
