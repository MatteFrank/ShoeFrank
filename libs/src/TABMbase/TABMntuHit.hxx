#ifndef _TABMntuHit_HXX
#define _TABMntuHit_HXX
/*!
  \file    TABMntuHit.hxx
  \brief   Declaration of TABMntuHit, the class for the BM hits
*/
/*------------------------------------------+---------------------------------*/

#include "TObject.h"
#include "TClonesArray.h"

#include "TAGdata.hxx"
#include "TABMparGeo.hxx"
#include "TABMhit.hxx"

#include "TString.h"

#include <map>
#include <string.h>
#include <fstream>
#include <bitset>
#include <algorithm>
#include <vector>

using namespace std;

//##############################################################################

class TABMntuHit : public TAGdata {
  public:
                    TABMntuHit();
    virtual         ~TABMntuHit();

    //default
    virtual void      SetupClones();
    virtual void      Clear(Option_t* opt="");
    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
    static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }


    //getters
    //! Get the total number of accepted BM hits
    Int_t             GetHitsN()     const { return fListOfHits->GetEntries(); }
    //! Get the number of hits selected for a track on the XZ view
    Int_t             GetSelhitsNx() const { return fSelhitsNX; }
    //! Get the number of hits selected for a track on the YZ view
    Int_t             GetSelhitsNy() const { return fSelhitsNY; }
    //! Get the total number of hits on the XZ view
    Int_t             GetTothitsNx() const { return fTothitsNX; }
    //! Get the total number of hits on the YZ view
    Int_t             GetTothitsNy() const { return fTothitsNY; }
    //! Get the total number of hits selected for a track
    Int_t             GetSelhitsNs() const { return fSelhitsNX + fSelhitsNY; }

    //setters
    //! Set the number of hits selected for a track on the XZ view
    void              SetSelhitsNx(Int_t selin) { fSelhitsNX=selin; }
    //! Set the number of hits selected for a track on the YZ view
    void              SetSelhitsNy(Int_t selin) { fSelhitsNY=selin; }

    //other
    TABMhit*          GetHit(Int_t i);
    const TABMhit*    GetHit(Int_t i) const;
    TABMhit*          NewHit(Int_t id, Int_t il, Int_t iv, Int_t ic, Double_t r, Double_t t, Double_t s);

    //CellOccupy methods
    void ClearCellOccupy();
    Int_t GetCellOccupy(Int_t pos);
    void PrintCellOccupy();

    //efficieny metods
    void Efficiency_paoloni(Int_t pivot[], Int_t probe[]);
    //! Reset the efficiency variables
    void ResetEffPaoloni(){fEffPaoloni=-3;fEffPaoloniXview=-3;fEffPaoloniYview=-3;return;};
    //! Get the total efficiency evaluation
    Float_t GetEffPaoloni(){return fEffPaoloni;};
    //! Get the efficiency evaluation for the XZ view
    Float_t GetEffPaolonixview(){return fEffPaoloniXview;};
    //! Get the efficiency evaluation for the YZ view
    Float_t GetEffPaoloniyview(){return fEffPaoloniYview;};

  private:
    TClonesArray*   fListOfHits;		     ///< list of TABMhit
    static TString fgkBranchName;        ///< Branch name in TTree

    map<Int_t,Int_t> fCellOccMap;        ///< BM occupancy map<cellid, counter for number of hits>
    Float_t          fEffPaoloni;        ///< value of the total efficiency calculated with the method described in Paoloni et al. (2012)
    Float_t          fEffPaoloniXview;   ///< value of the efficiency calculated with the method described in Paoloni et al. (2012) only for the xz view
    Float_t          fEffPaoloniYview;   ///< value of the efficiency calculated with the method described in Paoloni et al. (2012) only for the yz view

    //for the reconstructed track
    Int_t            fSelhitsNX;           ///< number of selected hits on x view for the reconstructed track
    Int_t            fSelhitsNY;           ///< number of selected hits on y view for the reconstructed track
    Int_t            fTothitsNX;           ///< Total number of  hits on xz view
    Int_t            fTothitsNY;           ///< Total number of  hits on yz view

    ClassDef(TABMntuHit,2)
};



#endif
