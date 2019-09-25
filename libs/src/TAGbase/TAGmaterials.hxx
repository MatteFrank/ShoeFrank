
#ifndef _TAGmaterials_HXX
#define _TAGmaterials_HXX

#include <vector>
#include <map>

#include "TString.h"
#include "TGeoMaterial.h"

#include "TAGbaseMaterials.hxx"

using namespace std;

class TGeoMixture;
class TGeoElementTable;

class TAGmaterials : public TAGbaseMaterials
{
public:
   TAGmaterials();
   virtual ~TAGmaterials();
   
   TGeoMaterial* CreateMaterial(TString formula, Float_t density, Float_t temperature = STP_temperature, Float_t pressure = STP_pressure);
   TGeoMixture*  CreateMixture(TString formula, const TString densities, const TString prop, Float_t density);

   string      PrintMaterialFluka();
   TString     GetFlukaMatName(TString matname);
  
   map<TString, TString> NameMap;
   
public:
    static TAGmaterials* Instance();

private:
    static TAGmaterials* fgInstance;

private:
   TString  AppendFluka(const Char_t* string, Int_t what = 1);
   TString  PrependFluka(const Char_t* string, Int_t what = 1);
   TString  PrependFlukaName(const Char_t* string, Int_t what = 1, Int_t align = 1);
   
   ClassDef(TAGmaterials,1)

};

#endif
