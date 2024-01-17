#ifndef _TABMntuRaw_HXX
#define _TABMntuRaw_HXX
/*!
  \file   TABMntuRaw.hxx
  \brief   Declaration of TABMntuRaw, the container of the BM raw hits (TABMrawHit)
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
class TABMntuRaw : public TAGdata {
  public:
                    TABMntuRaw();
    virtual         ~TABMntuRaw();

    TABMrawHit*       NewHit(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time);
    Int_t             GetHitsN() const;
    const TABMrawHit* GetHit(Int_t i_ind) const;
    TABMrawHit*       GetHit(Int_t i_ind);

    //! Get the trigger time
    Double_t          GetTrigtime()  const { return fTrigTime; }
    //! Get the trigger time (-1000 if only one type, otherwise: 0=majority and 1=fragmentation)
    Int_t             GetTrigtype()  const { return fTrigType; }
    //! Get the number of discharged hits
    Int_t             NDrop()        const { return fiNDrop;   }

    //! Set the trigger time
    void              SetTrigtime(Double_t trigin) { fTrigTime=trigin;}
    //! Set the trigger time
    void              SetTrigtype(Int_t trigin) { fTrigType=trigin;}

    void              AddDischarged();
    virtual void      SetupClones();
    virtual void      Clear(Option_t* opt="");
    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;

private:
    TClonesArray*   fListOfHits;    ///< List of BM accepted raw hits
    Int_t           fiNDrop;		    ///< Number of discharged tdc values
    Double_t        fTrigTime;      ///< Trigger time of the event
    Int_t           fTrigType;      ///< Trigger type of the event (-1000 if only one type, otherwise: 0=majority and 1=fragmentation)

   ClassDef(TABMntuRaw,1)
};

#endif
