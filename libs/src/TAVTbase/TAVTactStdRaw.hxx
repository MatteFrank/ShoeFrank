#ifndef _TAVTactStdRaw_HXX
#define _TAVTactStdRaw_HXX

#include <fstream>

#include "TAVTactBaseNtuHit.hxx"

/*!
 \file
 \version $Id: TAVTactStdRaw.hxx 
 \brief   Declaration of TAVTactStdRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAVTactStdRaw : public TAVTactBaseNtuHit {
public:
   
   explicit TAVTactStdRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactStdRaw();
   
   //! Action
   virtual Bool_t  Action();
   
   //! Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = 0, const TString treeName="tree", Bool_t dscBranch = true);
   
   //! close files
   virtual void    Close();
   
public:
   //! Set default folder name
   static void     SetDefaultFolderName(const Char_t* name) { fgDefaultFolderName = name; }
   //! Get default folder name
   static TString  GetDefaultFolderName()                   { return fgDefaultFolderName; }
   
private:
   ifstream          fRawFileAscii;     /// file streamm
   Int_t             fRunNumber;        /// run number
   
   TString           fPrefixName;       /// prefix folder name
   TString           fBaseName;         /// base file name
   
private:
   static       TString fgDefaultFolderName; /// default folder name
   static       TString fgDefaultExtName;    /// default extension name

private:
   //! Get Event
   Bool_t GetEvent();
   
   //! Set run number
   void   SetRunNumber(const TString& name);

   ClassDef(TAVTactStdRaw,0)
};

#endif
