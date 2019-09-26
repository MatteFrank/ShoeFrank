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
#include <fstream>
#include <sstream>

#include "TAGmaterials.hxx"

#include "TGeoManager.h"
#include "TObjString.h"

#include "TAGgeoTrafo.hxx"

TAGmaterials* TAGmaterials::fgInstance = 0;

ClassImp(TAGmaterials);

//______________________________________________________________________________
TAGmaterials::TAGmaterials()
 : TAGbaseMaterials()
{
   CreateDefaultMaterials();
}

//______________________________________________________________________________
TAGmaterials::~TAGmaterials()
{
}

//______________________________________________________________________________
TAGmaterials* TAGmaterials::Instance()
{
    if (fgInstance == 0)
        fgInstance = new TAGmaterials();

    return fgInstance;
}

//______________________________________________________________________________
TGeoMaterial* TAGmaterials::CreateMaterial(TString name, Float_t density,
                                            Float_t temperature, Float_t pressure)
{
   fIsotope.clear();
   fIsotopeWeight.clear();
   TGeoMaterial* mat = 0x0;
   TGeoMedium* med   = 0x0;

   TString formula = name;

   // check if already define
   if ( (mat = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->FindObject(name.Data())) != 0x0 )
      return mat;

   
   if (fgkCommonName[name] != "") {
      formula = fgkCommonName[name];
      Info("CreateMaterial()", "Using formula %s for material %s", formula.Data(), name.Data());
   }

   GetIsotopes(formula);

   if (fIsotope.size() == 1 && fIsotopeWeight[0] == 1) { // single element
      TGeoElement* element = fTable->FindElement(fIsotope[0].Data());
      mat = new TGeoMaterial(formula.Data(), element, density);
      med = new TGeoMedium(formula.Data(), mat->GetIndex(), mat);

      return mat;
   } else {
      mat =  new TGeoMixture(name.Data(), fIsotope.size(), density);
   }
   
   // mixture
   for (Int_t i = 0; i < fIsotope.size(); ++i) {
      if (fDebugLevel > 0)
         printf("%s %g\n", fIsotope[i].Data(), fIsotopeWeight[i]);
      TGeoElement* element = fTable->FindElement(fIsotope[i].Data());
      if (element == 0x0) {
         Error("CreateMixture()", "Unknown element %s in formula %s\n", fIsotope[i].Data(), formula.Data());
         return 0x0;
      }
      TString tmp = Form("%g", fIsotopeWeight[i]);
      if (tmp.Contains(".")) {
         ((TGeoMixture*)mat)->AddElement(element, fIsotopeWeight[i]);
      } else
         ((TGeoMixture*)mat)->AddElement(element, TMath::Nint(fIsotopeWeight[i]));

   }
   
   if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(name.Data())) == 0x0 )
      med = new TGeoMedium(name, mat->GetIndex(), mat);

   mat->SetTemperature(temperature);
   mat->SetPressure(pressure);
   return mat;
}

//______________________________________________________________________________
TGeoMixture* TAGmaterials::CreateMixture(TString formula, const TString densities, const TString prop, Float_t density)
{
   // create mixture with differents materials
   // the material, densities and proportion are seperated by a /
   TGeoMedium*  med = 0x0;
   
   vector<TString> listMat = GetStrings(formula);
   Float_t compDensity[listMat.size()];
   Float_t compProp[listMat.size()];
   
   GetCoeff(densities, compDensity, listMat.size());
   GetCoeff(prop, compProp, listMat.size());
   
   TGeoMixture* mix = 0x0;
   
   // check if already define
   if ( (mix = (TGeoMixture*)gGeoManager->GetListOfMaterials()->FindObject(formula.Data())) != 0x0 )
      return mix;
   
   mix = new TGeoMixture(formula.Data(), listMat.size(), density);
   
   for (Int_t i = 0; i < listMat.size(); ++i) {
      if (fDebugLevel > 0)
         printf("%s %f %f\n", listMat[i].Data(), compDensity[i], compProp[i]);
      
      TString name;
      if ((name = FindByValue(listMat[i])) != "") {
         Info("CreateMaterial()", "Using formula %s for material %s", name.Data(), listMat[i].Data());
         listMat[i] = name;
      }

      TGeoMaterial* subMat = 0x0;
      if ( (subMat = (TGeoMixture*)gGeoManager->GetListOfMaterials()->FindObject(listMat[i].Data())) == 0x0 )
         subMat = TAGmaterials::Instance()->CreateMaterial(listMat[i].Data(), compDensity[i]);
      
      if (subMat == 0x0) {
         Error("CreateMixture()", "Unknown material %s in formula %s\n", listMat[i].Data(), formula.Data());
         return 0x0;
      }
      mix->AddElement(subMat, compProp[i]);
   }
   
   if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(formula.Data())) == 0x0 )
      med = new TGeoMedium(formula.Data(), mix->GetIndex(), mix);
   
   return mix;
}


