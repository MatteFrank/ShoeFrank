#ifndef _TASTntuHit_HXX
#define _TASTntuHit_HXX
/*!
 \file
 \version $Id: TASTntuHit.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TASTntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TArrayI.h"

class TASThit : public TAGobject {
public:
   
  TASThit();
  TASThit(Double_t charge, Double_t de, Double_t time);
  virtual         ~TASThit();
  
  
  Double_t       GetTime()                  const   { return fTime;                 }
  Double_t       GetCharge()                const   { return fCharge;               }
  Double_t       GetDe()                    const   { return fCharge;               }
  
  inline void SetTime(double value){ fTime = value;}
  inline void SetCharge(double value){ fCharge = value;}
  inline void SetDe(double value){ fDe = value;}
  

  Int_t          GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
  Int_t          GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
  Int_t          GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
   
  void           Clear(Option_t* option = "C");
  void           AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);
   
private:
 
  Double32_t      fCharge;
  Double32_t      fDe;
  Double32_t      fTime;
  
  TArrayI         fMCindex;                  // Id of the hit created in the simulation
  TArrayI         fMcTrackIdx;               // Index of the track created in the simulation
   
   ClassDef(TASThit,1)
};

//##############################################################################

class TASTntuHit : public TAGdata {
public:
   
   TASTntuHit();
   virtual         ~TASTntuHit();
   
   Int_t             GetHitsN() const;
   
   TASThit*          GetHit(Int_t i_ind);
   const TASThit*    GetHit(Int_t i_ind) const;
   TASThit*          NewHit(double charge, double de, double time);
   virtual void      Clear(Option_t* opt="");
   void              SetupClones();
   
   void SetCharge(double value)         { fCharge = value;      }
   void SetTriggerTime(double value)    { fTrigTime = value;    }
   void SetTriggerTimeOth(double value) { fTrigTimeOth = value; }
   void SetTrigType(int value)          { fTrigType=value;      }
  
   double GetCharge()         const { return fCharge;      }
   double GetTriggerTime()    const { return fTrigTime;    }
   double GetTriggerTimeOth() const { return fTrigTimeOth; }
   int    GetTrigType()       const { return fTrigType;    }

  
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
  TClonesArray* fListOfHits;			    // hits
  Double32_t    fTrigTime;
  Double32_t    fTrigTimeOth;
  Double32_t    fCharge;
  int           fTrigType;

private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TASTntuHit,2)
};


#endif
