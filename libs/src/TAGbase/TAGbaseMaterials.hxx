
#ifndef _TAGbaseMaterials_HXX
#define _TAGbaseMaterials_HXX

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

   static map<TString, TString> fgkCommonName;
   static map<TString, Int_t>   fgkLowMat;
   static TString               fgkWhat;
   static Int_t                 fgkWhatWidth;

   struct FlukaMat_t {
	TString     Name;      // material name
	Int_t       Z;         // atomic number
	Double_t    Amean;  // mean mass number
	Double_t    Density;   // density (g/cmˆ3)
   };
   FlukaMat_t  fFlukaMat[128];

protected:
   vector<TString>              fIsotope;
   vector<Float_t>              fIsotopeWeight;
   TGeoElementTable*            fTable;
   map<TString, Int_t>          fPrintedElt;
   Int_t                        fDegugLevel;
   
protected:
   void             GetIsotopes(const TString formula);
   TString          GetSubFormula(const TString formula);
   void             GetIsotopeAndWeight(const TString formula);
   vector<TString>  GetStrings(TString key, const Char_t delimiter = '/');
   void             GetCoeff(TString key, Float_t* coeff, Int_t size,  const Char_t delimiter = '/');
   TString          FindByValue(TString value);
   virtual void     CreateDefaultMaterials();
   Bool_t           ReadFlukaDefMat();
   Int_t            GetFlukaMatId(Double_t Z);
   void             CheckFlukaMat(Double_t density, Double_t A, Double_t Z);


   ClassDef(TAGbaseMaterials,0)
};

#endif
