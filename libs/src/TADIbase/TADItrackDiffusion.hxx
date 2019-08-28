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
   Float_t  GetSigmaTheta(const TString& mat, Float_t x, Float_t energy, Float_t A, Float_t Z);
   
   Float_t  GetEnergyLoss(const TString& mat, Float_t thickness, Float_t energy, Float_t massNumber, Int_t atomicNumber);
   Float_t  GetEnergyLoss(Float_t energy, Float_t massNumber, Int_t atomicNumber, Float_t WEPL);
   Float_t  GetPCC(Float_t energy, Float_t massNumber);
   Float_t  GetBeta(Float_t energy);
   Float_t  GetWEPL(const TString& material, Float_t thickness);

   Float_t  GetRadLength(TString name); 
   Float_t  GetDensity(TString name);
   
private:
   TF1*     fFuncSigTheta;
   
private:
   static Float_t fgkX0w;   // radiation for water
   
   ClassDef(TADItrackDiffusion,0)
};

#endif