//______________________________________________________________________________
// Prints the MATERIAL, COMPOUND and LOW-MAT
// Aligned variables on the dedicated column
string TAGmaterials::PrintMaterialFluka()
{
   
  TList* list = gGeoManager->GetListOfMaterials();
   
  stringstream ss;

  ReadFlukaDefMat();
  
  for (Int_t i = 0; i < list->GetEntries(); ++i) {
      
    TGeoMixture *mix = (TGeoMixture *)gGeoManager->GetListOfMaterials()->At(i);
      
    Int_t nElements = mix->GetNelements();
        
    // Single material
    if (nElements == 1) {
         
      TGeoElement *element = mix->GetElement(0);
      
      if (fPrintedElt[element->GetName()] == 1)
	continue;

      TString cmd;

      int elId = GetFlukaMatId(element->Z());
      CheckFlukaMat(mix->GetDensity(), element->A(), element->Z());

      NameMap[mix->GetName()] = fFlukaMat[elId].Name;

      if (fgkLowMat[mix->GetName()] == 0) {
    
	cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g", element->A())) + PrependFluka(Form("%g",mix->GetDensity())) +
	  PrependFlukaName(fFlukaMat[elId].Name, 3);

	
	ss << cmd.Data() << endl;
            
      } else { // LOW-MAT
		
	cout << "LOW-MAT material? Check it!! "<< mix->GetName() << endl;
            
	//code to be checked!!!!!
	
	// cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g", element->A())) + PrependFlukaName(element->GetName(), 4);
	// ss << cmd.Data() << endl;

	// cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g",mix->GetDensity()), 2) +  PrependFlukaName(mix->GetName(), 3);
	// ss << cmd.Data() << endl;
	
      }
      
      fPrintedElt[element->GetName()] = 1;
      
    }
      
    // COMPOUND
    if (nElements != 1) {
      
      NameMap[mix->GetName()] = mix->GetName();
            
      for (Int_t e = 0; e < nElements; ++e) {
            
	TGeoElement *element = mix->GetElement(e);
            
	if (fPrintedElt[element->GetName()] == 1)
	  continue;

	int elId = GetFlukaMatId(element->Z());
	
	TString cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g", element->A()))
	  + PrependFluka(Form("%g", ( fFlukaMat[elId].Density ))) + PrependFlukaName(fFlukaMat[elId].Name, 3);
	ss << cmd.Data() << endl;
	
	fPrintedElt[element->GetName()] = 1;
      }

      TString cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%g",mix->GetDensity()), 3) +  PrependFlukaName(mix->GetName(), 3);
      ss << cmd.Data() << endl;

      cmd = AppendFluka("COMPOUND");
         
      for (Int_t e = 0; e < nElements; ++e) {
	
	if(e == 3) {
	  cmd += PrependFlukaName(mix->GetName(), 0);
	  ss << cmd.Data() << endl;
	  cmd = AppendFluka("COMPOUND");
	}
	
	TGeoElement *element = mix->GetElement(e);
	
	int elId = GetFlukaMatId(element->Z());
	    
	if (mix->GetNmixt() != 0x0) {
	  
	  if (fgkLowMat[mix->GetName()] != 0)
	    cmd += PrependFluka(Form("%d.", mix->GetNmixt()[e])) + PrependFluka(Form("%-g", mix->GetTemperature())) +  PrependFlukaName(fFlukaMat[elId].Name, 2);
	  else
	    cmd += PrependFluka(Form("%d.", mix->GetNmixt()[e])) + PrependFlukaName(fFlukaMat[elId].Name, 1, -1);
               
	}
	else
	  cmd += PrependFluka(Form("%-.3e", -mix->GetWmixt()[e])) + PrependFlukaName(fFlukaMat[elId].Name, 1, -1);

      }
      
      if (nElements == 2)
	cmd += PrependFlukaName(mix->GetName(), 2);
      else if (nElements == 3)
	cmd += PrependFlukaName(mix->GetName(), 0);
      else if (nElements == 4)
	cmd += PrependFlukaName(mix->GetName(), 4);
      else
	Warning("SaveFileFluka()", "Number of element in the compund material is %d (max: 3)", nElements);

      ss << cmd.Data() << endl;
    }
      
      
    // LOW-MAT
    if (fgkLowMat[mix->GetName()] != 0 && nElements == 1) {

      cout << "LOW-MAT material? Check it!! "<< mix->GetName() << endl;

      //code to be checked!!!!!
      
      // TGeoElement *element = mix->GetElement(0);
         
      // if (mix->GetNmixt() != 0x0) {
      // 	if (fgkLowMat[mix->GetName()] != 0) {               
      // 	  TString cmd = AppendFluka("LOW-MAT") +  PrependFlukaName(mix->GetName(), 1, -1) + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%4d.", -mix->GetNmixt()[0]));
      // 	  cmd += PrependFluka(Form("%-g", mix->GetTemperature())) +  PrependFlukaName(element->GetName(), 2);
      // 	  ss << cmd.Data() << endl;
      // 	}
      // }
    }
      
  }

  return ss.str();
      
}

