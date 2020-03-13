#ifndef _TAVTdigitizerE_HXX
#define _TAVTdigitizerE_HXX

#include <map>

#include "Riostream.h"
#include "TF1.h"
#include "TH2F.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom3.h"
#include "TString.h"

#include "TAVTbaseDigitizer.hxx"

// --------------------------------------------------------------------------------------
class TAVTparGeo;
class TAVTdigitizerE : public TAVTbaseDigitizer {
   
   // Class to digitize the energy into pixel based on given patterns
public:
   TAVTdigitizerE(TAVTbaseParGeo* parGeo);
   virtual ~TAVTdigitizerE();
   
   void           DefineRadii();

   Bool_t         MakeCluster(Double_t x0, Double_t y0, Double_t zin, Double_t zout);
   
private:
   std::map<Int_t, Float_t>  fMapRadius55;
   std::map<Int_t, Float_t>  fMapRadius50;
   std::map<Int_t, Float_t>  fMapRadius00;
   std::map<Int_t, Float_t>* fMapRadius;

private:
   Bool_t          SetRegion(Double_t x0, Double_t y0);
   void            DefineRadius00();
   void            DefineRadius50();
   void            DefineRadius55();
   
private:
   static Int_t    fgkMaxTurn;
   static Int_t    fgkShel00[];
   static Int_t    fgkShel55[];
   static Int_t    fgkShel50[];
   static Int_t    fgkMeshWidth;
   
};
        

#endif

