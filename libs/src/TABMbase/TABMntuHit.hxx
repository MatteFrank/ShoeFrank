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
   
    //Getters
  //    void SetAW();
    Int_t    GetCell()                  const {return fCell;};
    Int_t    GetPlane()                 const {return fPlane;};
    Int_t    GetView()                  const {return fView;};
    Double_t GetRdrift()                  const {return fRdrift;};
    Double_t GetTdrift()                const {return fTdrift;};
    Double_t GetChi2()                  const {return fChi2;};
    Double_t GetSigma()                 const {return fSigma;};
    TVector3 GetWirePos()               const {return fWirePos;};
    TVector3 GetWireDir()               const {return fWireDir;};
    Double_t GetResidual()              const {return fResidual;};
    Int_t    GetIsSelected()            const {return fIsSelected;};
    Int_t    GetIsFake()                const {return fIsFake;};
    Int_t    GetIdCell()                const { return fIdCell;};
    Int_t    GetMcIndex(Int_t index)    const { return fMCindex[index];           }
    Int_t    GetMcTrackIdx(Int_t index) const { return fMcTrackIdx[index];        }
    Int_t    GetMcTracksN()             const { return fMcTrackIdx.GetSize();     }

    //setters
    void     SetCell(Int_t in_cell)            { fCell = in_cell; return;};
    void     SetPlane(Int_t in_plane)          { fPlane = in_plane; return;};
    void     SetView(Int_t in_view)            { fView = in_view; return;};
    void     SetIdCell(Int_t in_fIdCell)       { fIdCell = in_fIdCell; return;};
    void     SetChi2(Double_t in_chi2)         { fChi2 = in_chi2; return;};
    void     SetSigma(Double_t in_sigma)       { fSigma = in_sigma; return;};
    void     SetRdrift(Double_t in_rdrift)     { fRdrift=in_rdrift;return;};
    void     SetTdrift(Double_t in_tdrift)     { fTdrift=in_tdrift;return;};
    void     SetIsSelected(Int_t in_is)        { fIsSelected=in_is;return;};
    void     SetResidual(Double_t res_in)      { fResidual=res_in;return;};
    void     SetResidualSigma(Double_t res_in) { fResidual=res_in*fSigma;return;};
    void     SetWirePos(TVector3 a_in)         { fWirePos=a_in;return;};
    void     SetWireDir(TVector3 w_in)         { fWireDir=w_in;return;};
    void     SetIsFake(Int_t in_fake)          { fIsFake=in_fake;return;};
   
   
   void      Clear(Option_t* option = "C");
   void      AddMcTrackIdx(Int_t trackId, Int_t mcId = -1);


  ClassDef(TABMntuHit,1)

  private:
    Int_t     fView;         //0=hit relevant for yz plane (sense wire on x direction), 1= hit relevant for xz plane (sense wire on y direction)
    Int_t     fPlane;        //0-5 number of bm plane
    Int_t     fCell;         //0-2 number of cell
    Int_t     fIdCell;       //0-36 number that identify uniquely the cell
    Double_t  fChi2;
    Double_t  fRdrift;
    Double_t  fTdrift;       //hit.time - T0 - irtime (irtime is the trigger time/start counter time)
    Double_t  fSigma;        //rdrift resolution
    Double_t  fResidual;     //difference between fRdrift and the fitted rdrift
    TVector3  fWirePos;      //Position of the hit's wire
    TVector3  fWireDir;      //direction of the hit's wire
    Int_t     fIsSelected;   //0=not set, 1=selected, -1=not selected

   //MC parameters
    Double_t  fRealrDrift;  //real frDrift, not smeared, only for MC
    Int_t     fIsFake;       //-1=not set, 0=primary hit, 1=secondary hit, 2=fake creator hit
    TArrayI   fMCindex;      // Id of the hit created in the simulation
    TArrayI   fMcTrackIdx;   // Id of the track created in the simulation
};


#endif


