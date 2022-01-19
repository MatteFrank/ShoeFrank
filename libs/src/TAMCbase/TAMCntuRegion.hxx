#ifndef _TAMntuRegion_HXX
#define _TAMntuRegion_HXX

/*!
 \file TAMCntuRegion.hxx
 \brief   Declaration of TAMCregion and TAMCntuRegion.
 */
/*------------------------------------------+---------------------------------*/

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
   
   //! Get track index
   Int_t         GetTrackIdx()    const  { return fID;         }
   //! Get Id (aka track index)
   Int_t         GetID()          const  { return fID;         }
   //! Get number of crossing region
   Int_t         GetCrossN()      const  { return fCrossN;     }
   //! Get number of old crossing region
   Int_t         GetOldCrossN()   const  { return fOldCrossN;  }
   //! Get poistion
   TVector3      GetPosition()    const  { return fPosition;   }
   //! Get momentum
   TVector3      GetMomentum()    const  { return fMomentum;   }
   //! Get mass
   Double_t      GetMass()        const  { return fMass;       }
   //! Get atomic charge
   Double_t      GetCharge()      const  { return fCharge;     }
   //! Get time
   Double_t      GetTime()        const  { return fTime;       }

   //! Set track index
   void          SetTrackIdx(int aid)      { fID = aid;        }
   //! Set Id (aka track index)
   void          SetID(int aid)            { fID = aid;        }
   //! Set number of crossing region
   void          SetCrossN(int reg)        { fCrossN = reg;    }
   //! Set number of old crossing region
   void          SetOldCrossN(int reg)     { fOldCrossN = reg; }
   //! Set poistion
   void          SetPosition(TVector3 pos) { fPosition = pos;  }
   //! Set momentum
   void          SetMomentum(TVector3 mom) { fMomentum = mom;  }
   //! Set mass
   void          SetMass(Double_t m)       { fMass     = m;    }
   //! Set atomic charge
   void          SetCharge(Double_t ch)    { fCharge   = ch;   }
   //! Set time
   void          SetTime(Double_t time)    { fTime     = time; }

   Int_t         fID;          ///< identity
   Int_t         fCrossN;      ///< number of crossed regions
   Int_t         fOldCrossN;   ///< number of old crossed regions
   TVector3      fPosition;    ///< position
   TVector3      fMomentum;    ///< momentum
   Double32_t    fMass;        ///< mass
   Double32_t    fCharge;      ///< charge
   Double32_t    fTime;        ///< time fo flight
   
   ClassDef(TAMCregion,1)
};

//##############################################################################

class TAMCntuRegion : public TAGdata {
public:
   TAMCntuRegion();
   virtual          ~TAMCntuRegion();
   
   // Get number of regions
   Int_t             GetRegionsN() const;
   // Get region
   TAMCregion*       GetRegion(Int_t i);
   // Get region (const)
   const TAMCregion* GetRegion(Int_t i) const;
   
   // New region
   TAMCregion*       NewRegion(Int_t id, Int_t nreg, Int_t nregold,TVector3 pos,TVector3 mom, Double_t mass,
                               Double_t charge, Double_t time);
   // Set up clones
   virtual void      SetupClones();
   // Clear
   virtual void      Clear(Option_t* opt="");
   // To stream
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   //! Get branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();  }
   //! Get default parameter name
   static const Char_t* GetDefParaName()  { return fgkDefParaName.Data(); }

private:
   TClonesArray*     fListOfRegions;    ///< region crossing list
  
private:
   static       TString fgkBranchName;  ///< Branch name
   static const TString fgkDefParaName; ///< Default parameter name

   ClassDef(TAMCntuRegion,1)
   
};

#endif
