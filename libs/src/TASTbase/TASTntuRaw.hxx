#ifndef _TASTntuRaw_HXX
#define _TASTntuRaw_HXX
/*!
 \file
 \version $Id: TASTntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TASTntuRaw.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TArrayI.h"

class TASTntuHit : public TAGobject {
public:
   
  TASTntuHit();
  TASTntuHit(Double_t charge, Double_t de, Double_t time);
  virtual         ~TASTntuHit();
  
  
  Double_t       GetTime()                  const   { return fTime;                 }
  Double_t       GetCharge()                const   { return fCharge;               }
  Double_t       GetDe()                const   { return fCharge;               }
  
  inline void SetTime(double value){ fTime = value;}
  inline void SetCharge(double value){ fCharge = value;}
  inline void SetDe(double value){ fDe = value;}
  

  Int_t          GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
  Int_t          GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
  Int_t          GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
   
  void           Clear(Option_t* option = "C");
  void           AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);
   
   
  ClassDef(TASTntuHit,1)
   
  private:
 
  Double32_t      fCharge;
  Double32_t      fDe;
  Double32_t      fTime;
  
   
  TArrayI         fMCindex;                  // Id of the hit created in the simulation
  TArrayI         fMcTrackIdx;               // Index of the track created in the simulation
   
};

//##############################################################################

class TASTntuRaw : public TAGdata {
public:
   
   TASTntuRaw();
   virtual         ~TASTntuRaw();
   
   Int_t             GetHitsN() const;
   
   TASTntuHit*       GetHit(Int_t i_ind);
   const TASTntuHit* GetHit(Int_t i_ind) const;
   TASTntuHit*       NewHit(double charge, double de, double time);
   virtual void      Clear(Option_t* opt="");
   void              SetupClones();
   
   void SetCharge(double value){m_Charge = value;}
   void SetTriggerTime(double value){m_TrigTime = value;}
   void SetTriggerTimeOth(double value){m_TrigTime_oth = value;}
   void SetTrigType(int value){m_TrigType=value;}
  
   double GetCharge()         const { return m_Charge;}
   double GetTriggerTime()    const { return m_TrigTime;}
   double GetTriggerTimeOth() const { return m_TrigTime_oth;}
   int    GetTrigType()       const { return m_TrigType;}

  
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
  TClonesArray* m_ListOfHits;			    // hits
  Double32_t    m_TrigTime;
  Double32_t    m_TrigTime_oth;
  Double32_t    m_Charge;
  int           m_TrigType;

private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TASTntuRaw,1)
};


#endif
