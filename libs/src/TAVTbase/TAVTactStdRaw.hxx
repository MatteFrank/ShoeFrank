#ifndef _TAVTactStdRaw_HXX
#define _TAVTactStdRaw_HXX

#include <fstream>

#include "TAVTactBaseNtuRaw.hxx"

/*!
 \file
 \version $Id: TAVTactStdRaw.hxx $
 \brief   Declaration of TAVTactStdRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAVTactStdRaw : public TAVTactBaseNtuRaw {
public:
   
   explicit TAVTactStdRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactStdRaw();
   
   virtual Bool_t  Action();
   
   //! Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = "");
   
   //! close files
   virtual void    Close();
   
public:
   static void     SetDefaultFolderName(const Char_t* name) { fgDefaultFolderName = name; }
   static TString  GetDefaultFolderName()                   { return fgDefaultFolderName; }
   
private:
   ifstream          fRawFileAscii;    // file streamm
   Int_t             fRunNumber;       // run number
   
   TString           fPrefixName;       // prefix folder name
   TString           fBaseName;         // base file name
   
private:
   static       TString fgDefaultFolderName;
   static       TString fgDefaultExtName;

private:

   //! Get Event
   Bool_t GetEvent();
   
   //! Set run number
   void   SetRunNumber(const TString& name);

};

#endif
