#ifndef _TAVTdigitizerG_HXX
#define _TAVTdigitizerG_HXX

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
class TAVTdigitizerG : public TAVTbaseDigitizer {
   
   // Class to digitize the energy into pixel based on given patterns
public:
   TAVTdigitizerG(TAVTbaseParGeo* parGeo);
   virtual ~TAVTdigitizerG();
   
   Bool_t   MakeCluster(Double_t x0, Double_t y0, Double_t zin, Double_t zout);
   
private:
   TF1*     fFuncClusterDisX;   // cluster distribution function in X
   TF1*     fFuncClusterDisY;   // cluster distribution function in Y
   
private:
   void     SetFunctions();
   Int_t    GetAdcValue(Float_t charge);

private:
   static Float_t  fgkThres;
   static Float_t  fgkFWTH;
};
        

#endif