//______________________________________________________________________________
TString TAGmaterials::AppendFluka(const Char_t* string, Int_t what)
{
   TString name = string;
   Int_t len = name.Length();
   
   Int_t blank = fgkWhatWidth*what - len;
   
   if (len > 10)
      Warning("AppendFluka()", "Digit number is %d (max: 10)", len);
      
   TString wBlank = fgkWhat(0, blank);
   
   name += wBlank;
   
   return name;
}

//______________________________________________________________________________
TString TAGmaterials::PrependFluka(const Char_t* string, Int_t what)
{
   TString name = string;
   
   if (name.IsDigit())
      name  += ".";
   
   Int_t len = name.Length();
   
   if (len > 10)
      Warning("PrependFluka()", "Digit number is %d (max: 10)", len);

   Int_t blank = fgkWhatWidth*what - len;
   TString wBlank = fgkWhat(0, blank);

   name.Prepend(wBlank);
   
   return name;
}

//______________________________________________________________________________
TString TAGmaterials::PrependFlukaName(const Char_t* string, Int_t what, Int_t align)
{
   TString name = string;
   
   Int_t len = 0;
   if (align != 1)
      len = name.Length();
   
   Int_t blank = fgkWhatWidth*what - len;
   TString wBlank = fgkWhat(0, blank);
   
   name.Prepend(wBlank);
   
   return name;
}



//______________________________________________________________________________
TString TAGmaterials::GetFlukaMatName(TString matname)
{

  TString flkname;
  
  map<TString,TString>::iterator it = NameMap.find(matname);

  if(it == NameMap.end()) 
    cout << "Key-value pair not present in map for material " << matname <<endl; 
  else{
    // cout << "Key-value pair present : "  << it->first << "->" << it->second << endl;;
    flkname = it->second;
  }
  
  return flkname;
}

