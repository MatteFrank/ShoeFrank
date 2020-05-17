#ifndef _TABMdatRaw_HXX
#define _TABMdatRaw_HXX
/*!
  \file
  \version $Id: TABMdatRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TABMdatRaw.
*/

#include "TAGdata.hxx"
#include "TABMrawHit.hxx"

#include <vector>
#include <string.h>
#include <fstream>
#include <bitset>
#include <algorithm>

#include "TString.h"

using namespace std; 

class TClonesArray;
class TABMdatRaw : public TAGdata {
  public:

                    TABMdatRaw();
    virtual         ~TABMdatRaw();

    TABMrawHit*       NewHit(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time);
    Int_t             GetHitsN() const;
    const TABMrawHit* GetHit(Int_t i_ind) const;
    TABMrawHit*       GetHit(Int_t i_ind);
   
    void              SetTrigtime(Double_t trigin) { fTrigTime=trigin;}
    Double_t          GetTrigtime()  const { return fTrigTime; }
    Int_t             NDrop()        const { return fiNDrop;   }

    void              AddDischarged();
    virtual void      SetupClones();
    virtual void      Clear(Option_t* opt="");
    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
private:
    TClonesArray*   fListOfHits;      
    Int_t           fiNDrop;		    //number of discharged tdc values
    Double_t        fTrigTime;

public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   static TString fgkBranchName;    // Branch name in TTree

   ClassDef(TABMdatRaw,1)
};

#endif
