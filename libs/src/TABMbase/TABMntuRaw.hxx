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

#include <map>
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

    //default
    virtual void      SetupClones();
    virtual void      Clear(Option_t* opt="");
    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
    static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }


    //getters
    Int_t             GetHitsN()    const{return fListOfHits->GetEntries();};
    Int_t             GetNselhitx() const{return fNselhitX;};
    Int_t             GetNselhity() const{return fNselhitY;};
    Int_t             GetNtothitx() const{return fNtothitX;};
    Int_t             GetNtothity() const{return fNtothitY;};
    Int_t             GetNselhits() const{return fNselhitX + fNselhitY;};

    //setters
    void              SetNselhitx(Int_t selin){fNselhitX=selin; return;};
    void              SetNselhity(Int_t selin){fNselhitY=selin; return;};

    //other
    TABMntuHit*       Hit(Int_t i){  return (TABMntuHit*) ((*fListOfHits)[i]);};
    const TABMntuHit* Hit(Int_t i) const{  return (const TABMntuHit*) ((*fListOfHits)[i]);};
    TABMntuHit*       NewHit(Int_t iv, Int_t il, Int_t ic, Int_t id, Double_t r, Double_t t, Double_t s);

    //CellOccupy
    void ClearCellOccupy();
    Int_t GetCellOccupy(Int_t pos);
    void PrintCellOccupy();

    //efficieny
    void Efficiency_paoloni(Int_t pivot[], Int_t probe[]);
    void ResetEffPaoloni(){fEffPaoloni=-3;fEffPaoloniXview=-3;fEffPaoloniYview=-3;return;};
    Float_t GetEffPaoloni(){return fEffPaoloni;};
    Float_t GetEffPaolonixview(){return fEffPaoloniXview;};
    Float_t GetEffPaoloniyview(){return fEffPaoloniYview;};

  private:
    TClonesArray*   fListOfHits;		     // list of TABMntuHit
    static TString fgkBranchName;        // Branch name in TTree

    map<Int_t,Int_t> fCellOccMap;        //BM occupancy map<cellid, counter for number of hits>
    Float_t          fEffPaoloni;        //value of the efficiency calculated with the Paoloni's method
    Float_t          fEffPaoloniXview;   //value of the efficiency calculated with the Paoloni's method for the xz view
    Float_t          fEffPaoloniYview;   //value of the efficiency calculated with the Paoloni's method for the yz view

    //for the reconstructed track
    Int_t            fNselhitX;           //number of selected hits on x view for the reconstructed track
    Int_t            fNselhitY;           //number of selected hits on y view for the reconstructed track
    Int_t            fNtothitX;           //Total number of  hits on x view
    Int_t            fNtothitY;           //Total number of  hits on y view

    ClassDef(TABMntuRaw,1)
};



#endif
