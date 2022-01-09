//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//

/*!
 \file TCGmaterials.cxx
 \brief Implementation of TCGmaterials.
*/

#include "TAGrecoManager.hxx"
#include "TCGmaterials.hxx"

#include "G4NistManager.hh"
#include "TGeoManager.h"
#include "TObjString.h"

#include "TAGgeoTrafo.hxx"

using namespace CLHEP;

TCGmaterials* TCGmaterials::fgInstance = 0;

/*!
 \class TCGmaterials
 \brief  Interface for creating G4 material/mixture from root
 */

//______________________________________________________________________________
//! Constructor
TCGmaterials::TCGmaterials()
 : TAGbaseMaterials()
{
   CreateDefaultMaterials();
}

//______________________________________________________________________________
//! Destructor
TCGmaterials::~TCGmaterials()
{
}

//______________________________________________________________________________
TCGmaterials* TCGmaterials::Instance()
{
   if (fgInstance == 0)
      fgInstance = new TCGmaterials();

   return fgInstance;
}

//______________________________________________________________________________
G4Material* TCGmaterials::CreateG4Material(TString name, G4double density, G4State state,G4double temperature, G4double pressure)
{
   TGeoMaterial* mat = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->FindObject(name.Data());
   /// If the material is already registered in TGeoManager, then convert it in G4material
   if ( mat != 0x0 ) return ConvertGeoMaterial(mat);

    /// If TGeoMaterial not defined, create G4Material
    TString formula = fgkCommonName[name] ;
    fIsotope.clear();
    fIsotopeWeight.clear();

    if (fgkCommonName[name] != "") {
        formula = fgkCommonName[name];
        Info("CreateMaterial()", "Using formula %s for material %s", formula.Data(), name.Data());
    }

    GetIsotopes(formula);

    G4Material *g4mat = new G4Material((G4String)name,density*g/cm3,(G4int)fIsotope.size(),state,temperature,pressure);
    for (int i = 0; i < (int)fIsotope.size(); ++i) {
        if (FootMcDebugLevel(1))
            printf("%s %g\n", fIsotope[i].Data(), fIsotopeWeight[i]);
        G4Element* g4element = G4NistManager::Instance()->FindOrBuildElement(fIsotope[0].Data());
        if(g4element) g4mat->AddElement(g4element,fIsotopeWeight[i]);
        else {
            Error("CreateMixture()", "Unknown element %s in formula %s\n", fIsotope[i].Data(), formula.Data());
            return 0x0;
        }
    }
    return g4mat;
}

//______________________________________________________________________________
G4Material* TCGmaterials::CreateG4Mixture(TString formula, const TString densities, const TString prop, G4double density)
{
    TGeoMixture* mix = (TGeoMixture*)gGeoManager->GetListOfMaterials()->FindObject(formula.Data());
    /// If the mixture is already registered in TGeoManager, then convert it in G4material
   if (mix != 0x0) return ConvertGeoMaterial(mix);

    /// If TGeoMixture not defined, create G4Material
    vector<TString> listMat = GetStrings(formula);
    Float_t* compDensity    = new Float_t[listMat.size()];
    Float_t* compProp       = new Float_t[listMat.size()];

    GetCoeff(densities, compDensity, listMat.size());
    GetCoeff(prop, compProp, listMat.size());

    G4Material *g4mat = new G4Material((G4String)formula,density*g/cm3,(G4int)listMat.size());
    for (int i = 0; i < (int)listMat.size(); ++i) {
        if (FootMcDebugLevel(1))
            printf("%s %.2e %.2e\n", listMat[i].Data(), compDensity[i], compProp[i]);

        TString name;
        if ((name = FindByValue(listMat[i])) != "") {
            Info("CreateG4Material()", "Using formula %s for material %s", name.Data(), listMat[i].Data());
            listMat[i] = name;
        }
        G4Material *g4subMat = CreateG4Material(listMat[i].Data(),compDensity[i]);
        if (g4subMat == 0x0) {
            Error("CreateMixture()", "Unknown material %s in formula %s\n", listMat[i].Data(), formula.Data());
            return 0x0;
        }
        g4mat->AddMaterial(g4subMat,compProp[i]);
    }

    delete[] compDensity;
    delete[] compProp;
   
    return g4mat;
}

//______________________________________________________________________________
G4Material* TCGmaterials::ConvertGeoMaterial(const TGeoMaterial *mat)
{
    G4String name(mat->GetName());
    G4double density = mat->GetDensity();
    G4int ncomponents = mat->GetNelements();
    G4double temp = mat->GetTemperature();
    G4double press = mat->GetPressure();
    G4State state = kStateUndefined ;
    switch (mat->GetState()) {
        case TGeoMaterial::kMatStateUndefined :
            state = kStateUndefined;
            break;
        case TGeoMaterial::kMatStateSolid :
            state = kStateSolid;
            break;
        case TGeoMaterial::kMatStateLiquid :
            state = kStateLiquid;
            break;
        case TGeoMaterial::kMatStateGas :
            state = kStateGas;
            break;
    }
    G4NistManager* man = G4NistManager::Instance();
    G4Material* g4mat  = man->FindOrBuildMaterial((G4String)name);
   
    if (g4mat == 0x0) {
      g4mat = new G4Material(name,density*g/cm3,ncomponents,state,temp,press);
      TGeoElement* element = new TGeoElement();
      G4Element* g4element = 0x0;
      if(mat->IsMixture()){
         const TGeoMixture *mixt = (const TGeoMixture*)mat;
         for(int i=0 ; i<ncomponents ; ++i){
            element = mixt->GetElement(i);
            g4element = new G4Element(element->GetTitle(),element->GetName(),G4double(mixt->GetZmixt()[i]),G4double(mixt->GetAmixt()[i])*g/mole);
            g4mat->AddElement(g4element,G4double(mixt->GetWmixt()[i]));
            if(FootMcDebugLevel(1)) printf("===> %s \t %s \t %.2d \t %.2f \n",element->GetTitle(),element->GetName(),element->Z(),element->A());
         }
      } else{
         element = mat->GetElement();
         g4element = new G4Element(element->GetTitle(),element->GetName(),G4double(element->Z()),G4double(element->A())*g/mole);
         g4mat->AddElement(g4element,1);
      }
    }
    return g4mat;
}


//______________________________________________________________________________
void TCGmaterials::CreateG4DefaultMaterials()
{
    G4Element* N = new G4Element("Nitrogen", "N", 7, 14.01*g/mole);
    G4Element* O = new G4Element("Oxygen"  , "O", 8, 16.00*g/mole);

    G4Material *air = new G4Material("Air",1.29e-3*g/cm3,2);
    air->AddElement(O,0.21);
    air->AddElement(N,0.79);

    new G4Material("Vacuum", 1, 1.008*g/mole, 1.0e-25*g/cm3, kStateGas, 2.73*kelvin, 3.e-18*pascal);
}



