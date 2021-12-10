
#ifndef _TAGbaseMaterials_HXX
#define _TAGbaseMaterials_HXX

/*!
 \file TAGbaseMaterials.hxx
 \brief   Declaration of TAGbaseMaterials.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>
#include <map>

#include "TString.h"
#include "TGeoMaterial.h"

#include "TAGobject.hxx"

using namespace std;

class TGeoElementTable;
class TAGionisMaterials;

class TAGbaseMaterials : public TAGobject
{
public:
   TAGbaseMaterials();
   virtual ~TAGbaseMaterials();
   
protected:
   TAGionisMaterials* fIonisation; //! pointer for ionisation property

   static map<TString, TString> fgkCommonName; ///< Common name for some material map
   static map<TString, Int_t>   fgkLowMat;     ///< Low material map for Fluka
   static TString               fgkWhat;       ///< kWhat symbol for Fluka
   static Int_t                 fgkWhatWidth;

   struct FlukaMat_t {
      TString     Name;      ///< material name
      Int_t       Z;         ///< atomic number
      Double_t    Amean;     ///< mean mass number
      Double_t    Density;   ///< density (g/cmˆ3)
   };
   FlukaMat_t  fFlukaMat[128]; ///< Fluka material

protected:
   vector<TString>              fIsotope;       ///< List of Isotope
   vector<Float_t>              fIsotopeWeight; ///< List of weights for isotope
   TGeoElementTable*            fTable;         ///< TGeoElement table
   map<TString, Int_t>          fPrintedElt;    ///< printed element

protected:
   //! Get isotope from formula
   void             GetIsotopes(const TString formula);
   //! Get sub formula
   TString          GetSubFormula(const TString formula);
   //! Get isotope and weight from formula
   void             GetIsotopeAndWeight(const TString formula);
   //! Get strings
   vector<TString>  GetStrings(TString key, const Char_t delimiter = '/');
   //! Get coefficient
   void             GetCoeff(TString key, Float_t* coeff, Int_t size,  const Char_t delimiter = '/');
   //! Find value from string
   TString          FindByValue(TString value);
   //! Create default materials
   virtual void     CreateDefaultMaterials();
   //! Read Fluka materials
   Bool_t           ReadFlukaDefMat();
   //! Get Fluka material Id
   Int_t            GetFlukaMatId(Double_t Z);
   //! Check Fluka material
   void             CheckFlukaMat(Double_t density, Double_t A, Double_t Z);

   ClassDef(TAGbaseMaterials,0)
};

#endif
