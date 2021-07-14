#ifndef _TACAntuHit_HXX
#define _TACAntuHit_HXX


#include "TArrayI.h"
#include "TClonesArray.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"

class TACAhit : public TAGobject {
public:
   TACAhit();
   TACAhit(int cha, double charge, double time, Int_t type = 0);
   virtual         ~TACAhit();
   
   Double_t        GetTime()          const { return fTime;      }
   Double_t        GetCharge()        const { return fCharge;    }
   Int_t           GetCrystalId()     const { return fCrystalId; }
   Int_t           GetType()          const { return fType;      }
   TVector3        GetPosition()      const { return fPosition;  }
   Bool_t          IsValid()          const { return fIsValid;   }

   void            SetTime(double time)     { fTime = time;      }
   void            SetCharge(double charge) { fCharge = charge;  }
   void            SetCrystalId(int id)     { fCrystalId = id;   }
   void            SetType(int type)        { fType = type;      }
   void            SetPosition(TVector3 pos){ fPosition = pos;   }
   void            SetValid(Bool_t t)       { fIsValid   = t;    }
  
   Int_t           GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
   Int_t           GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
   Int_t           GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }

   void            Clear(Option_t* option = "C");
   void            AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);

private:
   Double32_t      fTime;
   Double32_t      fCharge;
   Int_t           fCrystalId;
   Int_t           fType;
   TVector3        fPosition;
   Bool_t          fIsValid;
   
   TArrayI         fMCindex;                  // Id of the hit created in the simulation
   TArrayI         fMcTrackIdx;                // Id of the track created in the simulation
   
   ClassDef(TACAhit,2)
};

//##############################################################################

class TACAntuHit : public TAGdata {
public:
   TACAntuHit();
   virtual         ~TACAntuHit();
   
   Int_t              GetHitsN() const;
   TACAhit*        GetHit(Int_t i_ind);
   const TACAhit*  GetHit(Int_t i_ind) const;
   
   TACAhit*        NewHit(int crys, double charge, double time, int type = 0);
   
   void               SetTrigTime(Double_t time) { fTrigTime = time; }
   Double_t           GetTrigTime()        const { return fTrigTime; }
   
   Int_t              GetNTdc() const  { return fiNTdc;  }
   Int_t              GetNAdc() const  { return fiNTdc;  }
   Int_t              GetNDrop() const { return fiNDrop; }
   
   virtual void       Clear(Option_t* opt="");
   
   void               SetupClones();
   
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

private:
   TClonesArray*   fListOfHits; // hits
   Double_t        fTrigTime;     // SC trigger time
   Int_t           fiNAdc;		    //
   Int_t           fiNTdc;		    //
   Int_t           fiNDrop;		 //
 
private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TACAntuHit,2)
};

#endif
