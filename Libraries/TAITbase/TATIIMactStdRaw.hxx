#ifndef _TATIIMactStdRaw_HXX
#define _TATIIMactStdRaw_HXX

#include <fstream>

#include "TATIIMactBaseNtuHit.hxx"

/*!
 \file TATIIMactStdRaw.hxx
  \brief   Declaration of TATIIMactStdRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TATIIMactStdRaw : public TATIIMactBaseNtuHit {
public:
   
   explicit TATIIMactStdRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TATIIMactStdRaw();
   
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

   ClassDef(TATIIMactStdRaw,0)
};

#endif
