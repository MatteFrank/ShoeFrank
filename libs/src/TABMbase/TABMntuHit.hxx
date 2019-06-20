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
    TABMntuHit(Int_t iv, Int_t il, Int_t ic, Double_t r, Double_t t, Double_t s);
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
    Bool_t GetIsSelected() {return isSelected;};
    Bool_t GetIsFake() {return isFake;};
    Double_t GetRealRdrift(){return realRdrift;};
    Double_t GetRdriftSmear(){return fabs(realRdrift-rdrift);};

    void SmearRdrift(Int_t smear_type); //to smear rdrift with resolution, use it ONLY for MC events!

    //setters
    void SetChi2(Double_t in_chi2) { ichi2 = in_chi2;};
    void SetSigma(Double_t in_sigma) {sigma = in_sigma;};
    void SetRdrift(Double_t in_rdrift){rdrift=in_rdrift;};
    void SetRealRdrift(Double_t in_rdrift){realRdrift=in_rdrift;};
    void SetIsSelected(bool in_is){isSelected=in_is;};
    void SetResidual(Double_t res_in){residual=res_in;};
    void SetResidualSigma(Double_t res_in){residual=res_in*sigma;};
    void SetA0(TVector3 a_in){A0=a_in;};
    void SetWvers(TVector3 w_in){Wvers=w_in;};
    void SetIsFake(Int_t in_fake){isFake=in_fake;};
   
   Int_t           GetMcIndex(Int_t index)   const   { return fMCindex[index];      }
   Int_t           GetMcTrackI(Int_t index)  const   { return fMcTrackId[index];    }
   Int_t           GetMcTrackCount()         const   { return fMcTrackId.GetSize(); }
   
   void            Clear(Option_t* option = "C");
   void            AddMcTrackId(Int_t trackId, Int_t mcId = -1);


  ClassDef(TABMntuHit,1)

  private:
    Int_t iview;    
    Int_t ilayer;    
    Int_t icell;
    Double_t ichi2;
    Double_t rdrift;
    Double_t tdrift;//hit.time - T0 - irtime (irtime is the trigger time/start counter time) 
    //~ Double_t timmon;//hit.time - irtime
    Double_t sigma;//rdrift resolution
    Double_t residual;//difference between rdrift and the fitted rdrift 
    TVector3  A0;    //Position of the hit's wire
    TVector3  Wvers; //direction of the hit's wire
    
    Bool_t      isSelected;   //isSelected==true means that this hit is in the best track 
    
    //MC parameters
    Double_t  realRdrift;   //real rdrift, not smeared, only for MC 
    Int_t      isFake;   //true =  the hit is generated by FakeHitCreator
    TArrayC         fMCindex;                  // Id of the hit created in the simulation
    TArrayC         fMcTrackId;                // Id of the track created in the simulation
};


#include "TABMntuHit.icc"

#endif


