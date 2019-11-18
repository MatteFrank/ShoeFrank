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
#include "TArrayC.h"

class TASTntuHit : public TAGobject {
public:
   
   TASTntuHit();
   
   TASTntuHit(Int_t channel, Double_t charge, Double_t time);
   
   virtual         ~TASTntuHit();
   
   
   Int_t          GetChannel()              const   { return fChannel;             }
   Double_t       GetTime()                 const   { return fTime;                }
   Double_t       GetCharge()               const   { return fCharge;              }
   
   Int_t          GetMcIndex(Int_t index)   const   { return fMCindex[index];       }
   Int_t          GetMcTrackI(Int_t index)  const   { return fMcTrackIdx[index];    }
   Int_t          GetMcTrackCount()         const   { return fMcTrackIdx.GetSize(); }
   
   void           Clear(Option_t* option = "C");
   void           AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);
   
   
   ClassDef(TASTntuHit,1)
   
private:
   Int_t           fChannel;
   Double32_t      fCharge;
   Double32_t      fTime;
   
   TArrayC         fMCindex;                  // Id of the hit created in the simulation
   TArrayC         fMcTrackIdx;               // Index of the track created in the simulation
   
};

//##############################################################################

class TASTntuRaw : public TAGdata {
public:
   
   TASTntuRaw();
   virtual         ~TASTntuRaw();
   
   Int_t             GetHitsN() const;
   
   TASTntuHit*       Hit(Int_t i_ind);
   const TASTntuHit* Hit(Int_t i_ind) const;
   TASTntuHit*        NewHit(int channel, double charge, double time);
   virtual void      Clear(Option_t* opt="");
   void              SetupClones();
   inline void SetCharge(double value){m_Charge = value;}
   inline void SetTriggerTime(double value){m_TrigTime = value;}
   inline double GetCharge(){return m_Charge;}
   inline double GetTriggerTime(){return m_TrigTime;}
   inline int GetTrigType(){return m_TrigType;}
   inline void SetTrigType(int value){m_TrigType=value;}
   inline int GetDeltaClk(){return m_deltaclk;}
   inline void SetDeltaClk(double value){m_deltaclk=value;}
  

  
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
  TClonesArray*     fListOfHits;			    // hits
  double m_TrigTime;
  double m_Charge;
  double m_deltaclk;
  int m_TrigType;
private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TASTntuRaw,1)
};


#endif
