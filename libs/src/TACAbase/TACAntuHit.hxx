#ifndef _TACAntuHit_HXX
#define _TACAntuHit_HXX

/*!
 \file TACAntuHit.hxx
 \brief   Declaration of TACAntuHit.
 */
/*------------------------------------------+---------------------------------*/


#include "TArrayI.h"
#include "TClonesArray.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"

/*!
 \class TACAhit
 \brief Hit for CA detectors. **
 */

class TACAhit : public TAGobject {
public:
   TACAhit();
   TACAhit(int cha, double charge, double time, Int_t type = 0);
   virtual         ~TACAhit();

   //! Get time
   Double_t        GetTime()          const { return fTime;      }
   //! Get charge
   Double_t        GetCharge()        const { return fCharge;    }
   //! Get crystal id
   Int_t           GetCrystalId()     const { return fCrystalId; }
   //! Get type
   Int_t           GetType()          const { return fType;      }
   //! Get positiion
   TVector3        GetPosition()      const { return fPosition;  }
   //! Valid flag
   Bool_t          IsValid()          const { return fIsValid;   }

   //! Set time
   void            SetTime(double time)     { fTime = time;      }
   //! Set charge
   void            SetCharge(double charge) { fCharge = charge;  }
   //! Set crystal id
   void            SetCrystalId(int id)     { fCrystalId = id;   }
   //! Set type
   void            SetType(int type)        { fType = type;      }
   //! Set position
   void            SetPosition(TVector3 pos){ fPosition = pos;   }
   //! Set valid flag
   void            SetValid(Bool_t t)       { fIsValid   = t;    }

   //! Get MC index
   Int_t           GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
   //! Get MC track index
   Int_t           GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
   //! Get MC track number
   Int_t           GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }

   void            Clear(Option_t* option = "C");
   void            AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);

private:
   Double32_t      fTime;       ///< time
   Double32_t      fCharge;     ///< charge
   Int_t           fCrystalId;  ///< crystal id
   Int_t           fType;       ///< crystal type
   TVector3        fPosition;   ///< position
   Bool_t          fIsValid;    ///< validity flag

   TArrayI         fMCindex;     ///< Id of the hit created in the simulation
   TArrayI         fMcTrackIdx;  ///< Id of the track created in the simulation

   ClassDef(TACAhit,2)
};

//##############################################################################
/*!
 \class TACAntuHit
 \brief Hit container for CA detectors. **
 */

class TACAntuHit : public TAGdata {
public:
   TACAntuHit();
   virtual         ~TACAntuHit();

   Int_t              GetHitsN() const;
   TACAhit*        GetHit(Int_t i_ind);
   const TACAhit*  GetHit(Int_t i_ind) const;

   TACAhit*        NewHit(int crys, double charge, double time, int type = 0);

   //! Set trigger time
   void               SetTrigTime(Double_t time) { fTrigTime = time; }
   //! Get trigger time
   Double_t           GetTrigTime()        const { return fTrigTime; }
   //! Add number of drop
   void               AddNDrop()                 { fiNDrop++;}

   //! Get number of TDC
   Int_t              GetNTdc() const  { return fiNTdc;  }
   //! Get number of ADC
   Int_t              GetNAdc() const  { return fiNTdc;  }
   //! Get number of Drop
   Int_t              GetNDrop() const { return fiNDrop; }

   virtual void       Clear(Option_t* opt="");

   void               SetupClones();

   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;

public:
   //! Get branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

private:
   TClonesArray*   fListOfHits;   ///< list of hits
   Double_t        fTrigTime;     ///< SC trigger time
   Int_t           fiNAdc;		    ///< Adc number
   Int_t           fiNTdc;		    ///< TDC number
   Int_t           fiNDrop;		 ///< drop ?

private:
   static TString fgkBranchName;    ///< Branch name in TTree

   ClassDef(TACAntuHit,2)
};

#endif
