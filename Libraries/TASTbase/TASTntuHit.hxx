#ifndef _TASTntuHit_HXX
#define _TASTntuHit_HXX
/*!
 \file TASTntuHit.hxx
 \brief   Declaration of TASTntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TArrayI.h"

/*!
 \class TASThit
 \brief Hit class for ST
 */
class TASThit : public TAGobject {
public:
   
  TASThit();
  TASThit(Double_t charge, Double_t de, Double_t time, bool pileup = false);
  virtual         ~TASThit();
  
  //! Get the ST time in ns  
  Double_t       GetTime()                  const   { return fTime;             }
  //! Get the ST charge (in MC it provides the true ST time in ns)  
  Double_t       GetCharge()                const   { return fCharge;           }
  //! Get the ST edep (in DATA is not calibrated and in MC is the true edep)
  Double_t       GetDe()                    const   { return fDe;               }
  //! Get the ST rise time for resolution studies
    Bool_t         IsValid()                  const   { return fIsValid;          }

  inline void SetTime(double value)                 { fTime = value;            }
  inline void SetCharge(double value)               { fCharge = value;          }
  inline void SetDe(double value)                   { fDe = value;              }
  inline void SetPileUp(bool value)                 { fPileUp = value;          }

  Int_t          GetMcIndex(Int_t index)    const   { return fMCindex[index];   }
  Int_t          GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];}
  Int_t          GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
  //! Pile up from consecutive events
  bool           GetPileUp()                const   {return fPileUp; }
  void           Clear(Option_t* option = "C");
  void           AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);
   
//! Validity check for the ST hit reconstruction: not valid if under a default energy threshold
  void           SetValid(Bool_t t)                 { fIsValid   = t;           }
  
private:
 
  Double32_t      fCharge;   ///< charge in DATA. In MC here it is stored the true MC ST time in ns
  Double32_t      fDe;       ///< energy loss. Not calibrated in DATA. In MC here it is stored the true MC quantity
  Double32_t      fTime;     ///< time of ST in ns
  bool            fPileUp;

  
  TArrayI         fMCindex;                  // Id of the hit created in the simulation
  TArrayI         fMcTrackIdx;               // Index of the track created in the simulationOB

  Bool_t          fIsValid;
   
   ClassDef(TASThit,1)
};

//##############################################################################

/*!
 \class TASTntuHit
 \brief Hit conatiner class for St
 */
class TASTntuHit : public TAGdata {
public:
   
   TASTntuHit();
   virtual         ~TASTntuHit();
   
   Int_t             GetHitsN() const;
   
   TASThit*          GetHit(Int_t i_ind);
   const TASThit*    GetHit(Int_t i_ind) const;
   TASThit*          NewHit(double charge, double de, double time, bool pileup = false );
   virtual void      Clear(Option_t* opt="");
   void              SetupClones();
   
   void SetCharge(double value)         { fCharge = value;      }
   void SetTriggerTime(double value)    { fTrigTime = value;    }
   void SetTriggerTimeOth(double value) { fTrigTimeOth = value; }
   void SetTrigType(int value)          { fTrigType=value;      }
   void SetRiseTime(Double_t value)                 { fRiseTime = value;          }

  
   double GetCharge()             const { return fCharge;       }
   double GetTriggerTime()        const { return fTrigTime;     }
   double GetTriggerTimeOth()     const { return fTrigTimeOth;  }
   int    GetTrigType()           const { return fTrigType;     }
   Double_t       GetRiseTime()              const   { return fRiseTime;               }
  


private:
  TClonesArray* fListOfHits;			    // hits
  Double32_t    fTrigTime;
  Double32_t    fTrigTimeOth;
  Double32_t    fCharge;
  int           fTrigType;
  Double32_t      fRiseTime;

  
   ClassDef(TASTntuHit,2)
};


#endif
