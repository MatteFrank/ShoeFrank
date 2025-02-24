#ifndef _TAVTactStdDaqRaw_HXX
#define _TAVTactStdDaqRaw_HXX

#include <fstream>

#include "TAVTactBaseNtuHit.hxx"

/*!
 \file TAVTactStdDaqRaw.hxx
 \brief   Declaration of TAVTactStdDaqRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAVTactStdDaqRaw : public TAVTactBaseNtuHit {
public:
   
   explicit TAVTactStdDaqRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactStdDaqRaw();
   
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

   ClassDef(TAVTactStdDaqRaw,0)
};

#endif
