
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

   ClassDef(TAGbaseMaterials,0)
};

#endif
