#ifndef _TABMntuRaw_HXX
#define _TABMntuRaw_HXX
/*!
  \file
  \version $Id: TABMntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TABMntuRaw.
*/
/*------------------------------------------+---------------------------------*/

#include "TObject.h"
#include "TClonesArray.h"

#include "TAGdata.hxx"
#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"

#include "TString.h"

#include <string.h>
#include <fstream>
#include <bitset>
#include <algorithm>
#include <vector>

using namespace std;

//##############################################################################

class TABMntuRaw : public TAGdata {
  public:
                    TABMntuRaw();
    virtual         ~TABMntuRaw();

    TABMntuHit*       GetHit(Int_t i_ind);
    const TABMntuHit* GetHit(Int_t i_ind) const;
    TABMntuHit*       NewHit(Int_t iv, Int_t il, Int_t ic, Int_t id, Double_t r, Double_t t, Double_t s);
    
    virtual void      SetupClones();
    virtual void      Clear(Option_t* opt="");
    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
    static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

    Int_t             GetHitsN() const;
    Int_t             GetNselhitx() const{return fNselhitx;}
    Int_t             GetNselhity() const{return fNselhity;}
    Int_t             GetNselhits() const{return fNselhitx + fNselhity;}
    void              SetNselhitx(Int_t selin){fNselhitx=selin;}
    void              SetNselhity(Int_t selin){fNselhity=selin;}
    
    //fCellOccupy
    void ClearCellOccupy();
    Bool_t AddCellOccupyHit(Int_t pos);
    Bool_t RemoveCellOccupyHit(Int_t pos);
    Int_t GetCellOccupy(Int_t pos){return (pos<36 && pos>=0) ? fCellOccupy[pos] : -1;}
    void PrintCellOccupy();
    
    //efficieny
    void Efficiency_paoloni(Int_t pivot[], Int_t probe[], Double_t &efftot, Double_t &effxview, Double_t &effyview);
   void ResetEffPaoloni(){fEffPaoloni=-3;fEffPaolonixview=-3;fEffPaoloniyview=-3;}
    void SetEfficiency(Double_t efftot, Double_t effxview, Double_t effyview);
   
    Double_t GetEffPaoloni()      const { return fEffPaoloni;      }
    Double_t GetEffPaolonixview() const { return fEffPaolonixview; }
    Double_t GetEffPaoloniyview() const { return fEffPaoloniyview; }

  private:
    TClonesArray*   fListOfHits;			    // hits
    static TString fgkBranchName;    // Branch name in TTree
   
    Int_t           fCellOccupy[36];    //occupancy of the BM cell
    Double_t        fEffPaoloni;    //value of the efficiency calculated with the Paoloni's method
    Double_t        fEffPaolonixview;    //value of the efficiency calculated with the Paoloni's method for the x view
    Double_t        fEffPaoloniyview;    //value of the efficiency calculated with the Paoloni's method for the y view
    
    //for the reconstructed track
    Int_t           fNselhitx;    //number of selected hits on x view for the reconstructed track
    Int_t           fNselhity;    //number of selected hits on y view for the reconstructed track
    
    ClassDef(TABMntuRaw,1)
};

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

inline TABMntuHit* TABMntuRaw::GetHit(Int_t i)
{
  return (TABMntuHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

inline const TABMntuHit* TABMntuRaw::GetHit(Int_t i) const
{
  return (const TABMntuHit*) ((*fListOfHits)[i]);;
}


#endif
