#ifndef _TABMntuTrackTr_HXX
#define _TABMntuTrackTr_HXX

/*------------------------------------------+---------------------------------*/

#include "TAGgeoTrafo.hxx"
#include "TABMparGeo.hxx"
#include "TABMparCon.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"

#include "TString.h"
#include "TMath.h"
#include "TVectorD.h"
#include "TVector3.h"
#include "TF1.h"

#include <TDecompChol.h>

#include "math.h"

#define BM_trackpar 4

using namespace std;

class TABMntuTrackTr : public TObject {
  public:
                    TABMntuTrackTr();
    virtual         ~TABMntuTrackTr();

    //*************************************In common or to be modified for new tracking*************************
    void Calibrate(TF1* mypol, TF1* mypol2);
    void Clean();
    void Dump() const;
    TABMntuTrackTr(const TABMntuTrackTr &tr_in);

  //******************************************NEW TRACKING:*********************************************

    //~ Int_t SetNew(int nhi, double chi);
    //~ TABMntuTrackTr& operator=(TABMntuTrackTr const& in);
    TABMntuTrackTr& operator=(TABMntuTrackTr const& in){
      if(this!=&in){
        this->nhit=in.nhit;
        this->chi2Red=in.chi2Red;
        this->isConverged=in.isConverged;
        this->Pvers=in.Pvers;
        this->R0=in.R0;
        this->eff_fittedplane=in.eff_fittedplane;
      }
      return *this;
    }


    //Setters
    void SetNhit (Int_t nhi) { nhit = nhi;};
    void SetIsConverged(Int_t conv) {isConverged = conv;};
    void SetR0(Double_t x,Double_t y, Double_t z){R0.SetXYZ(x,y,z);};
    void SetR0(TVector3 r0in){R0=r0in;};
    void SetPvers(Double_t x,Double_t y, Double_t z){Pvers.SetXYZ(x,y,z);};
    void SetPvers(TVector3 pin){Pvers=pin;};
    void SetChi2Red(Double_t chi2red_in){chi2Red=chi2red_in;};
    void NewSet(TVectorD ftrackpar);//set Pvers and R0, used for the FIRST tracking
    void SetEffFittedPlane(Double_t eff_in){eff_fittedplane=eff_in;};

    //Getters
    Int_t  GetNhit() {return nhit;};
    Double_t GetChi2Red() {return chi2Red;};
    Int_t GetIsConverged(){return isConverged;};
    TVector3 GetPvers(){return Pvers;};
    TVector3 GetR0(){return R0;};
    Double_t GetEffFittedPlane(){return eff_fittedplane;};

    void PrintR0Pvers();

   TVector3 PointAtLocalZ(double zloc);

private:
    //parameters
    Int_t         nhit;	          //number of associated hits (different from nwire because of hits in the same cell)
    Double_t      chi2Red;      //mychi2 reduced
    Int_t         isConverged;    //fit converged flag: 0=not set, 1=converged, 2=not converged
    TVector3      Pvers;           //direction of the track from mylar1_pos to mylar2_pos
    TVector3      R0;              //position of the track on the xy plane at z=0
    Double_t      eff_fittedplane;//efficiency with the Paoloni's hit detection method only with the hits from the fitted tracks with chi2<chi2cut

    ClassDef(TABMntuTrackTr,2)

};


#endif
