#ifndef _TADItrackEmProperties_HXX
#define _TADItrackEmProperties_HXX
/*!
 \file TADItrackEmProperties.hxx
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
   
   //! Function scattering angle in sigma
   Double_t SigmaTheta(Double_t* x, Double_t* par);
   
   //! Compute scattering angle in sigma with a given beam name
   Float_t  GetSigmaTheta(const TString& matTarget, Float_t x, Float_t energy, const TString& beam);
   //! Compute scattering angle in sigma with a given A,Z
   Float_t  GetSigmaTheta(const TString& matTarget, Float_t x, Float_t energy, Float_t bA, Float_t bZ);

   //! Compute energy loss for a given A,Z
   Float_t  GetEnergyLoss(const TString& mat, Float_t thickness, Float_t energy, Float_t massNumber, Int_t atomicNumber);
   //! Compute energy loss for a given WEPL
   Float_t  GetEnergyLoss(Float_t energy, Float_t massNumber, Int_t atomicNumber, Float_t WEPL);
   //! Compute range
   Float_t  GetRange(const TString& mat, Float_t energy, Float_t Abeam, Int_t Zbeam);
   //! Compute dE/dX
   Float_t  GetdEdX(const TString& mat, Double_t beta,  Double_t zBeam);
   //! Compute momentum x c
   Float_t  GetPCC(Float_t energy, Float_t massNumber);
   //! Compute beta
   Float_t  GetBeta(Float_t energy);
   //! Compute WEPL factor
   Float_t  GetFacWEPL(const TString& material);

   //! Get radation length for a given material
   Float_t  GetRadLength(TString name);
   //! Get density for a given material
   Float_t  GetDensity(TString name);
   //! Get A for a given material
   Float_t  GetA(TString name, Bool_t eff=false);
   //! Get Z for a given material
   Float_t  GetZ(TString name, Bool_t eff=false);
   //! Get mean excitation energy for a given material
   Float_t  GetMeanExcitationEnergy(TString name);

   //! Get A for a given mixture
   Float_t  GetA(TGeoMixture* mix);
   //! Get Z for a given mixture
   Float_t  GetZ(TGeoMixture* mix);

private:
   TF1*     fFuncSigTheta;    ///< Sigma theta function
   
private:
   static Float_t fgkX0w;     ///< radiation for water
   static Float_t fgkElossK;  ///<  K factor for Bethe-Bloch formula

   ClassDef(TADItrackEmProperties,0)
};

#endif
