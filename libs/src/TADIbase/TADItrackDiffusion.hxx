#ifndef _TADItrackDiffusion_HXX
#define _TADItrackDiffusion_HXX
/*!
 \file
 \brief   Declaration of TADItrackDiffusion.
 
 \author C.A. Reidel & Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TF1.h"

#include "TAGpara.hxx"

class TADItrackDiffusion : public TAGpara {
   
public:
   TADItrackDiffusion();
   virtual ~TADItrackDiffusion();
   
   Double_t SigmaTheta(Double_t* x, Double_t* par);
   Float_t  SigmaThetaCalc(Float_t energy, TString mat, Float_t x, Float_t A, Float_t Z);
   
   Float_t  EnergyCalc(Float_t energy, Float_t massNumber, Int_t atomicNumber, Float_t WEPL);
   Float_t  PCCalc(Float_t energy, Float_t massNumber);
   Float_t  BetaCalc(Float_t energy);
   Float_t  WEPLCalc(const TString& material, Float_t thickness);

   Float_t  GetRadLength(TString name); 
   Float_t  GetDensity(TString name);
   
private:
   struct MaterialParameter_t {
      Float_t   RadiationLength; // Radiation length (g/cm2)
      Float_t   Density;         // Density (g/cm3)
   };
   
   // Parameters for energy loss calculation
   Float_t        fAlpha;
   Float_t        fPfactor;
   
   TF1*           fFuncSigTheta;
   
   ClassDef(TADItrackDiffusion,0)
};

#endif
