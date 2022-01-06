#ifndef _TABMtrack_HXX
#define _TABMtrack_HXX

/*------------------------------------------+---------------------------------*/

#include "TAGgeoTrafo.hxx"
#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"

#include "TString.h"
#include "TMath.h"
#include "TVectorD.h"
#include "TVector3.h"
#include "TF1.h"

#include <TDecompChol.h>

#include "math.h"

#define BM_trackpar 4

using namespace std;

class TABMtrack : public TObject {
  public:
                    TABMtrack();
    virtual         ~TABMtrack();

    void Calibrate(TF1* mypol, TF1* mypol2);
    void Clean();
    void Dump() const;
    TABMtrack(const TABMtrack &tr_in);

    TABMtrack& operator=(TABMtrack const& in);

    //Setters
    void SethitsNx (Int_t nhi)                          { fNHitX = nhi;}
    void SethitsNy (Int_t nhi)                          { fNHitY = nhi;}
    void SetIsConv(Int_t conv)                          { fIsConv = conv;}
    void SetOrigin(Double_t x,Double_t y, Double_t z)   { fOrigin.SetXYZ(x,y,z);}
    void SetOrigin(TVector3 r0in)                       { fOrigin=r0in;}
    void SetOriginX(Double_t x)                         { fOrigin.SetX(x);}
    void SetOriginY(Double_t y)                         { fOrigin.SetY(y);}
    void SetSlope(Double_t x,Double_t y, Double_t z)    { fSlope.SetXYZ(x,y,z);}
    void SetSlope(TVector3 pin)                         { fSlope=pin;}
    void SetSlopeX(Double_t x)                          { fSlope.SetX(x);}
    void SetSlopeY(Double_t y)                          { fSlope.SetY(y);}
    void SetChiSquare(Double_t chi2_in)                 { fChiSquare=chi2_in;}
    void SetChiSquareX(Double_t chi2x_in)               { fChiSquareX=chi2x_in;}
    void SetChiSquareY(Double_t chi2y_in)               { fChiSquareY=chi2y_in;}
    void SetGhost(Int_t gho_in)                         { fGhost=gho_in;}
    void SetTrackIdX(Int_t trk_in)                      { fTrackIdX=trk_in;}
    void SetTrackIdY(Int_t trk_in)                      { fTrackIdY=trk_in;}
    // void NewSet(TVectorD ftrackpar);//set fSlope and fOrigin, used for the FIRST tracking

    //Getters
    Int_t  GetHitsNx()            const {return fNHitX;}
    Int_t  GetHitsNy()            const {return fNHitY;}
    Int_t  GetHitsNtot()          const {return fNHitX+fNHitY;}
    Double_t GetChiSquare()       const {return fChiSquare;}
    Double_t GetChiSquareX()      const {return fChiSquareX;}
    Double_t GetChiSquareY()      const {return fChiSquareY;}
    Int_t GetIsConv()             const {return fIsConv;}
    TVector3 GetOrigin()          const {return fOrigin;}
    TVector3 GetSlope()           const {return fSlope;}
    Int_t GetIsGhost()            const {return fGhost;}
    Int_t GetTrackIdX()           const {return fTrackIdX;}
    Int_t GetTrackIdY()           const {return fTrackIdY;}

    //others
    void PrintTrackPosDir();
    TVector3 PointAtLocalZ(double zloc) const;
    TVector3 Intersection(Float_t zloc) const;
    Int_t mergeTrack(const TABMtrack &otherview);

private:
    Int_t         fNHitX;	              //number of associated hits (different from nwire because of hits in the same cell)
    Int_t         fNHitY;	              //number of associated hits (different from nwire because of hits in the same cell)
    Double_t      fChiSquare;           //reduced chi2
    Double_t      fChiSquareX;          //reduced chi2 on xz view
    Double_t      fChiSquareY;          //reduced chi2 on yz view
    Int_t         fIsConv;              //fit converged flag: 0=not set, 1=converged, 2=not converged
    TVector3      fSlope;               //direction of the track from mylar1_pos to mylar2_pos
    TVector3      fOrigin;              //position of the track on the xy plane at z=0
    Int_t         fGhost;               //to be checked with the vertex: -1=not set, 0=not fGhost, 1=fGhost
    Int_t         fTrackIdX;            //track id for the XZ view (view==1)
    Int_t         fTrackIdY;            //track id for the YZ view (view==0)

    ClassDef(TABMtrack,2)

};


#endif
