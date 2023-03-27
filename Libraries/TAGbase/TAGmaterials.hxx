
#ifndef _TAGmaterials_HXX
#define _TAGmaterials_HXX

/*!
 \file TAGmaterials.hxx
 \brief   Declaration of TAGmaterials.
 */
/*------------------------------------------+---------------------------------*/


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
   
   // Create material from formula and physical parameters
   TGeoMaterial* CreateMaterial(TString formula, Float_t density, Float_t temperature = STP_temperature, Float_t pressure = STP_pressure);
   // Create mixture from formula, density and proportion
   TGeoMixture*  CreateMixture(TString formula, const TString densities, const TString prop, Float_t density);

   // Print Fluka materials
   string      PrintMaterialFluka();
   // Get Fluka material
   TString     GetFlukaMatName(TString matname);
  
private:
   map<TString, TString> fNameMap;   ///< Map name
   
public:
    static TAGmaterials* Instance(); ///< Instance

private:
    static TAGmaterials* fgInstance; ///< Instance

private:
   // Add Fluka
   TString  AppendFluka(const Char_t* string, Int_t what = 1);
   // Prepend Fluka
   TString  PrependFluka(const Char_t* string, Int_t what = 1);
   // Prepend Fluka name
   TString  PrependFlukaName(const Char_t* string, Int_t what = 1, Int_t align = 1);
   
   ClassDef(TAGmaterials,1)
};

#endif
