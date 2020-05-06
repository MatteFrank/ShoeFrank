
#ifndef _TCGmaterials_HXX
#define _TCGmaterials_HXX

#include <vector>
#include <map>

#include "TString.h"
#include "TGeoMaterial.h"
#include "G4Material.hh"

#include "TAGbaseMaterials.hxx"

using namespace std;

class TGeoMixture;
class TGeoElementTable;

class TCGmaterials : public TAGbaseMaterials
{
public:
   TCGmaterials();
   virtual ~TCGmaterials();
   
    G4Material* CreateG4Material(TString name, G4double density=1.0, G4State state=kStateUndefined, G4double temperature=273.15, G4double pressure=1.013e5);
    G4Material* CreateG4Mixture(TString formula, const TString densities, const TString prop, G4double density);
    G4Material* ConvertGeoMaterial(const TGeoMaterial *mat);
    void CreateG4DefaultMaterials();

public:
   static TCGmaterials* Instance();
   
private:
   static TCGmaterials* fgInstance;

};

#endif
