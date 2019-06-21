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

#include <TRandom3.h>
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

    Int_t             GetHitsN() const;

    TABMntuHit*       Hit(Int_t i_ind);
    const TABMntuHit* Hit(Int_t i_ind) const;

    TABMntuHit*       NewHit(Int_t iv, Int_t il, Int_t ic, Double_t r, Double_t t, Double_t s);

    virtual void      SetupClones();

    virtual void      Clear(Option_t* opt="");

    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
    
    //cell_occupy
    void ClearCellOccupy();
    Bool_t AddCellOccupyHit(Int_t pos);
    Int_t GetCellOccupy(Int_t pos){return (pos<36 && pos>=0) ? cell_occupy.at(pos) : -1;};
    void PrintCellOccupy();
    
    //efficieny paoloni's method
    void Efficiency_paoloni(vector<Int_t> &pivot, vector<Int_t> &probe);
    void ResetEffPaoloni(){eff_paoloni=-3;eff_paolonixview=-3;eff_paoloniyview=-3;return;};
    Double_t GetEffPaoloni(){return eff_paoloni;};
    Double_t GetEffPaolonixview(){return eff_paolonixview;};
    Double_t GetEffPaoloniyview(){return eff_paoloniyview;};

  public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

  private:
    TClonesArray*   fListOfHits;			    // hits
   
  private:
   static TString fgkBranchName;    // Branch name in TTree
   
   vector<Int_t>   cell_occupy;    //occupancy of the BM cell
   Double_t        eff_paoloni;    //value of the efficiency calculated with the Paoloni's method
   Double_t        eff_paolonixview;    //value of the efficiency calculated with the Paoloni's method for the x view
   Double_t        eff_paoloniyview;    //value of the efficiency calculated with the Paoloni's method for the y view
   
   ClassDef(TABMntuRaw,1)
};

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

inline TABMntuHit* TABMntuRaw::Hit(Int_t i)
{
  return (TABMntuHit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

inline const TABMntuHit* TABMntuRaw::Hit(Int_t i) const
{
  return (const TABMntuHit*) ((*fListOfHits)[i]);;
}


#endif
