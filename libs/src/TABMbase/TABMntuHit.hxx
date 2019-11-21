#ifndef _TABMntuHit_HXX
#define _TABMntuHit_HXX

#include "TAGdata.hxx"
#include "TABMparGeo.hxx"

#include "TObject.h"
#include "TRandom3.h"
#include "TString.h"
#include "TAGdata.hxx"
#include "TArrayC.h"

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

    //~ void            SetData(Int_t id, Int_t iv, Int_t il, Int_t ic, Double_t x,    Double_t y, Double_t z, Double_t px, Double_t py, Double_t pz, Double_t r, Double_t t,Double_t s);
    Int_t           Cell() const;
    Int_t           Plane() const;
    Int_t           View() const;
    Double_t Dist() const; //return rdrift
    Double_t Tdrift() const;
    //~ Double_t Timmon() const;
    

    //Getters
    void SetAW();
    Double_t GetChi2() {return ichi2;};
    Double_t GetSigma() {return sigma;};
    TVector3 GetA0() {return A0;};
    TVector3 GetWvers() {return Wvers;};
    Double_t GetResidual(){return residual;};
    Int_t GetIsSelected() {return isSelected;};
    Int_t GetIsFake() {return isFake;};
    Double_t GetRealRdrift(){return realRdrift;};
    Double_t GetRdriftSmear(){return fabs(realRdrift-rdrift);};
    Int_t GetCellid(){return cellid;};

    //setters
    void SetCellid(Int_t in_cellid) { cellid = in_cellid; return;};
    void SetChi2(Double_t in_chi2) { ichi2 = in_chi2; return;};
    void SetSigma(Double_t in_sigma) {sigma = in_sigma; return;};
    void SetRdrift(Double_t in_rdrift){rdrift=in_rdrift;return;};
    void SetRealRdrift(Double_t in_rdrift){realRdrift=in_rdrift;return;};
    void SetIsSelected(Int_t in_is){isSelected=in_is;return;};
    void SetResidual(Double_t res_in){residual=res_in;return;};
    void SetResidualSigma(Double_t res_in){residual=res_in*sigma;return;};
    void SetA0(TVector3 a_in){A0=a_in;return;};
    void SetWvers(TVector3 w_in){Wvers=w_in;return;};
    void SetIsFake(Int_t in_fake){isFake=in_fake;return;};
   
   Int_t           GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
   Int_t           GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
   Int_t           GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
   
   void            Clear(Option_t* option = "C");
   void            AddMcTrackIdx(Int_t trackId, Int_t mcId = -1);


  ClassDef(TABMntuHit,1)

  private:
    Int_t iview; //0=hit relevant for yz plane (sense wire on x direction), 1= hit relevant for xz plane (sense wire on y direction)   
    Int_t ilayer; //0-5 number of bm plane   
    Int_t icell;  //0-2 number of cell
    Int_t cellid; //0-36 number that identify uniquely the cell
    Double_t ichi2;
    Double_t rdrift;
    Double_t tdrift;//hit.time - T0 - irtime (irtime is the trigger time/start counter time) 
    //~ Double_t timmon;//hit.time - irtime
    Double_t sigma;//rdrift resolution
    Double_t residual;//difference between rdrift and the fitted rdrift 
    TVector3  A0;    //Position of the hit's wire
    TVector3  Wvers; //direction of the hit's wire
    
    Int_t      isSelected;   //0=not set, 1=selected, -1=not selected 
    
    //MC parameters
    Double_t  realRdrift;   //real rdrift, not smeared, only for MC 
    Int_t      isFake;   //-1=not set, 0=primary hit, 1=secondary hit, 2=fake creator hit
    TArrayC         fMCindex;                  // Id of the hit created in the simulation
    TArrayC         fMcTrackIdx;                // Id of the track created in the simulation
};


#include "TABMntuHit.icc"

#endif


