#ifndef _TAITactStdDaqRaw_HXX
#define _TAITactStdDaqRaw_HXX

#include <fstream>

#include "TAITactBaseNtuHit.hxx"

/*!
 \file TAITactStdDaqRaw.hxx
 \brief   Declaration of TAITactStdDaqRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DEITREvent;

using namespace std;
class TAITactStdDaqRaw : public TAITactBaseNtuHit {
public:
   
   explicit TAITactStdDaqRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactStdDaqRaw();
   
   // Action
   virtual Bool_t  Action();
   
   // Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = "", const TString treeName = "",  Bool_t dscBranch = true);
   
   // close files
   virtual void    Close();
      
private:
   ifstream          fDaqFile;    ///< file streamm
   Int_t             fRunNumber;  ///< run number
   
private:
   // Get Event
   Bool_t GetEvent();
   
   // Set run number
   void   SetRunNumber(const TString& name);

   ClassDef(TAITactStdDaqRaw,0)
};

#endif
