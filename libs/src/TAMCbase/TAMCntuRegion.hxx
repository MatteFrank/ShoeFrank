#ifndef _TAMntuRegion_HXX
#define _TAMntuRegion_HXX


#include "TObject.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include "TAGdata.hxx"

class TAMCregion : public TObject {
public:
   TAMCregion();
   TAMCregion(Int_t id, Int_t nreg, Int_t nregold,TVector3 pos,TVector3 mom, Double_t mass,
              Double_t charge, Double_t time);
   
   virtual      ~TAMCregion();
   
   Int_t         GetTrackIdx()    const  { return fID;         }
   Int_t         GetID()          const  { return fID;         }
   Int_t         GetCrossN()      const  { return fCrossN;     }
   Int_t         GetOldCrossN()   const  { return fOldCrossN;  }
   TVector3      GetPosition()    const  { return fPosition;   }
   TVector3      GetMomentum()    const  { return fMomentum;   }
   Double_t      GetMass()        const  { return fMass;       }
   Double_t      GetCharge()      const  { return fCharge;     }
   Double_t      GetTime()        const  { return fTime;       }

   
   void          SetTrackIdx(int aid)      { fID = aid;        }
   void          SetID(int aid)            { fID = aid;        }
   void          SetCrossN(int reg)        { fCrossN = reg;    }
   void          SetOldCrossN(int reg)     { fOldCrossN = reg; }
   void          SetPosition(TVector3 pos) { fPosition = pos;  }
   void          SetMomentum(TVector3 mom) { fMomentum = mom;  }
   void          SetMass(Double_t m)       { fMass     = m;    }
   void          SetCharge(Double_t ch)    { fCharge   = ch;   }
   void          SetTime(Double_t time)    { fTime     = time; }

   Int_t         fID;          // identity
   Int_t         fCrossN;      // number of crossed regions
   Int_t         fOldCrossN;   // number of old crossed regions
   TVector3      fPosition;    // position
   TVector3      fMomentum;    // momentum
   Double32_t    fMass;        // mass
   Double32_t    fCharge;      // charge
   Double32_t    fTime;        // time fo flight
   
   ClassDef(TAMCregion,1)
};

//##############################################################################

class TAMCntuRegion : public TAGdata {
public:
   TAMCntuRegion();
   virtual          ~TAMCntuRegion();
   
   Int_t             GetRegionsN() const;

   TAMCregion*       GetRegion(Int_t i);
   const TAMCregion* GetRegion(Int_t i) const;
   
   TAMCregion*       NewRegion(Int_t id, Int_t nreg, Int_t nregold,TVector3 pos,TVector3 mom, Double_t mass,
                               Double_t charge, Double_t time);

   virtual void      SetupClones();
   virtual void      Clear(Option_t* opt="");
   
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();  }
   static const Char_t* GetDefParaName()  { return fgkDefParaName.Data(); }

private:
   TClonesArray*     fListOfRegions; // region crossing list
  
private:
   static       TString fgkBranchName;
   static const TString fgkDefParaName;

   ClassDef(TAMCntuRegion,1)
   
};

#endif
