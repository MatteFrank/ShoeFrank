
#ifndef _TCGmaterials_HXX
#define _TCGmaterials_HXX

/*!
 \file TCGmaterials.hxx
 \brief  Interface for creating G4 material/mixture from root
 
 \author M. Vanstalle
 */

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
   
   //! Create G4 material from root
    G4Material* CreateG4Material(TString name, G4double density=1.0, G4State state=kStateUndefined, G4double temperature=273.15, G4double pressure=1.013e5);
   //! Create G4 mixture from root
    G4Material* CreateG4Mixture(TString formula, const TString densities, const TString prop, G4double density);
   //! Convert root material to G4 material
    G4Material* ConvertGeoMaterial(const TGeoMaterial *mat);
   //! Create G4 default materials
    void CreateG4DefaultMaterials();

public:
   //! Get instance
   static TCGmaterials* Instance();
   
private:
   static TCGmaterials* fgInstance; ///< Instance

};

#endif
