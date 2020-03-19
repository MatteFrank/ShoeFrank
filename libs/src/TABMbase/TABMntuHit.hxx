#ifndef _TABMntuHit_HXX
#define _TABMntuHit_HXX

#include "TAGdata.hxx"
#include "TABMparGeo.hxx"

#include "TObject.h"
#include "TRandom3.h"
#include "TString.h"
#include "TAGdata.hxx"
#include "TArrayI.h"

#include <vector>
#include <string.h>
#include <fstream>
#include <bitset>
#include <algorithm>

using namespace std;

class TABMntuHit : public TAGdata {
  public:
    TABMntuHit();
    TABMntuHit(Int_t iv, Int_t il, Int_t ic,Int_t id, Double_t r, Double_t t, Double_t s);
    virtual         ~TABMntuHit();

    Int_t    Cell()   const;
    Int_t    Plane()  const;
    Int_t    View()   const;
    Double_t Dist()   const; //return frDrift
    Double_t Tdrift() const;
   
    //Getters
    void SetAW();
    Double_t GetChi2()                  const { return fChi2;        }
    Double_t GetSigma()                 const { return fSigma;       }
    TVector3 GetA0()                    const { return fA0;          }
    TVector3 GetWvers()                 const { return fWvers;       }
    Double_t GetResidual()              const { return fResidual;    }
    Int_t    GetIsSelected()            const { return fIsSelected;  }
    Int_t    GetIsFake()                const { return fIsFake;      }
    Double_t GetRealRdrift()            const { return fRealrDrift;  }
    Double_t GetRdriftSmear()           const { return fabs(fRealrDrift-frDrift); }
    Int_t    GetCellid()                const { return fiCellid;                  }
    Int_t    GetMcIndex(Int_t index)    const { return fMCindex[index];           }
    Int_t    GetMcTrackIdx(Int_t index) const { return fMcTrackIdx[index];        }
    Int_t    GetMcTracksN()             const { return fMcTrackIdx.GetSize();     }

    //setters
    void     SetfiCellid(Int_t in_fiCellid) { fiCellid = in_fiCellid;};
    void     SetChi2(Double_t in_chi2) { fChi2 = in_chi2;};
    void     SetSigma(Double_t in_sigma) {fSigma = in_sigma;};
    void     SetRdrift(Double_t in_frDrift){frDrift=in_frDrift;};
    void     SetTdrift(Double_t in_tdrift){ftDrift=in_tdrift;};
    void     SetRealRdrift(Double_t in_frDrift){fRealrDrift=in_frDrift;};
    void     SetIsSelected(Int_t in_is){fIsSelected=in_is;};
    void     SetResidual(Double_t res_in){fResidual=res_in;};
    void     SetResidualSigma(Double_t res_in){fResidual=res_in*fSigma;};
    void     SetA0(TVector3 a_in){fA0=a_in;};
    void     SetWvers(TVector3 w_in){fWvers=w_in;};
    void     SetIsFake(Int_t in_fake){fIsFake=in_fake;};
   
   
   void      Clear(Option_t* option = "C");
   void      AddMcTrackIdx(Int_t trackId, Int_t mcId = -1);


  ClassDef(TABMntuHit,1)

  private:
    Int_t     fiView;    //0=hit relevant for yz plane (sense wire on x direction), 1= hit relevant for xz plane (sense wire on y direction)
    Int_t     fiLayer;   //0-5 number of bm plane
    Int_t     fiCell;    //0-2 number of cell
    Int_t     fiCellid;  //0-36 number that identify uniquely the cell
    Double_t  fChi2;
    Double_t  frDrift;
    Double_t  ftDrift;   //hit.time - T0 - irtime (irtime is the trigger time/start counter time)

    Double_t  fSigma;    //frDrift resolution
    Double_t  fResidual; //difference between frDrift and the fitted frDrift
    TVector3  fA0;       //Position of the hit's wire
    TVector3  fWvers;    //direction of the hit's wire
    Int_t     fIsSelected;   //0=not set, 1=selected, -1=not selected
    
    //MC parameters
    Double_t  fRealrDrift;  //real frDrift, not smeared, only for MC
    Int_t     fIsFake;       //-1=not set, 0=primary hit, 1=secondary hit, 2=fake creator hit
    TArrayI   fMCindex;      // Id of the hit created in the simulation
    TArrayI   fMcTrackIdx;   // Id of the track created in the simulation
};


#include "TABMntuHit.icc"

#endif


