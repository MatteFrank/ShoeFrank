#ifndef _TADItrackEmProperties_HXX
#define _TADItrackEmProperties_HXX
/*!
 \file
 \brief   Declaration of TADItrackEmProperties.
 
 \author C.A. Reidel & Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TF1.h"

#include "TAGpara.hxx"

class TGeoMixture;
class TADItrackEmProperties : public TAGpara {
   
public:
   TADItrackEmProperties();
   virtual ~TADItrackEmProperties();
   
   Double_t SigmaTheta(Double_t* x, Double_t* par);
   Float_t  GetSigmaTheta(const TString& matTarget, Float_t x, Float_t energy, const TString& beam);
   Float_t  GetSigmaTheta(const TString& matTarget, Float_t x, Float_t energy, Float_t bA, Float_t bZ);

   Float_t  GetEnergyLoss(const TString& mat, Float_t thickness, Float_t energy, Float_t massNumber, Int_t atomicNumber);
   Float_t  GetEnergyLoss(Float_t energy, Float_t massNumber, Int_t atomicNumber, Float_t WEPL);
   Float_t  GetRange(const TString& mat, Float_t energy, Float_t Abeam, Int_t Zbeam);
   Float_t  GetdEdX(const TString& mat, Double_t beta,  Double_t zBeam);
   Float_t  GetPCC(Float_t energy, Float_t massNumber);
   Float_t  GetBeta(Float_t energy);
   Float_t  GetFacWEPL(const TString& material);

   Float_t  GetRadLength(TString name); 
   Float_t  GetDensity(TString name);
   Float_t  GetA(TString name, Bool_t eff=false);
   Float_t  GetZ(TString name, Bool_t eff=false);
   Float_t  GetMeanExcitationEnergy(TString name);

   Float_t  GetA(TGeoMixture* mix);
   Float_t  GetZ(TGeoMixture* mix);

private:
   TF1*     fFuncSigTheta;
   
private:
   static Float_t fgkX0w;     // radiation for water
   static Float_t fgkElossK;  // K factor for Bethe-Bloch formula

   ClassDef(TADItrackEmProperties,0)
};

#endif
