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
    TABMntuTrackTr& operator=(TABMntuTrackTr const& in);
   
    //Setters
    void SetNhitX (Int_t nhi)                           { fNHitX = nhi;};
    void SetNhitY (Int_t nhi)                           { fNHitY = nhi;};
    void SetOrigin(Double_t x,Double_t y, Double_t z)   { fOrigin.SetXYZ(x,y,z);};
    void SetOrigin(TVector3 r0in)                       { fOrigin=r0in;};
    void SetOriginX(Double_t x)                         { fOrigin.SetX(x);};
    void SetOriginY(Double_t y)                         { fOrigin.SetY(y);};
    void SetSlope(Double_t x,Double_t y, Double_t z)    { fSlope.SetXYZ(x,y,z);};
    void SetSlope(TVector3 pin)                         { fSlope=pin;};
    void SetSlopeX(Double_t x)                          { fSlope.SetX(x);};
    void SetSlopeY(Double_t y)                          { fSlope.SetY(y);};
    void SetChiSquare(Double_t chi2_in)                 { fChiSquare=chi2_in;};
    void SetChiSquareX(Double_t chi2x_in)               { fChiSquareX=chi2x_in;};
    void SetChiSquareY(Double_t chi2y_in)               { fChiSquareY=chi2y_in;};
    void SetGhost(Int_t gho_in)                         { fGhost=gho_in;};
    void SetNhit (Int_t nhi) { fHitsN = nhi;};
    void SetIsConverged(Int_t conv) {fIsConverged = conv;};
    void SetR0(Double_t x,Double_t y, Double_t z){fR0.SetXYZ(x,y,z);};
    void SetR0(TVector3 r0in){fR0=r0in;};
    void SetPvers(Double_t x,Double_t y, Double_t z){fPvers.SetXYZ(x,y,z);};
    void SetPvers(TVector3 pin){fPvers=pin;};
    void SetChi2Red(Double_t chi2Red_in){fChi2Red=chi2Red_in;};
    void NewSet(TVectorD ftrackpar);//set fPvers and R0, used for the FIRST tracking
    void SetEffFittedPlane(Double_t eff_in){fEffFittedPlane=eff_in;};

    //Getters
    Int_t  GetNhitX()             const {return fNHitX;};
    Int_t  GetNhitY()             const {return fNHitY;};
    Int_t  GetNhitTot()           const {return fNHitX+fNHitY;};
    Double_t GetChiSquare()       const {return fChiSquare;};
    Double_t GetChiSquareX()      const {return fChiSquareX;};
    Double_t GetChiSquareY()      const {return fChiSquareY;};
    TVector3 GetOrigin()          const {return fOrigin;};
    TVector3 GetSlope()           const {return fSlope;};
    Int_t GetIsGhost()            const {return fGhost;};
    Int_t    GetNhit()           const { return fHitsN;          }
    Double_t GetChi2Red()        const { return fChi2Red;        }
    Int_t    GetIsConverged()    const { return fIsConverged;    }
    TVector3 GetPvers()          const { return fPvers;          }
    TVector3 GetR0()             const { return fR0;             }
    Double_t GetEffFittedPlane() const { return fEffFittedPlane; }

    void PrintR0Pvers();

   TVector3 PointAtLocalZ(double zloc);
    Int_t mergeTrack(const TABMntuTrackTr &otherview);

private:
    //parameters
    Int_t         fNHitX;	              //number of associated hits (different from nwire because of hits in the same cell)
    Int_t         fNHitY;	              //number of associated hits (different from nwire because of hits in the same cell)
    Double_t      fChiSquare;           //reduced chi2
    Double_t      fChiSquareX;          //reduced chi2 on xz view
    Double_t      fChiSquareY;          //reduced chi2 on yz view
    TVector3      fSlope;               //direction of the track from mylar1_pos to mylar2_pos
    TVector3      fOrigin;              //position of the track on the xy plane at z=0
    Int_t         fGhost;               //to be checked with the vertex: -1=not set, 0=not fGhost, 1=fGhost
    Int_t         fHitsN;	         //number of associated hits (different from nwire because of hits in the same cell)
    Double_t      fChi2Red;         //mychi2 reduced
    Int_t         fIsConverged;     //fit converged flag: 0=not set, 1=converged, 2=not converged
    TVector3      fPvers;           //direction of the track from mylar1_pos to mylar2_pos
    TVector3      fR0;              //position of the track on the xy plane at z=0
    Double_t      fEffFittedPlane;  //efficiency with the Paoloni's hit detection method only with the hits from the fitted tracks with chi2<chi2cut

    ClassDef(TABMntuTrackTr,2)

};


#endif
