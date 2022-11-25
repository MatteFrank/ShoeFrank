#ifndef _TAITactStdRaw_HXX
#define _TAITactStdRaw_HXX

#include <fstream>

#include "TAITactBaseNtuHit.hxx"

/*!
 \file TAITactStdRaw.hxx
  \brief   Declaration of TAITactStdRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAITactStdRaw : public TAITactBaseNtuHit {
public:
   
   explicit TAITactStdRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactStdRaw();
   
   // Action
   virtual Bool_t  Action();
   
   // Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = 0, const TString treeName="tree", Bool_t dscBranch = true);
   
   // close files
   virtual void    Close();
   
private:
   ifstream        fRawFileAscii;     ///< file streamm
   
private:
   // Get Event
   Bool_t GetEvent();

   ClassDef(TAITactStdRaw,0)
};

#endif
